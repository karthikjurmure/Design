#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;


// ============================================================
// ENUMS
// ============================================================

enum class City {
    BANGALORE,
    DELHI
};

enum class SeatCategory {
    SILVER,
    GOLD,
    PLATINUM
};


// ============================================================
// MOVIE
// ============================================================

class Movie {
private:
    int movieId;
    string movieName;
    int duration;

public:
    Movie(int id, string name, int duration) {
        movieId = id;
        movieName = name;
        this->duration = duration;
    }

    int getMovieId() {
        return movieId;
    }

    string getMovieName() {
        return movieName;
    }

    int getDuration() {
        return duration;
    }
};


// ============================================================
// SEAT
// ============================================================

class Seat {
private:
    int seatId;
    int row;
    SeatCategory category;

public:
    Seat(int id, int row, SeatCategory category) {
        seatId = id;
        this->row = row;
        this->category = category;
    }

    int getSeatId() {
        return seatId;
    }

    int getRow() {
        return row;
    }
};


// ============================================================
// SCREEN
// ============================================================

class Screen {
private:
    int screenId;
    vector<Seat> seats;

public:
    Screen(int id) {
        screenId = id;

        // Create 100 seats
        for (int i = 1; i <= 100; i++) {

            SeatCategory category;

            if (i <= 40)
                category = SeatCategory::SILVER;
            else if (i <= 80)
                category = SeatCategory::GOLD;
            else
                category = SeatCategory::PLATINUM;

            seats.push_back(
                Seat(i, (i - 1) / 10 + 1, category)
            );
        }
    }

    int getScreenId() {
        return screenId;
    }

    vector<Seat>& getSeats() {
        return seats;
    }
};


// ============================================================
// SHOW
// ============================================================

class Show {
private:
    int showId;
    Movie* movie;
    Screen* screen;
    int startTime;

    // Stores booked seat IDs
    vector<int> bookedSeats;

public:
    Show(int id, Movie* movie, Screen* screen, int startTime) {
        showId = id;
        this->movie = movie;
        this->screen = screen;
        this->startTime = startTime;
    }

    int getShowId() {
        return showId;
    }

    Movie* getMovie() {
        return movie;
    }

    Screen* getScreen() {
        return screen;
    }

    int getStartTime() {
        return startTime;
    }

    bool isSeatBooked(int seatId) {

        return find(
            bookedSeats.begin(),
            bookedSeats.end(),
            seatId
        ) != bookedSeats.end();
    }

    bool bookSeat(int seatId) {

        if (isSeatBooked(seatId)) {
            return false;
        }

        bookedSeats.push_back(seatId);
        return true;
    }

    void cancelSeat(int seatId) {

        auto it = find(
            bookedSeats.begin(),
            bookedSeats.end(),
            seatId
        );

        if (it != bookedSeats.end()) {
            bookedSeats.erase(it);
        }
    }
};


// ============================================================
// THEATRE
// ============================================================

class Theatre {
private:
    int theatreId;
    string theatreName;
    City city;

    vector<Screen> screens;
    vector<Show*> shows;

public:
    Theatre(
        int id,
        string name,
        City city
    ) {
        theatreId = id;
        theatreName = name;
        this->city = city;
    }

    void addScreen(Screen screen) {
        screens.push_back(screen);
    }

    void addShow(Show* show) {
        shows.push_back(show);
    }

    int getTheatreId() {
        return theatreId;
    }

    string getTheatreName() {
        return theatreName;
    }

    City getCity() {
        return city;
    }

    vector<Show*>& getShows() {
        return shows;
    }

    Screen* getScreen() {

        if (screens.empty())
            return nullptr;

        return &screens[0];
    }
};


// ============================================================
// PAYMENT SERVICE
// ============================================================

class PaymentService {

public:

    bool processPayment(double amount) {

        cout << "\nProcessing payment of Rs."
             << amount << "...\n";

        // Simulating successful payment
        cout << "Payment successful!\n";

        return true;
    }
};


// ============================================================
// MOVIE CONTROLLER
// ============================================================

class MovieController {

private:

    // City -> Movies
    map<City, vector<Movie*>> cityVsMovies;

