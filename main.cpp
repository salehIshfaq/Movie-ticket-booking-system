#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
using namespace std;

int getValidatedInt(const string& prompt, int min = 1) {
    int value;
    cout << prompt;
    while (!(cin >> value) || value < min) {
        cout << "Invalid input. Please enter a number >= " << min << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

class Entity {
protected:
    string name;
public:
    Entity() {}
    Entity(const string& name) : name(name) {}
    virtual ~Entity() {}
    virtual void displayDetails() const = 0;
    string getName() const { return name; }
};

class Movie : public Entity {
private:
    int duration;
    string genre;
    string showDate;
    int totalSeats;
    int availableSeats;
public:
    Movie() {}
    Movie(string n, int d, string g, string s, int t)
        : Entity(n), duration(d), genre(g), showDate(s),
          totalSeats(t), availableSeats(t) {}

    string getGenre() const { return genre; }
    string getShowDate() const { return showDate; }
    int getAvailableSeats() const { return availableSeats; }
    int getTotalSeats() const { return totalSeats; }

    bool bookSeats(int seats) {
        if (seats <= availableSeats) {
            availableSeats -= seats;
            return true;
        }
        return false;
    }

    void cancelBooking(int seats) {
        availableSeats += seats;
        if (availableSeats > totalSeats) availableSeats = totalSeats;
    }

    void updateMovie(string n, int d, string g, string s, int t) {
        name = n;
        duration = d;
        genre = g;
        showDate = s;
        totalSeats = t;
        availableSeats = t;
    }

    void displayDetails() const override {
        cout << "Movie: " << name << "\n"
             << "Duration: " << duration << " min\n"
             << "Genre: " << genre << "\n"
             << "Show Date: " << showDate << "\n"
             << "Seats: " << availableSeats << "/" << totalSeats << "\n";
    }

    void writeToFile(ofstream& out) const {
        out << name << "\n" << duration << "\n" << genre << "\n"
            << showDate << "\n" << totalSeats << "\n" << availableSeats << "\n";
    }

    void readFromFile(ifstream& in) {
        getline(in, name);
        in >> duration;
        in.ignore();
        getline(in, genre);
        getline(in, showDate);
        in >> totalSeats >> availableSeats;
        in.ignore();
    }
};

class Customer : public Entity {
private:
    int age;
    string gender;
    string phone;
    string username;
public:
    string bookedMovie;
    string showDate;
    int bookedSeats;

    Customer() {}
    Customer(string n, int a, string g, string p, string bm, string sd, int bs, string user)
        : Entity(n), age(a), gender(g), phone(p), bookedMovie(bm),
          showDate(sd), bookedSeats(bs), username(user) {}

    void displayDetails() const override {
        cout << "Customer: " << name << ", Age: " << age << ", Gender: " << gender << "\n"
             << "Phone: " << phone << "\nMovie: " << bookedMovie
             << " on " << showDate << ", Seats: " << bookedSeats << "\n";
    }

    void writeToFile(ofstream& out) const {
        out << name << "\n" << age << "\n" << gender << "\n" << phone << "\n"
            << bookedMovie << "\n" << showDate << "\n" << bookedSeats << "\n"
            << username << "\n";
    }

    void readFromFile(ifstream& in) {
        getline(in, name);
        in >> age;
        in.ignore();
        getline(in, gender);
        getline(in, phone);
        getline(in, bookedMovie);
        getline(in, showDate);
        in >> bookedSeats;
        in.ignore();
        getline(in, username);
    }

    string getBookedMovie() const { return bookedMovie; }
    string getPhone() const { return phone; }
    string getUsername() const { return username; }
};

class User {
protected:
    string username;
    string password;
public:
    User(string u, string p) : username(u), password(p) {}
    virtual ~User() {}
    string getUsername() const { return username; }
    bool authenticate(const string& u, const string& p) {
        return u == username && p == password;
    }
    virtual void menu() = 0;
};

class MovieTicketBookingSystem;

class Admin : public User {
private:
    MovieTicketBookingSystem* system;
public:
    Admin(string u, string p, MovieTicketBookingSystem* sys)
        : User(u, p), system(sys) {}
    void menu() override;
};

class CustomerUser : public User {
private:
    MovieTicketBookingSystem* system;
public:
    CustomerUser(string u, string p, MovieTicketBookingSystem* sys)
        : User(u, p), system(sys) {}
    void menu() override;
};

class AuthenticationSystem {
private:
    vector<User*> users;
    MovieTicketBookingSystem* system;
public:
    AuthenticationSystem(MovieTicketBookingSystem* sys) : system(sys) {}
    ~AuthenticationSystem() {
        for (auto user : users)
            delete user;
    }

    User* login(const string& uname, const string& pass) {
        for (auto user : users)
            if (user->authenticate(uname, pass))
                return user;
        return nullptr;
    }

    void registerAdmin(const string& uname, const string& pass) {
        users.push_back(new Admin(uname, pass, system));
    }

    void registerCustomer(const string& uname, const string& pass) {
        users.push_back(new CustomerUser(uname, pass, system));
    }
};

class MovieTicketBookingSystem {
private:
    vector<Movie> movies;
    vector<Customer> customers;

    string toLower(const string& s) {
        string lower = s;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;
    }

public:
    void addMovie() {
        string name, genre, date;
        int duration, total;
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        duration = getValidatedInt("Duration (minutes): ");
        cout << "Genre: "; getline(cin, genre);
        cout << "Date (DD-MM-YYYY): "; getline(cin, date);
        total = getValidatedInt("Total Seats: ");
        movies.emplace_back(name, duration, genre, date, total);
    }

    void viewMovies() {
        if (movies.empty()) {
            cout << "No movies available.\n";
            return;
        }
        for (const auto& m : movies) {
            m.displayDetails();
            cout << "-----------------------\n";
        }
    }

    void deleteMovie(const string& inputName) {
        string targetName = toLower(inputName);
        bool found = false;

        auto it = movies.begin();
        while (it != movies.end()) {
            if (toLower(it->getName()) == targetName) {
                it = movies.erase(it);
                found = true;
                cout << "Movie \"" << inputName << "\" deleted successfully.\n";
                break;
            } else {
                ++it;
            }
        }

        if (!found) {
            cout << "Movie \"" << inputName << "\" not found.\n";
        }
    }

    void bookTicket(const string& currentUsername) {
        if (movies.empty()) {
            cout << "No movies available.\n";
            return;
        }

        cout << "Available Movies:\n";
        for (size_t i = 0; i < movies.size(); ++i) {
            cout << i + 1 << ". " << movies[i].getName() 
                 << " (" << movies[i].getGenre() << ") - " 
                 << movies[i].getShowDate() << " - Seats: "
                 << movies[i].getAvailableSeats() << "/" << movies[i].getTotalSeats() << "\n";
        }

        int choice = getValidatedInt("Select a movie by number: ", 1);
        if (choice > (int)movies.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        Movie& selectedMovie = movies[choice - 1];
        cout << "You selected: " << selectedMovie.getName() << "\n";

        int seats = getValidatedInt("Enter seats to book: ");
        if (selectedMovie.bookSeats(seats)) {
            string cname, gender, phone;
            int age;
            cin.ignore();
            cout << "Enter your name: "; getline(cin, cname);
            age = getValidatedInt("Enter your age: ", 0);
            cout << "Enter your gender: "; getline(cin, gender);
            cout << "Enter your phone number: "; getline(cin, phone);
            customers.emplace_back(cname, age, gender, phone,
                                   selectedMovie.getName(), selectedMovie.getShowDate(), seats,
                                   currentUsername);
            cout << "Booking successful!\n";
        } else {
            cout << "Not enough seats available.\n";
        }
    }

    void viewCustomers() {
        if (customers.empty()) {
            cout << "No customers found.\n";
            return;
        }
        for (const auto& c : customers) {
            c.displayDetails();
            cout << "------------------\n";
        }
    }

    void cancelBooking(const string& currentUsername) {
        vector<pair<int, Customer>> bookings;
        for (size_t i = 0; i < customers.size(); ++i) {
            if (customers[i].getUsername() == currentUsername) {
                bookings.emplace_back(i, customers[i]);
            }
        }

        if (bookings.empty()) {
            cout << "No bookings found for your account.\n";
            return;
        }

        cout << "Your bookings:\n";
        for (size_t i = 0; i < bookings.size(); ++i) {
            cout << i + 1 << ". ";
            bookings[i].second.displayDetails();
            cout << "-------------------\n";
        }

        int choice = getValidatedInt("Select booking to cancel (0 to exit): ", 0);
        if (choice == 0) return;

        if (choice > (int)bookings.size()) {
            cout << "Invalid choice.\n";
            return;
        }

        int idx = bookings[choice - 1].first;
        Customer& selectedCustomer = customers[idx];
        string movieName = selectedCustomer.getBookedMovie();
        int seatsToCancel = selectedCustomer.bookedSeats;

        auto it = find_if(movies.begin(), movies.end(),
                          [&](Movie& m) { return m.getName() == movieName; });
        if (it != movies.end()) {
            it->cancelBooking(seatsToCancel);
            cout << "Cancelled " << seatsToCancel << " seats for movie " << movieName << ".\n";
        }

        customers.erase(customers.begin() + idx);
    }

    void saveData() {
        ofstream mFile("movies.txt"), cFile("customers.txt");
        for (auto& m : movies) m.writeToFile(mFile);
        for (auto& c : customers) c.writeToFile(cFile);
    }

    void loadData() {
        ifstream mFile("movies.txt"), cFile("customers.txt");
        if (mFile) {
            while (mFile.peek() != EOF) {
                Movie m; m.readFromFile(mFile);
                if (m.getName() != "") movies.push_back(m);
            }
        }
        if (cFile) {
            while (cFile.peek() != EOF) {
                Customer c; c.readFromFile(cFile);
                if (c.getBookedMovie() != "") customers.push_back(c);
            }
        }
    }
};

void Admin::menu() {
    int choice;
    do {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Movie\n2. Delete Movie\n3. View Movies\n4. View Customers\n5. Logout\n";
        choice = getValidatedInt("Enter choice: ", 1);

        if (choice == 1) system->addMovie();
        else if (choice == 2) {
            system->viewMovies();
            cout << "Enter the exact movie name to delete: ";
            string name;
            getline(cin, name); // ✅ FIX: no cin.ignore() here
            system->deleteMovie(name);
        }
        else if (choice == 3) system->viewMovies();
        else if (choice == 4) system->viewCustomers();
    } while (choice != 5);
}

void CustomerUser::menu() {
    int choice;
    do {
        cout << "\n--- User Menu ---\n";
        cout << "1. View Movies\n2. Book Ticket\n3. Cancel Booking\n4. Logout\n";
        choice = getValidatedInt("Enter choice: ", 1);

        if (choice == 1) system->viewMovies();
        else if (choice == 2) system->bookTicket(username);
        else if (choice == 3) system->cancelBooking(username);
    } while (choice != 4);
}

int main() {
    MovieTicketBookingSystem system;
    system.loadData();
    AuthenticationSystem auth(&system);

    int mainChoice;
    do {
        cout << "\n=== Movie Ticket Booking System ===\n";
        cout << "1. Register\n2. Login\n3. Exit\n";
        mainChoice = getValidatedInt("Enter choice: ", 1);

        if (mainChoice == 1) {
            cout << "Register as:\n1. Admin\n2. Customer\n";
            int regChoice = getValidatedInt("Enter choice: ", 1);
            string uname, pass;
            cout << "Enter username: ";
            cin.ignore();
            getline(cin, uname);
            cout << "Enter password: ";
            getline(cin, pass);

            if (regChoice == 1) {
                auth.registerAdmin(uname, pass);
                cout << "Admin registered successfully!\n";
            } else if (regChoice == 2) {
                auth.registerCustomer(uname, pass);
                cout << "Customer registered successfully!\n";
            } else {
                cout << "Invalid choice.\n";
            }
        }
        else if (mainChoice == 2) {
            string uname, pass;
            cout << "Username: ";
            cin.ignore();
            getline(cin, uname);
            cout << "Password: ";
            getline(cin, pass);

            User* user = auth.login(uname, pass);
            if (user) {
                cout << "Login successful!\n";
                user->menu();
            } else {
                cout << "Invalid credentials.\n";
            }
        }
    } while (mainChoice != 3);

    system.saveData();
    cout << "Exiting system. Goodbye!\n";
    return 0;
}