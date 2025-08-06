#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <conio.h>

using namespace std;

string loggedInUsername = "";

// Helper function to read password with stars
int readPassword()
{
    string password = "";
    char ch;
    while ((ch = _getch()) != '\r')
    {  // Read until Enter is pressed
        if (ch == '\b')
        {  // If Backspace is pressed
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";  // Move back, print space, move back again
            }
        }
        else
        {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return stoi(password);  // Convert string to integer
}

class Ticket
{
public:
    string origin;
    string destination;
    string dateOfMoving;
    string hourOfMoving;
    string model;
    float price;
    int sitsAvailable;
    int id;


    Ticket()
        : origin("")
        ,destination("")
        ,dateOfMoving("")
        ,hourOfMoving("")
        ,model("")
        ,price(0.0)
        ,sitsAvailable(0)
        , id(0) {}

    Ticket(string ori ,
           string dest ,
           string dOfM, 
           string hOfM ,
           string mod ,
           float pri ,
           int sitsAva ,
           int id)
        // I had  move in every one of the lines below  like origin(move(ori)) but i am testing sth
        : origin(ori)
        , destination(dest)
        , dateOfMoving(dOfM)
        , hourOfMoving(hOfM)
        , model(mod)
        , price(pri)
        , sitsAvailable(sitsAva)
        , id(id) {}

};

class Place
{
public:

    string firstDay;
    string lastDay;
    int maxGuestNumber;
    bool isAvailable;
    float price;
    int id;


    Place()
        : firstDay("")
        ,lastDay("")
        ,maxGuestNumber(0)
        ,isAvailable(false)
        ,price(0.0)
        ,id(0) {}

    
    Place(string firstDay,
           string lastDay,
           int maxGuest,
           bool isAva ,
           float pri ,
           int id)
        : firstDay(firstDay)
        , lastDay(lastDay)
        , maxGuestNumber(maxGuest)
        , isAvailable(isAva)
        , price(pri)
        , id(id) {}
};

class User
{
public:
    string name;
    int password;
};

// Helper functions for file operations
void updateTransportAvailability(const string& filename, const string& id, int seatChange)
{
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cout << "Error: Could not open " << filename << " for reading.\n";
        return;
    }

    vector<string> lines;
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        vector<string> elements;
        string element;
        
        while (getline(ss, element, ','))
        {
            elements.push_back(element);
        }
        
        if (elements.size() >= 8 && elements[7] == id)
        {
            // update available seats
            int seats = stoi(elements[6]) + seatChange;
            elements[6] = to_string(seats);
            
            // reconstruct the line
            stringstream newLine;
            for (size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if (i < elements.size() - 1) newLine << ",";
            }
            lines.push_back(newLine.str());
        }else
        {
            lines.push_back(line);
        }
    }
    inFile.close();

    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cout << "Error: Could not open " << filename << " for writing.\n";
        return;
    }
    
    for (const auto& l : lines)
    {
        outFile << l << endl;
    }
    outFile.close();
}

void updateAccommodationAvailability(const string& filename, const string& id, bool makeAvailable)
{
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cout << "Error: Could not open " << filename << " for reading.\n";
        return;
    }

    vector<string> lines;
    string line;
    while (getline(inFile, line))
    {
        stringstream ss(line);
        vector<string> elements;
        string element;
        
        while (getline(ss, element, ','))
        {
            elements.push_back(element);
        }
        
        if (elements.size() >= 6 && elements[5] == id)
        {
            // Update availability
            elements[3] = makeAvailable ? "true" : "false";
            
            stringstream newLine;
            for (size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if (i < elements.size() - 1) newLine << ",";
            }
            lines.push_back(newLine.str());
        }else
        {
            lines.push_back(line);
        }
    }
    inFile.close();

    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cout << "Error: Could not open " << filename << " for writing.\n";
        return;
    }
    
    for (const auto& l : lines)
    {
        outFile << l << endl;
    }
    outFile.close();
}

struct Booking
{
    string username;
    string type;  // "Airplane", "Train", "Bus", "Hotel", "Villa"
    string id;
};

vector<Booking> readBookingFromFile(const string& filename)
{
    vector<Booking> bookings;
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        return bookings;
    }
    
    string line;
    while(getline(inFile,line))
    {
        stringstream ss(line);
        Booking b;

        getline(ss,b.username,',');
        getline(ss,b.type,',');
        getline(ss,b.id);

        bookings.push_back(b);
    }
    inFile.close();
    return bookings;
} 

