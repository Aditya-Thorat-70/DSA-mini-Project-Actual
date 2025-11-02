#ifndef DATA_H
#define DATA_H

#include <bits/stdc++.h>
using namespace std;

// ===================== FILE CONSTANTS ======================
static const string CARS_FILE = "cars.txt";
static const string BOOKINGS_FILE = "bookings.txt";
static const string DELIVERIES_FILE = "deliveries.txt";
static const string SELLERS_FILE = "sellers.txt";
static const string TRANSACTIONS_LOG = "transactions.log";

// ===================== FILE SETUP ======================
inline void ensure_file(const string &fname) {
    ifstream in(fname);
    if (!in.is_open()) {
        ofstream out(fname);
    }
}

// ===================== STRUCT DEFINITIONS ======================

// ---- Car Record ----
struct Car {
    int id;
    string brand;
    string model;
    int year;
    double price;
    string seller;
    string status;

    static bool from_line(const string &line, Car &c) {
        vector<string> parts;
        string cur;
        for (char ch : line) {
            if (ch == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(ch);
        }
        parts.push_back(cur);
        if (parts.size() != 7) return false;
        try {
            c.id = stoi(parts[0]);
            c.brand = parts[1];
            c.model = parts[2];
            c.year = stoi(parts[3]);
            c.price = stod(parts[4]);
            c.seller = parts[5];
            c.status = parts[6];
        } catch (...) { return false; }
        return true;
    }

    string to_line() const {
        ostringstream os;
        os << id << '|' << brand << '|' << model << '|' << year << '|'
           << fixed << setprecision(2) << price << '|' << seller << '|' << status;
        return os.str();
    }

    double dynamic_price() const {
        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        int current_year = 1900 + lt->tm_year;
        int age = current_year - year;
        double adjusted = price;
        if (age > 3) {
            int extra = age - 3;
            for (int i = 0; i < extra; ++i)
                adjusted *= 0.95;
        }
        return adjusted;
    }
};

// ---- Booking Record ----
struct Booking {
    int bookingId;
    int carId;
    string buyer;
    string date;
    string status;

    static bool from_line(const string &line, Booking &b) {
        vector<string> p;
        string cur;
        for (char ch : line) {
            if (ch == '|') { p.push_back(cur); cur.clear(); }
            else cur.push_back(ch);
        }
        p.push_back(cur);
        if (p.size() != 5) return false;
        try {
            b.bookingId = stoi(p[0]);
            b.carId = stoi(p[1]);
            b.buyer = p[2];
            b.date = p[3];
            b.status = p[4];
        } catch (...) { return false; }
        return true;
    }

