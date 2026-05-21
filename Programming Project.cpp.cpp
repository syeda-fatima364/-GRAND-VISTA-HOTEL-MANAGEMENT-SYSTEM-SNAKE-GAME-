/*
==========================================================================
   GRAND VISTA HOTEL MANAGEMENT SYSTEM
   + ARCADE: SNAKE GAME

   Compile on Windows (VS Code terminal):
     g++ hotel_management.cpp -o hotel.exe -std=c++11
     hotel.exe
==========================================================================
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
using namespace std;

// ============================================================
// COLORS
// ============================================================
#define RESET          "\033[0m"
#define BOLD           "\033[1m"
#define RED            "\033[31m"
#define GREEN          "\033[32m"
#define YELLOW         "\033[33m"
#define CYAN           "\033[36m"
#define WHITE          "\033[37m"
#define MAGENTA        "\033[35m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_WHITE   "\033[97m"

// ============================================================
// HOTEL CONSTANTS
// ============================================================
const int MAX_ROOMS   = 10;
const int MAX_GUESTS  = 20;
const int MAX_HISTORY = 50;

// ============================================================
// SNAKE CONSTANTS
// ============================================================
const int SNAKE_BOARD_WIDTH  = 30;   // board width  (inner cells)
const int SNAKE_BOARD_HEIGHT = 18;   // board height (inner cells)
const int MAX_SNAKE_LENGTH   = SNAKE_BOARD_WIDTH * SNAKE_BOARD_HEIGHT;

// ============================================================
// HOTEL CLASSES
// ============================================================
class Room {
public:
    int    roomNumber;
    string type;
    float  pricePerNight;
    bool   isOccupied;
    int    guestId;

    Room() {
        roomNumber   = 0;
        type         = "";
        pricePerNight = 0;
        isOccupied   = false;
        guestId      = -1;
    }
};

class Guest {
public:
    int    guestId;
    string name;
    string cnic;
    int    roomNumber;
    int    nightsStayed;
    float  totalBill;
    bool   isCheckedIn;

    Guest() {
        guestId      = 0;
        name         = "";
        cnic         = "";
        roomNumber   = 0;
        nightsStayed = 0;
        totalBill    = 0;
        isCheckedIn  = false;
    }
};

class Transaction {
public:
    string type;
    string guestName;
    string description;
    int    roomNumber;
    float  amount;

    Transaction() {
        type        = "";
        guestName   = "";
        description = "";
        roomNumber  = 0;
        amount      = 0;
    }
};

// ============================================================
// HOTEL GLOBALS
// ============================================================
Room        rooms[MAX_ROOMS];
Guest       guests[MAX_GUESTS];
Transaction history[MAX_HISTORY];
int   totalGuests       = 0;
int   totalTransactions = 0;
float totalRevenue      = 0.0;
int   dayNumber         = 1;

// ============================================================
// SNAKE GLOBALS
// ============================================================
struct Point {
    int x;
    int y;
};

Point snake[MAX_SNAKE_LENGTH];
int  snakeLength;
int  snakeDirectionX;
int  snakeDirectionY;
Point food;
bool snakeGameOver;
int  snakeScore;
int  snakeHighScore = 0;
int  snakeLevel;

// ============================================================
// CONSOLE HELPERS
// ============================================================
void gotoxy(int xPosition, int yPosition) {
    COORD cursorCoord;
    cursorCoord.X = (SHORT)xPosition;
    cursorCoord.Y = (SHORT)yPosition;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorCoord);
}

void hideCursor() {
    HANDLE consoleHandle       = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize   = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void showCursor() {
    HANDLE consoleHandle       = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize   = 1;
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void enableAnsi() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  consoleMode   = 0;
    GetConsoleMode(consoleHandle, &consoleMode);
    SetConsoleMode(consoleHandle, consoleMode | 0x0004);
}

// ============================================================
// HOTEL PROTOTYPES
// ============================================================
void initializeHotel();
void printLine(char character, int length);
void printLogo();
void mainMenu();
void checkInGuest();
void checkOutGuest();
void viewAllRooms();
void viewGuestList();
void addRoomService();
void generateDailyReport();
void generateRevenueReport();
void advanceDay();
int  findVacantRoom(string roomType);
int  findGuestByRoom(int roomNumber);
void recordTransaction(string transactionType, string guestName, int roomNumber, float amount, string description);
void pressEnterToContinue();

// ============================================================
// SNAKE PROTOTYPES
// ============================================================
void snakeArcade();
void snakeInit();
void spawnFood();
void drawSnake();
void snakeLoop();

// ============================================================
// MAIN
// ============================================================
int main() {
    enableAnsi();
    srand((unsigned)time(0));
    initializeHotel();
    printLogo();
    pressEnterToContinue();

    int menuChoice = 0;

    while (menuChoice != 9) {
        mainMenu();
        cout << BRIGHT_YELLOW << "  Enter choice: " << RESET;
        cin >> menuChoice;
        cin.ignore();

        switch (menuChoice) {
            case 1: checkInGuest();          break;
            case 2: checkOutGuest();         break;
            case 3: viewAllRooms();          break;
            case 4: viewGuestList();         break;
            case 5: addRoomService();        break;
            case 6: generateDailyReport();   break;
            case 7: generateRevenueReport(); break;
            case 8: advanceDay();            break;
            case 0: snakeArcade();           break;
            case 9:
                cout << BRIGHT_CYAN << "\n  Thanks for using Grand Vista HMS!\n" << RESET;
                break;
            default:
                cout << BRIGHT_RED << "\n  [!] Invalid choice.\n" << RESET;
                Sleep(700);
        }
    }

    showCursor();
    return 0;
}

// ============================================================
// HOTEL - INIT
// ============================================================
void initializeHotel() {
    string roomTypes[MAX_ROOMS]  = {"Single","Single","Single","Double","Double",
                                     "Double","Double","Suite","Suite","Suite"};
    float  roomPrices[MAX_ROOMS] = {2500,2500,2500,4500,4500,4500,4500,9000,9000,9000};

    for (int index = 0; index < MAX_ROOMS; index++) {
        rooms[index].roomNumber    = 101 + index;
        rooms[index].type          = roomTypes[index];
        rooms[index].pricePerNight = roomPrices[index];
        rooms[index].isOccupied    = false;
        rooms[index].guestId       = -1;
    }
}

void printLine(char character, int length) {
    cout << "  ";
    for (int index = 0; index < length; index++) {
        cout << character;
    }
    cout << "\n";
}

// ============================================================
// LOGO
// ============================================================
void printLogo() {
    system("cls");
    cout << CYAN << BOLD;
    cout << "\n";
    cout << "  +------------------------------------------------------+\n";
    cout << "  |                                                       |\n";
    cout << "  |    ____  ____    _    _   _ ____                     |\n";
    cout << "  |   / ___||  _ \\  / \\  | \\ | |  _ \\                |\n";
    cout << "  |  | |  _ | |_) |/ _ \\ |  \\| | | | |                  |\n";
    cout << "  |  | |_| ||  _ </ ___ \\| |\\  | |_| |                  |\n";
    cout << "  |   \\____||_| \\_/_/   \\_|_| \\_|____/                |\n";
    cout << "  |                                                       |\n";
    cout << "  |        " << BRIGHT_YELLOW << "V I S T A   H O T E L" << CYAN;
    cout << "                         |\n";
    cout << "  |        " << WHITE << "Hotel Management System " << CYAN;
    cout << "             |\n";
    cout << "  |      " << BRIGHT_MAGENTA << "*** ARCADE: SNAKE GAME INSIDE! ***" << CYAN;
    cout << "           |\n";
    cout << "  |                                                      |\n";
    cout << "  +------------------------------------------------------+\n";
    cout << RESET;
    cout << GREEN << "\n  [10 Rooms | Single/Double/Suite | Reports | Snake Arcade]\n" << RESET;
}

// ============================================================
// MAIN MENU
// ============================================================
void mainMenu() {
    system("cls");
    cout << CYAN << BOLD;
    cout << "\n  +--------------------------------------+\n";
    cout << "  |   GRAND VISTA HMS  -  DAY " << setw(2) << dayNumber << "        |\n";
    cout << "  +--------------------------------------+\n";
    cout << "  |  " << BRIGHT_GREEN   << "1." << CYAN << " Check-In Guest                 |\n";
    cout << "  |  " << BRIGHT_GREEN   << "2." << CYAN << " Check-Out Guest                |\n";
    cout << "  |  " << BRIGHT_YELLOW  << "3." << CYAN << " View All Rooms                 |\n";
    cout << "  |  " << BRIGHT_YELLOW  << "4." << CYAN << " View Guest List                |\n";
    cout << "  |  " << BRIGHT_MAGENTA << "5." << CYAN << " Add Room Service               |\n";
    cout << "  |  " << WHITE           << "6." << CYAN << " Daily Report                   |\n";
    cout << "  |  " << WHITE           << "7." << CYAN << " Revenue Report                 |\n";
    cout << "  |  " << BRIGHT_YELLOW  << "8." << CYAN << " Advance to Next Day            |\n";
    cout << "  |  " << BRIGHT_MAGENTA << "0." << CYAN << " [ARCADE] Play Snake            |\n";
    cout << "  |  " << BRIGHT_RED     << "9." << CYAN << " Exit                           |\n";
    cout << "  +--------------------------------------+\n";
    cout << RESET;
    cout << GREEN << "  Revenue: PKR " << fixed << setprecision(2) << totalRevenue;

    if (snakeHighScore > 0) {
        cout << BRIGHT_MAGENTA << "   |  SNAKE HI: " << snakeHighScore;
    }

    cout << RESET << "\n";
}

// ============================================================
// CHECK-IN
// ============================================================
void checkInGuest() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 54);
    cout << "  CHECK-IN NEW GUEST\n" << RESET;
    printLine('=', 54);

    if (totalGuests >= MAX_GUESTS) {
        cout << BRIGHT_RED << "  [!] Records full!\n" << RESET;
        pressEnterToContinue();
        return;
    }

    cout << BRIGHT_YELLOW << "\n  Room Types:\n" << RESET;
    cout << "  1. Single  - PKR 2,500/night\n";
    cout << "  2. Double  - PKR 4,500/night\n";
    cout << "  3. Suite   - PKR 9,000/night\n";
    cout << BRIGHT_YELLOW << "\n  Select type (1-3): " << RESET;

    int typeChoice;
    cin >> typeChoice;
    cin.ignore();

    string selectedRoomType = "";
    if      (typeChoice == 1) selectedRoomType = "Single";
    else if (typeChoice == 2) selectedRoomType = "Double";
    else if (typeChoice == 3) selectedRoomType = "Suite";

    if (selectedRoomType.empty()) {
        cout << BRIGHT_RED << "  [!] Invalid.\n" << RESET;
        pressEnterToContinue();
        return;
    }

    int roomIndex = findVacantRoom(selectedRoomType);

    if (roomIndex == -1) {
        cout << BRIGHT_RED << "  [!] No " << selectedRoomType << " rooms free!\n" << RESET;
        pressEnterToContinue();
        return;
    }

    cout << BRIGHT_CYAN << "\n  Guest Name    : " << RESET;
    string guestName;
    getline(cin, guestName);

    if (guestName.empty()) {
        guestName = "Walk-In Guest";
    }

    cout << BRIGHT_CYAN << "  CNIC          : " << RESET;
    string guestCnic;
    getline(cin, guestCnic);

    cout << BRIGHT_CYAN << "  Nights to Stay: " << RESET;
    int numberOfNights;
    cin >> numberOfNights;
    cin.ignore();

    if (numberOfNights <= 0) {
        numberOfNights = 1;
    }

    Guest newGuest;
    newGuest.guestId      = totalGuests + 1;
    newGuest.name         = guestName;
    newGuest.cnic         = guestCnic;
    newGuest.roomNumber   = rooms[roomIndex].roomNumber;
    newGuest.nightsStayed = numberOfNights;
    newGuest.totalBill    = 0;
    newGuest.isCheckedIn  = true;

    guests[totalGuests] = newGuest;
    totalGuests++;

    rooms[roomIndex].isOccupied = true;
    rooms[roomIndex].guestId    = newGuest.guestId;

    float estimatedBill = rooms[roomIndex].pricePerNight * numberOfNights;

    cout << BRIGHT_GREEN;
    cout << "\n  +--------------------------------+\n";
    cout << "  |  *** CHECK-IN SUCCESSFUL ***   |\n";
    cout << "  +--------------------------------+\n";
    cout << "  | Guest ID  : " << setw(19) << newGuest.guestId                  << " |\n";
    cout << "  | Name      : " << setw(19) << guestName.substr(0, 19)           << " |\n";
    cout << "  | Room No.  : " << setw(19) << newGuest.roomNumber                << " |\n";
    cout << "  | Type      : " << setw(19) << selectedRoomType                   << " |\n";
    cout << "  | Nights    : " << setw(19) << numberOfNights                     << " |\n";
    cout << "  | Est. Bill : PKR " << setw(15) << fixed << setprecision(2) << estimatedBill << " |\n";
    cout << "  +--------------------------------+\n" << RESET;

    recordTransaction("CheckIn", guestName, newGuest.roomNumber, 0, "Checked in - " + selectedRoomType);
    pressEnterToContinue();
}

// ============================================================
// CHECK-OUT
// ============================================================
void checkOutGuest() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 54);
    cout << "  CHECK-OUT GUEST\n" << RESET;
    printLine('=', 54);

    cout << BRIGHT_YELLOW << "  Enter Room Number: " << RESET;
    int targetRoomNumber;
    cin >> targetRoomNumber;
    cin.ignore();

    int guestIndex = findGuestByRoom(targetRoomNumber);

    if (guestIndex == -1) {
        cout << BRIGHT_RED << "  [!] No guest in room " << targetRoomNumber << "\n" << RESET;
        pressEnterToContinue();
        return;
    }

    Guest& currentGuest = guests[guestIndex];

    int roomArrayIndex = -1;
    for (int index = 0; index < MAX_ROOMS; index++) {
        if (rooms[index].roomNumber == targetRoomNumber) {
            roomArrayIndex = index;
            break;
        }
    }

    float roomCharge  = rooms[roomArrayIndex].pricePerNight * currentGuest.nightsStayed;
    float serviceCost = currentGuest.totalBill;
    float taxAmount   = (roomCharge + serviceCost) * 0.16;
    float grandTotal  = roomCharge + serviceCost + taxAmount;

    cout << GREEN;
    cout << "\n  +------------------------------------------+\n";
    cout << "  |              FINAL BILL                  |\n";
    cout << "  +------------------------------------------+\n";
    cout << "  | Guest   : " << left << setw(32) << currentGuest.name.substr(0, 32) << "|\n";
    cout << "  | Room    : " << left << setw(32) << currentGuest.roomNumber          << "|\n";
    cout << "  | Nights  : " << left << setw(32) << currentGuest.nightsStayed        << "|\n";
    cout << "  +------------------------------------------+\n" << right;
    cout << "  | Room Charges   : PKR " << setw(20) << fixed << setprecision(2) << roomCharge  << " |\n";
    cout << "  | Extra Services : PKR " << setw(20) << serviceCost                             << " |\n";
    cout << "  | Tax (16% GST)  : PKR " << setw(20) << taxAmount                              << " |\n";
    cout << "  +------------------------------------------+\n";
    cout << BRIGHT_GREEN;
    cout << "  | GRAND TOTAL    : PKR " << setw(20) << grandTotal << " |\n";
    cout << GREEN << "  +------------------------------------------+\n" << RESET;

    cout << BRIGHT_YELLOW << "\n  Amount Paid: PKR " << RESET;
    float amountPaid;
    cin >> amountPaid;
    cin.ignore();

    float changeAmount = amountPaid - grandTotal;

    if (changeAmount >= 0) {
        cout << BRIGHT_GREEN << "  [OK] Change: PKR " << fixed << setprecision(2) << changeAmount  << "\n" << RESET;
    } else {
        cout << BRIGHT_RED   << "  [!!] Balance due: PKR " << fixed << setprecision(2) << (-changeAmount) << "\n" << RESET;
    }

    totalRevenue += grandTotal;

    rooms[roomArrayIndex].isOccupied = false;
    rooms[roomArrayIndex].guestId    = -1;

    currentGuest.isCheckedIn = false;
    currentGuest.totalBill   = grandTotal;

    recordTransaction("CheckOut", currentGuest.name, currentGuest.roomNumber, grandTotal, "Checked out");

    cout << BRIGHT_CYAN << "\n  [OK] Checked out!\n" << RESET;
    pressEnterToContinue();
}

// ============================================================
// VIEW ROOMS
// ============================================================
void viewAllRooms() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 58);
    cout << "  ROOM STATUS BOARD\n" << RESET;
    printLine('=', 58);

    cout << BRIGHT_YELLOW << "  " << left
         << setw(8)  << "Room"
         << setw(10) << "Type"
         << setw(15) << "Price/Night"
         << setw(12) << "Status"
         << setw(10) << "GuestID"
         << "\n" << RESET;

    printLine('-', 56);

    int vacantCount   = 0;
    int occupiedCount = 0;

    for (int index = 0; index < MAX_ROOMS; index++) {
        if (rooms[index].isOccupied) {
            cout << BRIGHT_RED;
            occupiedCount++;
        } else {
            cout << BRIGHT_GREEN;
            vacantCount++;
        }

        string guestIdDisplay = rooms[index].isOccupied ? to_string(rooms[index].guestId) : "-";
        string statusDisplay  = rooms[index].isOccupied ? "OCCUPIED" : "VACANT";

        cout << "  " << left
             << setw(8)  << rooms[index].roomNumber
             << setw(10) << rooms[index].type
             << "PKR " << setw(11) << fixed << setprecision(0) << rooms[index].pricePerNight
             << setw(12) << statusDisplay
             << setw(10) << guestIdDisplay
             << "\n" << RESET;
    }

    printLine('=', 58);

    float occupancyRate = ((float)occupiedCount / MAX_ROOMS) * 100.0f;

    cout << BRIGHT_GREEN  << "  Vacant: "    << vacantCount   << RESET << "   "
         << BRIGHT_RED    << "Occupied: "    << occupiedCount << RESET << "   "
         << BRIGHT_YELLOW << "Occupancy: "  << fixed << setprecision(1) << occupancyRate << "%\n" << RESET;

    pressEnterToContinue();
}

// ============================================================
// VIEW GUESTS
// ============================================================
void viewGuestList() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 60);
    cout << "  CURRENT GUEST LIST\n" << RESET;
    printLine('=', 60);

    cout << BRIGHT_YELLOW << "  " << left
         << setw(6)  << "ID"
         << setw(22) << "Name"
         << setw(8)  << "Room"
         << setw(9)  << "Nights"
         << "Extra(PKR)\n" << RESET;

    printLine('-', 58);

    bool anyGuestFound = false;

    for (int index = 0; index < totalGuests; index++) {
        if (guests[index].isCheckedIn) {
            anyGuestFound = true;
            cout << BRIGHT_CYAN << "  " << left
                 << setw(6)  << guests[index].guestId
                 << setw(22) << guests[index].name.substr(0, 20)
                 << setw(8)  << guests[index].roomNumber
                 << setw(9)  << guests[index].nightsStayed
                 << fixed << setprecision(2) << guests[index].totalBill
                 << "\n" << RESET;
        }
    }

    if (!anyGuestFound) {
        cout << BRIGHT_RED << "  No guests checked in.\n" << RESET;
    }

    printLine('=', 60);
    pressEnterToContinue();
}

// ============================================================
// ROOM SERVICE
// ============================================================
void addRoomService() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 50);
    cout << "  ROOM SERVICE\n" << RESET;
    printLine('=', 50);

    cout << BRIGHT_YELLOW << "  Room Number: " << RESET;
    int targetRoomNumber;
    cin >> targetRoomNumber;
    cin.ignore();

    int guestIndex = findGuestByRoom(targetRoomNumber);

    if (guestIndex == -1) {
        cout << BRIGHT_RED << "  [!] No guest in room " << targetRoomNumber << "\n" << RESET;
        pressEnterToContinue();
        return;
    }

    cout << GREEN << "\n  Services:\n" << RESET;
    cout << "  1. Laundry           - PKR  500\n";
    cout << "  2. Room Cleaning     - PKR  300\n";
    cout << "  3. Breakfast         - PKR  800\n";
    cout << "  4. Airport Transfer  - PKR 2000\n";
    cout << "  5. Mini Bar          - PKR 1200\n";
    cout << "  6. Spa Session       - PKR 3500\n";
    cout << BRIGHT_YELLOW << "\n  Select (1-6): " << RESET;

    int serviceChoice;
    cin >> serviceChoice;
    cin.ignore();

    string serviceName  = "";
    float  servicePrice = 0;

    if      (serviceChoice == 1) { serviceName = "Laundry";          servicePrice = 500;  }
    else if (serviceChoice == 2) { serviceName = "Room Cleaning";    servicePrice = 300;  }
    else if (serviceChoice == 3) { serviceName = "Breakfast";        servicePrice = 800;  }
    else if (serviceChoice == 4) { serviceName = "Airport Transfer"; servicePrice = 2000; }
    else if (serviceChoice == 5) { serviceName = "Mini Bar";         servicePrice = 1200; }
    else if (serviceChoice == 6) { serviceName = "Spa Session";      servicePrice = 3500; }
    else {
        cout << BRIGHT_RED << "  [!] Invalid.\n" << RESET;
        pressEnterToContinue();
        return;
    }

    guests[guestIndex].totalBill += servicePrice;

    cout << BRIGHT_GREEN << "\n  [OK] " << serviceName << " added - PKR " << servicePrice << "\n" << RESET;

    recordTransaction("Service", guests[guestIndex].name, targetRoomNumber, servicePrice, serviceName);
    pressEnterToContinue();
}

// ============================================================
// DAILY REPORT
// ============================================================
void generateDailyReport() {
    system("cls");
    cout << CYAN << BOLD;
    printLine('=', 50);
    cout << "  DAILY REPORT - DAY " << dayNumber << "\n" << RESET;
    printLine('=', 50);

    int   totalCheckIns     = 0;
    int   totalCheckOuts    = 0;
    float totalServiceRevenue = 0;
    float totalRoomRevenue    = 0;

    for (int index = 0; index < totalTransactions; index++) {
        if (history[index].type == "CheckIn")  {
            totalCheckIns++;
        }
        if (history[index].type == "CheckOut") {
            totalCheckOuts++;
            totalRoomRevenue += history[index].amount;
        }
        if (history[index].type == "Service") {
            totalServiceRevenue += history[index].amount;
        }
    }

    int occupiedRoomCount = 0;
    for (int index = 0; index < MAX_ROOMS; index++) {
        if (rooms[index].isOccupied) {
            occupiedRoomCount++;
        }
    }

    float occupancyRate = ((float)occupiedRoomCount / MAX_ROOMS) * 100.0;

    cout << BRIGHT_YELLOW;
    cout << "\n  +-------------------------------------------+\n";
    cout << "  |         OPERATIONS SUMMARY                |\n";
    cout << "  +-------------------------------------------+\n";
    cout << "  | Total Rooms        : " << setw(22) << MAX_ROOMS         << " |\n";
    cout << "  | Rooms Occupied     : " << setw(22) << occupiedRoomCount << " |\n";
    cout << "  | Occupancy Rate     : " << setw(20) << fixed << setprecision(1) << occupancyRate << "%" << "  |\n";
    cout << "  | Check-Ins          : " << setw(22) << totalCheckIns     << " |\n";
    cout << "  | Check-Outs         : " << setw(22) << totalCheckOuts    << " |\n";
    cout << "  +-------------------------------------------+\n";
    cout << "  | Room Revenue    : PKR " << setw(20) << fixed << setprecision(2) << totalRoomRevenue    << " |\n";
    cout << "  | Service Revenue : PKR " << setw(20) << totalServiceRevenue                             << " |\n";
    cout << "  | Total Revenue   : PKR " << setw(20) << totalRevenue                                    << " |\n";
    cout << "  +-------------------------------------------+\n" << RESET;

    string performanceRating = "";
    string ratingColor       = "";

    if      (occupancyRate >= 80) { performanceRating = "EXCELLENT"; ratingColor = BRIGHT_GREEN;  }
    else if (occupancyRate >= 50) { performanceRating = "GOOD";      ratingColor = BRIGHT_YELLOW; }
    else if (occupancyRate >= 20) { performanceRating = "AVERAGE";   ratingColor = YELLOW;        }
    else                          { performanceRating = "POOR";      ratingColor = BRIGHT_RED;    }

    cout << ratingColor << "\n  Hotel Performance: " << performanceRating << "\n" << RESET;

    if (snakeHighScore > 0) {
        cout << BRIGHT_MAGENTA << "  Snake High Score : " << snakeHighScore << "\n" << RESET;
    }

    pressEnterToContinue();
}

// ============================================================
// REVENUE REPORT
// ============================================================
void generateRevenueReport() {
    system("cls");
    cout << MAGENTA << BOLD;
    printLine('=', 56);
    cout << "  FINANCIAL REVENUE REPORT\n" << RESET;
    printLine('=', 56);

    cout << BRIGHT_YELLOW << "\n  TRANSACTIONS:\n" << RESET;
    printLine('-', 54);

    cout << "  " << left
         << setw(12) << "Type"
         << setw(20) << "Guest"
         << setw(8)  << "Room"
         << "Amount(PKR)\n";

    printLine('-', 54);

    if (totalTransactions == 0) {
        cout << BRIGHT_RED << "  No transactions yet.\n" << RESET;
    }

    for (int index = 0; index < totalTransactions; index++) {
        if      (history[index].type == "CheckIn")  cout << GREEN;
        else if (history[index].type == "CheckOut") cout << BRIGHT_CYAN;
        else                                         cout << BRIGHT_MAGENTA;

        cout << "  " << left
             << setw(12) << history[index].type
             << setw(20) << history[index].guestName.substr(0, 18)
             << setw(8)  << history[index].roomNumber
             << fixed << setprecision(2) << history[index].amount
             << "\n" << RESET;
    }

    printLine('=', 56);

    float averageRevenuePerRoom  = (MAX_ROOMS > 0) ? totalRevenue / MAX_ROOMS : 0;
    float projectedMonthlyRevenue = totalRevenue * 30;
    float projectedYearlyRevenue  = totalRevenue * 365;

    cout << BRIGHT_YELLOW << "\n  BUSINESS ANALYTICS:\n";
    printLine('-', 42);
    cout << "  Total Revenue     : PKR " << fixed << setprecision(2) << totalRevenue             << "\n";
    cout << "  Avg / Room        : PKR " << averageRevenuePerRoom                                 << "\n";
    cout << "  Projected Monthly : PKR " << projectedMonthlyRevenue                               << "\n";
    cout << "  Projected Yearly  : PKR " << projectedYearlyRevenue                                << "\n";
    cout << "  Transactions      : "     << totalTransactions                                      << "\n" << RESET;

    pressEnterToContinue();
}

// ============================================================
// ADVANCE DAY
// ============================================================
void advanceDay() {
    dayNumber++;

    for (int index = 0; index < totalGuests; index++) {
        if (guests[index].isCheckedIn && guests[index].nightsStayed > 0) {
            guests[index].nightsStayed--;
        }
    }

    cout << BRIGHT_GREEN << "\n  [OK] Advanced to Day " << dayNumber << "\n" << RESET;
    Sleep(900);
}

// ============================================================
// HELPERS
// ============================================================
int findVacantRoom(string roomType) {
    for (int index = 0; index < MAX_ROOMS; index++) {
        if (rooms[index].type == roomType && !rooms[index].isOccupied) {
            return index;
        }
    }
    return -1;
}

int findGuestByRoom(int targetRoomNumber) {
    for (int index = 0; index < totalGuests; index++) {
        if (guests[index].roomNumber == targetRoomNumber && guests[index].isCheckedIn) {
            return index;
        }
    }
    return -1;
}

void recordTransaction(string transactionType, string guestName, int roomNumber, float amount, string description) {
    if (totalTransactions >= MAX_HISTORY) {
        return;
    }

    history[totalTransactions].type        = transactionType;
    history[totalTransactions].guestName   = guestName;
    history[totalTransactions].roomNumber  = roomNumber;
    history[totalTransactions].amount      = amount;
    history[totalTransactions].description = description;

    totalTransactions++;
}

void pressEnterToContinue() {
    cout << BRIGHT_YELLOW << "\n  Press Enter to continue..." << RESET;
    cin.ignore();
    cin.get();
}

// ============================================================
//   ____  _   _    _    _  _______
//  / ___|| \ | |  / \  | |/ / ____|
//  \___ \|  \| | / _ \ | ' /|  _|
//   ___) | |\  |/ ___ \| . \| |___
//  |____/|_| \_/_/   \_|_|\_\_____|
// ============================================================

void spawnFood() {
    // Place food randomly, not on snake
    while (true) {
        food.x = rand() % SNAKE_BOARD_WIDTH;
        food.y = rand() % SNAKE_BOARD_HEIGHT;

        bool foodIsOnSnake = false;

        for (int index = 0; index < snakeLength; index++) {
            if (snake[index].x == food.x && snake[index].y == food.y) {
                foodIsOnSnake = true;
                break;
            }
        }

        if (!foodIsOnSnake) {
            break;
        }
    }
}

void snakeInit() {
    snakeLength    = 4;
    snakeDirectionX = 1;
    snakeDirectionY = 0;
    snakeScore      = 0;
    snakeGameOver   = false;
    snakeLevel      = 1;

    // Start snake in the middle going right
    for (int index = 0; index < snakeLength; index++) {
        snake[index].x = SNAKE_BOARD_WIDTH  / 2 - index;
        snake[index].y = SNAKE_BOARD_HEIGHT / 2;
    }

    spawnFood();
}

// ============================================================
// DRAW SNAKE BOARD (redraws whole screen each frame)
// ============================================================
void drawSnake() {
    gotoxy(0, 0);

    // --- Top border + HUD ---
    cout << BRIGHT_YELLOW << BOLD;
    cout << "  +";
    for (int col = 0; col < SNAKE_BOARD_WIDTH; col++) {
        cout << "--";
    }
    cout << "-+\n";

    cout << "  | " << BRIGHT_GREEN  << "SCORE: " << setw(5) << snakeScore
         << BRIGHT_YELLOW           << "   LEVEL: " << snakeLevel
         << "   HI: " << BRIGHT_MAGENTA << setw(5) << snakeHighScore
         << BRIGHT_YELLOW;

    // Pad remaining space to fit the border width
    int paddingSpaces = SNAKE_BOARD_WIDTH * 2 - 30;
    for (int index = 0; index < paddingSpaces; index++) {
        cout << " ";
    }
    cout << " |\n";

    cout << "  +";
    for (int col = 0; col < SNAKE_BOARD_WIDTH; col++) {
        cout << "--";
    }
    cout << "-+\n" << RESET;

    // --- Board rows ---
    for (int row = 0; row < SNAKE_BOARD_HEIGHT; row++) {
        cout << BRIGHT_YELLOW << "  |" << RESET;

        for (int col = 0; col < SNAKE_BOARD_WIDTH; col++) {
            // Snake head
            if (col == snake[0].x && row == snake[0].y) {
                cout << BRIGHT_GREEN << "@@" << RESET;
            }
            else {
                // Check if this cell is snake body
                bool isBodySegment = false;
                for (int segmentIndex = 1; segmentIndex < snakeLength; segmentIndex++) {
                    if (snake[segmentIndex].x == col && snake[segmentIndex].y == row) {
                        isBodySegment = true;
                        break;
                    }
                }

                if (isBodySegment) {
                    cout << GREEN << "oo" << RESET;
                }
                // Food
                else if (food.x == col && food.y == row) {
                    cout << BRIGHT_RED << "()" << RESET;
                }
                // Empty cell
                else {
                    cout << "  ";
                }
            }
        }

        cout << BRIGHT_YELLOW << "|" << RESET << "\n";
    }

    // --- Bottom border ---
    cout << BRIGHT_YELLOW;
    cout << "  +";
    for (int col = 0; col < SNAKE_BOARD_WIDTH; col++) {
        cout << "--";
    }
    cout << "-+\n";
    cout << "  | Controls:  W = Up    S = Down    A = Left    D = Right    Q = Quit |\n";
    cout << "  +";
    for (int col = 0; col < SNAKE_BOARD_WIDTH; col++) {
        cout << "--";
    }
    cout << "-+\n" << RESET;
}

// ============================================================
// SNAKE MAIN LOOP
// ============================================================
void snakeLoop() {
    int pendingDirectionX = snakeDirectionX;
    int pendingDirectionY = snakeDirectionY;
    int frameDelayMs      = 180; // milliseconds per frame; decreases as level rises

    while (!snakeGameOver) {

        drawSnake();

        // --- Collect ALL keypresses buffered this frame ---
        while (_kbhit()) {
            char keyPressed = (char)_getch();

            if      (keyPressed == 'w' || keyPressed == 'W') {
                if (snakeDirectionY != 1)  { pendingDirectionX =  0; pendingDirectionY = -1; }
            }
            else if (keyPressed == 's' || keyPressed == 'S') {
                if (snakeDirectionY != -1) { pendingDirectionX =  0; pendingDirectionY =  1; }
            }
            else if (keyPressed == 'a' || keyPressed == 'A') {
                if (snakeDirectionX != 1)  { pendingDirectionX = -1; pendingDirectionY =  0; }
            }
            else if (keyPressed == 'd' || keyPressed == 'D') {
                if (snakeDirectionX != -1) { pendingDirectionX =  1; pendingDirectionY =  0; }
            }
            else if (keyPressed == 'q' || keyPressed == 'Q') {
                snakeGameOver = true;
                break;
            }
        }

        if (snakeGameOver) {
            break;
        }

        snakeDirectionX = pendingDirectionX;
        snakeDirectionY = pendingDirectionY;

        // --- Move snake: shift body backwards ---
        for (int segmentIndex = snakeLength - 1; segmentIndex > 0; segmentIndex--) {
            snake[segmentIndex] = snake[segmentIndex - 1];
        }

        // Move head forward
        snake[0].x += snakeDirectionX;
        snake[0].y += snakeDirectionY;

        // --- Wall collision ---
        if (snake[0].x < 0 || snake[0].x >= SNAKE_BOARD_WIDTH ||
            snake[0].y < 0 || snake[0].y >= SNAKE_BOARD_HEIGHT) {
            snakeGameOver = true;
            break;
        }

        // --- Self collision ---
        for (int segmentIndex = 1; segmentIndex < snakeLength; segmentIndex++) {
            if (snake[0].x == snake[segmentIndex].x && snake[0].y == snake[segmentIndex].y) {
                snakeGameOver = true;
                break;
            }
        }

        if (snakeGameOver) {
            break;
        }

        // --- Eat food ---
        if (snake[0].x == food.x && snake[0].y == food.y) {
            snakeLength++;
            snakeScore += 10 * snakeLevel;

            if (snakeScore > snakeHighScore) {
                snakeHighScore = snakeScore;
            }

            // Level up every 5 foods eaten
            int totalFoodsEaten = snakeLength - 4;  // started at length 4
            snakeLevel  = 1 + totalFoodsEaten / 5;
            frameDelayMs = 180 - (snakeLevel - 1) * 18;

            if (frameDelayMs < 60) {
                frameDelayMs = 60;
            }

            spawnFood();
        }

        Sleep(frameDelayMs);
    }
}

// ============================================================
// SNAKE ARCADE ENTRY
// ============================================================
void snakeArcade() {
    system("cls");
    showCursor();

    // Welcome screen
    cout << BRIGHT_GREEN << BOLD;
    cout << "\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |         GRAND VISTA HOTEL ARCADE                 |\n";
    cout << "  |                                                  |\n";
    cout << "  |           S N A K E   G A M E                   |\n";
    cout << "  |                                                  |\n";
    cout << "  |  @@  = Snake Head      oo = Snake Body           |\n";
    cout << "  |  ()  = Food (+10 pts x level)                   |\n";
    cout << "  |                                                  |\n";
    cout << "  |  Controls:  W / A / S / D  to move              |\n";
    cout << "  |             Q to quit anytime                   |\n";
    cout << "  |                                                  |\n";
    cout << "  |  Every 5 foods = Level Up = FASTER snake!       |\n";
    cout << "  |                                                  |\n";

    if (snakeHighScore > 0) {
        cout << "  |  Current High Score: " << setw(6) << snakeHighScore << "                      |\n";
    }

    cout << "  +--------------------------------------------------+\n";
    cout << RESET;

    char playAgainChoice = 'y';

    while (playAgainChoice == 'y' || playAgainChoice == 'Y') {
        cout << BRIGHT_YELLOW << "\n  Press Enter to start!\n" << RESET;
        cin.get();

        hideCursor();
        system("cls");

        snakeInit();
        snakeLoop();

        // Draw final frame after game over
        drawSnake();

        // Game over message
        gotoxy(2, SNAKE_BOARD_HEIGHT + 6);

        if (snakeScore >= snakeHighScore && snakeScore > 0) {
            cout << BRIGHT_YELLOW << BOLD << "  *** NEW HIGH SCORE: " << snakeScore << "! ***\n" << RESET;
        } else {
            cout << BRIGHT_RED    << BOLD << "  *** GAME OVER! ***\n"  << RESET;
            cout << "  Final Score : " << BRIGHT_YELLOW  << snakeScore     << RESET << "\n";
            cout << "  High Score  : " << BRIGHT_MAGENTA << snakeHighScore << RESET << "\n";
        }

        showCursor();
        cout << "\n  Play again? (y/n): ";
        cin >> playAgainChoice;
        cin.ignore();
        system("cls");
    }

    cout << BRIGHT_CYAN << "\n  Returning to Grand Vista HMS...\n" << RESET;
    Sleep(800);
}