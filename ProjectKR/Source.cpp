#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set> 
#include <regex> // Для використання регулярних виразів
#include <sstream> 

using namespace std;

// Клас, який представляє особу (абстрактний клас)
class Person {
public:
    string name;
    string phoneNumber;

    // Конструктор Person
    Person(const string& n, const string& phone) : name(n), phoneNumber(phone) {}

    // Чистий віртуальний метод для відображення інформації про особу
    virtual void displayInfo() const = 0;
};

// Клас для представлення квитка
class Ticket {
public:
    int id;
    string shipName; // Змінна для назви корабля
    string departurePort;
    string destinationPort;
    string date;
    int cabinClass;
    double price;

    Ticket(int i, const string& depart, const string& dest, const string& d, int cabin, double p, const string& ship) {
        id = i;
        shipName = ship;
        departurePort = depart;
        destinationPort = dest;
        date = d;
        cabinClass = cabin;
        price = p;
    }
};

// Клас для представлення пасажира, який наслідує особу
class Passenger : public Person {
public:
    int id;
    string address;
    vector<Ticket> tickets;

    Passenger(const string& n, const string& addr, const string& phone, int i) : Person(n, phone), id(i), address(addr) {}

    // Метод для відображення інформації про пасажира
    void displayInfo() const override {
        cout << endl;
        cout << "Passenger information:" << endl;
        cout << "Name: " << name << endl;
        cout << "Address: " << address << endl;
        cout << "Phone number: " << phoneNumber << endl;
        cout << "ID: " << id << endl;
        cout << endl;
    }
};

bool isValidPhoneNumber(const string& number) {
    // Регулярний вираз для перевірки формату номера телефону (+380xxxxxxxxx)
    regex pattern("^\\+380\\d{9}$");
    return regex_match(number, pattern);
}

bool isValidDate(const string& date) {
    regex pattern("^\\d{2}/\\d{2}/\\d{2}$");

    if (!regex_match(date, pattern)) {
        return false;
    }

    stringstream ss(date);
    char delimiter;
    int day, month, year;

    ss >> day >> delimiter >> month >> delimiter >> year;

    if (year < 24 || year > 25) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1) {
        return false;
    }

    // Перевірка днів для кожного місяця
    int daysInMonth = 0;
    switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        daysInMonth = 31;
        break;
    case 4: case 6: case 9: case 11:
        daysInMonth = 30;
        break;
    case 2:
        // Визначення кількості днів у лютому залежно від високосного року
        daysInMonth = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 29 : 28;
        break;
    default:
        return false;
    }

    if (day > daysInMonth) {
        return false;
    }

    return true;
}


// Клас для представлення касира, який також наслідує особу
class Cashier : public Person {
public:
    static string organization;  // Змінна organization статична
    double change;

    Cashier(const string& org, const string& n, const string& phone, double ch) : Person(n, phone) {
        organization = org;
        change = ch;
    }

    void displayInfo() const override {
        cout << endl;
        cout << "Cashier information:" << endl;
        cout << "Organization: " << organization << endl;
        cout << "Name: " << name << endl;
        cout << "Phone number: " << phoneNumber << endl;
        cout << "Change: $" << change << endl;
        cout << endl;
    }
};

string Cashier::organization = "FlexShip";  // Ініціалізуємо статичну змінну organization

// Клас, який представляє корабель
class Ship {
public:
    static int maxPassengerCapacity; // Максимальна кількість пасажирів на всіх кораблях

    static void setMaxPassengerCapacity(int capacity) {
        maxPassengerCapacity = capacity;
    }
};

// Ініціалізація статичної змінної maxPassengerCapacity
int Ship::maxPassengerCapacity = 10;

class ShipInfo {
public:
    string departurePort;
    string destinationPort;
    string date;

    ShipInfo() : departurePort(""), destinationPort(""), date("") {}

    ShipInfo(const string& depart, const string& dest, const string& d) : departurePort(depart), destinationPort(dest), date(d) {}
};

map<string, ShipInfo> shipInfoMap;

