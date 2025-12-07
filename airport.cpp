#include <windows.h>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <vector>

#define MAX_QUEUE_SIZE 5  // change this to change capacity

// Control IDs
#define IDC_FLIGHT_EDIT 101
#define IDC_AIRLINE_EDIT 102
#define IDC_ADD_BUTTON 201
#define IDC_TAKEOFF_BUTTON 202
#define IDC_REFRESH_BUTTON 203
#define IDC_LISTBOX 301
#define IDC_FRONT_BUTTON 204
#define IDC_EMPTY_BUTTON 205
#define IDC_FULL_BUTTON 206
#define IDC_SIZE_BUTTON 207
#define IDC_EXIT_BUTTON 208

// Global variables
HINSTANCE ghInst;
HWND hFlightEdit = NULL;
HWND hAirlineEdit = NULL;
HWND hListBox = NULL;

struct Plane {
    std::string flightNo;
    std::string airline;
    Plane() {}
    Plane(const std::string& f, const std::string& a) : flightNo(f), airline(a) {}
};

class RunwayQueue {
private:
    std::queue<Plane> q;
public:
    void loadFromFile() {
        q = std::queue<Plane>(); // clear
        std::ifstream fin("runway.txt");
        if (!fin.is_open()) return;
        std::string line;
        while (std::getline(fin, line)) {
            size_t pos = line.find(" - ");
            if (pos != std::string::npos) {
                std::string flight = line.substr(0, pos);
                std::string airline = line.substr(pos + 3);
                if (q.size() < MAX_QUEUE_SIZE) q.push(Plane(flight, airline));
            }
        }
        fin.close();
    }

    void saveToFile() {
        std::ofstream fout("runway.txt", std::ios::out | std::ios::trunc);
        if (!fout.is_open()) return;
        std::queue<Plane> temp = q;
        while (!temp.empty()) {
            Plane p = temp.front(); temp.pop();
            fout << p.flightNo << " - " << p.airline << "\n";
        }
        fout.close();
    }

    bool isEmpty() const { return q.empty(); }
    bool isFull() const { return q.size() >= MAX_QUEUE_SIZE; }
    int size() const { return (int)q.size(); }

    bool pushPlane(const Plane& p) {
        if (isFull()) return false;
        q.push(p);
        saveToFile();
        return true;
    }

    bool popPlane(Plane& outPlane) {
        if (isEmpty()) return false;
        outPlane = q.front();
        q.pop();
        saveToFile();
        return true;
    }

    bool peekFront(Plane& outPlane) const {
        if (isEmpty()) return false;
        outPlane = q.front();
        return true;
    }

    std::vector<Plane> listAll() const {
        std::vector<Plane> result;
        std::queue<Plane> temp = q;
        while (!temp.empty()) {
            result.push_back(temp.front());
            temp.pop();
        }
        return result;
    }

    // Helper to replace internal queue (used when loading)
    void setQueue(const std::queue<Plane>& nq) { q = nq; }
};

RunwayQueue gRunway;

// Utility: convert std::string to LPCSTR (ANSI)
LPCSTR toLPCSTR(const std::string& s) {
    return s.c_str();
}

// Add string to listbox
void RefreshListBox(HWND hList) {
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    std::vector<Plane> list = gRunway.listAll();
    if (list.empty()) {
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"<Queue is empty>");
        return;
    }
    // Add box-style entries as multiple lines concatenated into one string each
    for (const Plane& p : list) {
        std::ostringstream oss;
        oss << "------------------------------  ";
        oss << "| Flight : " << p.flightNo << "  ";
        oss << "| Airline: " << p.airline << "  ";
        oss << "------------------------------";
        std::string entry = oss.str();
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)entry.c_str());
    }
}