    vector<Movie*> allMovies;

public:

    void addMovie(Movie* movie, City city) {

        allMovies.push_back(movie);

        cityVsMovies[city].push_back(movie);
    }


    vector<Movie*>& getMoviesByCity(City city) {

        return cityVsMovies[city];
    }


    Movie* getMovieByName(string name) {

        for (Movie* movie : allMovies) {

            if (movie->getMovieName() == name) {
                return movie;
            }
        }

        return nullptr;
    }
};


// ============================================================
// THEATRE CONTROLLER
// ============================================================

class TheatreController {

private:

    // City -> Theatres
    map<City, vector<Theatre*>> cityVsTheatres;

public:

    void addTheatre(
        Theatre* theatre,
        City city
    ) {

        cityVsTheatres[city].push_back(theatre);
    }


    // Find all shows for a particular movie
    vector<pair<Theatre*, Show*>>
    getShows(
        Movie* movie,
        City city
    ) {

        vector<pair<Theatre*, Show*>> result;

        for (Theatre* theatre : cityVsTheatres[city]) {

            for (Show* show : theatre->getShows()) {

                if (
                    show->getMovie()->getMovieId()
                    ==
                    movie->getMovieId()
                ) {

                    result.push_back(
                        {theatre, show}
                    );
                }
            }
        }

        return result;
    }
};


// ============================================================
// MOVIE FACTORY
// ============================================================

class MovieFactory {

public:

    static Movie* createMovie(
        int id,
        string name,
        int duration
    ) {

        return new Movie(
            id,
            name,
            duration
        );
    }
};


// ============================================================
// BOOKING SERVICE - SINGLETON
// ============================================================

class BookingService {

private:

    static BookingService* instance;

    MovieController movieController;
    TheatreController theatreController;
    PaymentService paymentService;


    // Private constructor
    BookingService() {

    }


    // --------------------------------------------------------
    // SELECT CITY
    // --------------------------------------------------------

    City selectCity() {

        cout << "\n============================\n";
        cout << "       SELECT CITY\n";
        cout << "============================\n";

        cout << "1. Bangalore\n";
        cout << "2. Delhi\n";

        int choice;
        cin >> choice;

        if (choice == 1)
            return City::BANGALORE;

        return City::DELHI;
    }


    // --------------------------------------------------------
    // SELECT MOVIE
    // --------------------------------------------------------

    Movie* selectMovie(City city) {

        vector<Movie*>& movies =
            movieController.getMoviesByCity(city);


        cout << "\n============================\n";
        cout << "       SELECT MOVIE\n";
        cout << "============================\n";


        for (int i = 0; i < movies.size(); i++) {

            cout << i + 1
                 << ". "
                 << movies[i]->getMovieName()
                 << "\n";
        }


        int choice;
        cin >> choice;


        return movies[choice - 1];
    }


    // --------------------------------------------------------
    // SELECT SHOW
    // --------------------------------------------------------

    Show* selectShow(
        City city,
        Movie* movie
    ) {

        auto shows =
            theatreController.getShows(
                movie,
                city
            );


        cout << "\n============================\n";
        cout << "       SELECT SHOW\n";
        cout << "============================\n";


        for (int i = 0; i < shows.size(); i++) {

            Theatre* theatre =
                shows[i].first;

            Show* show =
                shows[i].second;


            cout << i + 1
                 << ". "
                 << theatre->getTheatreName()
                 << " - "
                 << show->getStartTime()
                 << ":00\n";
        }


        int choice;
        cin >> choice;


        return shows[choice - 1].second;
    }


    // --------------------------------------------------------
    // BOOK SEAT
    // --------------------------------------------------------