void showUserBookings(const vector<Booking>& bookings, const string& username)
{
    cout << "\nYour bookings:\n";
    int count = 1;
    for (const auto& b : bookings)
    {
        if (b.username == username)
        {
            cout << count++ << ". Type: [" << b.type << "] ID: " << b.id << endl;
        }
    }
    if (count == 1)
        cout << "No bookings found.\n";
}

void returnBooking(vector<Booking>& bookings, const string& username, const string& id, const string& type)
{
    auto it = remove_if(bookings.begin(), bookings.end(),
        [&](const Booking& b)
        {
            return b.username == username && b.id == id && b.type == type;
        });
    
    if (it != bookings.end())
    {
        bookings.erase(it, bookings.end());
        
        // Update availability/seats in respective files
        if (type == "Airplane")
        {
            updateTransportAvailability("Airplane_info.txt", id, 1);
        }
        else if (type == "Train")
        {
            updateTransportAvailability("Train_info.txt", id, 1);
        }
        else if (type == "Bus")
        {
            updateTransportAvailability("Buss_info.txt", id, 1);
        }
        else if (type == "Hotel")
        {
            updateAccommodationAvailability("Hotel_info.txt", id, true);
        }
        else if (type == "Villa")
        {
            updateAccommodationAvailability("Villa_info.txt", id, true);
        }
        
        cout << "Booking cancelled successfully.\n";
    }
    else
    {
        cout << "Booking not found.\n";
    }
}

void writeBookingsToFile(const vector<Booking>& bookings, const string& filename)
{
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cout << "Error: Could not open bookings file for writing.\n";
        return;
    }
    
    for (const auto& b : bookings) {
        outfile << b.username << "," << b.type << "," << b.id << endl;
    }
    outfile.close();
}

class Airplane : public Ticket
{
public:
    Airplane(): Ticket() {}

    Airplane(string ori ,string dest ,string dOfM, string hOfM ,string mod ,float pri ,int sitsAva ,int id) 
           : Ticket( ori ,dest ,dOfM, hOfM ,mod ,pri ,sitsAva ,id) {}

    
};

class Train : public Ticket
{
    public:
    Train(): Ticket() {}
    
    Train(string ori ,string dest ,string dOfM, string hOfM ,string mod ,float pri ,int sitsAva ,int id) 
           : Ticket( ori ,dest ,dOfM, hOfM ,mod ,pri ,sitsAva, id) {}


};

class Buss : public Ticket
{
    public:
    Buss(): Ticket() {}
    
    Buss(string ori ,string dest ,string dOfM, string hOfM ,string mod ,float pri ,int sitsAva, int id) 
           : Ticket( ori ,dest ,dOfM, hOfM ,mod ,pri ,sitsAva,id) {}


};

class Villa : public Place
{
public:

    Villa() : Place() {}

    Villa(string firstDay ,string lastDay ,int maxGuest,bool isAva ,float pri ,int id)
        : Place( firstDay ,lastDay ,maxGuest,isAva ,pri ,id) {}



    
};

class Hotel : public Place
{
public:
    Hotel() : Place() {}

    Hotel(string firstDay ,string lastDay ,int maxGuest,bool isAva ,float pri ,int id)
        : Place( firstDay ,lastDay ,maxGuest,isAva ,pri ,id) {}

};

class RegisterSystem
{
public:
    User user;
    
    void registerUser()
    {
        cout << "Enter Your name: ";
        cin >> user.name;
        cout << "Enter your password: ";
        user.password = readPassword();

        ofstream user_information("user_info.txt",ios::app);
        if(!user_information.is_open())
        {
            cout << "Can not open file!!!" << endl;
            return;
        }
        user_information << user.name << "," << user.password << endl;
        user_information.close();
    }

