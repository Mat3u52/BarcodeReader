#define IDB_BUTTON1 1000
#define IDB_BUTTON2 1001
#define IDB_BUTTON3 1002
#define PRODUCT_NAME "mandelbrot"

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

int flag;
//int flag10;
RECT rect;

void Draw(HDC hdc){

    Rectangle(hdc, 10, 19, 225, 100);
    HGDIOBJ original = NULL;
    original = SelectObject(hdc, GetStockObject(DC_PEN));

    switch(flag){
    case 1:
        {
        SetDCPenColor(hdc, RGB(0, 240, 0));
            MoveToEx(hdc, 20, 85, NULL);
            LineTo(hdc, 110, 85);
            MoveToEx(hdc, 20, 84, NULL);
            LineTo(hdc, 110, 84);
        SelectObject(hdc, original);
        }
        break;
    case 2:
    {
    SetDCPenColor(hdc, RGB(250, 0, 0));
        MoveToEx(hdc, 125, 85, NULL);
        LineTo(hdc, 215, 85);
        MoveToEx(hdc, 125, 84, NULL);
        LineTo(hdc, 215, 84);
    SelectObject(hdc, original);
    }
        break;
    }

}
/*
//void Draw01(HDC hdc){
std::string Draw01(){
    switch(flag10){
    case 3:
            SYSTEMTIME czas;
            char tab[32];
            char tekst[128];

            SYSTEMTIME st;
            GetLocalTime(&czas);
            if(czas.wHour<10){
                strcpy(tekst, "0");
                itoa(czas.wHour, tab, 10);
                strcat(tekst, tab);
            }else{
                itoa(czas.wHour, tekst, 10);
            }
            strcat(tekst, ":");
            itoa(czas.wMinute, tab, 10);
            if(czas.wMinute<10) strcat(tekst, "0");
                strcat(tekst, tab);
                itoa(czas.wSecond, tab, 10);
                strcat(tekst, ":");
            if(czas.wSecond<10) strcat(tekst, "0");
                strcat(tekst, tab);

            //SetBkMode(hdc, TRANSPARENT);
            //TextOut(hdc, 20, 130, tekst, strlen(tekst));

            //std::string time01 = tekst;
            return tekst;
            //TextOut(hdc, 20, 150, showSN.c_str(), strlen(showSN.c_str()));

            //InvalidateRect(NULL, NULL, FALSE);
    break;
    }

}
*/
bool fileExists(const std::string& fileName){
    std::fstream plikBL;
    plikBL.open(fileName.c_str(), std::ios::in); //| std::ios::nocreate
    if(plikBL.is_open()){
        plikBL.close();
        return true;
    }
    plikBL.close();
    return false;
}
//void logSave(std::string logTime, std::string logSN){
void logSave(std::string logSN){

    SYSTEMTIME timeSave;
    GetLocalTime(&timeSave);

    char bufferLog[256];
    sprintf(bufferLog,
             "%d-%02d-%02d",
             timeSave.wYear,
             timeSave.wMonth,
             timeSave.wDay,
             timeSave.wHour,
             timeSave.wMinute,
             timeSave.wSecond,
             timeSave.wMilliseconds);

    std::string strTimeSave = "log/";

    strTimeSave += bufferLog;
    strTimeSave += ".txt";

    SYSTEMTIME czas000;
    char tab000[32];
    char tekst000[128];

    GetLocalTime(&czas000);
        if(czas000.wHour<10){
            strcpy(tekst000, "0");
            itoa(czas000.wHour, tab000, 10);
            strcat(tekst000, tab000);
        }else{
            itoa(czas000.wHour, tekst000, 10);
        }
        strcat(tekst000, ":");
        itoa(czas000.wMinute, tab000, 10);
        if(czas000.wMinute<10) strcat(tekst000, "0");
            strcat(tekst000, tab000);
            itoa(czas000.wSecond, tab000, 10);
            strcat(tekst000, ":");
        if(czas000.wSecond<10) strcat(tekst000, "0");
            strcat(tekst000, tab000);

    std::fstream fileLog;
    fileLog.open(strTimeSave, std::ios_base::out | std::ios::app);
    if(fileLog.is_open()){
        //fileLog<<logTime<<" "<<logSN<<std::endl;
        fileLog<<tekst000<<" "<<logSN<<std::endl;
        fileLog.close();
    }
    fileLog.close();
}
void killProcessByName(const char *filename){
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while(hRes){
        if(strcmp(pEntry.szExeFile, filename) == 0){
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if(hProcess != NULL){
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}
