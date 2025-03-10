#include <iostream>
#include <windows.h>

bool rightButtonDown = false;
bool leftButtonDown = false;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* mouseInfo = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        if (wParam == WM_RBUTTONDOWN)
        {
            rightButtonDown = true;
        }
        else if (wParam == WM_RBUTTONUP)
        {
            rightButtonDown = false;
        }
        else if (wParam == WM_LBUTTONDOWN)
        {
            leftButtonDown = true;
        }
        else if (wParam == WM_LBUTTONUP)
        {
            leftButtonDown = false;
        }
        else if (rightButtonDown && wParam == WM_MOUSEWHEEL)
        {
            SHORT wheelDelta = GET_WHEEL_DELTA_WPARAM(mouseInfo->mouseData);
            if (wheelDelta > 0)
            {
                // Switch between windows (Alt+Tab)
                keybd_event(VK_MENU, 0, 0, 0);
                keybd_event(VK_TAB, 0, 0, 0);
                keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
            }
            else
            {
                // Minimize all windows and show desktop
                keybd_event(VK_LWIN, 0, 0, 0);
                keybd_event('D', 0, 0, 0);
                keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
            }
        }
        else if (leftButtonDown && rightButtonDown)
        {
            // Switch between windows (Alt+Tab)
            keybd_event(VK_MENU, 0, 0, 0);
            keybd_event(VK_TAB, 0, 0, 0);
            keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    if (mouseHook == NULL)
    {
        std::cerr << "Failed to set the mouse hook!" << std::endl;
        return 1;
    }

    // Adjust mouse wheel sensitivity
    int scrollSensitivity = 5;
    int originalScrollLines;
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &originalScrollLines, 0);
    SystemParametersInfo(SPI_SETWHEELSCROLLLINES, scrollSensitivity * originalScrollLines, NULL, SPIF_SENDCHANGE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Restore original mouse wheel sensitivity
    SystemParametersInfo(SPI_SETWHEELSCROLLLINES, originalScrollLines, NULL, SPIF_SENDCHANGE);

    UnhookWindowsHookEx(mouseHook);

    return 0;
}
