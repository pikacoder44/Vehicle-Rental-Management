#include <iostream>
#include <exception>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

using namespace std;
// Clears cin fail states and discards broken inputs like "abc"
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Forces the user to enter a valid, positive integer
int getValidPositiveInt(string prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value > 0)
        {
            return value;
        }
        cout << "Invalid input! Please enter a positive whole number greater than 0.\n";
        clearInputBuffer();
    }
}

bool isValidDate(const string &dateStr)
{
    // 1. Check basic length and format: DD/MM/YYYY (exactly 10 characters)
    if (dateStr.length() != 10 || dateStr[2] != '/' || dateStr[5] != '/')
    {
        return false;
    }

    // 2. Extract digits and verify they are numbers
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue; // Skip the slashes
        if (!isdigit(dateStr[i]))
            return false;
    }

    // 3. Convert pieces to integers
    int day = stoi(dateStr.substr(0, 2));
    int month = stoi(dateStr.substr(3, 2));
    int year = stoi(dateStr.substr(6, 4));

    // 4. Validate Year and Month boundaries
    if (year < 2000 || year > 2100)
        return false; // Keeps years reasonable
    if (month < 1 || month > 12)
        return false;

    // 5. Validate Days based on specific months
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Handle Leap Year for February
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        daysInMonth[2] = 29;
    }

    if (day < 1 || day > daysInMonth[month])
    {
        return false;
    }

    return true;
}

// A simple loop helper to force a valid date entry from the user
string getValidDateStr(string prompt)
{
    string dateInput;
    while (true)
    {
        cout << prompt;
        cin >> dateInput;
        if (isValidDate(dateInput))
        {
            return dateInput;
        }
        cout << "Invalid date format or real-world calendar error! Use DD/MM/YYYY (e.g., 28/06/2026).\n";
    }
}
// Converts a valid DD/MM/YYYY string into a YYYYMMDD integer for easy mathematical comparison
int dateToComparableInt(const string &dateStr)
{
    int day = stoi(dateStr.substr(0, 2));
    int month = stoi(dateStr.substr(3, 2));
    int year = stoi(dateStr.substr(6, 4));

    return (year * 10000) + (month * 100) + day;
}

class IRentable
{
public:
    virtual void issueVehicle() = 0;
    virtual void returnVehicle() = 0;
    virtual double calculateRentalRate(int days) = 0;
    virtual ~IRentable() {}
};

// Exception classes for handling specific errors
class VehicleAlreadyRentedException : public exception
{
public:
    const char *what() const noexcept override { return "Vehicle is already rented."; }
};

class InvalidLicenseException : public exception
{
public:
    const char *what() const noexcept override { return "Invalid vehicle registration number."; }
};

class VehicleNotRentedException : public exception
{
public:
    const char *what() const noexcept override { return "Vehicle is not currently rented."; }
};

class Vehicle : public IRentable
{
private:
    int registrationNumber;
    int mileage;
    bool isRented = false;

public:
    string name;
    string type;
    string model;

    Vehicle(string name, string type, int registrationNumber, int mileage, string model, bool rentedStatus = false)
    {
        if (registrationNumber <= 0)
            throw InvalidLicenseException();
        this->name = name;
        this->type = type;
        this->registrationNumber = registrationNumber;
        this->mileage = mileage;
        this->model = model;
        this->isRented = rentedStatus;
    }
    virtual ~Vehicle() {}

    int getRegistrationNumber() const { return registrationNumber; }
    int getMileage() const { return mileage; }
    bool getIsRented() const { return isRented; }

    void issueVehicle() override
    {
        if (isRented)
            throw VehicleAlreadyRentedException();
        isRented = true;
    }

    void returnVehicle() override
    {
        if (!isRented)
            throw VehicleNotRentedException();
        isRented = false;
    }

    virtual double calculateRentalRate(int days) override { return 0; }
};

// Derived Classes
class Sedan : public Vehicle
{
public:
    int runtimeRate = 1500;
    Sedan(string name, int runtime, string type, int registrationNumber, int mileage, string model, bool rentedStatus = false)
        : Vehicle(name, type, registrationNumber, mileage, model, rentedStatus) { this->runtimeRate = runtime; }
    double calculateRentalRate(int days) override { return days * runtimeRate; }
};

class SUV : public Vehicle
{
public:
    SUV(string name, string type, int registrationNumber, int mileage, string model, bool rentedStatus = false)
        : Vehicle(name, type, registrationNumber, mileage, model, rentedStatus) {}
    double calculateRentalRate(int days) override { return days * 3500; }
};

