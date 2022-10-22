#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <cmath>
using namespace std;
#define PI 3.14159265
#include "upgradehead.h"
//getting base address of a process. not my function
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
/*
//getting address from offset
DWORD_PTR GetAddressPoiny(DWORD_PTR baseadress, vector<DWORD_PTR> offsets, DWORD_PTR addresstoadd, HANDLE hProc) {
    DWORD_PTR pointerNY;
    //reading first address
    ReadProcessMemory(hProc, (LPVOID*)(baseadress + addresstoadd), &pointerNY, sizeof(pointerNY), NULL);
    
    //adding offsets and finding what addresses are there and then adding offsets to those
    for (int i = 0; i < offsets.size() - 1; i++) {
        ReadProcessMemory(hProc, (LPVOID*)(pointerNY + offsets[i]), &pointerNY, sizeof(pointerNY), NULL);
    }

    //adding the last address that points to the value and not to a address
    pointerNY += offsets[offsets.size() - 1];

    return pointerNY;
}
//the same as above, but with vectors
vector<DWORD_PTR> GetAddressVector(DWORD_PTR baseadress, vector<vector<DWORD_PTR>> offsets, vector<DWORD_PTR> addresstoadd, HANDLE hProc) {
    vector<DWORD_PTR> pointerNY(offsets.size());
    for(int i = 0; i < offsets.size(); i++) 
        ReadProcessMemory(hProc, (LPVOID*)(baseadress + addresstoadd[i]), &pointerNY[i], sizeof(pointerNY[i]), NULL);
    for(int j = 0; j < offsets.size(); j++) 
        for (int i = 0; i < offsets[j].size() - 1; i++) 
            ReadProcessMemory(hProc, (LPVOID*)(pointerNY[j] + offsets[j][i]), &pointerNY[j], sizeof(pointerNY[j]), NULL);
    for(int i = 0; i < offsets.size(); i++) {
        pointerNY[i] += offsets[i][offsets[i].size() - 1];
    }
    return pointerNY;
}
*/
//creating a structure for coordinates
struct coordinates{
    float x, y, z;
};
/*
//easier function for writing memory
void writememory(float a, DWORD_PTR address, HANDLE hProc){
    WriteProcessMemory(hProc, (LPVOID)address, &a, sizeof(a), NULL);
}
//easier function for reading memory
void readmemory(float *a, DWORD_PTR address, HANDLE hProc){
    ReadProcessMemory(hProc, (LPVOID)address, a, sizeof(*a), NULL);
}
*/
int main(void) {
    //finding window
    HWND hWnd = FindWindowA(NULL, "The Forgotten City  ");
    if (hWnd == 0) {
        cerr << "Could not find window." << endl;
    }
    else {
        //getting access to window memory 
        DWORD PID;
        GetWindowThreadProcessId(hWnd, &PID);
        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS , false, PID);

        if (!hProc) {
            cerr << "Cannot open process." << endl;
        }
        else {
            //setting console cursor to invincible
            CONSOLE_CURSOR_INFO info;
            info.dwSize = 100;
            info.bVisible = false;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

            //getting base address
            DWORD_PTR baseadress = GetProcessBaseAddress(PID);

            //getting coordinates and yaw addresses
            
            vector<DWORD_PTR> addresstoadd = {0x0419A7B0, 0x0419A7B0, 0x0419A7B0};
            vector<vector<DWORD_PTR>> offsetsxyz = {{ 0x60, 0x1D4 }, {0x60, 0x1D8}, { 0x60, 0x1D0 }};
            vector<DWORD_PTR> offsetsRightLeft = { 0x60, 0x194 };
            vector<MemPoint> pointsxyz = {MemPoint(baseadress, addresstoadd[0], offsetsxyz[0], hProc), MemPoint(baseadress, addresstoadd[1], offsetsxyz[1], hProc), MemPoint(baseadress, addresstoadd[2], offsetsxyz[2], hProc)};
            MemPoint pointyaw(baseadress, addresstoadd[0], offsetsRightLeft, hProc);
            //declaring variables
            coordinates coor;
            float yaw = 0;

            while (!(GetAsyncKeyState(VK_BACK) & 0x8000)) {
                //getting varibles from game memory
                pointsxyz[0].readmemory(&coor.x);
                pointsxyz[1].readmemory(&coor.y);
                pointsxyz[2].readmemory(&coor.z);
                pointyaw.readmemory(&yaw);
                //adding to yaw 180, so it ranges from 360 to 0 and not from 180 to -180
                yaw += 180;

                cout << coor.x << "/" << coor.y << "/" << coor.z << "/" << yaw << "/";
                //checking if up arrow is pressed
                if (GetAsyncKeyState(VK_UP) & 0x8000) {
                    //math to get x and z coordinates from yaw
                    coor.x += -10*sin(yaw * PI * 2 / 360);
                    coor.z += -10 * cos(yaw * PI * 2 / 360);

                    //writing to process memory
                    pointsxyz[0].writememory(coor.x);
                    pointsxyz[2].writememory(coor.z);
                }
                //waiting for 10 ms
                Sleep(10);

                //clearing cmd output
                system("cls");
            }
            //getting errors from win 32
            cout << GetLastError();

            //closing handle
            CloseHandle(hProc);
        }
    } 
    return 0;
}
