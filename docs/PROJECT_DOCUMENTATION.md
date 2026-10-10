# SecureBallot — Detailed Technical Documentation

This document is the deep-dive companion to the top-level [`README.md`](../README.md). Where the README explains *what* and *why*, this file focuses on *how the code actually works*: the architecture, the role of every source file, the interface/data flows, the workflows, EEPROM/RTC usage, the logging flow, and known limitations.

> **Target:** NXP LPC2148 (ARM7TDMI-S), built with the Keil/ARM toolchain (inferred from `LPC21xx.h` / `lpc214x.h` and the `__irq` keyword).
> **Status:** Educational prototype. Not compiled or hardware-tested in the authoring environment of this repository.

---

## 1. Overall Architecture

SecureBallot is a single-threaded, interrupt-assisted embedded application:

```
                 +---------------------------+
   RFID card --> | UART1 + receive ISR       |---> rfidBuf[9], flag
                 | (uart1int.c)              |
                 +---------------------------+
                              |
                              v
   +=========================================================+
   |                     main() loop                          |
   |  (secure_ballot_system_main.c)                           |
   |   - init peripherals                                     |
   |   - first-run EEPROM seeding                             |
   |   - wait for flag -> Authenticate() -> branch by role    |
   +=========================================================+
        |                        |                      |
        v                        v                      v
  OfficerMenu             VoterMenu               Invalid card
  (officer.c)             (voter.c)               -> log + show
        |                        |
        v                        v
  admin actions            voting guards -> CastVote
  (EEPROM writes)          (EEPROM read/write)
        |                        |
        +-------> LogEvent() <---+           (logger.c)
                     |
                     v
                 UART0  --->  PC (CSV audit log)
```

Persistent state lives in the **AT24C256 EEPROM** over I²C. Time comes from the **on-chip RTC**. The UI is the **20x4 LCD** plus the **4x4 keypad**. The only asynchronous event is the **UART1 receive interrupt** that captures RFID card frames.

Key global variables shared between the ISR and `main()` (declared `volatile`):

| Variable | File | Purpose |
|----------|------|---------|
| `rfidBuf[9]` | `uart1int.c` | Holds the 8 received ID characters + terminator |
| `ind` | `uart1int.c` | Write index into the buffer (legacy) |
| `flag` | `uart1int.c` | Set to 1 on STX, cleared on ETX; `main()` polls it |
| `i` | `uart1int.c` | Active buffer index used inside the ISR |

---

## 2. Role of Every Source File

### `src/secure_ballot_system_main.c`
Program entry point and top-level orchestrator.

- **`main()`** performs, in order:
  1. Peripheral init: `InitLCD`, `Init_KPM`, `init_i2c`, `RTC_Init`, `UART0_Init`, `UART1_Init`.
  2. A fixed RTC time is programmed (`SetRTCTimeInfo(15,30,0)`, date `22/9/2026`, day `2`).
  3. **First-run seeding** guarded by `if (i2c_eeprom_randomread(..., EEPROM_INIT_FLAG) != EEPROM_INIT_VALUE)`: writes officer PIN `0000`, voter PINs `1111`/`2222`/`3333`, voting window 00:00-23:59, zeroes all counts and voter statuses, then stores the marker `0xA5`.
  4. Logs `SYSTEM_START`.
  5. Shows the welcome screen and enters `while(1)`.
- **Main loop:** waits for `flag == 1`, calls `Logger_SetCardID(rfidBuf)`, logs `CARD_SCAN`, runs `Authenticate(rfidBuf)`, then `switch(role)`:
  - `OFFICER` -> logs success + officer login, opens a menu loop (`1 SET TIME`, `2 START`, `3 STOP`, `4 RESULT`, `5 RESET`, `6 RTC`, `7 EXIT`).
  - `VOTER1/2/3` -> shows voter label, logs authentication success, calls `VoterMenu(n)`.
  - default (`INVALID_CARD`) -> logs failed auth, shows `INVALID CARD`.
- In every branch `flag` is reset to `0` before proceeding, and the loop returns to the scan screen at the end.

> **Note:** `extern volatile u8 flag;` is declared at the top; the commented-out `rfidBuf` extern shows the author consolidated these declarations into headers over time.

### `src/authenticate.c`
Maps a raw 8-character card ID to a role using `strcmp` against four hard-coded IDs. Returns `OFFICER`, `VOTER1`, `VOTER2`, `VOTER3`, or `INVALID_CARD` (constants from `component_defines.h`).

### `src/voter.c`
All voter-side logic. Key functions:

