#include "data.h"

void pause_and_continue() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ✅ Properly writes back after marking booked
void Mark_Booked_Cars(int carId) {
    auto cars = read_all_cars();
    bool found = false;
    for (auto &c : cars) {
        if (c.id == carId && c.status == "Available") {
            c.status = "Booked";
            found = true;
            log_action("Car ID " + to_string(carId) + " marked as Booked");
            break;
        }
    }
    // if (found) write_all_cars(cars);
    // else cout << "❌ Car not found or already booked.\n";
}

// ✅ Sorting Menu
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

    switch (ch) {
        case 1:
            sort_by_brand(cars);
            break;
        case 2:
            sort_by_price(cars);
            break;
        case 0:
            return;
        default:
            cout << "Invalid choice.\n";
            return;
    }

    cout << "\n--- Sorted Cars ---\n";
    for (auto &c : cars)
        cout << "ID: " << c.id << " | " << c.brand << " " << c.model
             << " | Year: " << c.year << " | Price: " << c.price
             << " | Status: " << c.status << "\n";
}

// ✅ Buyer Menu
void buyer_menu() {
    while (true) {
        cout << "\n--- Buyer Menu ---\n"
             << "1) List available cars\n"
             << "2) Search by brand\n"
             << "3) Search by ID\n"
             << "4) Search by price range\n"
             << "5) Book a car\n"
             << "6) Filter + Recommendations\n"
             << "7) Sort Cars\n"
             << "0) Back\nChoice: ";

        int ch; cin >> ch; cin.ignore();

        switch (ch) {
            case 0:
                return;

            case 1: {
                auto cars = read_all_cars();
                for (auto &c : cars)
                    if (c.status == "Available")
                        cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                             << " | Year: " << c.year
                             << " | Price" << c.price << "\n";
                break;
            }

            case 2: {
                cout << "Enter brand: ";
                string brand; getline(cin, brand);
                auto cars = read_all_cars();
                bool found = false;
                for (auto &c : cars)
                    if (c.brand == brand && c.status == "Available") {
                        cout << c.id << " | " << c.brand << " " << c.model << " | ₹" << c.price << "\n";
                        found = true;
                    }
                if (!found) cout << "No cars found for this brand.\n";
                break;
            }

            case 3: {
                cout << "Enter Car ID: ";
                int id; cin >> id; cin.ignore();
                auto cars = read_all_cars();
                bool found = false;
                for (auto &c : cars)
                    if (c.id == id) {
                        cout << "Car: " << c.brand << " " << c.model << " | ₹" << c.price
                             << " | Status: " << c.status << "\n";
                        found = true;
                    }
                if (!found) cout << "Car not found.\n";
                break;
            }

            case 4: {
                double minP, maxP;
                cout << "Enter min price: "; cin >> minP;
                cout << "Enter max price: "; cin >> maxP;
                cin.ignore();
                auto cars = read_all_cars();
                for (auto &c : cars)
                    if (c.status == "Available" && c.price >= minP && c.price <= maxP)
                        cout << c.id << " | " << c.brand << " " << c.model << " | ₹" << c.price << "\n";
                break;
            }

            case 5: {
                cout << "Enter Car ID to book: ";
                int id; cin >> id; cin.ignore();

                auto cars = read_all_cars();
                bool ok = false;
                for (auto &c : cars)
                    if (c.id == id && c.status == "Available") {
                        c.status = "Booked"; ok = true;
                    }

                if (!ok) { cout << "Car not available.\n"; break; }

                write_all_cars(cars);
                cout << "Enter your name: ";
                string name; getline(cin, name);

                Booking b{next_booking_id(), id, name, "Today", "Pending"};
                append_booking(b);
                log_action("Booking created ID " + to_string(b.bookingId));
                Mark_Booked_Cars(id);
                cout << " Booking successful!\n";
                break;
            }

            case 6: {
                string brand;
                double minPrice = 0, maxPrice = 0;
                cout << "Brand: "; getline(cin, brand);
                cout << "Min Price: "; cin >> minPrice;
                cout << "Max Price: "; cin >> maxPrice;
                cin.ignore();

                auto cars = read_all_cars();
                vector<Car> filtered;
                for (auto &c : cars)
                    if (c.status == "Available" &&
                        (brand.empty() || c.brand == brand) &&
                        (minPrice == 0 || c.price >= minPrice) &&
                        (maxPrice == 0 || c.price <= maxPrice))
                        filtered.push_back(c);

                cout << "\n--- Filtered Cars ---\n";
                for (auto &c : filtered)
                    cout << c.id << " | " << c.brand << " " << c.model << " | ₹" << c.price << "\n";

                unordered_map<string, vector<string>> similarBrands = {
                    {"Hyundai", {"Kia"}}, {"Kia", {"Hyundai"}},
                    {"Suzuki", {"Toyota"}}, {"Toyota", {"Suzuki"}},
                    {"Tata", {"Mahindra"}}, {"Mahindra", {"Tata"}},
                    {"Honda", {"Nissan"}}, {"Nissan", {"Honda"}}
                };

                double avgPrice = (minPrice + maxPrice) / 2.0;
                double lower = avgPrice * 0.95, upper = avgPrice * 1.05;
                vector<string> related = similarBrands[brand];
                vector<Car> recommended;

                for (auto &c : cars) {
                    if (find(related.begin(), related.end(), c.brand) != related.end() ||
                        (c.price >= lower && c.price <= upper))
                        if (c.status == "Available") recommended.push_back(c);
                }

                if (!recommended.empty()) {
                    cout << "\n--------- Recommended For You ---------\n";
                    for (auto &c : recommended)
                        cout << c.brand << " " << c.model << " | ₹" << c.price << "\n";
                } else cout << "(No recommendations found.)\n";
                break;
            }

            case 7:
                sort_menu();
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
}

// ✅ Seller menu (Linked List)
void seller_menu() {
    CarLinkedList list;
    while (true) {
        cout << "\n--- Seller Menu ---\n"
             << "1) Add Car\n2) View Linked List Cars\n0) Back\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        switch (ch) {
            case 0: return;
            case 1: {
                Car c;
                c.id = next_car_id();
                cout << "Brand: "; getline(cin, c.brand);
                cout << "Model: "; getline(cin, c.model);
                cout << "Year: "; cin >> c.year;
                cout << "Price: "; cin >> c.price; cin.ignore();
                cout << "Seller: "; getline(cin, c.seller);
                c.status = "Available";

                list.insert(c);
                append_car(c);
                log_action("Car added " + to_string(c.id));
                cout << "Car added successfully!\n";
                break;
            }
            case 2: {
                auto cars = list.to_vector();
                if (cars.empty()) cout << "List is empty.\n";
                else for (auto &c : cars)
                    cout << c.id << " | " << c.brand << " " << c.model << " | ₹" << c.price << "\n";
                break;
            }
            default:
                cout << "Invalid input.\n";
        }
    }
}

// ✅ Admin Panel (Queue + Stack)
void admin_panel() {
    DeliveryQueue queue;
    DeliveryStack stack;

    while (true) {
        cout << "\n--- Admin Panel ---\n"
             << "1) Load Bookings to Queue\n"
             << "2) Process Next Delivery\n"
             << "3) Show All Deliveries (Stack)\n"
             << "0) Back\nChoice: ";
        int ch; cin >> ch; cin.ignore();

        switch (ch) {
            case 0: return;
            case 1: {
                auto bookings = read_all_bookings();
                for (auto &b : bookings)
                    if (b.status == "Pending") queue.enqueue(b);
                cout << "Pending bookings loaded to queue.\n";
                break;
            }
            case 2: queue.processNext(); break;
            case 3: {
                auto deliveries = read_all_deliveries();
                for (auto &d : deliveries) stack.push(d);
                auto all = stack.to_vector();
                cout << "\n--- Deliveries (Stack View) ---\n";
                for (auto &d : all)
                    cout << "BookingID: " << d.bookingId << " | CarID: " << d.carId
                         << " | Buyer: " << d.buyer << " | Status: " << d.status << "\n";
                break;
            }
            default: cout << "Invalid option.\n";
        }
    }
}

// ✅ Main function using switch-case
int main() {
    ensure_file(CARS_FILE);
    ensure_file(BOOKINGS_FILE);
    ensure_file(DELIVERIES_FILE);
    ensure_file(SELLERS_FILE);
    ensure_file(TRANSACTIONS_LOG);

    bool running = true;
    while (running) {
        cout << "\n=== Car Market ===\n"
             << "1) Buyer\n2) Seller\n3) Admin\n0) Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();

        switch (ch) {
            case 0:
                running = false;
                cout << "Exiting program. Goodbye!\n";
                break;
            case 1:
                buyer_menu();
                break;
            case 2:
                seller_menu();
                break;
            case 3:
                admin_panel();
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}