// Функція для додавання пасажира
void addPassenger(vector<Passenger>& passengers, set<int>& usedIds) {
    string name, address, phoneNumber;
    int id;

    // Отримання від користувача інформації про пасажира
    cout << "Enter passenger name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter address: ";
    getline(cin, address);

    // Перевірка номера телефону
    do {
        cout << "Enter phone number (format: +380xxxxxxxxx): ";
        getline(cin, phoneNumber);
        if (!isValidPhoneNumber(phoneNumber)) {
            cout << "Invalid phone number format. Please enter a valid phone number in the format +380xxxxxxxxx." << endl;
        }
    } while (!isValidPhoneNumber(phoneNumber));

    // Запитайте ідентифікатор пасажира і перевірте, чи він є унікальним та більший за 0
    do {
        cout << "Enter passenger ID (must be greater than or equal to 1): ";
        cin >> id;
        if (cin.fail() || usedIds.count(id) || id < 1) {
            cout << "Invalid input for passenger ID. Please choose a different ID greater than or equal to 1." << endl;
            cin.clear();
            cin.ignore();
        }

    } while (cin.fail() || usedIds.count(id) || id < 1);

    cout << endl;

    // Створення нового пасажира та його додавання до вектора пасажирів
    Passenger passenger(name, address, phoneNumber, id);
    passengers.push_back(passenger);

    // Додавання ID пасажира до множини використаних ID
    usedIds.insert(id);

    cout << "Passenger added successfully." << endl << endl;
}


int countPassengersOnShip(const vector<Passenger>& passengers, const string& shipName);

