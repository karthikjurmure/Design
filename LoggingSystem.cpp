#include <iostream>
#include <memory>
#include <vector>

using namespace std;
enum class Loglevel { INFO, WARN, ERROR };
class Formatter {
public:
  virtual string format(string s) = 0;
  virtual ~Formatter() {};
};
class PlainFormatter : public Formatter {
public:
  string format(string msg) override { return "[LOG] " + msg; }
};
class JSONFOrmatter : public Formatter {
public:
  string format(string msg) override { return "{log:" + msg + "}"; }
};
class Appender {
protected:
  Formatter *formatter;

public:
  Appender(Formatter *f) { this->formatter = f; }
  virtual void append(string s) = 0;
  virtual ~Appender() {};
};
class ConsoleAppender : public Appender {
public:
  ConsoleAppender(Formatter *f) : Appender(f) {}
  void append(string s) override { cout << formatter->format(s) << endl; }
};
//chain of responsibility behaviour
class LogHandler {
protected:
  LogHandler *next = nullptr;
  vector<Appender *> appenders;

public:
  void setNext(LogHandler *l) { next = l; }
  void addAppender(Appender *app) { appenders.push_back(app); }
  virtual bool canHandle(Loglevel level) = 0;
  void handle(Loglevel level, string msg) {
    if (canHandle(level)) {
      for (auto it : appenders) {
        it->append(msg);
      }
    }
    if (next) {
      next->handle(level, msg);
    }
  }
};
class InfoHandler : public LogHandler {
public:
  bool canHandle(Loglevel level) override { return level == Loglevel::INFO; }
};
class WarnHandler : public LogHandler {
public:
  bool canHandle(Loglevel level) override { return level == Loglevel::WARN; }
};
class ErrorHandler : public LogHandler {
public:
  bool canHandle(Loglevel level) override { return level == Loglevel::ERROR; }
};
class Logger {
private:
  LogHandler *head;

public:
  Logger(LogHandler *h) { this->head = h; }
  void log(Loglevel level, string msg) { head->handle(level, msg); }
};
int main() {
  Formatter *format = new PlainFormatter();
  Appender *appender = new ConsoleAppender(format);
  LogHandler *info = new InfoHandler();
  LogHandler *warn = new WarnHandler();
  LogHandler *error = new ErrorHandler();
  info->setNext(warn);
  warn->setNext(error);
  info->addAppender(appender);
  warn->addAppender(appender);
  error->addAppender(appender);

  Logger logger(info);

  logger.log(Loglevel::INFO, "Application Started");
  logger.log(Loglevel::WARN, "Memory Usage High");
  logger.log(Loglevel::ERROR, "Database Connection Failed");

  return 0;
}
