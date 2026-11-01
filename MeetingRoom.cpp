#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------- Person
class Person {
private:
  string name;
  string email;

public:
  Person(const string &name, const string &email) : name(name), email(email) {}

  const string &getName() const { return name; }
  const string &getEmail() const { return email; }
};

// ---------------------------------------------------------------- Meeting
class Meeting {
private:
  int startTime;
  int endTime;
  Person *organizer;           // non-owning
  vector<Person *> attendees;  // non-owning

public:
  Meeting(int start, int end, Person *organizer,
          const vector<Person *> &attendees)
      : startTime(start), endTime(end), organizer(organizer),
        attendees(attendees) {}

  int getStartTime() const { return startTime; }
  int getEndTime() const { return endTime; }
  Person *getOrganizer() const { return organizer; }
  const vector<Person *> &getAttendees() const { return attendees; }

  bool isValid() const { return organizer != nullptr && startTime < endTime; }

  // Two time ranges overlap only if each starts before the other ends.
  // Back-to-back meetings (one ends at 12, next starts at 12) do NOT overlap.
  bool overlapsWith(const Meeting &other) const {
    return startTime < other.endTime && endTime > other.startTime;
  }
};

// ---------------------------------------------------------------- Room
class Room {
private:
  string roomId;
  vector<Meeting *> meetings;  // non-owning

public:
  explicit Room(const string &id) : roomId(id) {}

  const string &getRoomId() const { return roomId; }
  const vector<Meeting *> &getMeetings() const { return meetings; }

  bool isAvailable(const Meeting &candidate) const {
    for (const Meeting *existing : meetings) {
      if (existing->overlapsWith(candidate))
        return false;
    }
    return true;
  }

  void addMeeting(Meeting *meeting) { meetings.push_back(meeting); }
};

// ---------------------------------------------------------------- Services
class INotificationService {
public:
  virtual ~INotificationService() = default;
  virtual void notify(const vector<Person *> &recipients,
                      const Meeting &meeting) = 0;
};

class ICalendarService {
public:
  virtual ~ICalendarService() = default;
  virtual void createInvite(const Meeting &meeting, const Room &room) = 0;
};

class EmailNotificationService : public INotificationService {
public:
  void notify(const vector<Person *> &recipients,
              const Meeting &meeting) override {
    for (const Person *p : recipients) {
      cout << "Email sent to " << p->getEmail() << " for meeting "
           << meeting.getStartTime() << "-" << meeting.getEndTime() << endl;
    }
  }
};

class SimpleCalendarService : public ICalendarService {
public:
  void createInvite(const Meeting &meeting, const Room &room) override {
    cout << "Calendar invite created for room " << room.getRoomId() << " ("
         << meeting.getStartTime() << "-" << meeting.getEndTime() << ")"
         << endl;
  }
};

// ---------------------------------------------------------------- Manager
class BookingManager {
private:
  INotificationService &notificationService;
  ICalendarService &calendarService;

public:
  BookingManager(INotificationService &n, ICalendarService &c)
      : notificationService(n), calendarService(c) {}

  bool bookRoom(Meeting *meeting, Room &room) {
    if (meeting == nullptr || !meeting->isValid()) {
      cout << "Booking Failed: invalid meeting.\n";
      return false;
    }
    if (!room.isAvailable(*meeting)) {
      cout << "Booking Failed: room " << room.getRoomId()
           << " is already occupied.\n";
      return false;
    }

    room.addMeeting(meeting);

    vector<Person *> recipients = meeting->getAttendees();
    recipients.push_back(meeting->getOrganizer());
    notificationService.notify(recipients, *meeting);
    calendarService.createInvite(*meeting, room);

    cout << "Meeting Room Booked Successfully\n";
    return true;
  }
};

int main() {
  Person p1("Alice", "alice@gmail.com");
  Person p2("Bob", "bob@gmail.com");
  Person p3("Charlie", "charlie@gmail.com");

  Room r1("R1");
  EmailNotificationService notifier;
  SimpleCalendarService calendar;
  BookingManager manager(notifier, calendar);

  vector<Person *> attendees = {&p2, &p3};

  Meeting m1(10, 12, &p1, attendees);  // should succeed
  Meeting m2(11, 14, &p1, attendees);  // overlaps m1 -> fail
  Meeting m3(12, 14, &p1, attendees);  // back-to-back with m1 -> succeed
  Meeting m4(15, 14, &p1, attendees);  // invalid time range -> fail

  manager.bookRoom(&m1, r1);
  manager.bookRoom(&m2, r1);
  manager.bookRoom(&m3, r1);
  manager.bookRoom(&m4, r1);
  return 0;
}
