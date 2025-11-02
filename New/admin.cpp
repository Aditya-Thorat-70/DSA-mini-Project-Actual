#include "data.h"
#include <iomanip>
using namespace std;

void admin_panel() {
    while (true) {
        cout << "\n=== Admin Panel ===\n"
             << "1) View All Cars\n"
             << "2) Update Car Status\n"
             << "3) View Bookings\n"
             << "4) View Deliveries\n"
             << "5) View Logs\n"
             << "0) Back\nChoice: ";
        int ch; cin >> ch; cin.ignore();

        switch (ch) {
            case 0: return;

            case 1: {
                auto cars = read_all_cars();
                cout << left << setw(6) << "ID" << setw(12) << "Brand"
                     << setw(15) << "Model" << setw(8) << "Year"
                     << setw(12) << "Price" << setw(15) << "Seller"
                     << setw(12) << "Status" << "\n";
                cout << string(80, '-') << "\n";
                for (auto &c : cars)
                    cout << left << setw(6) << c.id
                         << setw(12) << c.brand
                         << setw(15) << c.model
                         << setw(8) << c.year
                         << setw(12) << c.price
                         << setw(15) << c.seller
                         << setw(12) << c.status << "\n";
                break;
            }

            case 2: {
                int id; string status;
                cout << "Enter Car ID: "; cin >> id; cin.ignore();
                cout << "Enter new status (Available/Booked/Sold): ";
                getline(cin, status);
                admin_update_car_status(id, status);
                break;
            }

            case 3: {
                auto b = read_all_bookings();
                cout << left << setw(10) << "BookingID"
                     << setw(8) << "CarID"
                     << setw(20) << "Buyer"
                     << setw(15) << "Status" << "\n";
                cout << string(55, '-') << "\n";
                for (auto &x : b)
                    cout << left << setw(10) << x.bookingId
                         << setw(8) << x.carId
                         << setw(20) << x.buyer
                         << setw(15) << x.status << "\n";
                break;
            }

            case 4: {
                auto d = read_all_deliveries();
                cout << left << setw(10) << "BookingID"
                     << setw(8) << "CarID"
                     << setw(20) << "Buyer"
                     << setw(15) << "Status" << "\n";
                cout << string(55, '-') << "\n";
                for (auto &x : d)
                    cout << left << setw(10) << x.bookingId
                         << setw(8) << x.carId
                         << setw(20) << x.buyer
                         << setw(15) << x.status << "\n";
                break;
            }

            case 5: {
                ifstream in(TRANSACTIONS_LOG);
                string line; while (getline(in, line)) cout << line << "\n";
                break;
            }

            default: cout << "Invalid choice.\n";
        }
    }
}
int main(){
    cout << "Admin console\nPassword: ";
    string pwd; getline(cin, pwd);
    if (pwd != "admin123") { cout << "Wrong password.\n"; return 0; }
    admin_panel();
}
