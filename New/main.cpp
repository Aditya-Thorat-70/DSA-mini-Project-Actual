#include "data.h"
#include <iomanip>
using namespace std;

// ---------- Utility Pause ----------
void pause_and_continue() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------- Pretty Print Car Table ----------
void print_car_table(const vector<Car>& cars) {
    cout << left << setw(6) << "ID"
         << setw(12) << "Brand"
         << setw(15) << "Model"
         << setw(8) << "Year"
         << setw(12) << "Price"
         << setw(25) << "Seller"
         << setw(12) << "Status" << "\n";
    cout << string(80, '-') << "\n";

    for (auto &c : cars) {
        cout << left << setw(6) << c.id
             << setw(12) << c.brand
             << setw(15) << c.model
             << setw(8) << c.year
             << setw(12) << fixed << setprecision(2) << c.price
             << setw(25) << c.seller
             << setw(12) << c.status << "\n";
    }
}
// ---------- Mark Booked Car ----------
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

    if (found) write_all_cars(cars);
    //else cout << " Car with ID " << carId << " not found.\n";
}

//Filter + recommend cars
void Filter_and_recommended(){
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
                print_car_table(results);

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
    double lowerLimit = minPrice * 0.95;
    double upperLimit = maxPrice * 1.05;

    vector<string> related = similarBrands[brand];
    vector<Car> recommended;

    for (auto &c : cars) {
        if (c.status != "Available" ) continue;

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
        if (brandMatch && (!alreadyListed && priceMatch)) {
            recommended.push_back(c);
        }
    }

    if (!recommended.empty()) {
        cout << "\n--------- Recommended For You ---------\n";
        print_car_table(recommended);
    } else {
        cout << "\n(No additional recommendations found.)\n";
    }
            }
        }




// ---------- List Cars ----------
void list_cars_for_sale() {
    auto cars = read_all_cars();
    vector<Car> available;
    for (auto &c : cars)
        if (c.status == "Available") available.push_back(c);

    cout << "\n--- Available Cars ---\n";
    if (available.empty()) cout << "No available cars.\n";
    else print_car_table(available);
}

// ---------- Buyer Menu ----------
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

        switch (ch) {
            case 1: {
                list_cars_for_sale();
                break;
            }
            case 2: {
                cout << "Enter brand: ";
                string brand;
                getline(cin, brand);
                auto cars = read_all_cars();
                vector<Car> results;
                for (auto &c : cars)
                    if (c.status == "Available" && c.brand == brand)
                        results.push_back(c);
                if (results.empty()) cout << "No cars found for this brand.\n";
                else print_car_table(results);
                break;
            }
            case 3: {
                cout << "Enter Car ID: ";
                int id; cin >> id; cin.ignore();
                auto cars = read_all_cars();
                vector<Car> result;
                for (auto &c : cars)
                    if (c.id == id) result.push_back(c);
                if (result.empty()) cout << "Car not found.\n";
                else print_car_table(result);
                break;
            }
            case 4: {
                double minP, maxP;
                cout << "Enter min price: "; cin >> minP;
                cout << "Enter max price: "; cin >> maxP;
                cin.ignore();
                auto cars = read_all_cars();
                vector<Car> results;
                for (auto &c : cars) {
                    double dp = c.price;
                    if (c.status == "Available" && dp >= minP && dp <= maxP)
                        results.push_back(c);
                }
                if (results.empty()) cout << "No cars in that price range.\n";
                else print_car_table(results);
                break;
            }
            case 5: {
                cout << "Enter Car ID to book: ";
                int id; cin >> id; cin.ignore();
                auto cars = read_all_cars();
                bool ok = false;
                for (auto &c : cars)
                    if (c.id == id && c.status == "Available") { ok = true; c.status = "Booked"; }

                if (!ok) { cout << "Car not available.\n"; break; }

                write_all_cars(cars);
                cout << "Enter your name: ";
                string name; getline(cin, name);

                Booking b{next_booking_id(), id, name, "Today", "Pending"};
                append_booking(b);
                log_action("Booking created " + to_string(b.bookingId));
                cout << "Booking successful! ID: " << b.bookingId << "\n";
                Mark_Booked_Cars(id);
                break;
            }
            case 6: {
               Filter_and_recommended();
                break;
            }
            default:
                cout << "Invalid choice.\n";
        }
    }
}

// ---------- Seller Menu ----------
void seller_menu() {
    CarLinkedList carList;
    while (true) {
        cout << "\n--- Seller Menu ---\n"
             << "1) Add Car\n"
             << "2) View Added Cars (Linked List)\n"
             << "0) Back\nChoice: ";
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
                carList.insert(c);
                append_car(c);
                log_action("Car added " + to_string(c.id));
                cout << "Car added successfully!\n";
                break;
            }
            case 2: {
                auto cars = carList.to_vector();
                if (cars.empty()) cout << "No cars in list.\n";
                else print_car_table(cars);
                break;
            }
            default:
                cout << "Invalid choice.\n";
        }
    }
}




// ---------- MAIN ----------
int main() {
    ensure_file(CARS_FILE);
    ensure_file(BOOKINGS_FILE);
    ensure_file(DELIVERIES_FILE);
    ensure_file(SELLERS_FILE);
    ensure_file(TRANSACTIONS_LOG);

    bool running = true;
    while (running) {
        cout << "\n=== Car Market ===\n"
             << "1) Buyer\n2) Seller\n0) Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();

        switch (ch) {
            case 1: buyer_menu(); break;
            case 2: seller_menu(); break;
            //case 3: admin_panel(); break;
            case 0: running = false; break;
            default: cout << "Invalid choice.\n";
        }
    }
    cout << "\nThank you for using Car Market System!\n";
    return 0;
}
