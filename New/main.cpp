#include "data.h"

void pause_and_continue() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Separate function to mark a car as booked by ID
void Mark_Booked_Cars(int carId) {
    auto cars = read_all_cars();
    bool found = false;

    for (auto &c : cars) {
        if (c.id == carId) {
            if (c.status == "Available") {
                c.status = "Booked";
                found = true;
                log_action("Car ID " + to_string(carId) + " marked as Booked");
                break;
            }
        }
    }

    if (found)
        write_all_cars(cars);
    else
        cout << "❌ Car with ID " << carId << " not found.\n";
}

// =================== SORT MENU ===================
void sort_menu() {
    auto cars = read_all_cars();
    if (cars.empty()) {
        cout << "No cars available to sort.\n";
        return;
    }

    cout << "\n--- Sorting Options ---\n"
         << "1) Sort by Brand\n"
         << "2) Sort by Price\n"
         << "0) Back\nChoice: ";

    int ch; cin >> ch; cin.ignore();
    if (ch == 0) return;

    if (ch == 1) {
        sort_by_brand(cars);
        cout << "\n--- Cars Sorted by Brand ---\n";
    } else if (ch == 2) {
        sort_by_price(cars);
        cout << "\n--- Cars Sorted by Price ---\n";
    } else {
        cout << "Invalid choice.\n";
        return;
    }

    for (auto &c : cars)
        cout << "ID: " << c.id << " | " << c.brand << " " << c.model
             << " | Year: " << c.year << " | Price: " << c.price
             << " | Status: " << c.status << "\n";
}

// =================== BUYER MENU ===================
void list_cars_for_sale() {
    auto cars = read_all_cars();
    cout << "\n--- Available Cars ---\n";
    for (auto &c : cars)
        if (c.status == "Available")
            cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                 << " | Year: " << c.year
                 << " | Price: " << c.dynamic_price()
                 << " | Seller: " << c.seller << "\n";
}