class FreightTruck : public Vehicle
{
private:
    double dailyRate;
    double extraWeightFee;

public:
    FreightTruck(string name, double dailyRate, double extraWeightFee, string type, int registrationNumber, int mileage, string model, bool rentedStatus = false)
        : Vehicle(name, type, registrationNumber, mileage, model, rentedStatus)
    {
        this->dailyRate = dailyRate;
        this->extraWeightFee = extraWeightFee;
    }
    double calculateRentalRate(int days) override { return (days * dailyRate) + extraWeightFee; }
};

// Customer Class
class Customer
{
public:
    int customerId = 0;
    string name = "";
    string phoneNumber = "";
    Customer() {}
    Customer(int customerId, string name, string phoneNumber)
    {
        this->customerId = customerId;
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
};

// Rental Contract Class
class RentalContract
{
public:
    int contractId = 0;
    Customer customer;
    string vehicleName = ""; // Explicit storage for loading historical logs
    int vehicleRegNo = 0;
    string issueDate = "";
    string returnDate = "";
    double totalCost = 0.0;
    string status = "InUse"; // Default status

    RentalContract(int contractId, const Customer &customer, Vehicle *v, string issueDate, string returnDate, int days)
        : customer(customer), vehicleName(v->name), vehicleRegNo(v->getRegistrationNumber()), issueDate(issueDate), returnDate(returnDate)
    {
        this->contractId = contractId;
        this->totalCost = v->calculateRentalRate(days);
        this->status = "InUse";
    }

    // Secondary constructor specifically for parsing back items from log files
    RentalContract(int id, Customer cust, string vName, int regNo, string iDate, string rDate, double cost, string stat)
        : customer(cust), vehicleName(vName), vehicleRegNo(regNo), issueDate(iDate), returnDate(rDate), totalCost(cost), status(stat)
    {
        this->contractId = id;
    }

