#include <iostream>
#include <vector>
using namespace std;
class Person {
private:
  string name;
  string email;

public:
  Person(string n, string e) {
    this->name = n;
    this->email = e;
  }
  string getname() { return name; }
  string getemail() { return email; }
};
class Meeting {
private:
  int starttime;
  int endtime;
  Person *organizer;
  vector<Person *> attende;

public:
  Meeting(int s, int e, Person *p, vector<Person *> participants) {
    this->starttime = s;
    this->endtime = e;
    this->organizer = p;
    this->attende = participants;
  }
  int getStartTime() { return starttime; }

  int getEndTime() { return endtime; }

  vector<Person *> getParticipants() { return attende; }
};
class Room {
private:
  string roomid;
  vector<Meeting *> meetings;

public:
  Room(string i) { this->roomid = i; }
  void addMeeting(Meeting *m) { meetings.push_back(m); }
  string getRoomid() { return roomid; }
  vector<Meeting *> getMeetings() { return meetings; }
};
class NotificationService {
public:
  void notify(vector<Person *> persons) {
    for (auto it : persons) {
      cout << "Notification sent to " << it->getemail() << endl;
    }
  }
};
class CalendarService {
public:
  void createInvite() { cout << "Calendar Invite Created" << endl; }
};
class BookingManager {
private:
  Room *room;
  NotificationService *notificationService;
  CalendarService *calendarService;

public:
  BookingManager(Room *room, NotificationService *n, CalendarService *c) {
    this->room = room;
    this->notificationService = n;
    this->calendarService = c;
  }
  bool bookRoom(Meeting *meeting, Room *room) {
    for (auto it : room->getMeetings()) {
      if (it->getEndTime() > meeting->getStartTime()) {
        cout << "Booking Failed.\n";

        cout << "Meeting room already occupied.\n";

        return false;
      }
    }
    room->addMeeting(meeting);
    notificationService->notify(meeting->getParticipants());
    calendarService->createInvite();
    cout << "Meeting Room Booked Successfully\n";
    return true;
  }
};
int main() {
  Person p1("Alice", "alice@gmail.com");

  Person p2("Bob", "bob@gmail.com");

  Person p3("Charlie", "charlie@gmail.com");
  Room r1("R1");
  NotificationService ns;
  CalendarService cs;
  BookingManager bm(&r1, &ns, &cs);
  vector<Person *> participants;
  participants.push_back(&p2);
  participants.push_back(&p3);
  Meeting m1(10, 12, &p1, participants);
  bm.bookRoom(&m1, &r1);
  Meeting m2(11, 14, &p1, participants);
  bm.bookRoom(&m2, &r1);
}