    bool login(User &user)
    {
        string inputName;
        int inputPass;

        cout << "Enter Your name: ";
        cin >> inputName;
        cout << "Enter your password: ";
        inputPass = readPassword();

        ifstream user_information_in("user_info.txt");
        if (!user_information_in.is_open())
        {
            cout << "Can not open file!!!" << endl;
            return false;
        }

        string line;
        while (getline(user_information_in, line))
        {
            size_t pos1 = line.find(',');
            if (pos1 == string::npos)
                continue;

            string fileName = line.substr(0, pos1);
            //int filePass = stoi(line.substr(pos1 + 1));
            // testing
            string passStr = line.substr(pos1 + 1);
            int filePass = atoi(passStr.c_str());

            if (inputName == fileName && inputPass == filePass)
            {
                user.name = fileName;
                user.password = filePass;
                user_information_in.close();

                loggedInUsername = fileName; 

                return true;
            }
        }

        user_information_in.close();
        cout << "Invalid name or password." << endl;
        return false;
    }
};

class System
{

public:
void bookAirplane();
void bookTrain();
void bookBuss();
void bookHotel();
void bookVilla();

void addAirplane();
void addTrain();
void addBuss();
void addHotel();
void addVilla();

void seeAirplane();
void seeTrain();
void seeBuss();
void seeHotel();
void seeVilla();

void cancelBooking();
void viewMyBookings();

void menu()
{
    int intake;

    do
    {
        cout << "\t Welcome " << endl
             << "1.Book Airplane Ticket" << endl
             << "2.Airplane list" << endl
             << "3.Book Train Ticket" << endl
             << "4.Train list" << endl
             << "5.Book Buss Ticket" << endl
             << "6.Buss list" << endl
             << "7.Book Hotel" << endl
             << "8.Hotel list" << endl
             << "9.Book Villa" << endl
             << "10.Villa list" << endl
             << "11.Cancel Booking" << endl
             << "12.View My Bookings" << endl
             << "0.Sign Out" << endl;

        cin >> intake;
        switch (intake)
        {
        case 1:
            bookAirplane();
            break;
        case 2:
            seeAirplane();
            break;
        case 3:
            bookTrain();
            break;
        case 4:
            seeTrain();
            break;
        case 5:
            bookBuss();
            break;
        case 6:
            seeBuss();
            break;
        case 7:
            bookHotel();
            break;
        case 8:
            seeHotel();
            break;
        case 9:
            bookVilla();
            break;
        case 10:
            seeVilla();
            break;
        case 11:
            cancelBooking();
            break;
        case 12:
            viewMyBookings();
            break;
        case 0:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid input." << endl;
            break;
        }
    } while (intake != 0);
}

void adminMenu()
{
    int option;

    do
    {
        cout << "\t Admin Menu \t"<< endl
             << "1.Add Airplane Ticket" << endl
             << "2.Add Train Ticket" << endl
             << "3.Add Buss Ticket" << endl
             << "4.Add Hotel" << endl
             << "5.Add Villa" << endl
             << "6.Sign Out" << endl;

        cin >> option;
        if(option > 0 && option < 7)
        {
            switch (option)
            {
            case 1:
                addAirplane();
                break;
            case 2:
                addTrain();
                break;
            case 3:
                addBuss();
                break;
            case 4:
                addVilla();
                break;
            case 5:
                addVilla();
                break;
            case 6:
                cout << "Signing out!!!" << endl;
                break;
            
            default:
                cout << "Invalid input" << endl;
                break;
            }

        }
        else
        {
            cout << "Invalid input.";
        }
    } while (option != 6);
}

};