- **`CheckVotingStatus()`** — returns 1 only if the EEPROM voting-enabled flag is 1 **and** the current RTC time is within `[start, end]`. Reads `EEPROM_VOTING_ENABLE`, the four time bytes, and `GetRTCTimeInfo`. Rejects if `now < start` or `now > end` (with hour/minute edge handling).
- **`DisplayVoterMenu()`** — draws the 4-line voter menu.
- **`VoterMenu(voterNo)`** — dispatches `1 VOTING`, `2 EDIT PASSWORD`, `3 EXIT`.
- **`Voting(voterNo)`** — the guarded path: `CheckVotingStatus()` -> `AlreadyVoted()` -> `PasswordCheck()` -> `ShowPartyMenu()` -> `CastVote()`.
- **`PasswordCheck(voterNo)`** — reads a 4-digit PIN from the keypad (masked with `*`, `#` to confirm, `C` to backspace), compares to the 4 bytes stored at the voter's EEPROM password address.
- **`ShowPartyMenu()`** — prints the four party choices.
- **`CastVote(voterNo)`** — validates the key (`1`-`4`), reads the chosen party count, increments it, writes it back, and sets the voter's status byte to 1. (Contains commented-out green-LED toggles.)
- **`AlreadyVoted(voterNo)`** — reads the voter's status byte; returns 1 if already 1.
- **`EditPassword(voterNo)`** — verifies the old PIN, then writes a new 4-digit PIN to EEPROM.

### `src/officer.c`
All officer-side logic. Key functions:

- **`Get2Digit(max)`** — reads exactly two digits then `#`, validates `value <= max`, returns the number; used for time/date entry.
- **`SetVotingTime()`** — prompts for start hour/min and end hour/min (each via `Get2Digit`), validates `end > start`, then writes the four bytes to EEPROM.
- **`OfficerPasswordCheck()`** — reads a 4-digit PIN and compares to the 4 officer-PIN bytes in EEPROM.
- **`StartVoting()` / `StopVoting()`** — after the password check, write `1` / `0` to `EEPROM_VOTING_ENABLE`.
- **`ViewResult()`** — after the password check, reads the four party counts, displays them, and scans for the maximum to announce the leader.
- **`ResetVoting()`** — after the password check, zeroes all party counts, all voter statuses, and the voting-enabled flag.
- **`RTC_Edit()`** — prompts for hour/min/sec, day, month, year (20YY), weekday, then writes them via the RTC setters.


### `src/logger.c`
PC audit logging over UART0.

- Keeps `static u8 currentCardID[9]` (default `"N/A"`), set/read by `Logger_SetCardID` / `Logger_GetCardID`.
- **`SendDateTime()`** (static) builds the `DD-MM-YYYY, HH:MM:SS` prefix from the RTC.
- **`LogEvent(event, cardID, status, description)`** — emits one CSV line: `date-time, EVENT, CARD_ID, STATUS, DESCRIPTION`.

### `src/uart0.c`
UART0 driver (PC link). `UART0_Init` configures P0.0/P0.1, 8N1, DLAB sequencing, 9600 baud. Provides blocking `U0_Tx`, `U0_Rx`, and helpers `U0_TxStr`, `U0_TxU32`, `U0_TxS32`, `U0_TxF32`, `U0_RxStr` (echoing).

### `src/uart1int.c`
UART1 driver + RFID receive ISR.

- **`UART1_Init`** — configures P0.8 (TXD1)/P0.9 (RXD1), 9600 baud, enables the RDA interrupt (`U1IER = 0x01`), and installs `UART1_isr` on VIC slot 0 / channel 7 as an IRQ.
- **`UART1_isr`** — on each received byte: if `0x02` (STX) set `flag=1`; if `0x03` (ETX) set `flag=0` and `i=0`; else store into `rfidBuf[i++]`. Acknowledges with `VICVectAddr = 0`. (Contains a large commented-out block showing earlier attempts.)
- Also provides `U1_Tx`, `U1_Rx`, and the same string/number helpers as UART0.

### `src/i2c.c`
Register-level I2C primitives for the LPC2148: `init_i2c` (pin functions + 100 kHz timing via `I2SCLL/I2SCLH = LOADVAL`, enable with `I2CONSET`), plus `i2c_start`, `i2c_restart`, `i2c_stop`, `i2c_write`, `i2c_mack`, `i2c_nack`.

### `src/i2c_eeprom.c`
EEPROM-level API built on the primitives:

