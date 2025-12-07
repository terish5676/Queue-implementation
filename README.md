# Airport Takeoff Queue (Win32 GUI)

A lightweight Win32 GUI application that simulates an airport runway takeoff queue using a fixed-capacity FIFO (First-In-First-Out) queue data structure. Planes can be added to the queue, cleared for takeoff, and all operations are persisted to disk for learning purposes.

## Features

- **Fixed-capacity FIFO queue implementation** - Models real-world runway constraints
- **Win32 GUI interface** - Interactive controls for queue management
- **Persistent storage** - Queue state saved to `runway.txt` between sessions
- **Takeoff logging** - Append-only `takeoff_log.txt` tracks all departures
- **Real-time status display** - View queue contents and statistics instantly
- **Simple single-file implementation** - Easy to understand and modify

## UI Layout

```
┌──────────────────────────────────────────────┐
│  Flight Number: [________]  Airline: [________]  │
│  [Add Plane]                                 │
│                                              │
│  Current Queue:                              │
│  ┌────────────────────────────────────────┐ │
│  │ 1. AA123 - American Airlines           │ │
│  │ 2. DL456 - Delta                       │ │
│  │ 3. UA789 - United                      │ │
│  └────────────────────────────────────────┘ │
│                                              │
│  [Allow Takeoff]  [Refresh Queue]           │
│  [Front Plane]    [Is Empty?]               │
│  [Is Full?]       [Queue Size]              │
│  [Exit]                                      │
└──────────────────────────────────────────────┘
```

## Prerequisites

- **Operating System**: Windows 7 or later
- **Compiler** (choose one):
  - Visual Studio 2015 or later (MSVC)
  - MinGW-w64 (g++)
  - Any C++ compiler with Win32 API support

## Files

- **`airport.cpp`** - Main application source code containing:
  - Plane structure definition
  - Queue implementation (enqueue, dequeue, front, isEmpty, isFull)
  - File I/O functions (load/save queue, logging)
  - Win32 window procedure and event handlers
  - GUI control creation and layout
  
- **`runway.txt`** - Persistent storage of queued planes (auto-created)
  - Format: One plane per line as `FLIGHTNO - AIRLINE`
  - Automatically updated on add/takeoff operations
  
- **`takeoff_log.txt`** - Append-only log of departed planes (auto-created)
  - Format: `FLIGHTNO - AIRLINE took off`
  - Permanent record of all takeoffs

## Build Instructions

### Visual Studio (MSVC)

1. Open a **Developer Command Prompt** (or **x64 Native Tools Command Prompt**)
2. Navigate to the project directory
3. Compile:
```bash
cl /EHsc /Fe:airport.exe airport.cpp user32.lib gdi32.lib
```

### MinGW (g++)

1. Open **PowerShell**, **Command Prompt**, or **MSYS2 shell**
2. Navigate to the project directory
3. Compile:
```bash
g++ airport.cpp -o airport.exe -mwindows
```

**Note**: The code uses narrow (ANSI) Win32 APIs (`CreateWindowA`, `MessageBoxA`, etc.), so default compiler settings should work fine.

## Running the Application

### From GUI
Double-click `airport.exe` in File Explorer

### From Command Line
```bash
.\airport.exe
```

## Usage Guide

### Adding a Plane
1. Enter a **Flight Number** (e.g., `AA123`)
2. Enter an **Airline** (e.g., `American Airlines`)
3. Click **Add Plane**
4. The plane is added to the queue and saved to `runway.txt`

### Allowing Takeoff
1. Click **Allow Takeoff**
2. The front plane is removed from the queue
3. A message box shows which plane departed
4. The takeoff is logged to `takeoff_log.txt`
5. `runway.txt` is updated to reflect the new queue state

### Viewing Queue Information
- **Refresh Queue** - Updates the display with current queue contents
- **Front Plane** - Shows details of the next plane to take off
- **Is Empty?** - Checks if the runway queue is empty
- **Is Full?** - Checks if the queue has reached capacity
- **Queue Size** - Shows the current number of planes waiting

### Exiting
Click **Exit** to close the application. Queue state is preserved in `runway.txt`.

## File Formats

### runway.txt (Queue State)
Each line represents one plane in the queue:
```
AA123 - American Airlines
DL456 - Delta
UA789 - United
BA101 - British Airways
```

**Important**: Use the format `FLIGHTNO - AIRLINE` with spaces around the dash.

### takeoff_log.txt (Takeoff History)
Each line represents one completed takeoff:
```
AA123 - American Airlines took off
DL456 - Delta took off
UA789 - United took off
```

## Persistence Behavior