void System::bookAirplane()
{
    int inputPlaneId;
    cout << "Please enter airplane id ticket: ";
    cin >> inputPlaneId;

    ifstream airplaneDefault_in("Airplane_info.txt");
    if(!airplaneDefault_in.is_open())
    {
        cout << "Trouble in opening file!!!" << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool found = false;

    // Read all lines first
    while (getline(airplaneDefault_in, line))
    {
        istringstream iss(line);
        vector<string> elements;
        string element;
        
        while(getline(iss, element, ','))
        {
            elements.push_back(element);
        }

        if(elements.size() >= 8)
        {
            int ticketId = stoi(elements[7]);
            int seatsAvailable = stoi(elements[6]);

            if(ticketId == inputPlaneId && seatsAvailable > 0)
            {
                elements[6] = to_string(seatsAvailable - 1);
                found = true;

                // Record the booking
                ofstream bookingFile("Bookings.txt", ios::app);
                if(bookingFile.is_open()) {
                    bookingFile << loggedInUsername << ",Airplane," << ticketId << endl;
                    bookingFile.close();
                }
            }

            // Reconstruct the line
            stringstream newLine;
            for(size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if(i != elements.size() - 1)
                    newLine << ",";
            }
            lines.push_back(newLine.str());
        }
    }
    airplaneDefault_in.close();

    ofstream airplaneDefault_out("Airplane_info.txt");
    if(!airplaneDefault_out.is_open())
    {
        cout << "Error writing to file!!!" << endl;
        return;
    }

    for(const auto& l : lines)
    {
        airplaneDefault_out << l << endl;
    }
    airplaneDefault_out.close();

    if(found)
        cout << "Airplane Ticket booked successfully." << endl;
    else
        cout << "Airplane Ticket not found or no seats available." << endl;
}

void System::bookTrain()
{
    int inputTrainId;
    cout << "Please enter train id ticket: ";
    cin >> inputTrainId;

    ifstream trainDefault_in("Train_info.txt");
    if(!trainDefault_in.is_open())
    {
        cout << "Trouble in opening file!!!" << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool found = false;

    while (getline(trainDefault_in, line))
    {
        istringstream iss(line);
        vector<string> elements;
        string element;
        
        while(getline(iss, element, ','))
        {
            elements.push_back(element);
        }

        if(elements.size() >= 8)
        {
            int ticketId = stoi(elements[7]);
            int seatsAvailable = stoi(elements[6]);

            if(ticketId == inputTrainId && seatsAvailable > 0)
            {
                elements[6] = to_string(seatsAvailable - 1);
                found = true;

        
                ofstream bookingFile("Bookings.txt", ios::app);
                if(bookingFile.is_open())
                {
                    bookingFile << loggedInUsername << ",Train," << ticketId << endl;
                    bookingFile.close();
                }
            }

            stringstream newLine;
            for(size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if(i != elements.size() - 1)
                    newLine << ",";
            }
            lines.push_back(newLine.str());
        }
    }
    trainDefault_in.close();

    ofstream trainDefault_out("Train_info.txt");
    if(!trainDefault_out.is_open())
    {
        cout << "Error writing to file!!!" << endl;
        return;
    }

    for(const auto& l : lines)
    {
        trainDefault_out << l << endl;
    }
    trainDefault_out.close();

    if(found)
        cout << "Train Ticket booked successfully." << endl;
    else
        cout << "Train Ticket not found or no seats available." << endl;
}

void System::bookBuss()
{
    int inputBussId;
    cout << "Please enter bus id ticket: ";
    cin >> inputBussId;

    ifstream bussDefault_in("Buss_info.txt");
    if(!bussDefault_in.is_open())
    {
        cout << "Trouble in opening file!!!" << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool found = false;

    // Read all lines first
    while (getline(bussDefault_in, line))
    {
        istringstream iss(line);
        vector<string> elements;
        string element;
        
        while(getline(iss, element, ','))
        {
            elements.push_back(element);
        }

        if(elements.size() >= 8)
        {
            int ticketId = stoi(elements[7]);
            int seatsAvailable = stoi(elements[6]);

            if(ticketId == inputBussId && seatsAvailable > 0)
            {
                elements[6] = to_string(seatsAvailable - 1);
                found = true;

                ofstream bookingFile("Bookings.txt", ios::app);
                if(bookingFile.is_open())
                {
                    bookingFile << loggedInUsername << ",Bus," << ticketId << endl;
                    bookingFile.close();
                }
            }

            stringstream newLine;
            for(size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if(i != elements.size() - 1)
                    newLine << ",";
            }
            lines.push_back(newLine.str());
        }
    }
    bussDefault_in.close();

    ofstream bussDefault_out("Buss_info.txt");
    if(!bussDefault_out.is_open())
    {
        cout << "Error writing to file!!!" << endl;
        return;
    }

    for(const auto& l : lines)
    {
        bussDefault_out << l << endl;
    }
    bussDefault_out.close();

    if(found)
        cout << "Bus Ticket booked successfully." << endl;
    else
        cout << "Bus Ticket not found or no seats available." << endl;
}    

void System::bookVilla()
{
    int inputVillaId;
    cout << "Please enter Villa id: ";
    cin >> inputVillaId;

    ifstream villaDefault_in("Villa_info.txt");
    if(!villaDefault_in.is_open())
    {
        cout << "Trouble in opening file!!!" << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool found = false;

    while (getline(villaDefault_in, line))
    {
        istringstream iss(line);
        vector<string> elements;
        string element;
        
        while(getline(iss, element, ','))
        {
            elements.push_back(element);
        }

        if(elements.size() >= 6)
        {
            int villaId = stoi(elements[5]);
            string availability = elements[3];

            if(villaId == inputVillaId && availability == "true")
            {
                elements[3] = "false";
                found = true;

                ofstream bookingFile("Bookings.txt", ios::app);
                if(bookingFile.is_open())
                {
                    bookingFile << loggedInUsername << ",Villa," << villaId << endl;
                    bookingFile.close();
                }
            }

            stringstream newLine;
            for(size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if(i != elements.size() - 1)
                    newLine << ",";
            }
            lines.push_back(newLine.str());
        }
    }
    villaDefault_in.close();

    // Write all lines back
    ofstream villaDefault_out("Villa_info.txt");
    if(!villaDefault_out.is_open())
    {
        cout << "Error writing to file!!!" << endl;
        return;
    }

    for(const auto& l : lines)
    {
        villaDefault_out << l << endl;
    }
    villaDefault_out.close();

    if(found)
        cout << "Villa booked successfully." << endl;
    else
        cout << "Villa not found or it is unavailable." << endl;
}

void System::bookHotel()
{
    int inputHotelId;
    cout << "Please enter Hotel id: ";
    cin >> inputHotelId;

    ifstream hotelDefault_in("Hotel_info.txt");
    if(!hotelDefault_in.is_open())
    {
        cout << "Trouble in opening file!!!" << endl;
        return;
    }

    vector<string> lines;
    string line;
    bool found = false;

    while (getline(hotelDefault_in, line))
    {
        istringstream iss(line);
        vector<string> elements;
        string element;
        
        while(getline(iss, element, ','))
        {
            elements.push_back(element);
        }

        if(elements.size() >= 6)
        {
            int hotelId = stoi(elements[5]);
            string availability = elements[3];

            if(hotelId == inputHotelId && availability == "true")
            {
                elements[3] = "false";
                found = true;

                ofstream bookingFile("Bookings.txt", ios::app);
                if(bookingFile.is_open())
                {
                    bookingFile << loggedInUsername << ",Hotel," << hotelId << endl;
                    bookingFile.close();
                }
            }

            stringstream newLine;
            for(size_t i = 0; i < elements.size(); i++)
            {
                newLine << elements[i];
                if(i != elements.size() - 1)
                    newLine << ",";
            }
            lines.push_back(newLine.str());
        }
    }
    hotelDefault_in.close();

    ofstream hotelDefault_out("Hotel_info.txt");
    if(!hotelDefault_out.is_open())
    {
        cout << "Error writing to file!!!" << endl;
        return;
    }

    for(const auto& l : lines)
    {
        hotelDefault_out << l << endl;
    }
    hotelDefault_out.close();

    if(found)
        cout << "Hotel booked successfully." << endl;
    else
        cout << "Hotel not found or it is not available." << endl;
}

void System::addAirplane()
{
    string origin;
    string destination;
    string dateOfMoving;
    string hourOfMoving;
    string model;
    float price;
    int sitsAvailable;
    int id;

    cout << "Enter the starting point: ";
    cin >> origin;
    cout << "Enter the destination: ";
    cin >> destination;
    cout << "Enter the day of takeoff: ";
    cin >> dateOfMoving;
    cout << "Enter the time of takeoff: ";
    cin >> hourOfMoving;
    cout << "Enter the airplane model: ";
    cin >> model;
    cout << "Enter the ticket price: ";
    cin >> price;
    cout << "Enter the number of sits available: ";
    cin >> sitsAvailable;
    cout << "Enter the airplane Id: ";
    cin >> id;

    ofstream Airplane_in("Airplane_info.txt",ios::app);

    if(!Airplane_in.is_open())
    {
        cout << "Can not open file!!!" << endl;
        return;
    }

    Airplane_in << origin << "," << destination << "," << dateOfMoving << "," 
                << hourOfMoving << "," << model << "," << price << "," << sitsAvailable << "," << id << endl;
    
    Airplane_in.close();
    cout << "Info added successfully" << endl;
    //test
    return;
}

void System::addTrain()
{
    string origin;
    string destination;
    string dateOfMoving;
    string hourOfMoving;
    string model;
    float price;
    int sitsAvailable;
    int id;

    cout << "Enter the starting point: ";
    cin >> origin;
    cout << "Enter the destination: ";
    cin >> destination;
    cout << "Enter the day of takeoff: ";
    cin >> dateOfMoving;
    cout << "Enter the time of takeoff: ";
    cin >> hourOfMoving;
    cout << "Enter the airplane model: ";
    cin >> model;
    cout << "Enter the ticket price: ";
    cin >> price;
    cout << "Enter the number of sits available: ";
    cin >> sitsAvailable;
    cout << "Enter the airplane Id: ";
    cin >> id;

    ofstream Train_in("Train_info.txt",ios::app);

    if(!Train_in.is_open())
    {
        cout << "Can not open file!!!";
        return;
    }

    Train_in << origin << "," << destination << "," << dateOfMoving << "," 
                << hourOfMoving << "," << model << "," << price << "," << sitsAvailable << "," << id << endl;
    
    Train_in.close();
    cout << "Info added successfully" << endl;

}

void System::addBuss()
{
    string origin;
    string destination;
    string dateOfMoving;
    string hourOfMoving;
    string model;
    float price;
    int sitsAvailable;
    int id;

    cout << "Enter the starting point: ";
    cin >> origin;
    cout << "Enter the destination: ";
    cin >> destination;
    cout << "Enter the day of takeoff: ";
    cin >> dateOfMoving;
    cout << "Enter the time of takeoff: ";
    cin >> hourOfMoving;
    cout << "Enter the airplane model: ";
    cin >> model;
    cout << "Enter the ticket price: ";
    cin >> price;
    cout << "Enter the number of sits available: ";
    cin >> sitsAvailable;
    cout << "Enter the airplane Id: ";
    cin >> id;

    ofstream Buss_in("Buss_info.txt",ios::app);

    if(!Buss_in.is_open())
    {
        cout << "Can not open file!!!";
        return;
    }

    Buss_in << origin << "," << destination << "," << dateOfMoving << "," 
                << hourOfMoving << "," << model << "," << price << "," << sitsAvailable << "," << id << endl;
    
    Buss_in.close();
    cout << "Info added successfully" << endl;

}

void System::addHotel()
{
    string firstDay;
    string lastDay;
    int maxGuestNumber;
    bool isAvailable;
    float price;
    int id;

    cout << "Enter the first day that it is available (YYYY/MM/DD): ";
    cin >> firstDay;
    cout << "Enter the last day that it is available (YYYY/MM/DD): ";
    cin >> lastDay;
    cout << "Enter the Max number of guests: ";
    cin >> maxGuestNumber;
    cout << "Enter the price per night: ";
    cin >> price;
    isAvailable = true;  // New hotels are always available initially
    cout << "Enter the Hotel Id: ";
    cin >> id;

    ofstream Hotel_in("Hotel_info.txt", ios::app);
    if(!Hotel_in.is_open())
    {
        cout << "Can not open file!!!" << endl;
        return;
    }

    Hotel_in << firstDay << "," << lastDay << "," << maxGuestNumber << "," 
             << (isAvailable ? "true" : "false") << "," << price << "," << id << endl;
    
    Hotel_in.close();
    cout << "Hotel info added successfully" << endl;
}

void System::addVilla()
{
    string firstDay;
    string lastDay;
    int maxGuestNumber;
    bool isAvailable;
    float price;
    int id;

    cout << "Enter the first day that it is available (YYYY/MM/DD): ";
    cin >> firstDay;
    cout << "Enter the last day that it is available (YYYY/MM/DD): ";
    cin >> lastDay;
    cout << "Enter the Max number of guests: ";
    cin >> maxGuestNumber;
    cout << "Enter the price per night: ";
    cin >> price;
    isAvailable = true;  // New villas are always available initially
    cout << "Enter the Villa Id: ";
    cin >> id;

    ofstream Villa_in("Villa_info.txt", ios::app);
    if(!Villa_in.is_open())
    {
        cout << "Can not open file!!!" << endl;
        return;
    }

    Villa_in << firstDay << "," << lastDay << "," << maxGuestNumber << "," 
             << (isAvailable ? "true" : "false") << "," << price << "," << id << endl;
    
    Villa_in.close();
    cout << "Villa info added successfully" << endl;
}

void System::seeAirplane()
{
    ifstream Airplane_info_in("Airplane_info.txt");

    string line;
    while(getline(Airplane_info_in,line))
    {
        cout << line << endl;
    }
    Airplane_info_in.close();
}

void System::seeTrain()
{
    ifstream Train_info_in("Train_info.txt");

    string line;
    while(getline(Train_info_in,line))
    {
        cout << line << endl;
    }
    Train_info_in.close();
}

void System::seeBuss()
{
    ifstream Buss_info_in("Buss_info.txt");

    string line;
    while(getline(Buss_info_in,line))
    {
        cout << line << endl;
    }
    Buss_info_in.close();
}

void System::seeVilla()
{
    ifstream Villa_info_in("Villa_info.txt");

    string line;
    while(getline(Villa_info_in,line))
    {
        cout << line << endl;
    }
    Villa_info_in.close();
}

void System::seeHotel()
{
    ifstream Hotel_info_in("Hotel_info.txt");

    string line;
    while(getline(Hotel_info_in,line))
    {
        cout << line << endl;
    }
    Hotel_info_in.close();
}

void System::cancelBooking()
{
    vector<Booking> bookings = readBookingFromFile("Bookings.txt");
    showUserBookings(bookings, loggedInUsername);
    
    if (bookings.empty())
    {
        return;
    }
    
    string id;
    string type;
    cout << "\nEnter the ID of the booking you want to cancel: ";
    cin >> id;
    cout << "Enter the type (Airplane/Train/Bus/Hotel/Villa): ";
    cin >> type;
    
    returnBooking(bookings, loggedInUsername, id, type);
    writeBookingsToFile(bookings, "Bookings.txt");
}

void System::viewMyBookings()
{
    vector<Booking> bookings = readBookingFromFile("Bookings.txt");
    showUserBookings(bookings, loggedInUsername);
}

int main()
{
    Airplane airDefault("Esfahan","booshehr","1404/02/25","16:00","Airbus",2500.0,40,1);
    Airplane airDefault2("Esfahan","Tehran","1404/02/23","22:00","Booing",3390.0,40,2);

    ofstream airplaneDefault("Airplane_info.txt",ios::app);

    if(airplaneDefault.is_open())
    {
        airplaneDefault << airDefault.origin << ","
                        << airDefault.destination
                        << "," << airDefault.dateOfMoving
                        << "," << airDefault.hourOfMoving
                        << "," << airDefault.model
                        << "," << airDefault.price
                        << "," << airDefault.sitsAvailable
                        << "," << airDefault.id << endl;

        airplaneDefault << airDefault2.origin << ","
                        << airDefault2.destination
                        << "," << airDefault2.dateOfMoving
                        << "," << airDefault2.hourOfMoving
                        << "," << airDefault2.model
                        << "," << airDefault2.price
                        << "," << airDefault2.sitsAvailable 
                        << "," << airDefault2.id << endl;

        airplaneDefault.close();
    }
    else
    {
        cout << "Can not set the default tickets because the file is not open!";
    }

    Train trainDefault("Esfahan","Mashhad","1404/02/26","17:00","Fadak",1300.0,80,10);
    Train trainDefault2("Esfahan","Tabriz","1404/02/28","22:00","Sabz",390.0,60,20);

    ofstream trainDefaultFile("Train_info.txt",ios::app);

    if(trainDefaultFile.is_open())
    {
        trainDefaultFile << trainDefault.origin << ","
                        << trainDefault.destination
                        << "," << trainDefault.dateOfMoving
                        << "," << trainDefault.hourOfMoving
                        << "," << trainDefault.model
                        << "," << trainDefault.price
                        << "," << trainDefault.sitsAvailable
                        << "," << trainDefault.id << endl;

        trainDefaultFile << trainDefault2.origin << ","
                        << trainDefault2.destination
                        << "," << trainDefault2.dateOfMoving
                        << "," << trainDefault2.hourOfMoving
                        << "," << trainDefault2.model
                        << "," << trainDefault2.price
                        << "," << trainDefault2.sitsAvailable
                        << "," << trainDefault2.id << endl;

        trainDefaultFile.close();
    }
    else
    {
        cout << "Can not set the default tickets because the file is not open!";
    }

    Buss bussDefault("Esfahan","Hamedan","1404/02/29","08:00","Maral",420.0,24,100);
    Buss bussDefault2("Esfahan","Ahvaz","1404/02/22","22:00","Man",630.0,24,200);

    ofstream bussDefaultFile("Buss_info.txt",ios::app);

    if(bussDefaultFile.is_open())
    {
        bussDefaultFile << bussDefault.origin << ","
                        << bussDefault.destination
                        << "," << bussDefault.dateOfMoving
                        << "," << bussDefault.hourOfMoving
                        << "," << bussDefault.model
                        << "," << bussDefault.price
                        << "," << bussDefault.sitsAvailable
                        << "," << bussDefault.id << endl;

        bussDefaultFile << bussDefault2.origin << ","
                        << bussDefault2.destination
                        << "," << bussDefault2.dateOfMoving
                        << "," << bussDefault2.hourOfMoving
                        << "," << bussDefault2.model
                        << "," << bussDefault2.price
                        << "," << bussDefault2.sitsAvailable
                        << "," << bussDefault2.id << endl;

        bussDefaultFile.close();
    }
    else
    {
        cout << "Can not set the default tickets because the file is not open!";
    }

    Villa villaDefault("1404/03/07","1404/03/11",10,true,2100,1000);
    Villa villaDefault2("1404/03/10","1404/03/22",15,true,2500,2000);

    ofstream villaDefaultFile("Villa_info.txt",ios::app);

    if(villaDefaultFile.is_open())
    {
        villaDefaultFile << villaDefault.firstDay << ","
                        << villaDefault.lastDay << ","
                        << villaDefault.maxGuestNumber << ","
                        << (villaDefault.isAvailable ? "true" : "false") << ","
                        << villaDefault.price << ","
                        << villaDefault.id << endl;

        villaDefaultFile << villaDefault2.firstDay << ","
                        << villaDefault2.lastDay << ","
                        << villaDefault2.maxGuestNumber << ","
                        << (villaDefault2.isAvailable ? "true" : "false") << ","
                        << villaDefault2.price << ","
                        << villaDefault2.id << endl;
        villaDefaultFile.close();
    }
    else
    {
        cout << "Can not set the default villa data because the file is not open!" << endl;
    }

    Hotel hotelDefault("1404/03/12","1404/03/16",10,true,3800,10000);
    Hotel hotelDefault2("1404/03/14","1404/03/28",15,true,2900,20000);

    ofstream hotelDefaultFile("Hotel_info.txt",ios::app);

    if(hotelDefaultFile.is_open())
    {
        hotelDefaultFile << hotelDefault.firstDay << ","
                        << hotelDefault.lastDay << ","
                        << hotelDefault.maxGuestNumber << ","
                        << (hotelDefault.isAvailable ? "true" : "false") << ","
                        << hotelDefault.price << ","
                        << hotelDefault.id << endl;

        hotelDefaultFile << hotelDefault2.firstDay << ","
                        << hotelDefault2.lastDay << ","
                        << hotelDefault2.maxGuestNumber << ","
                        << (hotelDefault2.isAvailable ? "true" : "false") << ","
                        << hotelDefault2.price << ","
                        << hotelDefault2.id << endl;
        hotelDefaultFile.close();
    }
    else
    {
        cout << "Can not set the default hotel data because the file is not open!" << endl;
    }

    RegisterSystem regsys;
    System sys;

    int input;
    do
    {
        
        cout << "Wellcom to ALI BABA App" << endl
             << "1.Sign Up" << endl
             << "2.Log In" << endl
             << "3.Admin" << endl
             << "4.Exit" << endl;

        cin >> input;
        switch (input)
        {
        case 1:
            regsys.registerUser();
            break;
        case 2:
            if(regsys.login(regsys.user))
            {
                sys.menu();
            }
            break;
        case 3:
            int adminPass;
            cout << "Enter the admin password: ";
            adminPass = readPassword();
            if(adminPass == 1234)
            {
                sys.adminMenu();
            }
            else
            {
                cout << "Invalid admin password!" << endl;
            }
            break;
        case 4:
            cout << "Exiting..." << endl;
            break;
        default:
        cout << "Invalid input" << endl;
            break;
        }
    } while (input != 4);
    

    return 0;
}