// Функція для додавання квитка для пасажира
void addTicket(vector<Passenger>& passengers, const string& shipName, set<int>& usedTicketIds) {
    // Оголошення змінних для збереження даних про квиток
    int passengerId, ticketId;
    double price;
    int cabinClass;

    int maxPassengerCapacity = Ship::maxPassengerCapacity;

    // Отримання від користувача ідентифікатора пасажира
    do {
        cout << "Enter passenger ID: ";
        if (!(cin >> passengerId) || passengerId <= 0) {
            cout << "Invalid input for passenger ID. Please choose a different ID." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (passengerId < 1);

    // Змінні для лічильників різних класів та знайденого пасажира
    int economyClassCount = 0;
    int businessClassCount = 0;
    int firstClassCount = 0;

    // Пошук пасажира за введеним ідентифікатором
    bool passengerFound = false;
    for (Passenger& passenger : passengers) {
        if (passenger.id == passengerId) {
            passengerFound = true;
            if (!passenger.tickets.empty()) {   // Перевірка, чи у пасажира вже є квиток
                cout << "Passenger already has a ticket. Cannot add another ticket." << endl;
                return;
            }

            int passengersOnShip = countPassengersOnShip(passengers, shipName);

            // Перевірка на максимальну кількість пасажирів на кораблі
            if (passengersOnShip >= maxPassengerCapacity) {
                cout << "Cannot add a ticket. The ship is full." << endl;
                return;
            }

            // Отримання ID квитка та інформації про рейс
            do {
                cout << "Enter ticket ID (must be greater than or equal to 1): ";
                cin >> ticketId;

                if (cin.fail() || usedTicketIds.count(ticketId) || ticketId < 1) {
                    cout << "Invalid input for ticket ID. Please choose a different ID greater than or equal to 1." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            } while (cin.fail() || usedTicketIds.count(ticketId) || ticketId < 1);

            // Отримання інформації про рейс, якщо вона є відсутня
            string departurePort, destinationPort, date;

            if (shipInfoMap.find(shipName) != shipInfoMap.end()) {
                departurePort = shipInfoMap[shipName].departurePort;
                destinationPort = shipInfoMap[shipName].destinationPort;
                date = shipInfoMap[shipName].date;
            }
            else {
                cout << "Enter departure port: ";
                cin.ignore();
                getline(cin, departurePort);

                do {
                    cout << "Enter destination port: ";
                    getline(cin, destinationPort);

                    if (destinationPort == departurePort) {
                        cout << "Destination port cannot be the same as departure port. Please enter a different destination port." << endl;
                    }
                } while (destinationPort == departurePort);
                
                // Перевірка формату дати
                do {
                    cout << "Enter date (dd/mm/yy): ";
                    getline(cin, date);
                    if (!isValidDate(date)) {
                        cout << "Invalid date. Please enter date for 24-25 years" << endl;
                    }
                } while (!isValidDate(date));

                shipInfoMap[shipName] = ShipInfo(departurePort, destinationPort, date);
            }
            // Введення класу кабіни та ціни квитка 
            while (true) {
                cout << "Enter cabin class (1 - Economy (51 - 249$), 2 - Business (251 - 499$), 3 - First (501 - 999$)): ";
                if (!(cin >> cabinClass) || (cabinClass < 1 || cabinClass > 3)) {
                    cout << "Invalid input. Please enter a valid cabin class (1, 2, or 3)." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    cout << "Enter price: $";
                    if (!(cin >> price)) {
                        cout << "Invalid input. Please enter a valid price." << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else {// Валідація ціни для вибраного класу
                        if ((cabinClass == 1 && (price <= 50 || price >= 250)) ||
                            (cabinClass == 2 && (price <= 250 || price >= 500)) ||
                            (cabinClass == 3 && (price <= 500 || price >= 1000))) {
                            cout << "Invalid price for the selected cabin class. Please enter a valid price." << endl;
                        }
                        else {
                            break;
                        }
                    }
                }
            }
            // Лічильники кількості квитків у різних класах для даного корабля
            for (const Passenger& pass : passengers) {
                for (const Ticket& ticket : pass.tickets) {
                    if (ticket.shipName == shipName) {
                        if (ticket.cabinClass == 1) {
                            economyClassCount++;
                        }
                        else if (ticket.cabinClass == 2) {
                            businessClassCount++;
                        }
                        else if (ticket.cabinClass == 3) {
                            firstClassCount++;
                        }
                    }
                }
            }
            // Перевірка на наявність вільних місць у вибраному класі
            if ((cabinClass == 1 && economyClassCount >= 6) ||
                (cabinClass == 2 && businessClassCount >= 2) ||
                (cabinClass == 3 && firstClassCount >= 2)) {
                cout << endl;
                cout << "Cannot add a ticket. The selected cabin class is full." << endl;
                cout << endl;
                return;
            }
            // Додавання нового квитка для пасажира
            usedTicketIds.insert(ticketId);
            Ticket ticket(ticketId, departurePort, destinationPort, date, cabinClass, price, shipName);
            passenger.tickets.push_back(ticket);
            cout << endl;
            cout << "Ticket added successfully." << endl << endl;
            break;
        }
    }
    // Повідомлення, якщо пасажир не знайдений
    if (!passengerFound) {
        cout << "Passenger not found." << endl << endl;
    }
}

// Функція для відображення інформації про пасажирів
void displayPassengers(const vector<Passenger>& passengers) {
    if (passengers.empty()) {
        cout << "No passengers available." << endl;
        return;
    }

    cout << "Passenger list:" << endl;
    for (const auto& passenger : passengers) {
        passenger.displayInfo();
    }
}
// Функція для відображення інформації про касира
void displayCashierInfo(const Cashier& cashier) {
    cashier.displayInfo();
}

void displayTickets(const vector<Passenger>& passengers) {
    if (passengers.empty()) {
        cout << "No passengers available." << endl;
        return;
    }

    cout << "Ticket list:" << endl;
    for (const auto& passenger : passengers) {
        cout << "Passenger: " << passenger.name << endl;
        if (passenger.tickets.empty()) {
            cout << "No tickets available." << endl;
        }
        else {
            for (const auto& ticket : passenger.tickets) {
                cout << "Ticket ID: " << ticket.id << endl;
                cout << "Ship Name: " << ticket.shipName << endl;
                cout << "Departure Port: " << ticket.departurePort << endl;
                cout << "Destination Port: " << ticket.destinationPort << endl;
                cout << "Date: " << ticket.date << endl;
                cout << "Cabin Class: " << ticket.cabinClass << endl;
                cout << "Price: $" << ticket.price << endl;
                cout << endl;
            }
        }
    }
}

int countPassengersOnShip(const vector<Passenger>& passengers, const string& shipName) {
    int count = 0;
    for (const Passenger& passenger : passengers) {
        for (const Ticket& ticket : passenger.tickets) {
            if (ticket.shipName == shipName) {
                count++;
            }
        }
    }
    if (count == 0) {
        cout << "No tickets have been created for the ship " << shipName << "." << endl;
    }
    return count;
}

vector<string> getUniqueShipNames(const vector<Passenger>& passengers) {
    vector<string> uniqueShipNames;
    bool hasTickets = false; // Прапорець, що показує, чи є у пасажирів квитки
    for (const Passenger& passenger : passengers) {
        for (const Ticket& ticket : passenger.tickets) {
            hasTickets = true;
            if (find(uniqueShipNames.begin(), uniqueShipNames.end(), ticket.shipName) == uniqueShipNames.end()) {
                uniqueShipNames.push_back(ticket.shipName);
            }
        }
    }
    return uniqueShipNames;
}

vector<int> getMostPopularCabinClasses(const vector<Passenger>& passengers) {
    map<int, int> cabinClassCounts;
    int maxCount = 0;
    bool hasTickets = false;
    for (const Passenger& passenger : passengers) {
        for (const Ticket& ticket : passenger.tickets) {
            hasTickets = true;
            cabinClassCounts[ticket.cabinClass]++;
            maxCount = max(maxCount, cabinClassCounts[ticket.cabinClass]);
        }
    }
    vector<int> mostPopularCabinClasses;
    for (const auto& entry : cabinClassCounts) {
        if (entry.second == maxCount) {
            mostPopularCabinClasses.push_back(entry.first);
        }
    }
    if (!hasTickets) {
        cout << "No tickets found." << endl;
    }
    return mostPopularCabinClasses;
}

double calculateTotalRevenue(const vector<Passenger>& passengers) {
    double totalRevenue = 0.0;
    bool hasTickets = false;
    for (const Passenger& passenger : passengers) {
        for (const Ticket& ticket : passenger.tickets) {
            hasTickets = true;
            totalRevenue += ticket.price;
        }
    }
    if (!hasTickets) {
        cout << "No tickets found." << endl;
    }
    return totalRevenue;
}

int countPassengers(const vector<Passenger>& passengers) {
    int count = 0;
    for (const Passenger& passenger : passengers) {
        count++;
    }
    return count;
}

vector<string> getMostPopularDestinationPorts(const vector<Passenger>& passengers) {
    map<string, int> destinationPortCounts;
    bool hasTickets = false;
    for (const Passenger& passenger : passengers) {
        for (const Ticket& ticket : passenger.tickets) {
            hasTickets = true;
            destinationPortCounts[ticket.destinationPort]++;
        }
    }
    vector<string> mostPopularDestinationPorts;
    int maxCount = 0;
    for (const auto& entry : destinationPortCounts) {
        if (entry.second > maxCount) {
            mostPopularDestinationPorts.clear();
            mostPopularDestinationPorts.push_back(entry.first);
            maxCount = entry.second;
        }
        else if (entry.second == maxCount) {
            mostPopularDestinationPorts.push_back(entry.first);
        }
    }
    if (!hasTickets) {
        cout << "No tickets found." << endl;
    }
    return mostPopularDestinationPorts;
}

void saveReservationsToFile(const vector<Passenger>& passengers, const string& filename) {
    ofstream file(filename);

    if (!file) {
        cout << "Error opening the file." << endl;
        return;
    }

    for (const Passenger& passenger : passengers) {
        if (!passenger.tickets.empty()) {
            file << "Reservations for Passenger: " << passenger.name << " (ID: " << passenger.id << ")" << endl;
            for (const Ticket& ticket : passenger.tickets) {
                file << "Ticket ID: " << ticket.id << endl;
                file << "Ship Name: " << ticket.shipName << endl;
                file << "Departure Port: " << ticket.departurePort << endl;
                file << "Destination Port: " << ticket.destinationPort << endl;
                file << "Date: " << ticket.date << endl;
                file << "Cabin Class: " << ticket.cabinClass << endl;
                file << "Price: $" << ticket.price << endl;
                file << endl;
            }
        }
    }

    file.close();
}

void saveDataToFile(const vector<Passenger>& passengers, const string& filename) {
    ofstream file(filename);

    if (!file) {
        cout << "Error opening the file." << endl;
        return;
    }

    for (const Passenger& passenger : passengers) {
        file << "Passenger" << endl;
        file << "Name: " << passenger.name << endl;
        file << "Address: " << passenger.address << endl;
        file << "Phone number: " << passenger.phoneNumber << endl;
        file << "ID: " << passenger.id << endl;
        file << endl;
    }

    file.close();
    cout << "Data saved to file successfully." << endl;
}

void displayDataFromFile(const string& passengerFilename) {
    ifstream file(passengerFilename);

    if (!file) {
        cout << "Error opening the file for displaying data." << endl;
        return;
    }

    string line;
    bool isEmpty = true; // Прапорець, що показує, чи файл порожній

    while (getline(file, line)) {
        cout << line << endl;
        isEmpty = false; // Файл не порожній, якщо прочитано хоча б один рядок
    }

    file.close();

    if (isEmpty) {
        cout << "The file is empty." << endl;
    }
    else {
        cout << "Data displayed from file successfully." << endl;
    }
}


void clearDataFiles(const string& passengerFilename, const string& reservationFilename) {
    ofstream passengerFile(passengerFilename, ios::trunc); // Відкриття файлу для очищення даних
    ofstream reservationFile(reservationFilename, ios::trunc); // Відкриття файлу для очищення даних

    if (!passengerFile || !reservationFile) {
        cout << "Error opening the files for clearing data." << endl;
        return;
    }

    // Очищення вмісту файлів
    passengerFile.close();
    reservationFile.close();
    cout << "Data files cleared successfully." << endl;
}


int main() {
    vector<Passenger> passengers;
    set<int> usedIds; // Створіть множину для зберігання використаних ідентифікаторів
    set<int> usedTicketIds; // Створіть множину для зберігання використаних ID квитків
    Cashier cashier("FlexShip", "Jane", "555-1234", 500.0);
    int choice;

    clearDataFiles("passenger_data.txt", "reservations.txt");

    while (true) {
        cout << "Menu:" << endl;
        cout << "1. Creation Menu:" << endl;
        cout << "2. Display Menu:" << endl;
        cout << "3. Requests to the system:" << endl;
        cout << "4. Load Menu: " << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting program." << endl;
            break;
        }

        switch (choice) {
        case 1:
        {
            while (true) {
            int creationChoice;
            cout << endl;
            cout << "Creation Menu:" << endl;
            cout << "1. Create passenger" << endl;
            cout << "2. Create ticket" << endl;
            cout << "0. Back to main menu" << endl;
            cout << "Enter your choice: ";
            cin >> creationChoice;

            switch (creationChoice) {
            case 0:
                break;
            case 1: // Create passenger
            {
                cout << endl;
                addPassenger(passengers, usedIds); // Додайте usedIds як другий аргумент
                saveDataToFile(passengers, "passenger_data.txt");
                break;
            }
            case 2: // Create ticket
            {
                if (passengers.empty()) {
                    cout << endl;
                    cout << "No passengers available to create a ticket for." << endl;
                    break;
                }
                cout << endl;
                string shipName;
                cout << "Enter ship name: ";
                cin.ignore();
                getline(cin, shipName);
                addTicket(passengers, shipName, usedTicketIds); // Додайте usedTicketIds як другий аргумент
                saveReservationsToFile(passengers, "reservations.txt");
                break;
            }
            default:
                cout << "Invalid choice. Please enter a number from the menu." << endl << endl;
                break;
            }
            if (creationChoice == 0) {
                cout << endl;
                break;
            }
            }
            break;
        }
        case 2: // Дісплей пасажирів, касира, квитків
        {
            while (true) {
            int displayChoice;
            cout << endl;
            cout << "Display Menu:" << endl;
            cout << "1. Display passengers" << endl;
            cout << "2. Display cashier information" << endl;
            cout << "3. Display tickets" << endl;
            cout << "0. Back to main menu" << endl;
            cout << "Enter your choice: ";
            cin >> displayChoice;

            switch (displayChoice) {
            case 1:
                cout << endl;
                if (passengers.empty()) {
                    cout << "No passenger available." << endl << endl;
                }
                else {
                    displayPassengers(passengers);
                }
                break;
            case 2:
                cout << endl;
                displayCashierInfo(cashier);
                break;
            case 3:
                cout << endl;
                if (usedTicketIds.empty()) {
                    cout << "No ticket available." << endl << endl;
                }
                else {
                    displayTickets(passengers);
                }
                break;
            case 0:
                break;
            default:
                cout << "Invalid choice. Please enter a number from the menu." << endl << endl;
                break;
            }
            if (displayChoice == 0) {
                cout << endl;
                break;
            }
            }
            break;
        }
        case 3: {
            while (true) {
                cout << endl;
                cout << "Requests to the system:" << endl;
                cout << "1. Count passenger on ship" << endl;
                cout << "2. Display unique ship names" << endl;
                cout << "3. Display most popular cabin class" << endl;
                cout << "4. Calculate total revenue" << endl;
                cout << "5. Display most popular destination ports" << endl;
                cout << "6. Total passengers" << endl;
                cout << "0. Back to main menu" << endl;
                cout << "Enter your choice: ";
                int shipStatisticsChoice;
                cin >> shipStatisticsChoice;

                switch (shipStatisticsChoice) {
                case 0:
                    break;
                case 1: {
                    vector<string> uniqueShipNames = getUniqueShipNames(passengers);
                    if (uniqueShipNames.empty()) {
                        cout << endl;
                        cout << "No ships available" << endl;
                    }
                    else {
                        cout << endl;
                        cout << "Available ships:" << endl;
                        for (const string& ship : uniqueShipNames) {
                            cout << ship << endl;
                        }
                        cout << endl;

                        string shipName;
                        cout << "Enter ship name: ";
                        cin.ignore();
                        getline(cin, shipName);

                        // Перевірка наявності введеної назви корабля у списку
                        auto shipFound = find(uniqueShipNames.begin(), uniqueShipNames.end(), shipName);
                        if (shipFound == uniqueShipNames.end()) {
                            cout << "Ship '" << shipName << "' not found." << endl << endl;
                        }
                        else {
                            int count = countPassengersOnShip(passengers, shipName);
                            cout << endl;
                            cout << "Passengers on ship " << shipName << ": " << count << endl << endl;
                        }
                    }
                    break;
                }
                case 2: {
                    vector<string> uniqueShipNames = getUniqueShipNames(passengers);
                    bool shipsExist = !uniqueShipNames.empty(); // Перевірка наявності кораблів

                    if (!shipsExist) {
                        cout << endl;
                        cout << "No ships found." << endl << endl;
                    }
                    else {
                        cout << endl;
                        cout << "Unique ship names:" << endl;
                        for (const string& shipName : uniqueShipNames) {
                            cout << shipName << endl;
                        }
                        cout << endl;
                    }
                    break;
                }
                case 3: {
                    if (usedTicketIds.empty()) {
                        cout << endl;
                        cout << "No tickets created yet." << endl << endl;
                    }
                    else {
                        cout << endl;
                        vector<int> mostPopularCabinClasses = getMostPopularCabinClasses(passengers);

                        cout << "Most popular cabin class(es):";
                        for (int cabinClass : mostPopularCabinClasses) {
                            cout << " " << cabinClass;
                        }
                        cout << endl << endl;
                    }
                    break;
                }
                case 4: {
                    if (usedTicketIds.empty()) {
                        cout << endl;
                        cout << "No tickets created yet." << endl << endl;
                    }
                    else {
                        cout << endl;
                        double totalRevenue = calculateTotalRevenue(passengers);
                        cout << "Total revenue: $" << totalRevenue << endl << endl;
                    }
                    break;
                }
                case 5: {
                    if (usedTicketIds.empty()) {
                        cout << endl;
                        cout << "No tickets created yet." << endl << endl;
                    }
                    else {
                        cout << endl;
                        vector<string> mostPopularDestinationPorts = getMostPopularDestinationPorts(passengers);
                        cout << "Most popular destination port(s): ";
                        for (const string& port : mostPopularDestinationPorts) {
                            cout << port << " ";
                        }
                        cout << endl << endl;
                    }
                    break;
                }
                case 6: {
                    cout << endl;
                    int passengerCount = countPassengers(passengers);
                    cout << "Total passengers: " << passengerCount << endl << endl;
                    break;
                }
                default:
                    cout << "Invalid choice. Please enter a number from the menu." << endl << endl;
                    break;
                }

                if (shipStatisticsChoice == 0) {
                    cout << endl;
                    break;
                }
            }
            break;
        }
        case 4: {
             int loadChoice;
             cout << endl;
             cout << "Load Menu:" << endl;
             cout << "1. Load passenger data" << endl;
             cout << "2. Load reservations data" << endl;
             cout << "0. Back to main menu" << endl;
             cout << "Enter your choice: ";
             cin >> loadChoice;

            switch (loadChoice) {
    case 0:
        break;
    case 1: {
        cout << endl;
        displayDataFromFile("passenger_data.txt");
        cout << endl;
        break;
    }
    case 2: {
        cout << endl;
        displayDataFromFile("reservations.txt");
        cout << endl;
        break;
    }
    default:
        cout << "Invalid choice. Please enter a number from the menu." << endl << endl;
        break;
    }
    if (loadChoice == 0) {
        cout << endl;
        break;
    }
    break;
}
        default:
            cout << "Invalid choice. Please enter a number from the menu." << endl << endl;
            break;
        }
    }
    cout << "Saving data to file before exiting..." << endl;
    saveDataToFile(passengers, "passenger_data.txt");
    cout << "Exiting program." << endl;

    cout << "Saving reservations data to file before exiting..." << endl;
    saveReservationsToFile(passengers, "reservations.txt");
    cout << "Reservations saved to file successfully." << endl;
    cout << "Exiting program." << endl;

    return 0;
}