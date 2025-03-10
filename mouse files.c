#include <iostream>
#include <windows.h>

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* mouseInfo = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        if (wParam == WM_RBUTTONDOWN && GetKeyState(VK_LBUTTON) < 0)
        {
            POINT startPos;
            GetCursorPos(&startPos);

            MSG msg;
            bool gesturePerformed = false;

            while (!gesturePerformed)
            {
                if (GetKeyState(VK_LBUTTON) >= 0)
                {
                    POINT currentPos;
                    GetCursorPos(&currentPos);

                    if (currentPos.y < startPos.y)
                    {
                        // Show Windows Timeline
                        keybd_event(VK_LWIN, 0, 0, 0);
                        keybd_event('VK_TAB', 0, 0, 0);
                        keybd_event('VK_TAB', 0, KEYEVENTF_KEYUP, 0);
                        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
                        gesturePerformed = true;
                    }
                    else if (currentPos.x > startPos.x)
                    {
                        // Switch between windows (Alt+Tab)
                        keybd_event(VK_MENU, 0, 0, 0);
                        keybd_event(VK_TAB, 0, 0, 0);
                        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
                        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
                        gesturePerformed = true;
                    }
                    else if (currentPos.y > startPos.y)
                    {
                        // Minimize all windows and show desktop
                        keybd_event(VK_LWIN, 0, 0, 0);
                        keybd_event('D', 0, 0, 0);
                        keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
                        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
                        gesturePerformed = true;
                    }
                }

                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);

    return 0;
}

