#include "data.h"

int main() {
    cout << "Admin console\nPassword: ";
    string pwd; getline(cin, pwd);
    if (pwd != "admin123") { cout << "Wrong password.\n"; return 0; }

    while (true) {
        cout << "\n--- Admin Menu ---\n"
             << "1) View bookings\n"
             << "2) Process next booking\n"
             << "3) Mark top delivery delivered\n"
             << "4) View logs\n"
             << "0) Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();
        if (ch == 0) break;

        if (ch == 1) {
            auto bookings = read_all_bookings();
            for (auto &b : bookings)
                cout << b.bookingId << " | Car:" << b.carId
                     << " | Buyer:" << b.buyer
                     << " | Status:" << b.status << "\n";
        }
        else if (ch == 2) {
            auto bookings = read_all_bookings();
            if (bookings.empty()) { cout << "No bookings.\n"; continue; }
            Booking b = bookings.front();
            bookings.erase(bookings.begin());
            b.status = "OutForDelivery";
            write_all_bookings(bookings);
            append_delivery(b);
            log_action("Booking " + to_string(b.bookingId) + " -> OutForDelivery");
            cout << "Processed booking " << b.bookingId << "\n";
        }
        else if (ch == 3) {
            auto deliveries = read_all_deliveries();
            if (deliveries.empty()) { cout << "No deliveries.\n"; continue; }
            Booking b = deliveries.back();
            deliveries.pop_back();
            b.status = "Delivered";
            write_all_deliveries(deliveries);
            log_action("Delivered booking " + to_string(b.bookingId));
            cout << "Marked delivered.\n";
        }
        else if (ch == 4) {
            ifstream in(TRANSACTIONS_LOG);
            string line; while (getline(in, line)) cout << line << "\n";
        }
    }
    return 0;
}