- **On startup**: The program reads `runway.txt` to restore the previous queue state
- **On add**: The queue is immediately saved to `runway.txt`
- **On takeoff**: The queue is saved to `runway.txt` and the departure is logged to `takeoff_log.txt`
- **Between runs**: Queue state persists, allowing you to close and reopen the program without losing data

## Configuration

### Changing Queue Capacity
Open `airport.cpp` and modify the constant:
```cpp
const int MAX_QUEUE_SIZE = 10;  // Change to your desired capacity
```
Then recompile the program.

## Common Issues

### Program doesn't start
- Ensure you're running on a Windows system
- Run from Command Prompt to see any error messages
- Check that required DLLs are present (usually bundled with Windows)

### Queue appears empty on startup
- Verify that `runway.txt` exists in the same directory as `airport.exe`
- Check file format: each line must be `FLIGHTNO - AIRLINE` (note spaces around dash)
- Ensure the file is readable (not locked by another program)

### Changes don't persist
- Confirm the program has write permissions in its directory
- Check if antivirus software is blocking file writes
- Try running the program from a user-writable location (not Program Files)

### Compilation errors about Unicode
- The code uses ANSI (narrow) APIs by default
- If you see Unicode-related errors, ensure you're not forcing Unicode compilation
- Alternatively, convert all APIs to wide versions (`CreateWindowW`, `WCHAR`, etc.)

### List control doesn't display entries
- Click **Refresh Queue** to force a display update
- Verify `runway.txt` exists and has valid format
- Check that the queue isn't actually empty

## Learning Objectives

This project demonstrates:

- **Data Structures**: Fixed-capacity queue with enqueue/dequeue operations
- **Win32 GUI Programming**: Window creation, controls, message handling
- **File I/O**: Reading and writing structured text files
- **Event-Driven Programming**: Responding to button clicks and user input
- **State Persistence**: Saving and restoring application state
- **Resource Management**: Proper handling of Windows API resources

## Code Structure

The `airport.cpp` file is organized as follows:

```
[Constants & Globals]
  - MAX_QUEUE_SIZE
  - Control IDs (ID_BTN_ADD, etc.)
  - Global variables (hInst, queue arrays)

[Plane Structure]
  - flightNumber
  - airline

[Queue Operations]
  - enqueue() - Add plane to queue
  - dequeue() - Remove plane from queue
  - front() - View front plane
  - isEmpty() - Check if empty
  - isFull() - Check if full
  - size() - Get current size

[File Operations]
  - loadQueue() - Read runway.txt on startup
  - saveQueue() - Write queue to runway.txt
  - logTakeoff() - Append to takeoff_log.txt

[GUI Functions]
  - Window Procedure (WndProc)
  - Control creation
  - Event handlers for buttons
  - List update functions

[Entry Point]
  - WinMain() - Application initialization
```

## Extending the Project

### Beginner Enhancements
- Add input validation (prevent empty flight numbers)
- Implement duplicate flight number checking
- Add a "Clear Queue" button to empty the runway
- Show queue capacity in the window title

### Intermediate Enhancements
- Convert to Unicode (wide character APIs)
- Add colored list items (VIP flights, delays)
- Implement a delay timer between takeoffs
- Add statistics (total flights, average wait time)
- Create a menu bar with File/Help options

### Advanced Enhancements
- Implement priority queue for emergency takeoffs
- Add multiple runway support
- Create graphical visualization of the runway
- Network support for multi-client simulation
- Database persistence (SQLite) instead of text files
- Add flight scheduling with time slots

## Troubleshooting Tips

**Debug Mode**: Run from Command Prompt to see console output:
```bash
.\airport.exe
```

**Test Files Manually**: Create `runway.txt` with sample data:
```
TEST123 - Test Airlines
DEMO456 - Demo Airways
```

**Reset Everything**: Delete `runway.txt` and `takeoff_log.txt` to start fresh.

## Contributing

This is an educational project. Feel free to:
- Fork and modify for learning purposes
- Add features and submit improvements
- Use as a base for more complex simulations
- Share with students learning Win32 programming

## License

No license file included. This is provided as-is for educational purposes. Add a `LICENSE` file if you want to specify reuse terms.

## Resources

- [Win32 API Documentation](https://docs.microsoft.com/en-us/windows/win32/)
- [Queue Data Structure](https://en.wikipedia.org/wiki/Queue_(abstract_data_type))
- [FIFO Scheduling](https://en.wikipedia.org/wiki/FIFO_(computing_and_electronics))

## Contact

This repository contains a single-file Win32 GUI sample for learning:
- Queue data structure implementation
- File persistence patterns
- Basic Win32 GUI programming
- Event-driven application design

**Questions or suggestions?** Open an issue or modify the code to suit your learning needs!

---

**Version**: 1.0  
**Last Updated**: December 2025  
**Compatibility**: Windows 7+
