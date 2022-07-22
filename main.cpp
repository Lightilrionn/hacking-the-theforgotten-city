#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <cmath>
using namespace std;
#define PI 3.14159265
DWORD_PTR GetProcessBaseAddress(DWORD processID)
{
    DWORD_PTR   baseAddress = 0;
    HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    HMODULE* moduleArray;
    LPBYTE      moduleArrayBytes;
    DWORD       bytesRequired;

    if (processHandle)
    {
        if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
        {
            if (bytesRequired)
            {
                moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

                if (moduleArrayBytes)
                {
                    unsigned int moduleCount;

                    moduleCount = bytesRequired / sizeof(HMODULE);
                    moduleArray = (HMODULE*)moduleArrayBytes;

                    if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
                    {
                        baseAddress = (DWORD_PTR)moduleArray[0];
                    }

                    LocalFree(moduleArrayBytes);
                }
            }
        }

        CloseHandle(processHandle);
    }

    return baseAddress;
}
DWORD_PTR GetAddressPoiny(DWORD_PTR baseadress, vector<DWORD_PTR> offsets, DWORD_PTR addresstoadd, HANDLE hProc) {
    DWORD_PTR pointerNY;
    ReadProcessMemory(hProc, (LPVOID*)(baseadress + addresstoadd), &pointerNY, sizeof(pointerNY), NULL);
    for (int i = 0; i < offsets.size() - 1; i++) {
        ReadProcessMemory(hProc, (LPVOID*)(pointerNY + offsets[i]), &pointerNY, sizeof(pointerNY), NULL);
    }
    pointerNY += offsets[offsets.size() - 1];
    return pointerNY;
}
int main(void) {
    //TCHAR title[500];
    //int i = 0;
    //while (i < 10) {
        //GetWindowText(GetForegroundWindow(), title, 500);
        //printf("%s\n", title);
        //i++;
        //system("pause");
    //}
    

    HWND hWnd = FindWindowA(NULL, "The Forgotten City  ");
    if (hWnd == 0) {
        cerr << "Could not find window." << endl;
    }
    else {
        DWORD PID;
        GetWindowThreadProcessId(hWnd, &PID);
        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS , false, PID);
        if (!hProc) {
            cerr << "Cannot open process." << endl;
        }
        else {
            CONSOLE_CURSOR_INFO info;
            info.dwSize = 100;
            info.bVisible = false;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
            DWORD_PTR baseadress = GetProcessBaseAddress(PID);
            DWORD_PTR addresstoadd = 0x0419A7B0;
            vector<DWORD_PTR> offsetsY = {0x60, 0x1D8};
            vector<DWORD_PTR> offsetsX = { 0x60, 0x1D4 };
            vector<DWORD_PTR> offsetsZ = { 0x60, 0x1D0 };
            vector<DWORD_PTR> offsetsRightLeft = { 0x60, 0x194 };
            float let = -0.01;
            float hi = 0;
            vector<DWORD_PTR> xyzaddress = { GetAddressPoiny(baseadress, offsetsX, addresstoadd, hProc), GetAddressPoiny(baseadress, offsetsY, addresstoadd, hProc), GetAddressPoiny(baseadress, offsetsZ, addresstoadd, hProc) };
            DWORD_PTR rightLeftAdd = GetAddressPoiny(baseadress, offsetsRightLeft, addresstoadd, hProc);
            vector<float> xyz = { 0, 0, 0 };
            float rightLeftZ = 0;
            float rightLeftX = 0;
            while (!(GetAsyncKeyState(VK_BACK) & 0x8000)) {
                ReadProcessMemory(hProc, (LPVOID)xyzaddress[0], &xyz[0], sizeof(xyz[1]), NULL);
                ReadProcessMemory(hProc, (LPVOID)xyzaddress[1], &xyz[1], sizeof(xyz[1]), NULL);
                ReadProcessMemory(hProc, (LPVOID)xyzaddress[2], &xyz[2], sizeof(xyz[1]), NULL);
                ReadProcessMemory(hProc, (LPVOID)rightLeftAdd, &rightLeftZ, sizeof(rightLeftZ), NULL);
                ReadProcessMemory(hProc, (LPVOID)rightLeftAdd, &rightLeftX, sizeof(rightLeftX), NULL);
                rightLeftZ += 180;
                rightLeftX += 180;
                
                cout << xyz[0] << "/" << xyz[1] << "/" << xyz[2];
                if (GetAsyncKeyState(VK_LEFT) & 0x8000) {

                }
                if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {

                }
                if (GetAsyncKeyState(VK_UP) & 0x8000) {
                    
                    xyz[0] += -10*sin(rightLeftX * PI * 2 / 360);
                    xyz[2] += -10 * cos(rightLeftZ * PI * 2 / 360);
                    WriteProcessMemory(hProc, (LPVOID)xyzaddress[0], &xyz[0], sizeof(xyz[0]), NULL);
                    WriteProcessMemory(hProc, (LPVOID)xyzaddress[2], &xyz[2], sizeof(xyz[2]), NULL);
                }
                if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                    xyz[2] += rightLeftZ - 10;
                    xyz[0] += rightLeftX - 10;
                }
                Sleep(10);
                std::cout << "\x1B[2J\x1B[H";
            }
            cout << GetLastError();

            CloseHandle(hProc);

            cin.get();

        }

    }
    
    return 0;
}
