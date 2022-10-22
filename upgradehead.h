#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <cmath>

using namespace std;

class MemPoint{
    private:
        DWORD_PTR baseadress;
        DWORD_PTR addresstoadd;
        vector<DWORD_PTR> offsets;
        DWORD_PTR address;
        HANDLE hProc;
    public:
        void writememory(float a){
            WriteProcessMemory(hProc, (LPVOID)address, &a, sizeof(a), NULL);
        }
        void readmemory(float *a){
            ReadProcessMemory(hProc, (LPVOID)address, a, sizeof(*a), NULL);
        }
        MemPoint(DWORD_PTR baseadress1, DWORD_PTR addresstoadd1,vector<DWORD_PTR> offsets1,HANDLE hProc1){
            baseadress = baseadress1;
            addresstoadd = addresstoadd1;
            offsets=offsets1;
            hProc=hProc1;
            DWORD_PTR pointerNY;
            //reading first address
            ReadProcessMemory(hProc, (LPVOID*)(baseadress + addresstoadd), &pointerNY, sizeof(pointerNY), NULL);
            
            //adding offsets and finding what addresses are there and then adding offsets to those
            for (int i = 0; i < offsets.size() - 1; i++) {
                ReadProcessMemory(hProc, (LPVOID*)(pointerNY + offsets[i]), &pointerNY, sizeof(pointerNY), NULL);
            }

            //adding the last address that points to the value and not to a address
            pointerNY += offsets[offsets.size() - 1];

            address=pointerNY;
        }
};