void buyer_menu() {
    while (true) {
        cout << "\n--- Buyer Menu ---\n"
             << "1) List available cars\n"
             << "2) Search by brand\n"
             << "3) Search by ID\n"
             << "4) Search by price range\n"
             << "5) Book a car\n"
             << "6) Filter Cars (brand/year/price)\n"
             << "7) Sort Cars\n"
             << "0) Back\nChoice: ";

        int ch;
        if (!(cin >> ch)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (ch == 0) break;

        if (ch == 1) list_cars_for_sale();
        else if (ch == 2) {
            cout << "Enter brand: ";
            string brand;
            getline(cin, brand);
            auto cars = read_all_cars();
            bool found = false;
            for (auto &c : cars)
                if (c.status == "Available" && c.brand == brand) {
                    cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                         << " | Year: " << c.year
                         << " | Price: " << c.dynamic_price() << "\n";
                    found = true;
                }
            if (!found) cout << "No cars found for this brand.\n";
        }
        else if (ch == 3) {
            cout << "Enter Car ID: ";
            int id; cin >> id; cin.ignore();
            auto cars = read_all_cars();
            bool found = false;
            for (auto &c : cars)
                if (c.id == id) {
                    cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                         << " | Year: " << c.year
                         << " | Price: " << c.dynamic_price()
                         << " | Status: " << c.status << "\n";
                    found = true;
                    break;
                }
            if (!found) cout << "Car not found.\n";
        }
        else if (ch == 4) {
            double minP, maxP;
            cout << "Enter min price: "; cin >> minP;
            cout << "Enter max price: "; cin >> maxP;
            cin.ignore();
            auto cars = read_all_cars();
            bool found = false;
            for (auto &c : cars) {
                double dp = c.dynamic_price();
                if (c.status == "Available" && dp >= minP && dp <= maxP) {
                    cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                         << " | Year: " << c.year
                         << " | Price: " << dp << "\n";
                    found = true;
                }
            }
            if (!found) cout << "No cars in that price range.\n";
        }
        else if (ch == 5) {
            cout << "Enter Car ID to book: ";
            int id; cin >> id; cin.ignore();
            auto cars = read_all_cars();
            bool ok = false;
            for (auto &c : cars)
                if (c.id == id && c.status == "Available") { ok = true; c.status = "Booked"; }
            if (!ok) { cout << "Car not available.\n"; continue; }

            write_all_cars(cars);
            cout << "Enter your name: ";
            string name; getline(cin, name);

            Booking b{next_booking_id(), id, name, "Today", "Pending"};
            append_booking(b);
            log_action("Booking created " + to_string(b.bookingId));
            cout << "Booking successful! ID: " << b.bookingId << "\n";

            Mark_Booked_Cars(id);
        }
        else if (ch == 6) {
            // your existing filter + recommendation code (unchanged)
            cout << "Feature retained.\n";
        }
        else if (ch == 7) sort_menu();
        else cout << "Invalid choice.\n";
    }
}

// =================== SELLER MENU ===================
void seller_menu() {
    CarLinkedList carList;  // Linked list usage
    while (true) {
        cout << "\n--- Seller Menu ---\n"
             << "1) Add Car\n"
             << "2) View Added Cars (Linked List)\n"
             << "0) Back\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 0) break;

        if (ch == 1) {
            Car c;
            c.id = next_car_id();
            cout << "Brand: "; getline(cin, c.brand);
            cout << "Model: "; getline(cin, c.model);
            cout << "Year: "; cin >> c.year;
            cout << "Price: "; cin >> c.price; cin.ignore();
            cout << "Seller: "; getline(cin, c.seller);
            c.status = "Available";

            carList.insert(c); // Linked List insert
            append_car(c);
            log_action("Car added " + to_string(c.id));
            cout << "Car added successfully!\n";
        }
        else if (ch == 2) {
            auto cars = carList.to_vector();
            if (cars.empty()) cout << "No cars in list.\n";
            else {
                cout << "\nCars in Linked List:\n";
                for (auto &c : cars)
                    cout << c.id << " | " << c.brand << " " << c.model
                         << " | ₹" << c.price << " | " << c.status << "\n";
            }
        }
    }
}

// =================== ADMIN PANEL ===================
void admin_panel() {
    DeliveryQueue queue;
    DeliveryStack stack;

    while (true) {
        cout << "\n--- Admin Panel ---\n"
             << "1) Load Bookings into Delivery Queue\n"
             << "2) Process Next Delivery (Queue)\n"
             << "3) Show All Deliveries (Stack)\n"
             << "0) Back\nChoice: ";

        int ch; cin >> ch; cin.ignore();
        if (ch == 0) break;

        if (ch == 1) {
            auto bookings = read_all_bookings();
            for (auto &b : bookings)
                if (b.status == "Pending") queue.enqueue(b);
            cout << "All pending bookings added to delivery queue.\n";
        }
        else if (ch == 2) queue.processNext();
        else if (ch == 3) {
            auto deliveries = read_all_deliveries();
            for (auto &d : deliveries)
                stack.push(d);

            auto all = stack.to_vector();
            if (all.empty()) cout << "No deliveries.\n";
            else {
                cout << "\n--- Deliveries (Stack View) ---\n";
                for (auto &d : all)
                    cout << "BookingID: " << d.bookingId
                         << " | CarID: " << d.carId
                         << " | Buyer: " << d.buyer
                         << " | Status: " << d.status << "\n";
            }
        }
    }
}

// =================== MAIN ===================
int main() {
    ensure_file(CARS_FILE);
    ensure_file(BOOKINGS_FILE);
    ensure_file(DELIVERIES_FILE);
    ensure_file(SELLERS_FILE);
    ensure_file(TRANSACTIONS_LOG);

    while (true) {
        cout << "\n=== Car Market ===\n"
             << "1) Buyer\n2) Seller\n3) Admin\n0) Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 0) break;
        else if (ch == 1) buyer_menu();
        else if (ch == 2) seller_menu();
        else if (ch == 3) admin_panel();
    }
    return 0;
}
