# RFID-Based-Secure-Electronic-Voting-System
# SecureBallot – RFID-Based Secure Electronic Voting System

## 📌 Project Overview

**SecureBallot** is an embedded electronic voting system developed using the **LPC2148 ARM7 microcontroller and Embedded C**. It uses RFID technology to identify registered voters and provides password-based authentication, voting-time validation, vote counting, duplicate-voting prevention, and election officer controls.

The system integrates an RFID reader, LCD, keypad, RTC, EEPROM, and UART communication to manage the voting process and maintain election audit information.

## 🎯 Objectives

- Authenticate registered voters using RFID cards.
- Provide an additional password-verification mechanism.
- Prevent duplicate voting using voter-status tracking.
- Allow voting only during the authorized voting period.
- Store voting configuration and vote counts in non-volatile EEPROM.
- Provide an officer menu to control voting and view results.
- Send system events and audit information to a PC through serial communication.
- Preserve ballot secrecy by not associating the selected party with the voter's RFID ID in the audit log.

## 🛠️ Hardware Requirements

| Component | Purpose |
|---|---|
| LPC2148 ARM7 Microcontroller | Main controller for the voting system |
| RFID Reader and RFID Cards | Voter identification |
| 20×4 LCD | Displaying menus, messages, and results |
| 4×4 Matrix Keypad | Password entry and menu selection |
| AT24C256 EEPROM | Storing voter information, voting configuration, status, and vote counts |
| Real-Time Clock (RTC) | Maintaining date and time |
| LEDs / Buzzer | Indicating success, errors, and access denial |
| MAX232 | Serial voltage-level interfacing where required |
| USB-to-UART Converter | Connecting the controller to a PC serial terminal |

## 💻 Software Requirements

- Embedded C
- Keil µVision IDE / Keil C compiler
- Flash Magic
- Serial terminal application
- LPC2148 device support files and peripheral drivers

## ⚙️ Technologies and Interfaces

- **Microcontroller:** LPC2148 ARM7
- **Programming Language:** Embedded C
- **RFID Communication:** UART1 at 9600 baud
- **PC Communication:** UART0
- **EEPROM Communication:** I²C
- **User Interface:** 20×4 LCD and 4×4 keypad
- **Time Management:** RTC
- **Development Tools:** Keil µVision and Flash Magic

## 🔄 System Working Principle

### 1. System Initialization

When the system is powered ON, the microcontroller initializes the LCD, keypad, UART interfaces, I²C, EEPROM, RTC, and serial logging modules. The LCD displays the project name and then shows **"Waiting for Card"**.

### 2. RFID Card Detection

When a voter presents an RFID card, the reader transmits the card ID to the LPC2148 through UART1.

The UART1 receive interrupt is used to receive the RFID data frame. The controller extracts the card ID and checks whether it belongs to a registered voter.

### 3. Voting-Time Validation

The controller reads the voting enable flag and configured voting start and end times from EEPROM. It compares the configured period with the current RTC time.

If voting is disabled or outside the permitted time, access is denied and the system returns to the waiting state.

### 4. Voter Authentication

If the voting period is valid, the controller checks the RFID ID against the registered voter records.

An invalid card is rejected. An authorized voter proceeds to the voter menu.

### 5. Password Verification

The voter selects the voting option and enters a password using the keypad. The LCD displays asterisks instead of the actual password.

If the password is correct, the party selection menu is displayed. Otherwise, the system rejects the voting operation.

### 6. Vote Recording

After the voter selects a party, the corresponding vote count is read from EEPROM, incremented, and written back to EEPROM.

The LCD displays **"Vote Casted Successfully"**, and the green LED provides confirmation. Voter status is maintained to prevent duplicate voting.

### 7. Election Officer Mode

The authorized officer can access the following operations:

- Set voting start and end times
- Start voting
- Stop voting
- View party-wise results
- Reset voting data
- Edit RTC date and time
- Exit the officer menu

Officer authentication is required for protected operations.

### 8. PC-Based Audit Logging

UART0 provides a separate serial communication channel for sending system and administrative events to a PC.

Examples of audit events include:

- `SYSTEM_START`
- `CARD_SCAN`
- `AUTHENTICATION_FAILED`
- `VOTING_ACCESS_BLOCKED`
- `DUPLICATE_VOTING_ATTEMPT`
- `VOTE_CAST_SUCCESS`
- `START_VOTING`
- `STOP_VOTING`
- `VIEW_RESULT`
- `RESET_VOTING`

