#include <iostream>
#include <windows.h>
#include <cmath>
#include <chrono>
#include <shellapi.h> // For system tray
#include "resource.h" // For icon resources

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAYICON 1
#define ID_EXIT 1000

NOTIFYICONDATA nid = {};
HMENU hPopMenu;
WNDCLASSEX wc = {};
HWND hWnd;

// Configuration
const int MOVEMENT_THRESHOLD = 10;      // Minimum pixels to trigger horizontal actions
const int VERTICAL_THRESHOLD = 5;       // More sensitive threshold for up/down actions
const int CURSOR_MOVE_STEPS = 20;       // Number of steps for smooth cursor movement
const int CURSOR_MOVE_DELAY = 1;        // Milliseconds between each cursor movement step
const int SLOW_MOUSE_SPEED = 4;         // Mouse speed when action button held (1-20, where 10 is normal)
const double VERTICAL_COOLDOWN = 1500.0; // Cooldown for vertical actions in milliseconds (1.5 seconds)

// Command types enum
enum class ActiveCommand {
    NONE,
    ALT_TAB_RIGHT,
    ALT_TAB_LEFT,
    VIRTUAL_DESKTOP_LEFT,
    VIRTUAL_DESKTOP_RIGHT,
    WINDOWS_TAB_UP,
    SHOW_DESKTOP_OR_TAB_DOWN
};

// State tracking
struct {
    bool actionButtonDown = false;  // Mouse Button 5 (can be changed to right button)
    POINT initialPos = {0, 0};     // Position when action button was pressed
    bool isLocked = false;         // Whether cursor is currently locked
    POINT lockedPos = {0, 0};      // Position where cursor is locked
    bool altTabActive = false;     // Whether Alt+Tab is currently being held
    ActiveCommand currentCommand = ActiveCommand::NONE;  // Currently active command
    bool isReturningToOrigin = false; // Whether cursor is currently returning to origin
    POINT lastPos = {0, 0};        // Last position before releasing action button
    int originalMouseSpeed = 10;    // Store original mouse speed
    std::chrono::steady_clock::time_point lastUpwardActionTime = std::chrono::steady_clock::now();  // Last time upward action was triggered
    std::chrono::steady_clock::time_point lastDownwardActionTime = std::chrono::steady_clock::now();  // Last time downward action was triggered
} state;

// Forward declarations
void performAction(const std::string& action);
void handleMouseMovement(const POINT& currentPos);
void lockCursor();
void unlockCursor();
void startAltTab(bool goRight);
void releaseAltTab();

// Mouse actions
void performVirtualDesktopSwitch(bool goRight) {
    // Only perform if no other command is active or if we're continuing the same direction
    ActiveCommand newCommand = goRight ? ActiveCommand::VIRTUAL_DESKTOP_RIGHT : ActiveCommand::VIRTUAL_DESKTOP_LEFT;
    if (state.currentCommand == ActiveCommand::NONE || state.currentCommand == newCommand) {
        state.currentCommand = newCommand;
        
        // Windows + Ctrl + Arrow (Left/Right)
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event(goRight ? VK_RIGHT : VK_LEFT, 0, 0, 0);
        keybd_event(goRight ? VK_RIGHT : VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    }
}

bool checkVerticalCooldown(bool isUpward) {
    auto currentTime = std::chrono::steady_clock::now();
    auto& lastActionTime = isUpward ? state.lastUpwardActionTime : state.lastDownwardActionTime;
    
    auto timeSinceLastAction = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastActionTime).count();
    
    if (timeSinceLastAction < VERTICAL_COOLDOWN) {
        return false; // Still in cooldown
    }
    
    lastActionTime = currentTime;
    return true;
}

void performWindowsTabView() {
    // Check cooldown for upward action
    if (!checkVerticalCooldown(true)) {
        return;
    }

    // Only perform if no other command is active or if continuing Windows+Tab
    if (state.currentCommand == ActiveCommand::NONE || 
        state.currentCommand == ActiveCommand::WINDOWS_TAB_UP) {
        state.currentCommand = ActiveCommand::WINDOWS_TAB_UP;
        
        // Windows + Tab
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_TAB, 0, 0, 0);
        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    }
}

