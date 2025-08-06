#  Ali Baba Booking System

Ali Baba Booking System is a modern C++ console application for booking and managing reservations for transportation (airplanes, trains, buses) and accommodations (hotels, villas). It features a clear, object-oriented structure and persistent data management via simple text files. The project is an excellent demonstration of practical C++ skills, software design, and user/admin flows.

---

## Features

- **User Authentication**
  - Register and log in with password-masked input.
- **Booking & Management**
  - Book and cancel airplane, train, bus, hotel, and villa reservations.
  - View all your bookings.
  - Automatic seat/availability management.
- **Admin Portal**
  - Add new tickets and accommodations through a protected admin menu.
- **Persistent Data**
  - All records managed via plain `.txt` files—no database required.
- **Object-Oriented Design**
  - Uses C++ classes, inheritance, and encapsulation for clarity and extensibility.

---

## Usage

1. **Compile the Program**

    ```bash
    g++ "TicketCLI.cpp" -o TicketCLI
    ```

2. **Run the Application**

    ```bash
    TicketCLI.exe
    ```

3. **Sign Up / Log In**
    - Choose to create a new account or log in as an existing user.
    - Passwords are hidden as you type (Windows only).

4. **User Menu**
    - Book tickets (Airplane, Train, Bus).
    - Book accommodations (Hotel, Villa).
    - View or cancel your bookings.

5. **Admin Menu**
    - Enter the admin password (`1234`) for access.
    - Add new tickets or accommodations to the system.

6. **Data Files**
    - All information is automatically saved and updated in `.txt` files.

---

## Project Structure

- `TicketCLI.cpp` – Main application source code.
- `Airplane_info.txt` – Stores airplane ticket data.
- `Train_info.txt` – Stores train ticket data.
- `Buss_info.txt` – Stores bus ticket data.
- `Hotel_info.txt` – Stores hotel room data.
- `Villa_info.txt` – Stores villa data.
- `Bookings.txt` – Records all user bookings.
- `user_info.txt` – Stores registered user credentials.

---

## How It Works

- **User Registration & Login:**  
  Users create an account or log in with their name and password (input is masked for privacy).

- **Booking & Cancellation:**  
  Users select available tickets or accommodations and book them. Seats and availability are updated in real time. Cancellations restore availability.

- **Admin Functionality:**  
  An admin can add new tickets or accommodations by entering the admin menu (protected by password). New records are appended to the respective data files.

- **Persistent Storage:**  
  All app data is stored in simple `.txt` files, making the system easy to understand and modify, and suitable for learning or extension.

- **Security Note:**  
  Passwords are stored as plaintext for demonstration purposes—do not use for production without adding secure password storage!

---



> Created by [Erfan Atabzadeh]  
