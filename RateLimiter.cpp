
#include <iostream>
#include <unordered_map>
#include <queue>
#include <memory>
#include <chrono>
#include <algorithm>
using namespace std;

// ---------- ENUMS ----------
enum class UserTier { FREE, PREMIUM };
enum class RateLimitType { TOKEN_BUCKET, FIXED_WINDOW, SLIDING_WINDOW_LOG };

// ---------- USER ----------
class User {
    string id;
    UserTier tier;

public:
    User(string id, UserTier tier) : id(id), tier(tier) {}

    string getId() const { return id; }
    UserTier getTier() const { return tier; }
};

// ---------- CONFIG ----------
class RateLimitConfig {
public:
    int maxRequests, windowSeconds;

    RateLimitConfig(int maxReq, int window)
        : maxRequests(maxReq), windowSeconds(window) {}
};

// Current time in milliseconds
long long nowMs() {
    return chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now().time_since_epoch()
    ).count();
}

// ---------- STRATEGY INTERFACE ----------
class RateLimiter {
protected:
    RateLimitConfig config;

public:
    RateLimiter(RateLimitConfig c) : config(c) {}
    virtual ~RateLimiter() = default;

    virtual bool allowRequest(string userId) = 0;
};

// ---------- 1. TOKEN BUCKET ----------
class TokenBucket : public RateLimiter {
    unordered_map<string, double> tokens;
    unordered_map<string, long long> lastTime;

public:
    TokenBucket(RateLimitConfig c) : RateLimiter(c) {}

    bool allowRequest(string id) override {
        long long now = nowMs();
        double windowMs = config.windowSeconds * 1000.0;

        // First request: initialize a full bucket
        if (!lastTime.count(id)) {
            tokens[id] = config.maxRequests;
            lastTime[id] = now;
        }

        // Refill tokens gradually
        double elapsed = now - lastTime[id];
        tokens[id] = min(
            (double)config.maxRequests,
            tokens[id] + elapsed * config.maxRequests / windowMs
        );
        lastTime[id] = now;

        // Consume one token
        if (tokens[id] >= 1) {
            tokens[id]--;
            return true;
        }

        return false;
    }
};

// ---------- 2. FIXED WINDOW ----------
class FixedWindow : public RateLimiter {
    unordered_map<string, int> count;
    unordered_map<string, long long> window;

public:
    FixedWindow(RateLimitConfig c) : RateLimiter(c) {}

    bool allowRequest(string id) override {
        long long now = nowMs() / 1000;
        long long currentWindow = now / config.windowSeconds;

        // Reset count when the window changes
        if (!window.count(id) || window[id] != currentWindow) {
            window[id] = currentWindow;
            count[id] = 0;
        }

        if (count[id] < config.maxRequests) {
            count[id]++;
            return true;
        }

        return false;
    }
};

// ---------- 3. SLIDING WINDOW LOG ----------
class SlidingWindowLog : public RateLimiter {
    unordered_map<string, queue<long long>> logs;

public:
    SlidingWindowLog(RateLimitConfig c) : RateLimiter(c) {}

    bool allowRequest(string id) override {
        long long now = nowMs() / 1000;
        auto& q = logs[id];

        // Remove expired requests
        while (!q.empty() &&
               now - q.front() >= config.windowSeconds) {
            q.pop();
        }

        // Allow if below limit
        if (q.size() < config.maxRequests) {
            q.push(now);
            return true;
        }

        return false;
    }
};

// ---------- FACTORY ----------
class RateLimiterFactory {
public:
    static unique_ptr<RateLimiter> create(
        RateLimitType type, RateLimitConfig config) {

        if (type == RateLimitType::TOKEN_BUCKET)
            return make_unique<TokenBucket>(config);

        if (type == RateLimitType::FIXED_WINDOW)
            return make_unique<FixedWindow>(config);

        return make_unique<SlidingWindowLog>(config);
    }
};

// ---------- SERVICE ----------
class RateLimiterService {
    unordered_map<UserTier, unique_ptr<RateLimiter>> limiters;

public:
    RateLimiterService() {
        limiters[UserTier::FREE] =
            RateLimiterFactory::create(
                RateLimitType::TOKEN_BUCKET,
                RateLimitConfig(10, 60));

        limiters[UserTier::PREMIUM] =
            RateLimiterFactory::create(
                RateLimitType::FIXED_WINDOW,
                RateLimitConfig(100, 60));
    }

    bool allowRequest(const User& user) {
        return limiters.at(user.getTier())->allowRequest(user.getId());
    }
};

// ---------- MAIN ----------
int main() {
    RateLimiterService service;

    User freeUser("user1", UserTier::FREE);
    User premiumUser("user2", UserTier::PREMIUM);

    for (int i = 1; i <= 15; i++)
        cout << "Free " << i << ": "
             << (service.allowRequest(freeUser) ? "ALLOWED" : "BLOCKED")
             << '\n';

    for (int i = 1; i <= 105; i++)
        cout << "Premium " << i << ": "
             << (service.allowRequest(premiumUser) ? "ALLOWED" : "BLOCKED")
             << '\n';
}
