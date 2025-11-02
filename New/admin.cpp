#include "data.h"

void admin_update_car() {
    int id;
    cout << "\nEnter Car ID to update status: ";
    cin >> id;
    cin.ignore();

    auto cars = read_all_cars();
    for (auto &c : cars) {
        if (c.id == id) {
            cout << "\n----- Car Details -----\n";
            cout << "ID: " << c.id << "\nBrand: " << c.brand
                 << "\nModel: " << c.model
                 << "\nYear: " << c.year
                 << "\nPrice: " << c.price
                 << "\nCurrent Status: " << c.status << "\n";

            cout << "\nEnter new status (Available / Booked / Sold): ";
            string newStatus;
            getline(cin, newStatus);
            admin_update_car_status(id, newStatus);
            return;
        }
    }
    cout << "❌ Car ID not found.\n";
}

void admin_panel() {
    while (true) {
        cout << "\n========= ADMIN PANEL =========\n"
             << "1) View All Cars\n"
             << "2) Update Car Status\n"
             << "3) View Bookings\n"
             << "4) Show Delivery Records\n"
             << "5) View Logs\n"
             << "0) Back\n"
             << "Choice: ";

        int ch;
        cin >> ch;
        cin.ignore();

        switch (ch) {
        case 0: return;

        case 1: {
            auto cars = read_all_cars();
            cout << left << setw(8) << "ID"
                 << setw(12) << "Brand"
                 << setw(12) << "Model"
                 << setw(8) << "Year"
                 << setw(12) << "Price"
                 << setw(12) << "Status" << "\n";
            cout << string(60, '-') << "\n";
            for (auto &c : cars)
                cout << left << setw(8) << c.id
                     << setw(12) << c.brand
                     << setw(12) << c.model
                     << setw(8) << c.year
                     << setw(12) << c.price
                     << setw(12) << c.status << "\n";
            break;
        }

        case 2: admin_update_car(); break;

        case 3: {
            auto bookings = read_all_bookings();
            for (auto &b : bookings)
                cout << b.bookingId << " | Car:" << b.carId
                     << " | Buyer:" << b.buyer
                     << " | Status:" << b.status << "\n";
            break;
        }

        case 4: {
            auto deliveries = read_all_deliveries();
            for (auto &d : deliveries)
                cout << d.bookingId << " | Car:" << d.carId
                     << " | Buyer:" << d.buyer
                     << " | Status:" << d.status << "\n";
            break;
        }

        case 5: {
            ifstream in(TRANSACTIONS_LOG);
            string line;
            while (getline(in, line)) cout << line << "\n";
            break;
        }

        default:
            cout << "Invalid choice.\n";
        }
    }
}