- **`i2c_eeprom_bytewrite(slave, addr, data)`** — start, `slave<<1`, high address byte, low address byte, data, stop, then `dly_ms(10)` for the write cycle.
- **`i2c_eeprom_randomread(slave, addr)`** — dummy-write the address, restart, `slave<<1 | 1`, read with NACK, stop.
- **`i2c_eeprom_pagewrite`** / **`i2c_eeprom_seqread`** — block transfers using MACK for all but the last byte.

### `src/rtc.c`
On-chip RTC driver. `RTC_Init` resets and enables the RTC with the external clock source (`RTC_ENABLE | RTC_CLKSRC`) for the LPC2148 path. Setters/getters read/write `HOUR/MIN/SEC/DOM/MONTH/YEAR/DOW`. Also includes `DisplayRTCTime/Date/Day` helpers (not used by the voting flow).

### `src/lcd.c`
HD44780 LCD driver. `InitLCD` sets data + control pins as outputs and runs the standard 8-bit init sequence. `WriteLCD` pulses EN; `CmdLCD`/`CharLCD`/`StrLCD`/`U32LCD` provide the command and text API. Extra helpers (`S32LCD`, `F32LCD`, `HEXLCD`, `OCTLCD`, `BuildCGRAM`) are present but mostly unused by the app.

### `src/kpm.c`
4x4 keypad driver. `Init_KPM` configures rows (P1.16-19) as outputs. `ColScan`/`RowCheck`/`ColCheck` implement the scan; `KeyScan` blocks for a full press+release and returns the mapped character. `ReadNum`/`ReadPassword` provide convenience input routines (largely superseded by inline entry in `voter.c`/`officer.c`).

### `src/delay.c`
Busy-wait delays `dly_us`, `dly_ms`, `dly_s` scaled for the 60 MHz clock (multipliers 12, 12000, 12000000).

### Headers
- **`include/component_defines.h`** — all pin macros, clock/UART/I2C config, EEPROM slave addresses, the full EEPROM memory map, and role constants.
- **`include/functiondeclaration.h`** — project data types (`u8`, `s8`, ...) and every function prototype, plus the shared RFID externs.

---


## 3. Interfaces and Communication Flow

| Interface | Peripheral | Devices | Direction | Driver |
|-----------|-----------|---------|-----------|--------|
| UART0 (P0.0/P0.1) | UART0 | PC serial console | MCU -> PC (log) | `uart0.c` |
| UART1 (P0.8/P0.9) | UART1 | RFID reader | Reader -> MCU | `uart1int.c` (ISR) |
| I2C (SCL/SDA) | I2C @ 100 kHz | AT24C256 EEPROM | Both | `i2c.c` + `i2c_eeprom.c` |
| GPIO (P0.13-23) | GPIO | 20x4 LCD | MCU -> LCD | `lcd.c` |
| GPIO (P1.16-23) | GPIO | 4x4 keypad | Keypad -> MCU | `kpm.c` |
| On-chip | RTC | internal timekeeping | internal | `rtc.c` |

**Data flow for a card scan:**

```
RFID card
   -> RFID reader (UART frame: STX + 8 chars + ETX)
   -> LPC2148 UART1 RBR
   -> UART1_isr() fills rfidBuf, sets flag
   -> main() sees flag, copies id, LogEvent(CARD_SCAN)
   -> Authenticate() -> role
   -> officer or voter flow
```

---

## 4. Authentication and Voting Workflows

### 4.1 Authentication (who is this?)
`Authenticate(cardID)` compares the captured ID to the fixed table and returns a role. This step only answers **identity/role**. It does **not** by itself allow a vote.

### 4.2 Voting-period validation (is it allowed right now?)
`CheckVotingStatus()` checks the officer-set enabled flag **and** the RTC time window. This is independent of who the voter is.

### 4.3 Vote recording (what did they choose?)
`ShowPartyMenu()` + `CastVote()` record one increment to the chosen party's EEPROM counter.

### 4.4 Duplicate-vote prevention (have they voted before?)
`AlreadyVoted()` reads the voter's status byte; after a vote it is set to 1, blocking a second vote.

### 4.5 Officer administration
The officer menu controls the window, the enabled flag, results, reset, and RTC — all behind `OfficerPasswordCheck()`.

### 4.6 Audit logging
`LogEvent()` writes a timestamped CSV line to UART0 for each significant event.

**Voter voting call chain (guarded):**

```
VoterMenu -> '1' -> Voting(voterNo)
   Voting:
     if CheckVotingStatus()==0 -> block + log, return
     if AlreadyVoted(voterNo) -> "ALREADY VOTED", return
     if PasswordCheck(voterNo):
         ShowPartyMenu()
         CastVote(voterNo)   // increments count, sets status=1
     else:
         "WRONG PASSWORD"
```