The PC-side serial logger can display received events and save them in a CSV or text file with date and time information. The audit log should not record the selected party alongside the voter's RFID ID.

## 🧩 System Block Diagram

```text
             RFID Reader
                  |
                UART1
                  |
                  v
          +----------------+
          |                |
          |    LPC2148     |------ LCD (20×4)
          | ARM7 Controller|------ 4×4 Keypad
          |                |------ LEDs / Buzzer
          +----------------+
             |          |
            I²C        UART0
             |          |
             v          v
       AT24C256       MAX232 /
        EEPROM        USB-UART
             |          |
             v          v
       Voting Data   PC Serial Logger

          RTC
           |
           v
     Time Validation
```

*Note: This is a conceptual block diagram. Actual pin connections depend on the hardware configuration.*

## 🔐 Security Features

- RFID-based registered voter identification
- Password-based voter authentication
- Duplicate-voting prevention through voter-status tracking
- Voting-time validation using RTC and stored configuration
- Officer authentication for election administration
- Audit logging of authentication and administrative events
- Ballot-secrecy-conscious logging

**Note:** This project is an educational embedded-system prototype. It should not be considered ready for real public elections without independent security, privacy, reliability, and election-integrity testing.

## 📂 Suggested Project Structure

```text
SecureBallot/
├── main.c
├── delay.c
├── delay.h
├── lcd.c
├── lcd.h
├── keypad.c
├── keypad.h
├── uart0.c
├── uart0.h
├── uart1.c
├── uart1.h
├── i2c.c
├── i2c.h
├── eeprom.c
├── eeprom.h
├── rtc.c
├── rtc.h
├── rfid.c
├── rfid.h
├── interrupt.c
├── interrupt.h
├── voter.c
├── voter.h
├── voting.c
├── voting.h
├── officer.c
├── officer.h
└── README.md
```

*This is a suggested organization. Include only the source files and modules that actually exist in your repository.*

## 🚀 Setup and Execution

1. Install Keil µVision and the LPC2148 device support package.
2. Create or open the SecureBallot project.
3. Add the required source and header files to the project.
4. Configure the controller clock, peripheral pins, UART baud rates, and interrupt settings according to the hardware connections.
5. Build the project and resolve any compilation or linking errors.
6. Connect the LPC2148 to the required peripherals.
7. Program the generated HEX file into the microcontroller using Flash Magic.
8. Open a PC serial terminal and select the correct COM port and baud rate for UART0 logging.
9. Initialize the RTC and configure the voting settings and registered voter records in EEPROM.
10. Test the system using authorized cards, invalid cards, incorrect passwords, duplicate-card attempts, and voting outside the permitted time.

**Important:** Pin mappings, EEPROM address assignments, voter credentials, officer credentials, and party names must match the actual source code and hardware configuration. The project description does not define all these implementation-specific values.

## 🧪 Suggested Testing

| Test Case | Expected Behavior |
|---|---|
| Registered RFID card during valid voting hours | Continue to voter authentication |
| Unregistered RFID card | Reject authentication |
| Voting disabled | Deny voting access |
| Current time outside voting period | Deny voting access |
| Incorrect voter password | Reject voting operation |
| Voter who has already voted | Reject duplicate voting attempt |
| Correct password and party selection | Update the selected party's vote count |
| Authorized officer operation | Perform the selected administrative function |
| UART0 serial connection | Display available audit events on the PC |

## 📚 Learning Outcomes

This project provides practical learning in:

- Embedded C programming
- LPC2148 ARM7 microcontroller architecture
- UART communication and interrupt handling
- RFID interfacing and serial data-frame processing
- I²C protocol and external EEPROM interfacing
- RTC-based time management
- LCD and matrix-keypad interfacing
- Non-volatile data storage and vote-count management
- Peripheral integration and embedded-system debugging

## 👨‍💻 Project Summary

**SecureBallot** demonstrates how a microcontroller-based system can combine RFID identification, password verification, RTC-based voting-time validation, EEPROM storage, and serial audit logging to implement an electronic voting workflow.

**Technologies:** LPC2148 | Embedded C | RFID | UART | UART Interrupts | I²C | AT24C256 EEPROM | RTC | LCD | Keypad | Keil µVision | Flash Magic