// Show message helper
void ShowInfo(HWND hwnd, const std::string& title, const std::string& msg) {
    MessageBox(hwnd, msg.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

// Handlers
void OnAddPlane(HWND hwnd) {
    // Get text from edit controls
    char flightBuf[256], airlineBuf[256];
    GetWindowTextA(hFlightEdit, flightBuf, sizeof(flightBuf));
    GetWindowTextA(hAirlineEdit, airlineBuf, sizeof(airlineBuf));
    std::string flight = flightBuf;
    std::string airline = airlineBuf;

    // Trim (simple)
    while (!flight.empty() && isspace((unsigned char)flight.back())) flight.pop_back();
    while (!airline.empty() && isspace((unsigned char)airline.back())) airline.pop_back();

    if (flight.empty() || airline.empty()) {
        ShowInfo(hwnd, "Input Error", "Please enter both Flight Number and Airline.");
        return;
    }

    if (gRunway.isFull()) {
        ShowInfo(hwnd, "Queue Full", "Queue is FULL! Cannot add more planes.");
        return;
    }

    bool ok = gRunway.pushPlane(Plane(flight, airline));
    if (ok) {
        ShowInfo(hwnd, "Added", "Plane added to takeoff queue.");
        RefreshListBox(hListBox);
        // Clear edit boxes
        SetWindowTextA(hFlightEdit, "");
        SetWindowTextA(hAirlineEdit, "");
    } else {
        ShowInfo(hwnd, "Error", "Failed to add plane.");
    }
}

void OnAllowTakeoff(HWND hwnd) {
    if (gRunway.isEmpty()) {
        ShowInfo(hwnd, "Empty", "Queue is empty. No plane can take off.");
        return;
    }
    Plane p;
    bool ok = gRunway.popPlane(p);
    if (!ok) {
        ShowInfo(hwnd, "Error", "Unable to perform takeoff.");
        return;
    }

    // Log to takeoff_log.txt (append)
    std::ofstream log("takeoff_log.txt", std::ios::app);
    if (log.is_open()) {
        log << p.flightNo << " - " << p.airline << " - Took Off\n";
        log.close();
    }

    // Show takeoff info
    std::ostringstream oss;
    oss << "Plane Taking Off:\nFlight: " << p.flightNo << "\nAirline: " << p.airline;
    ShowInfo(hwnd, "Takeoff", oss.str());

    RefreshListBox(hListBox);
}

void OnRefresh(HWND hwnd) {
    RefreshListBox(hListBox);
}

void OnShowFront(HWND hwnd) {
    Plane p;
    if (!gRunway.peekFront(p)) {
        ShowInfo(hwnd, "Front Plane", "Queue is empty. No front plane.");
        return;
    }
    std::ostringstream oss;
    oss << "Front Plane Details:\nFlight: " << p.flightNo << "\nAirline: " << p.airline;
    ShowInfo(hwnd, "Front Plane", oss.str());
}

void OnCheckEmpty(HWND hwnd) {
    ShowInfo(hwnd, "Is Empty?", gRunway.isEmpty() ? "Queue is EMPTY." : "Queue is NOT empty.");
}

void OnCheckFull(HWND hwnd) {
    ShowInfo(hwnd, "Is Full?", gRunway.isFull() ? "Queue is FULL." : "Queue is NOT full.");
}

void OnSize(HWND hwnd) {
    std::ostringstream oss;
    oss << "Queue Size: " << gRunway.size() << " / " << MAX_QUEUE_SIZE;
    ShowInfo(hwnd, "Queue Size", oss.str());
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
    {
        // Load queue from file first
        gRunway.loadFromFile();

        // Create Labels and Edit controls
        CreateWindowA("STATIC", "Flight Number:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, ghInst, NULL);
        hFlightEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 18, 180, 22, hwnd, (HMENU)IDC_FLIGHT_EDIT, ghInst, NULL);

        CreateWindowA("STATIC", "Airline:", WS_VISIBLE | WS_CHILD, 20, 50, 100, 20, hwnd, NULL, ghInst, NULL);
        hAirlineEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 48, 180, 22, hwnd, (HMENU)IDC_AIRLINE_EDIT, ghInst, NULL);

        // Buttons
        CreateWindowA("BUTTON", "Add Plane", WS_VISIBLE | WS_CHILD, 330, 18, 120, 26, hwnd, (HMENU)IDC_ADD_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Allow Takeoff", WS_VISIBLE | WS_CHILD, 330, 52, 120, 26, hwnd, (HMENU)IDC_TAKEOFF_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Refresh Queue", WS_VISIBLE | WS_CHILD, 470, 18, 120, 26, hwnd, (HMENU)IDC_REFRESH_BUTTON, ghInst, NULL);

        // Additional buttons
        CreateWindowA("BUTTON", "Front Plane", WS_VISIBLE | WS_CHILD, 470, 52, 120, 26, hwnd, (HMENU)IDC_FRONT_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Is Empty?", WS_VISIBLE | WS_CHILD, 610, 18, 100, 26, hwnd, (HMENU)IDC_EMPTY_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Is Full?", WS_VISIBLE | WS_CHILD, 610, 52, 100, 26, hwnd, (HMENU)IDC_FULL_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Queue Size", WS_VISIBLE | WS_CHILD, 730, 18, 100, 26, hwnd, (HMENU)IDC_SIZE_BUTTON, ghInst, NULL);
        CreateWindowA("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 730, 52, 100, 26, hwnd, (HMENU)IDC_EXIT_BUTTON, ghInst, NULL);

        // ListBox for queue display
        hListBox = CreateWindowA("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL, 20, 90, 810, 360, hwnd, (HMENU)IDC_LISTBOX, ghInst, NULL);

        // Initial populate
        RefreshListBox(hListBox);
    }
    break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id) {
        case IDC_ADD_BUTTON:
            OnAddPlane(hwnd);
            break;
        case IDC_TAKEOFF_BUTTON:
            OnAllowTakeoff(hwnd);
            break;
        case IDC_REFRESH_BUTTON:
            OnRefresh(hwnd);
            break;
        case IDC_FRONT_BUTTON:
            OnShowFront(hwnd);
            break;
        case IDC_EMPTY_BUTTON:
            OnCheckEmpty(hwnd);
            break;
        case IDC_FULL_BUTTON:
            OnCheckFull(hwnd);
            break;
        case IDC_SIZE_BUTTON:
            OnSize(hwnd);
            break;
        case IDC_EXIT_BUTTON:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// WinMain: entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ghInst = hInstance;
    const char CLASS_NAME[] = "AirportQueueWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = ghInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Airport Takeoff Queue - Win32 GUI",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, // fixed size window
        CW_USEDEFAULT, CW_USEDEFAULT, 860, 520,
        NULL,
        NULL,
        ghInst,
        NULL
    );

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}