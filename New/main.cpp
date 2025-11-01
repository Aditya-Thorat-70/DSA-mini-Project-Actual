#include "data.h"

void pause_and_continue() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Mark_Booked_Cars(int carId){
    auto cars = read_all_cars();
    //vector<Car> cars = read_all_cars();
    bool found = false;

    for (auto &c : cars) {
        if (c.id == carId) {
            if (c.status == "Available") {
                c.status = "Booked";
                found = true;
                log_action("Car ID " + to_string(carId) + " marked as Booked");
            }
        }
    }

}

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

        // 1) List all available
        if (ch == 1) {
            list_cars_for_sale();
        }

        // 2) Search by brand
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

        // 3) Search by ID
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

        // 4) Search by price range
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

        // 5) Book a car
        else if (ch == 5) {
            cout << "Enter Car ID to book: ";
            int id; cin >> id; cin.ignore();
            auto cars = read_all_cars();
            bool ok = false;
            bool booked = false;
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

            //Mark that car as booked by the searching through id
            Mark_Booked_Cars(id);
        }

        // 6) Filter Cars (brand/year/price)
        else if (ch == 6) {
            string brand;
            int minYear = 0, maxYear = 0;
            double minPrice = 0, maxPrice = 0;

            cout << "Filter by Brand (leave blank for any): ";
            getline(cin, brand);
            cout << "Filter by Min Year (0 for any): ";
            cin >> minYear;
            cout << "Filter by Max Year (0 for any): ";
            cin >> maxYear;
            cout << "Filter by Min Price (0 for any): ";
            cin >> minPrice;
            cout << "Filter by Max Price (0 for any): ";
            cin >> maxPrice;
            cin.ignore();

            auto cars = read_all_cars();
            vector<Car> results;
            for (auto &c : cars) {
                if (c.status != "Available") continue;
                if (!brand.empty() && c.brand != brand) continue;
                if (minYear != 0 && c.year < minYear) continue;
                if (maxYear != 0 && c.year > maxYear) continue;
                double dp = c.price; // use original listed price for filtering
if (minPrice != 0 && dp < minPrice) continue;
if (maxPrice != 0 && dp > maxPrice) continue;

                results.push_back(c);
            }

            cout << "\n--- Filtered Cars ---\n";
            if (results.empty()) cout << "No cars match your filters.\n";
            else {
                for (auto &c : results)
                    cout << "ID: " << c.id << " | " << c.brand << " " << c.model
                         << " | Year: " << c.year
                         << " | Price: " << c.price
                         << " | Seller: " << c.seller << "\n";

                  //cout<<"/ ---------------- RECOMMENDED CARS ----------------" <<endl; 
    unordered_map<string, vector<string>> similarBrands = {
        {"Hyundai", {"Kia"}},
        {"Kia", {"Hyundai"}},
        {"Suzuki", {"Toyota"}},
        {"Toyota", {"Suzuki"}},
        {"Tata", {"Mahindra"}},
        {"Mahindra", {"Tata"}},
        {"Honda", {"Nissan"}},
        {"Nissan", {"Honda"}}
    };

    double avgPrice = (minPrice + maxPrice) / 2.0;
    double lowerLimit = avgPrice * 0.95;
    double upperLimit = avgPrice * 1.05;

    vector<string> related = similarBrands[brand];
    vector<Car> recommended;

    for (auto &c : cars) {
        //if (!c.available) continue;

        bool priceMatch = (c.price >= lowerLimit && c.price <= upperLimit);
        bool brandMatch = (find(related.begin(), related.end(), c.brand) != related.end());

        // Exclude cars already shown in main filter
        bool alreadyListed = false;
        for (auto &f : results) {
            if (f.id == c.id) {
                alreadyListed = true;
                break;
            }
        }

        // Recommendation condition: similar brand OR close price range
        if (!alreadyListed && (priceMatch || brandMatch)) {
            recommended.push_back(c);
        }
    }

    if (!recommended.empty()) {
        cout << "\n--------- Recommended For You ---------\n";
        for (auto &c : recommended) {
            cout << "Brand: " << c.brand << " | Model: " << c.model
                 << " | Year: " << c.year << " | Price: ₹" << c.price << endl;
        }
    } else {
        cout << "\n(No additional recommendations found.)\n";
    }     
            }
        }

        else {
            cout << "Invalid choice.\n";
        }
    }
}


void seller_menu() {
    while (true) {
        cout << "\n--- Seller Menu ---\n"
             << "1) Add Car\n0) Back\nChoice: ";
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
            append_car(c);
            log_action("Car added " + to_string(c.id));
            cout << "Car added successfully!\n";
        }
    }
}

int main() {
    ensure_file(CARS_FILE);
    ensure_file(BOOKINGS_FILE);
    ensure_file(DELIVERIES_FILE);
    ensure_file(SELLERS_FILE);
    ensure_file(TRANSACTIONS_LOG);

    while (true) {
        cout << "\n=== Car Market ===\n"
             << "1) Buyer\n2) Seller\n0) Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 0) break;
        else if (ch == 1) buyer_menu();
        else if (ch == 2) seller_menu();
    }
    return 0;
}