void performShowDesktopOrWindowsTab() {
    // Check cooldown for downward action
    if (!checkVerticalCooldown(false)) {
        return;
    }

    // Only perform if no other command is active or if continuing the same command
    if (state.currentCommand == ActiveCommand::NONE || 
        state.currentCommand == ActiveCommand::SHOW_DESKTOP_OR_TAB_DOWN) {
        state.currentCommand = ActiveCommand::SHOW_DESKTOP_OR_TAB_DOWN;
        
        // Check if a window is focused
        HWND focusedWindow = GetForegroundWindow();
        if (focusedWindow != NULL && focusedWindow != GetDesktopWindow()) {
            // Show desktop (Windows + D) if window is focused
            keybd_event(VK_LWIN, 0, 0, 0);
            keybd_event('D', 0, 0, 0);
            keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
        } else {
            // Windows + Tab if no window is focused
            performWindowsTabView();
        }
    }
}

void startAltTab(bool goRight) {
    // Only start if no other command is active or if we're in Alt+Tab mode
    ActiveCommand newCommand = goRight ? ActiveCommand::ALT_TAB_RIGHT : ActiveCommand::ALT_TAB_LEFT;
    
    // If we're already in Alt+Tab mode (either direction), just change direction
    bool isAltTabActive = (state.currentCommand == ActiveCommand::ALT_TAB_RIGHT || 
                          state.currentCommand == ActiveCommand::ALT_TAB_LEFT);
    
    if (state.currentCommand == ActiveCommand::NONE || isAltTabActive) {
        // If not in Alt+Tab mode, start it
        if (!state.altTabActive) {
            keybd_event(VK_MENU, 0, 0, 0);
            keybd_event(VK_TAB, 0, 0, 0);
            keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
            state.altTabActive = true;
        }
        
        // Press Right or Left arrow
        keybd_event(goRight ? VK_RIGHT : VK_LEFT, 0, 0, 0);
        keybd_event(goRight ? VK_RIGHT : VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
        
        state.currentCommand = newCommand;
    }
}

void releaseAltTab() {
    // Release Alt key if it's being held
    if (state.altTabActive) {
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        state.altTabActive = false;
    }
    
    if (state.currentCommand == ActiveCommand::ALT_TAB_RIGHT || 
        state.currentCommand == ActiveCommand::ALT_TAB_LEFT) {
        state.currentCommand = ActiveCommand::NONE;
    }
}

void performAltTab(bool goRight = true) {
    // Hold Alt and press Right/Left for switching
    startAltTab(goRight);
}

// Smooth cursor movement
void moveCursorSmoothly(const POINT& from, const POINT& to) {
    state.isReturningToOrigin = true;
    
    for (int step = 1; step <= CURSOR_MOVE_STEPS; step++) {
        if (!state.actionButtonDown) {
            break; // Stop if action button is released
        }
        
        // Calculate intermediate position
        POINT currentPos;
        currentPos.x = from.x + ((to.x - from.x) * step) / CURSOR_MOVE_STEPS;
        currentPos.y = from.y + ((to.y - from.y) * step) / CURSOR_MOVE_STEPS;
        
        // Move cursor
        SetCursorPos(currentPos.x, currentPos.y);
        
        // Small delay for smooth movement
        Sleep(CURSOR_MOVE_DELAY);
    }
    
    // Ensure we reach the exact destination
    if (state.actionButtonDown) {
        SetCursorPos(to.x, to.y);
    }
    
    state.isReturningToOrigin = false;
}

// Mouse movement handling
void handleMouseMovement(const POINT& currentPos) {
    if (!state.actionButtonDown || state.isReturningToOrigin) return;

    int deltaX = currentPos.x - state.initialPos.x;
    int deltaY = currentPos.y - state.initialPos.y;

    // Use different thresholds for horizontal and vertical movements
    bool horizontalThresholdMet = abs(deltaX) > MOVEMENT_THRESHOLD;
    bool verticalThresholdMet = abs(deltaY) > VERTICAL_THRESHOLD;

    // Only trigger if movement exceeds threshold
    if (horizontalThresholdMet || verticalThresholdMet) {
        // Store current position
        state.lastPos = currentPos;
        
        // Determine primary direction of movement
        if (horizontalThresholdMet && abs(deltaX) > abs(deltaY)) {
            // Horizontal movement
            if (deltaX > 0) {
                performVirtualDesktopSwitch(false);  // Moving right now triggers Left
            } else {
                performVirtualDesktopSwitch(true);   // Moving left now triggers Right
            }
        } else {
            // Vertical movement
            if (deltaY > 0) {
                performShowDesktopOrWindowsTab();  // Down
            } else {
                performWindowsTabView();          // Up
            }
        }
        
        // Reset initial position after action
        state.initialPos = state.initialPos; // Keep the original position
    }
}

// Cursor control
void lockCursor() {
    if (!state.isLocked) {
        GetCursorPos(&state.lockedPos);
        state.isLocked = true;
    }
    SetCursorPos(state.lockedPos.x, state.lockedPos.y);
}

void unlockCursor() {
    state.isLocked = false;
}

// Mouse speed control
void setMouseSpeed(int speed) {
    SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)speed, SPIF_SENDCHANGE);
}