    void bookSeat(Show* show) {

        cout << "\n============================\n";
        cout << "       SELECT SEAT\n";
        cout << "============================\n";

        cout << "Enter seat number (1-100): ";

        int seatNumber;
        cin >> seatNumber;


        // Check if seat is already booked
        if (show->isSeatBooked(seatNumber)) {

            cout << "Seat already booked!\n";

            return;
        }


        // Temporarily book the seat
        show->bookSeat(seatNumber);


        // Payment
        bool paymentSuccessful =
            paymentService.processPayment(250);


        if (paymentSuccessful) {

            cout << "\n================================\n";
            cout << "       BOOKING SUCCESSFUL\n";
            cout << "================================\n";

            cout << "Movie : "
                 << show->getMovie()->getMovieName()
                 << "\n";

            cout << "Show  : "
                 << show->getStartTime()
                 << ":00\n";

            cout << "Seat  : "
                 << seatNumber
                 << "\n";

            cout << "Price : Rs.250\n";

            cout << "================================\n";
        }

        else {

            // Payment failed
            // Rollback seat booking

            show->cancelSeat(seatNumber);

            cout << "Payment failed.\n";
        }
    }


public:

    // --------------------------------------------------------
    // SINGLETON
    // --------------------------------------------------------

    static BookingService* getInstance() {

        if (instance == nullptr) {

            instance = new BookingService();
        }

        return instance;
    }


    // --------------------------------------------------------
    // INITIALIZE DATA
    // --------------------------------------------------------

    void initialize() {

        // -------------------------
        // Create Movies
        // -------------------------

        Movie* barbie =
            MovieFactory::createMovie(
                1,
                "BARBIE",
                128
            );


        Movie* oppenheimer =
            MovieFactory::createMovie(
                2,
                "OPPENHEIMER",
                180
            );


        // Add movies to cities

        movieController.addMovie(
            barbie,
            City::BANGALORE
        );

        movieController.addMovie(
            barbie,
            City::DELHI
        );


        movieController.addMovie(
            oppenheimer,
            City::BANGALORE
        );

        movieController.addMovie(
            oppenheimer,
            City::DELHI
        );


        // -------------------------
        // Create Theatres
        // -------------------------

        Theatre* inox =
            new Theatre(
                1,
                "INOX",
                City::BANGALORE
            );


        Theatre* pvr =
            new Theatre(
                2,
                "PVR",
                City::DELHI
            );


        // -------------------------
        // Create Screens
        // -------------------------

        inox->addScreen(
            Screen(1)
        );

        pvr->addScreen(
            Screen(1)
        );


        // -------------------------
        // Create Shows
        // -------------------------

        Show* barbieInox =
            new Show(
                1,
                barbie,
                inox->getScreen(),
                10
            );


        Show* oppenheimerInox =
            new Show(
                2,
                oppenheimer,
                inox->getScreen(),
                18
            );


        Show* barbiePvr =
            new Show(
                3,
                barbie,
                pvr->getScreen(),
                14
            );


        Show* oppenheimerPvr =
            new Show(
                4,
                oppenheimer,
                pvr->getScreen(),
                20
            );


        // Add shows to theatres

        inox->addShow(barbieInox);
        inox->addShow(oppenheimerInox);

        pvr->addShow(barbiePvr);
        pvr->addShow(oppenheimerPvr);


        // Add theatres to controller

        theatreController.addTheatre(
            inox,
            City::BANGALORE
        );

        theatreController.addTheatre(
            pvr,
            City::DELHI
        );
    }


    // --------------------------------------------------------
    // START BOOKING
    // --------------------------------------------------------

    void startBooking() {

        cout << "\n";
        cout << "====================================\n";
        cout << "       WELCOME TO BOOKMYSHOW\n";
        cout << "====================================\n";


        while (true) {

            // Step 1
            City city = selectCity();


            // Step 2
            Movie* movie =
                selectMovie(city);


            // Step 3
            Show* show =
                selectShow(
                    city,
                    movie
                );


            // Step 4
            bookSeat(show);


            cout << "\nDo you want to book another ticket?\n";
            cout << "1. Yes\n";
            cout << "2. No\n";


            int choice;
            cin >> choice;


            if (choice == 2)
                break;
        }


        cout << "\nThank you for using BookMyShow!\n";
    }
};


// ============================================================
// STATIC SINGLETON INSTANCE
// ============================================================

BookingService*
BookingService::instance = nullptr;


// ============================================================
// MAIN
// ============================================================

int main() {

    // Get Singleton instance
    BookingService* bookingService =
        BookingService::getInstance();


    // Create initial data
    bookingService->initialize();


    // Start application
    bookingService->startBooking();


    return 0;
}