    string to_line() const {
        ostringstream os;
        os << bookingId << '|' << carId << '|' << buyer << '|' << date << '|' << status;
        return os.str();
    }
};

// ===================== DSA STRUCTURES ======================

// ---- Linked List for Cars ----
struct CarNode {
    Car data;
    CarNode *next;
    CarNode(const Car &c) : data(c), next(nullptr) {}
};

class CarLinkedList {
    CarNode *head;
public:
    CarLinkedList() : head(nullptr) {}
    void insert(const Car &c) {
        CarNode *newNode = new CarNode(c);
        if (!head) head = newNode;
        else {
            CarNode *temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    vector<Car> to_vector() const {
        vector<Car> res;
        CarNode *temp = head;
        while (temp) {
            res.push_back(temp->data);
            temp = temp->next;
        }
        return res;
    }

    ~CarLinkedList() {
        CarNode *curr = head;
        while (curr) {
            CarNode *next = curr->next;
            delete curr;
            curr = next;
        }
    }
};

// Forward declarations
inline void append_delivery(const Booking& b);
inline void log_action(const string &s);

// ---- Stack for Deliveries ----
struct DeliveryNode {
    Booking data;
    DeliveryNode *next;
    DeliveryNode(const Booking &b) : data(b), next(nullptr) {}
};

class DeliveryStack {
    DeliveryNode *topNode;
public:
    DeliveryStack() : topNode(nullptr) {}

    void push(const Booking &b) {
        DeliveryNode *node = new DeliveryNode(b);
        node->next = topNode;
        topNode = node;
    }

    bool pop(Booking &b) {
        if (!topNode) return false;
        b = topNode->data;
        DeliveryNode *temp = topNode;
        topNode = topNode->next;
        delete temp;
        return true;
    }

    bool isEmpty() const { return topNode == nullptr; }

    vector<Booking> to_vector() const {
        vector<Booking> res;
        DeliveryNode *temp = topNode;
        while (temp) {
            res.push_back(temp->data);
            temp = temp->next;
        }
        reverse(res.begin(), res.end());
        return res;
    }

    ~DeliveryStack() {
        Booking temp;
        while (!isEmpty()) pop(temp);
    }
};

// ---- Queue for Deliveries ----
class DeliveryQueue {
    queue<Booking> q;
public:
    void enqueue(const Booking &b) {
        q.push(b);
        cout << "Booking added to delivery queue: " << b.bookingId << endl;
    }

    void processNext() {
        if (q.empty()) {
            cout << "No pending deliveries.\n";
            return;
        }
        Booking b = q.front(); q.pop();
        b.status = "Delivered";
        append_delivery(b);
        log_action("Delivered booking ID " + to_string(b.bookingId));
        cout << "Delivered booking: " << b.bookingId << endl;
    }

    bool empty() const { return q.empty(); }
};

// ---- Sorting Utilities ----
inline void sort_by_brand(vector<Car>& cars) {
    sort(cars.begin(), cars.end(), [](const Car &a, const Car &b) {
        return a.brand < b.brand;
    });
}

inline void sort_by_price(vector<Car>& cars) {
    sort(cars.begin(), cars.end(), [](const Car &a, const Car &b) {
        return a.price < b.price;
    });
}

// ===================== FILE HANDLING ======================

inline vector<Car> read_all_cars() {
    ensure_file(CARS_FILE);
    ifstream in(CARS_FILE);
    vector<Car> res;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Car c;
        if (Car::from_line(line, c)) res.push_back(c);
    }
    return res;
}

inline void write_all_cars(const vector<Car>& cars) {
    ofstream out(CARS_FILE, ios::trunc);
    for (const auto &c : cars) out << c.to_line() << '\n';
}

inline void append_car(const Car& c) {
    ofstream out(CARS_FILE, ios::app);
    out << c.to_line() << '\n';
}

inline vector<Booking> read_all_bookings() {
    ensure_file(BOOKINGS_FILE);
    ifstream in(BOOKINGS_FILE);
    vector<Booking> res;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Booking b;
        if (Booking::from_line(line, b)) res.push_back(b);
    }
    return res;
}

inline void write_all_bookings(const vector<Booking>& v) {
    ofstream out(BOOKINGS_FILE, ios::trunc);
    for (auto &b : v) out << b.to_line() << '\n';
}

inline void append_booking(const Booking& b) {
    ofstream out(BOOKINGS_FILE, ios::app);
    out << b.to_line() << '\n';
}

inline vector<Booking> read_all_deliveries() {
    ensure_file(DELIVERIES_FILE);
    ifstream in(DELIVERIES_FILE);
    vector<Booking> res;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Booking b;
        if (Booking::from_line(line, b)) res.push_back(b);
    }
    return res;
}

inline void write_all_deliveries(const vector<Booking>& v) {
    ofstream out(DELIVERIES_FILE, ios::trunc);
    for (auto &b : v) out << b.to_line() << '\n';
}

inline void append_delivery(const Booking& b) {
    ofstream out(DELIVERIES_FILE, ios::app);
    out << b.to_line() << '\n';
}

// ===================== LOGGING ======================
inline void log_action(const string &s) {
    ofstream out(TRANSACTIONS_LOG, ios::app);
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
    out << "[" << buf << "] " << s << '\n';
}

// ===================== ID GENERATORS ======================
inline int next_car_id() {
    auto cars = read_all_cars();
    int mx = 100;
    for (auto &c : cars) mx = max(mx, c.id);
    return mx + 1;
}

inline int next_booking_id() {
    auto bookings = read_all_bookings();
    int mx = 1000;
    for (auto &b : bookings) mx = max(mx, b.bookingId);
    auto del = read_all_deliveries();
    for (auto &b : del) mx = max(mx, b.bookingId);
    return mx + 1;
}

// ===================== EXTRA FEATURE ======================
inline bool mark_booked_car(int carId) {
    vector<Car> cars = read_all_cars();
    bool found = false;
    for (auto &c : cars) {
        if (c.id == carId) {
            if (c.status == "Available") {
                c.status = "Booked";
                found = true;
                log_action("Car ID " + to_string(carId) + " marked as Booked");
            } else {
                cout << "Car ID " << carId << " is already " << c.status << ".\n";
                return false;
            }
            break;
        }
    }
    if (found) {
        write_all_cars(cars);
        cout << "✅ Car ID " << carId << " has been marked as 'Booked'.\n";
        return true;
    } else {
        cout << "❌ Car with ID " << carId << " not found.\n";
        return false;
    }
}

#endif // DATA_H
