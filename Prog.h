#define IDB_BUTTON1 1000
#define IDB_BUTTON2 1001
#define IDB_BUTTON3 1002
#define PRODUCT_NAME "mandelbrot"

int flag;
int flag10;
RECT rect;
std::string time01 = "";
std::string time02 = "";
std::string time03 = "";
std::string time04 = "";
std::string time05 = "";
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
void Draw01(HDC hdc){
time05 = time04;
time04 = time03;
time03 = time02;
time02 = time01;
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

            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 20, 130, tekst, strlen(tekst));

            TextOut(hdc, 20, 150, time02.c_str(), strlen(time02.c_str()));
            TextOut(hdc, 20, 170, time03.c_str(), strlen(time03.c_str()));
            TextOut(hdc, 20, 190, time04.c_str(), strlen(time04.c_str()));
            TextOut(hdc, 20, 210, time05.c_str(), strlen(time05.c_str()));
            //std::string time01 = tekst;

            //TextOut(hdc, 20, 150, showSN.c_str(), strlen(showSN.c_str()));

            //InvalidateRect(NULL, NULL, FALSE);
    break;
    }

}
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


    std::fstream fileLog;
    fileLog.open(strTimeSave, std::ios_base::out | std::ios::app);
    if(fileLog.is_open()){
        //fileLog<<logTime<<" "<<logSN<<std::endl;
        fileLog<<time01<<" "<<logSN<<std::endl;
        fileLog.close();
    }
    fileLog.close();
}
