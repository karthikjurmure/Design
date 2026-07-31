#include <iostream>
#include <unordered_map>
#include <queue>
#include <memory>
#include <chrono>
#include <algorithm>
#include <string>

using namespace std;

// ===================== ENUMS =====================

enum class UserTier { FREE, PREMIUM };

enum class RateLimitType {
    TOKEN_BUCKET,
    FIXED_WINDOW,
    SLIDING_WINDOW_LOG
};

// ===================== MODELS =====================

class User {
    string userId;
    UserTier tier;
public:
    User(const string& id, UserTier t): userId(id), tier(t) {}
    string getUserId() const { return userId; }
    UserTier getTier() const { return tier; }
};

class RateLimitConfig {
    int maxRequests;
    int windowInSeconds;
public:
    RateLimitConfig(int maxReq,int window):
        maxRequests(maxReq),windowInSeconds(window){}
    int getMaxRequests() const { return maxRequests; }
    int getWindowInSeconds() const { return windowInSeconds; }
};

// ===================== BASE =====================

class RateLimiter {
protected:
    RateLimitConfig config;
    RateLimitType type;
public:
    RateLimiter(const RateLimitConfig& c, RateLimitType t)
        : config(c), type(t) {}
    virtual ~RateLimiter() = default;
    virtual bool allowRequest(const string& userId)=0;
};

// ===================== TOKEN BUCKET =====================

class TokenBucketRateLimiter : public RateLimiter {
    unordered_map<string,int> tokens;
    unordered_map<string,long long> lastRefill;

    int refillTokens(const string& userId,long long now){
        double refillRate=(double)config.getWindowInSeconds()/config.getMaxRequests();

        if(lastRefill.find(userId)==lastRefill.end())
            lastRefill[userId]=now;

        long long elapsed=(now-lastRefill[userId])/1000;

        int refill=(int)(elapsed/refillRate);

        int current=tokens.count(userId)?
                    tokens[userId]:
                    config.getMaxRequests();

        current=min(config.getMaxRequests(),current+refill);

        if(refill>0)
            lastRefill[userId]=now;

        return current;
    }

public:
    TokenBucketRateLimiter(const RateLimitConfig& c)
        : RateLimiter(c,RateLimitType::TOKEN_BUCKET){}

    bool allowRequest(const string& userId) override{
        long long now=chrono::duration_cast<chrono::milliseconds>(
                chrono::system_clock::now().time_since_epoch()).count();

        int current=refillTokens(userId,now);

        if(current>0){
            tokens[userId]=current-1;
            return true;
        }
        return false;
    }
};

// ===================== FIXED WINDOW =====================

class FixedWindowRateLimiter : public RateLimiter {

    unordered_map<string,int> requestCount;
    unordered_map<string,long long> windowStart;

public:

    FixedWindowRateLimiter(const RateLimitConfig& c)
        : RateLimiter(c,RateLimitType::FIXED_WINDOW){}

    bool allowRequest(const string& userId) override{

        long long currentWindow=
            chrono::duration_cast<chrono::seconds>(
                chrono::system_clock::now().time_since_epoch()
            ).count()/config.getWindowInSeconds();

        if(windowStart.find(userId)==windowStart.end()){
            windowStart[userId]=currentWindow;
            requestCount[userId]=0;
        }

        if(windowStart[userId]!=currentWindow){
            windowStart[userId]=currentWindow;
            requestCount[userId]=0;
        }

        if(requestCount[userId]<config.getMaxRequests()){
            requestCount[userId]++;
            return true;
        }

        return false;
    }
};

// ===================== SLIDING WINDOW LOG =====================

class SlidingWindowLogRateLimiter : public RateLimiter {

    unordered_map<string,queue<long long>> requestLog;

public:

    SlidingWindowLogRateLimiter(const RateLimitConfig& c)
        : RateLimiter(c,RateLimitType::SLIDING_WINDOW_LOG){}

    bool allowRequest(const string& userId) override{

        long long now=
            chrono::duration_cast<chrono::seconds>(
                chrono::system_clock::now().time_since_epoch()
            ).count();

        queue<long long>& log=requestLog[userId];

        while(!log.empty() &&
              now-log.front()>=config.getWindowInSeconds())
            log.pop();

        if((int)log.size()<config.getMaxRequests()){
            log.push(now);
            return true;
        }

        return false;
    }
};

// ===================== FACTORY =====================

class RateLimiterFactory{

public:

    static unique_ptr<RateLimiter>
    createRateLimiter(RateLimitType type,
                      const RateLimitConfig& config){

        switch(type){

        case RateLimitType::TOKEN_BUCKET:
            return make_unique<TokenBucketRateLimiter>(config);

        case RateLimitType::FIXED_WINDOW:
            return make_unique<FixedWindowRateLimiter>(config);

        case RateLimitType::SLIDING_WINDOW_LOG:
            return make_unique<SlidingWindowLogRateLimiter>(config);

        default:
            return nullptr;
        }
    }
};

// ===================== SERVICE =====================

class RateLimiterService{

    unordered_map<UserTier,unique_ptr<RateLimiter>> limiters;

public:

    RateLimiterService(){

        limiters[UserTier::FREE]=
            RateLimiterFactory::createRateLimiter(
                RateLimitType::TOKEN_BUCKET,
                RateLimitConfig(10,60));

        limiters[UserTier::PREMIUM]=
            RateLimiterFactory::createRateLimiter(
                RateLimitType::FIXED_WINDOW,
                RateLimitConfig(100,60));
    }

    bool allowRequest(const User& user){

        return limiters[user.getTier()]
            ->allowRequest(user.getUserId());
    }
};

// ===================== MAIN =====================

int main(){

    RateLimiterService service;

    User freeUser("user1",UserTier::FREE);
    User premiumUser("user2",UserTier::PREMIUM);

    cout<<"===== FREE USER ====="<<endl;

    for(int i=1;i<=15;i++){

        bool allowed=service.allowRequest(freeUser);

        cout<<"Request "<<i<<" : "
            <<(allowed?"ALLOWED":"BLOCKED")
            <<endl;
    }

    cout<<"\n===== PREMIUM USER ====="<<endl;

    for(int i=1;i<=105;i++){

        bool allowed=service.allowRequest(premiumUser);

        cout<<"Request "<<i<<" : "
            <<(allowed?"ALLOWED":"BLOCKED")
            <<endl;
    }

    return 0;
}