---

## 5. EEPROM and RTC Usage

### 5.1 EEPROM (AT24C256 @ 0x50)
- Slave address `0x50`; the code shifts it left (`<<1`) and ORs the R/W bit.
- Addresses are 16-bit, sent high byte then low byte.
- Byte writes are followed by `dly_ms(10)` to respect the device write-cycle time.
- The full memory map (voting control, times, reserved IDs, PINs, statuses, counts, officer PIN, init marker) is defined in `component_defines.h` and reproduced in the README [section 14](#14-i2c-communication--at24c256-eeprom-storage).
- A one-time init marker (`0xA5` at `0x0080`) prevents re-seeding defaults on every reboot.

### 5.2 RTC (on-chip LPC2148)
- Enabled with the external clock source in `RTC_Init`.
- Time/date are read for the voting-window comparison (`GetRTCTimeInfo`) and for the log timestamp (`GetRTCDateInfo` + `GetRTCTimeInfo`).
- `main()` writes a fixed start time; the officer can change it via `RTC_Edit`.

---

## 6. Logging Flow

Every `LogEvent(event, cardID, status, description)` call:

1. Calls `SendDateTime()` -> reads RTC date & time, writes `DD-MM-YYYY, HH:MM:SS`.
2. Writes `, EVENT`, `, CARD_ID`, `, STATUS`, `, DESCRIPTION`, then `\r\n`.
3. All output goes through `U0_Tx*` on UART0 at 9600 baud.

The card ID used in the log is the one most recently stored via `Logger_SetCardID()` (set in `main()` right after a card is detected), so every subsequent event in that session is attributed to the correct card.

**Sample log excerpt (illustrative format only):**

```
22-09-2026, 15:30:05, SYSTEM_START, N/A, SUCCESS, System initialized
22-09-2026, 15:31:12, CARD_SCAN, 12543380, RECEIVED, RFID detected
22-09-2026, 15:31:12, AUTHENTICATION, 12543380, SUCCESS, Voter authenticated
```

---


## 7. Known Limitations and Code Observations

These are reported as-is; **no source was changed to "fix" them.**

1. **Busy-wait / blocking.** `KeyScan`, UART logging, and EEPROM delays all block the CPU. The device cannot multitask while waiting.
2. **Approximate delays.** `delay.c` uses instruction-count loops; accuracy depends on the actual clock and compiler optimization and has not been measured here.
3. **LCD line-address mismatch.** `component_defines.h` defines `GOTO_LINE3_POS0 = 0x90` and `GOTO_LINE4_POS0 = 0xD0`, but the screens use `0x94` and `0xD4`. The literal values in the code are what take effect; the macros are effectively unused for those rows.
4. **Unused / legacy code.** `ind` is declared and initialized but the ISR uses `i`. `ReadNum`, `ReadPassword`, and several LCD helpers (`F32LCD`, `HEXLCD`, `OCTLCD`, `BuildCGRAM`) and the RTC `DisplayRTC*` helpers are present but unused by the app. Large commented-out blocks exist in `uart1int.c`.
5. **Reserved-but-unused EEPROM ID regions.** `EEPROM_VOTER*_ID` are defined but never read; card IDs live in `authenticate.c`.
6. **Single-byte counters** limit each party to 255 votes.
7. **Hard-coded RTC time** in `main()` (a specific 2026 date/time) is written on every boot before the officer edits it.
8. **`u32 arr[4]` in `ViewResult`** with `max=-1` relies on unsigned wrap-around to initialise the maximum search; it works but is fragile style.
9. **No active LED/buzzer** despite commented placeholders; no pin is defined for them.
10. **Plain-text PINs and card IDs** — a security concern for anything beyond a demo.

---

## 8. Setup and Testing Notes

- **Build:** Create a Keil uVision project for the **LPC2148**, add the `src/*.c` files, and add `include/` to the C/C++ include path. The Keil device headers `LPC21xx.h` / `lpc214x.h` must be available in the toolchain. **A build has not been performed in this repository's environment.**
- **First run:** On a blank EEPROM the firmware seeds defaults once (marker `0xA5`). Officer PIN `0000`; voter PINs `1111`/`2222`/`3333`.
- **Terminal:** Connect UART0 to a PC at 9600 8N1 to capture the CSV log.
- **Wiring:** See README [section 23](#23-hardware-setup--wiring) for the pins that are confirmed in source. Power/ground/pull-ups/crystal/level-shifter details are **not specified** in the supplied files.
- **Testing:** Follow the checklist in README [section 24](#24-testing--validation). No hardware test results are claimed here.

---

*End of documentation.*