    void saveTransaction()
    {
        ofstream outFile("transactions_log.txt", ios::app);
        if (!outFile)
            return;
        // Format layout: ID CustName VehicleName VehicleReg IssueDate ReturnDate Cost Status
        outFile << contractId << " "
                << customer.name << " "
                << vehicleName << " "
                << vehicleRegNo << " "
                << issueDate << " "
                << returnDate << " "
                << totalCost << " "
                << status << "\n";
        outFile.close();
    }
};

// Global File Sync Operations
void updateInventoryFile(const vector<Vehicle *> &fleet)
{
    ofstream outFile("inventory.txt", ios::trunc);
    if (!outFile)
        return;
    for (const auto &v : fleet)
    {
        outFile << v->name << " " << v->type << " " << v->getRegistrationNumber() << " "
                << v->getMileage() << " " << v->model << " " << (v->getIsRented() ? 1 : 0) << "\n";
    }
    outFile.close();
}

void updateTransactionLogFile(const vector<RentalContract> &contracts)
{
    ofstream outFile("transactions_log.txt", ios::trunc); // Overwrite update dump
    if (!outFile)
        return;
    for (const auto &con : contracts)
    {
        outFile << con.contractId << " "
                << con.customer.name << " "
                << con.vehicleName << " "
                << con.vehicleRegNo << " "
                << con.issueDate << " "
                << con.returnDate << " "
                << con.totalCost << " "
                << con.status << "\n";
    }
    outFile.close();
}

void loadInventory(vector<Vehicle *> &fleet)
{
    for (auto v : fleet)
        delete v;
    fleet.clear();
    ifstream inFile("inventory.txt");
    if (!inFile)
        return;
    string name, type, model;
    int regNo, mileage, statusInt;
    while (inFile >> name >> type >> regNo >> mileage >> model >> statusInt)
    {
        bool rentedStatus = (statusInt == 1);
        if (type == "Sedan")
            fleet.push_back(new Sedan(name, 2000, type, regNo, mileage, model, rentedStatus));
        else if (type == "SUV")
            fleet.push_back(new SUV(name, type, regNo, mileage, model, rentedStatus));
        else
            fleet.push_back(new FreightTruck(name, 4000, 1500, type, regNo, mileage, model, rentedStatus));
    }
    inFile.close();
}

void saveCustomerToFile(const Customer &c)
{
    ofstream outFile("customers.txt", ios::app);
    if (outFile)
    {
        outFile << c.customerId << " " << c.name << " " << c.phoneNumber << "\n";
        outFile.close();
    }
}

void loadCustomers(vector<Customer> &customers)
{
    customers.clear();
    ifstream inFile("customers.txt");
    if (!inFile)
        return;
    int id;
    string name, phone;
    while (inFile >> id >> name >> phone)
        customers.push_back(Customer(id, name, phone));
    inFile.close();
}

void loadContracts(vector<RentalContract> &contracts, int &idCounter)
{
    contracts.clear();
    ifstream inFile("transactions_log.txt");
    if (!inFile)
        return;
    int id, regNo;
    string cName, vName, iDate, rDate, status;
    double cost;
    while (inFile >> id >> cName >> vName >> regNo >> iDate >> rDate >> cost >> status)
    {
        Customer tempCust(0, cName, "Unknown");
        contracts.push_back(RentalContract(id, tempCust, vName, regNo, iDate, rDate, cost, status));
        if (id > idCounter)
            idCounter = id; // Ensure counter dynamically moves to match logs
    }
    inFile.close();
}

void addVehicleMenu()
{
    string name, type, model;
    int registrationNumber, mileage;
    cout << "\n========== Register New Vehicle ==========\n";
    cout << "Vehicle Name (no spaces): ";
    cin >> name;
    cout << "Vehicle Type (Sedan/SUV/Truck): ";
    cin >> type;
    cout << "Registration Number: ";
    cin >> registrationNumber;
    if (registrationNumber <<= 0)
        throw InvalidLicenseException();
    cout << "Mileage: ";
    cin >> mileage;
    cout << "Model (no spaces): ";
    cin >> model;

    ofstream outFile("inventory.txt", ios::app);
    if (!outFile)
        return;
    outFile << name << " " << type << " " << registrationNumber << " " << mileage << " " << model << " 0\n";
    outFile.close();
    cout << "\nVehicle recorded to inventory database!\n";
}

int main()
{
    vector<Vehicle *> fleet;
    vector<Customer> customers;
    vector<RentalContract> contracts;
    int contractIdCounter = 1000;

    loadInventory(fleet);
    loadCustomers(customers);
    loadContracts(contracts, contractIdCounter); // Restores everything into scope at startup!

    int choice;
    do
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        cout << "\n============================================================\n";
        cout << "       SMART FLEET & VEHICLE RENTAL MANAGEMENT SYSTEM\n";
        cout << "============================================================\n";
        cout << "1. View Available Vehicles\n";
        cout << "2. Add New Vehicle\n";
        cout << "3. Rent Vehicle\n";
        cout << "4. Return Vehicle\n";
        cout << "5. View Customers\n";
        cout << "6. Register Customer\n";
        cout << "7. View Rental Contracts (History & Live Logs)\n";
        cout << "8. Calculate Rental Cost Estimation\n";
        cout << "9. Force Rewrite Sync Logs Explicitly\n";
        cout << "10. Exit\n";
        cout << "============================================================\n";
        cout << "Enter your choice: ";

        // SAFE MENU INPUT CHECK
        if (!(cin >> choice))
        {
            cout << "\nInvalid choice! Please enter a number.\n";
            clearInputBuffer();
            cout << "\nPress Enter to try again...";
            cin.get();
            continue; // Skips the switch statement entirely and re-draws the menu
        }
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        try
        {
            switch (choice)
            {
            case 1:
                loadInventory(fleet);
                cout << "\n--- Current Active Fleet Assets ---\n";
                if (fleet.empty())
                    cout << "No vehicles listed yet.\n";
                for (const auto &v : fleet)
                {
                    cout << "Reg #" << v->getRegistrationNumber() << " | " << v->name
                         << " [" << v->type << "] - Model: " << v->model
                         << " | Status: " << (v->getIsRented() ? "RENTED" : "AVAILABLE") << "\n";
                }
                break;
            case 2:
                addVehicleMenu();
                loadInventory(fleet);
                break;
            case 3:
            {
                cout << "\n--- Process New Rental Contract ---\n";
                if (customers.empty())
                {
                    cout << "Error: No clients registered. Use Option 6 first.\n";
                    break;
                }
                int inputReg, inputCustId, rentalDays;
                string iDate, rDate;
                cout << "Enter Customer ID: ";
                cin >> inputCustId;
                cout << "Enter Vehicle Registration Number: ";
                cin >> inputReg;

                Customer *targetCustomer = nullptr;
                for (auto &c : customers)
                {
                    if (c.customerId == inputCustId)
                    {
                        targetCustomer = &c;
                        break;
                    }
                }
                Vehicle *targetVehicle = nullptr;
                for (auto &v : fleet)
                {
                    if (v->getRegistrationNumber() == inputReg)
                    {
                        targetVehicle = v;
                        break;
                    }
                }

                if (!targetCustomer)
                    cout << "Error: Customer not found.\n";
                else if (!targetVehicle)
                    cout << "Error: Vehicle records not found.\n";
                else
                {
                    targetVehicle->issueVehicle(); // Throws safety errors if already rented

                    // FIXED: This enforces a clean, positive integer greater than 0
                    rentalDays = getValidPositiveInt("Enter Rental Duration (Days): ");

                    // FIXED: Ensures return date cannot chronologically happen before start date
                    while (true)
                    {
                        iDate = getValidDateStr("Enter Start Date (DD/MM/YYYY): ");
                        rDate = getValidDateStr("Enter Return Date (DD/MM/YYYY): ");

                        if (dateToComparableInt(rDate) >= dateToComparableInt(iDate))
                        {
                            break; // Dates are logical, move forward
                        }

                        cout << "\nLogic Error: Return date cannot be before than the start date!\n";
                        cout << "Please re-enter the dates correctly.\n\n";
                    }

                    RentalContract newContract(++contractIdCounter, *targetCustomer, targetVehicle, iDate, rDate, rentalDays);
                    newContract.saveTransaction();
                    contracts.push_back(newContract);

                    updateInventoryFile(fleet);
                    cout << "Contract #" << contractIdCounter << " provisioned.\n";
                }
                break;
            }
            case 4:
            {
                cout << "\n--- Process Vehicle Return Drop-off ---\n";
                int inputReg;
                cout << "Enter Vehicle Registration Number: ";
                cin >> inputReg;
                Vehicle *targetVehicle = nullptr;
                for (auto &v : fleet)
                {
                    if (v->getRegistrationNumber() == inputReg)
                    {
                        targetVehicle = v;
                        break;
                    }
                }

                if (targetVehicle)
                {
                    targetVehicle->returnVehicle();
                    updateInventoryFile(fleet);

                    // CRUCIAL STATUS MATCHING STEP:
                    // Find the 'InUse' contract for this vehicle and flip it to 'Returned'
                    bool contractUpdated = false;
                    for (auto &con : contracts)
                    {
                        if (con.vehicleRegNo == inputReg && con.status == "InUse")
                        {
                            con.status = "Returned";
                            contractUpdated = true;
                            break;
                        }
                    }
                    if (contractUpdated)
                    {
                        updateTransactionLogFile(contracts); // Write changes directly to file system!
                        cout << "Associated system transaction logs successfully switched to 'Returned'.\n";
                    }
                }
                else
                    cout << "Asset match error.\n";
                break;
            }
            case 5:
                cout << "\n--- System Customers List ---\n";
                if (customers.empty())
                    cout << "Database register empty.\n";
                for (const auto &c : customers)
                    cout << "ID: " << c.customerId << " | Name: " << c.name << " | Phone: " << c.phoneNumber << "\n";
                break;
            case 6:
            {
                int id;
                string cName, phone;
                cout << "\n--- Register New Client Profile ---\n";
                cout << "Assign unique Customer ID: ";
                cin >> id;

                // CORRECT FIX: Check if this Customer ID already exists
                bool idExists = false;
                for (const auto &c : customers)
                {
                    if (c.customerId == id)
                    {
                        idExists = true;
                        break;
                    }
                }

                if (idExists)
                {
                    cout << "Error: Customer ID " << id << " is already taken! Please assign a unique ID.\n";
                    break; // Abort registration and return to menu
                }

                cout << "Client Full Name (no spaces): ";
                cin >> cName;
                cout << "Contact Number: ";
                cin >> phone;

                // Proceed with registration if ID is unique
                Customer newCust(id, cName, phone);
                customers.push_back(newCust);
                saveCustomerToFile(newCust);
                cout << "Customer profile successfully registered and saved.\n";
                break;
            }
            case 7:
                cout << "\n--- Complete Transaction Log & Performance Register ---\n";
                if (contracts.empty())
                    cout << "No transaction tracking elements loaded in local database.\n";
                for (const auto &con : contracts)
                {
                    cout << "Contract ID: " << con.contractId
                         << " | Client: " << con.customer.name
                         << " | Vehicle: " << con.vehicleName << " (Reg #" << con.vehicleRegNo << ")"
                         << " | Period: " << con.issueDate << " to " << con.returnDate
                         << " | Total: $" << con.totalCost
                         << " | Status: " << con.status << "\n";
                }
                break;
            case 8:
            {
                int inputReg, estimateDays;
                cout << "\n--- Instant Price Quoter Tool ---\n";
                cout << "Enter Target Vehicle Registration Number: ";
                cin >> inputReg;
                cout << "Enter Intended Rental Window Days: ";
                cin >> estimateDays;
                bool found = false;
                for (auto &v : fleet)
                {
                    if (v->getRegistrationNumber() == inputReg)
                    {
                        cout << "Estimated total for " << v->name << ": $" << v->calculateRentalRate(estimateDays) << endl;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    cout << "Registration not matching fleet inventory items.\n";
                break;
            }
            case 9:
                updateTransactionLogFile(contracts);
                updateInventoryFile(fleet);
                cout << "Database updates forced down stream manually.\n";
                break;
            case 10:
                cout << "\nThank you for using our system!\n";
                break;
            default:
                cout << "Invalid selection.\n";
            }
        }
        catch (const exception &e)
        {
            cout << "\nError: " << e.what() << endl;
        }

        if (choice != 10)
        {
            cout << "\nPress Enter to return to the Main Menu...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    } while (choice != 10);

    for (auto v : fleet)
        delete v;
    fleet.clear();
    return 0;
}