// Mouse hook callback
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* mouseInfo = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        switch (wParam) {
            case WM_XBUTTONDOWN:
                if (HIWORD(mouseInfo->mouseData) == XBUTTON2) {  // Mouse Button 5
                    state.actionButtonDown = true;
                    GetCursorPos(&state.initialPos);
                    
                    // Store and modify mouse speed only when action button is pressed
                    SystemParametersInfo(SPI_GETMOUSESPEED, 0, &state.originalMouseSpeed, 0);
                    setMouseSpeed(SLOW_MOUSE_SPEED);
                }
                break;

            case WM_XBUTTONUP:
                if (HIWORD(mouseInfo->mouseData) == XBUTTON2) {  // Mouse Button 5
                    state.actionButtonDown = false;
                    // Get current position before moving
                    POINT currentPos;
                    GetCursorPos(&currentPos);
                    // Always return to initial position
                    moveCursorSmoothly(currentPos, state.initialPos);
                    unlockCursor();
                    releaseAltTab(); // Release any held Alt+Tab combinations
                    state.currentCommand = ActiveCommand::NONE;  // Reset active command
                    
                    // Restore original mouse speed
                    setMouseSpeed(state.originalMouseSpeed);
                }
                break;

            case WM_MOUSEMOVE:
                if (state.actionButtonDown && !state.isReturningToOrigin) {
                    POINT currentPos;
                    GetCursorPos(&currentPos);
                    handleMouseMovement(currentPos);
                }
                break;

            case WM_MOUSEWHEEL:
                if (state.actionButtonDown) {
                    SHORT wheelDelta = GET_WHEEL_DELTA_WPARAM(mouseInfo->mouseData);
                    // Determine scroll direction and use appropriate Alt+Tab variant
                    // Note: wheelDelta > 0 means scrolling up
                    performAltTab(wheelDelta > 0); // Up = Right, Down = Left
                    return 1; // Prevent the wheel event from being processed by other applications
                }
                break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function declarations for window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateTrayIcon(HWND hwnd);
void ShowContextMenu(HWND hwnd);
void AddToStartup();

// Add to Windows startup
void AddToStartup() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    
    HKEY hKey;
    LPCTSTR keyPath = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
    RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_SET_VALUE, &hKey);
    RegSetValueEx(hKey, TEXT("MouseGestures"), 0, REG_SZ, (BYTE*)szPath, (strlen(szPath) + 1) * sizeof(TCHAR));
    RegCloseKey(hKey);
}

// Create hidden window for system tray
HWND CreateHiddenWindow(HINSTANCE hInstance) {
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("MouseGesturesClass");
    
    RegisterClassEx(&wc);
    
    return CreateWindowEx(
        0,
        TEXT("MouseGesturesClass"),
        TEXT("Mouse Gestures"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
}

// Create system tray icon
void CreateTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAYICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TRAYICON));
    strcpy(nid.szTip, "Mouse Gestures (Right-click to exit)");
    
    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Show context menu
void ShowContextMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    
    hPopMenu = CreatePopupMenu();
    InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, TEXT("Exit"));
    
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hPopMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                ShowContextMenu(hwnd);
            }
            break;
            
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_EXIT) {
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
            break;
            
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Create hidden window and system tray icon
    hWnd = CreateHiddenWindow(hInstance);
    if (!hWnd) {
        MessageBoxA(NULL, "Failed to create window!", "Error", MB_ICONERROR);
        return 1;
    }
    
    CreateTrayIcon(hWnd);
    AddToStartup();

    // Set up mouse hook
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (mouseHook == NULL) {
        MessageBoxA(NULL, "Failed to set the mouse hook!", "Error", MB_ICONERROR);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        return 1;
    }

    // Get initial mouse speed
    SystemParametersInfo(SPI_GETMOUSESPEED, 0, &state.originalMouseSpeed, 0);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    setMouseSpeed(state.originalMouseSpeed);
    UnhookWindowsHookEx(mouseHook);
    Shell_NotifyIcon(NIM_DELETE, &nid);

    return 0;
}

