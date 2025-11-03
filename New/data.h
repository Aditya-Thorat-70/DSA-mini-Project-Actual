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

// ===================== FILE CHECK ======================
inline void ensure_file(const string &fname) {
    ifstream in(fname);
    if (!in.is_open()) {
        ofstream out(fname);
    }
}

// ===================== STRUCT DEFINITIONS ======================
struct Car {
    int id;
    string brand, model;
    int year;
    double price;
    string seller, status;

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
            for (int i = 0; i < extra; ++i) adjusted *= 0.95;
        }
        return adjusted;
    }
};

struct Booking {
    int bookingId, carId;
    string buyer, date, status;

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

// ===================== LINKED LIST ======================
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
        CarNode *n = new CarNode(c);
        if (!head) head = n;
        else {
            CarNode *temp = head;
            while (temp->next) temp = temp->next;
            temp->next = n;
        }
    }
    vector<Car> to_vector() const {
        vector<Car> res;
        CarNode *t = head;
        while (t) {
            res.push_back(t->data);
            t = t->next;
        }
        return res;
    }
    ~CarLinkedList() {
        CarNode *c = head;
        while (c) {
            CarNode *n = c->next;
            delete c;
            c = n;
        }
    }
};

// Forward declarations
inline void append_delivery(const Booking &b);
inline void log_action(const string &s);
inline bool admin_update_car_status(int id, const string &status);

// ===================== STACK ======================
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
        DeliveryNode *n = new DeliveryNode(b);
        n->next = topNode;
        topNode = n;
    }
    bool pop(Booking &b) {
        if (!topNode) return false;
        b = topNode->data;
        DeliveryNode *t = topNode;
        topNode = topNode->next;
        delete t;
        return true;
    }
    vector<Booking> to_vector() const {
        vector<Booking> res;
        DeliveryNode *t = topNode;
        while (t) { res.push_back(t->data); t = t->next; }
        reverse(res.begin(), res.end());
        return res;
    }
    bool isEmpty() const { return topNode == nullptr; }
};

// ===================== QUEUE ======================
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

        // ✅ Update booking file
        vector<Booking> allBookings = read_all_bookings();
        for (auto &bk : allBookings)
            if (bk.bookingId == b.bookingId) {
                bk.status = "Delivered";
                break;
            }
        write_all_bookings(allBookings);

        // ✅ Update car status to "Sold"
        admin_update_car_status(b.carId, "Sold");

        log_action("Delivered booking ID " + to_string(b.bookingId));
        cout << "✅ Delivered booking: " << b.bookingId << " (Car ID " << b.carId << ")\n";
    }

    bool empty() const { return q.empty(); }
};

// ===================== FILE HANDLING ======================
inline vector<Car> read_all_cars() {
    ensure_file(CARS_FILE);
    ifstream in(CARS_FILE);
    vector<Car> v; string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Car c;
        if (Car::from_line(line, c)) v.push_back(c);
    }
    return v;
}

inline void write_all_cars(const vector<Car>& v) {
    ofstream out(CARS_FILE, ios::trunc);
    for (auto &c : v) out << c.to_line() << '\n';
}

inline void append_car(const Car &c) {
    ofstream out(CARS_FILE, ios::app);
    out << c.to_line() << '\n';
}

inline vector<Booking> read_all_bookings() {
    ensure_file(BOOKINGS_FILE);
    ifstream in(BOOKINGS_FILE);
    vector<Booking> v; string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Booking b;
        if (Booking::from_line(line, b)) v.push_back(b);
    }
    return v;
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
    vector<Booking> v; string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Booking b;
        if (Booking::from_line(line, b)) v.push_back(b);
    }
    return v;
}

inline void write_all_deliveries(const vector<Booking>& v) {
    ofstream out(DELIVERIES_FILE, ios::trunc);
    for (auto &b : v) out << b.to_line() << '\n';
}

inline void append_delivery(const Booking& b) {
    ofstream out(DELIVERIES_FILE, ios::app);
    out << b.to_line() << '\n';
}

inline void log_action(const string &s) {
    ofstream out(TRANSACTIONS_LOG, ios::app);
    time_t t = time(nullptr);
    out << "[" << ctime(&t);
    out.seekp(-1, ios::cur);
    out << "] " << s << '\n';
}

// ===================== UTILITIES ======================
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
    return mx + 1;
}

inline bool admin_update_car_status(int id, const string &status) {
    auto cars = read_all_cars();
    for (auto &c : cars)
        if (c.id == id) {
            string old = c.status;
            c.status = status;
            write_all_cars(cars);
            log_action("Admin updated car ID " + to_string(id) + " status: " + old + " -> " + status);
            cout << " Car " << id << " updated from " << old << " to " << status << endl;
            return true;
        }
    cout << " Car ID not found.\n";
    return false;
}

#endif
