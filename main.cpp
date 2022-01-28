#define NIIF_NONE 0x00000000
#define NIIF_INFO 0x00000001
#define NIIF_WARNING 0x00000002
#define NIIF_ERROR 0x00000003
#define NIIF_USER 0x00000004
#define NIIF_NOSOUND 0x00000010
#define NIIF_LARGE_ICON 0x00000020
#define NIIF_RESPECT_QUIET_TIME 0x00000080

#define NIF_INFO 0x00000010

#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include "Prog.h"

#define ID_TRAY1   601
#define CMSG_TRAY1 0x8001

#define _WIN32_IE 0x0600

#define NIN_BALLOONSHOW 0x00000402
#define NIN_BALLOONHIDE 0x00000403
#define NIN_BALLOONTIMEOUT 0x00000404
#define NIN_BALLOONUSERCLICK 0x00000405

#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

const WORD ID_TIMER = 3001;

DCB dcb;
HANDLE hNumPort;
char lpBuffor_read[33], lpBuffor_write[33];
DWORD RS_ile;
char tabw[4], tabr;
LPSTR buf;
LRESULT CALLBACK WinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
int dlugos;
COMSTAT statsread; //status
string showSN = "";
string showSN01 = "";
string showSN02 = "";
string showSN03 = "";
string showSN04 = "";

string time01 = "";
string time02 = "";
string time03 = "";
string time04 = "";
string time05 = "";

fstream plik;

bool endOfFile0 = false;
bool endOfFile1 = false;

int flag01 = 0;
bool flagBLExist = false;
bool flagBLExistDraw = false;

bool flagSync = false;

int WINAPI WinMain( HINSTANCE hTInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd ){
    WNDCLASSEX winc1;
    ZeroMemory( & winc1, sizeof( WNDCLASSEX ) );
    winc1.cbClsExtra = NULL;
    winc1.cbSize = sizeof( WNDCLASSEX );
    winc1.cbWndExtra = NULL;
    winc1.hbrBackground =( HBRUSH ) COLOR_WINDOW;
    winc1.hCursor = LoadCursor( NULL, IDC_ARROW );
    //winc1.hIcon = NULL;
    winc1.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    winc1.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1074));
    //winc1.hIconSm = NULL;
    winc1.hInstance = hTInst;
    winc1.lpfnWndProc =( WNDPROC ) WinProc;
    winc1.lpszClassName = "Window Class";
    winc1.lpszMenuName = NULL;
    winc1.style = CS_HREDRAW | CS_VREDRAW;

//-----------Only one instance--------------------------------------
	HANDLE hMutex = CreateMutex(NULL, FALSE, "COMMutex");
	if(hMutex && GetLastError() != 0){
		//MessageBox( NULL, "The app already exists.",
		//	"HotShot", MB_ICONEXCLAMATION );
		PostQuitMessage(0);
		return 0;
	}
//-----------Only one instance / the end------------------------------

//-----------Verification the Barcodelist.bar exist------------------
    if(fileExists("barcodelist.bar")){
       //cout<<"Plik istnieje"<< endl;
       //MessageBox(NULL, "barcodelist.bar - exist", "barcodelist.bar - exist", MB_ICONEXCLAMATION);
       flagBLExist = true;
    }else{
       //cout<<"Plik nie istnieje"<< endl;
       //MessageBox( NULL, "barcodelist.bar - not exist", "barcodelist.bar - not exist", MB_ICONEXCLAMATION );
       flagBLExist = false;
    }
//-----------Verification the Barcodelist.bar exist / the end-----------------------

    if(!RegisterClassEx(& winc1)){
        int nResult = GetLastError();
        MessageBox(NULL,
        "Window class creation failed\r\n",
        "Window Class Failed",
        MB_ICONERROR);
    }

    HWND hWnd = CreateWindowEx( NULL,
    "Window Class",
    "COM 10",
    //WS_OVERLAPPEDWINDOW,
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    200,
    200,
    240,
    440,
    HWND_DESKTOP,
    NULL,
    hTInst,
    NULL );

    if(!hWnd){
        int nResult = GetLastError();
            MessageBox(NULL,
                "Window creation Failed\r\n",
                "Window Creation Failed",
                MB_ICONERROR);
    }

//----Tray------------------------------------------------

    LPSTR sTip = "COM 10";
    LPSTR sTytul = "Communications Port (COM10)"; //add
    LPSTR sOpis = "I am ready to intercept."; //add
    NOTIFYICONDATA nid;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO; // add NIF_INFO
    nid.uCallbackMessage = CMSG_TRAY1;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    nid.dwInfoFlags = NIIF_INFO; // add
    lstrcpy(nid.szTip, sTip);
    lstrcpy( nid.szInfoTitle, sTytul ); //add
    lstrcpy( nid.szInfo, sOpis ); //add

    BOOL r;
    r = Shell_NotifyIcon(NIM_ADD, & nid);
    if(!r) MessageBox(hWnd, "No way, No icon...", "Leeee...", MB_ICONEXCLAMATION);

//---the end tray---------------------------------------

    //ShowWindow( hWnd, nShowCmd ); // hidden window
    ShowWindow(hWnd, SW_HIDE); // hidden window

    MSG msg;
    ZeroMemory(& msg, sizeof(MSG));

    while(GetMessage(& msg, NULL, 0, 0)){
        TranslateMessage( & msg );
        DispatchMessage( & msg );
    }

    return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){

PAINTSTRUCT ps;
HDC hdc;

RECT rect0[2];
rect0[0].left = 20;
rect0[0].right = 215;
rect0[0].top = 125;
rect0[0].bottom = 212;
rect0[1].left = 20;
rect0[1].right = 215;
rect0[1].top = 265;
rect0[1].bottom = 330;

    switch(msg){
        case WM_TIMER:
            {
            ClearCommError(hNumPort, NULL, & statsread); //sprawdza status
                if(statsread.cbInQue != 0){
                if(ReadFile(hNumPort, & tabr, 1, & RS_ile, 0)){
                    //char buf[ 256 ]; // unsigned
                    char buf[512];
                    wsprintf(buf, "%d", tabr);
                    endOfFile0 = true;

                    if(endOfFile0 == true && endOfFile1 == true){
                        plik.open("barcode001.txt", ios::out);
                        plik<<"";
                        plik.close();
                        endOfFile0 = false;
                        endOfFile1 = false;

                        showSN04 = showSN03;
                        showSN03 = showSN02;
                        showSN02 = showSN01;
                        showSN01 = showSN;



                        logSave(showSN);

                        showSN = "";
                        flag01 = 0;
                    }
                            //cout << "Dec "<< buf << endl; // add work
                            //cout << "(int) "<< (int)buf << endl;

                            //cout << endl;
                            int res0 = strcmp(buf, "48");
                            if(res0 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "0";
                                showSN += "0";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"0";
                                plik.close();
                            }
                            int res1 = strcmp(buf, "49");
                            if(res1 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "1";
                                showSN += "1";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"1";
                                plik.close();
                            }
                            int res2 = strcmp(buf, "50");
                            if(res2 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "2";
                                showSN += "2";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"2";
                                plik.close();
                            }
                            int res3 = strcmp(buf, "51");
                            if(res3 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "3";
                                showSN += "3";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"3";
                                plik.close();
                            }
                            int res4 = strcmp(buf, "52");
                            if(res4 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "4";
                                showSN += "4";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"4";
                                plik.close();
                            }
                            int res5 = strcmp(buf, "53");
                            if(res5 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "5";
                                showSN += "5";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"5";
                                plik.close();
                            }
                            int res6 = strcmp(buf, "54");
                            if(res6 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "6";
                                showSN += "6";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"6";
                                plik.close();
                            }
                            int res7 = strcmp(buf, "55");
                            if(res7 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "7";
                                showSN += "7";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"7";
                                plik.close();
                            }
                            int res8 = strcmp(buf, "56");
                            if(res8 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "8";
                                showSN += "8";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"8";
                                plik.close();
                            }
                            int res9 = strcmp(buf, "57");
                            if(res9 == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "9";
                                showSN += "9";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"9";
                                plik.close();
                            }
                            int resA = strcmp(buf, "65");
                            if(resA == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "A";
                                showSN += "A";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"A";
                                plik.close();
                            }
                            int resB = strcmp(buf, "66");
                            if(resB == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "B";
                                showSN += "B";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"B";
                                plik.close();
                            }
                            int resC = strcmp(buf, "67");
                            if(resC == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "C";
                                showSN += "C";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"C";
                                plik.close();
                            }
                            int resD = strcmp(buf, "68");
                            if(resD == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "D";
                                showSN += "D";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"D";
                                plik.close();
                            }
                            int resE = strcmp(buf, "69");
                            if(resE == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "E";
                                showSN += "E";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"E";
                                plik.close();
                            }
                            int resF = strcmp(buf, "70");
                            if(resF == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "F";
                                showSN += "F";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"F";
                                plik.close();
                            }
                            int resG = strcmp(buf, "71");
                            if(resG == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "G";
                                showSN += "G";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"G";
                                plik.close();
                            }
                            int resH = strcmp(buf, "72");
                            if(resH == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "H";
                                showSN += "H";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"H";
                                plik.close();
                            }
                            int resI = strcmp(buf, "73");
                            if(resI == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "I";
                                showSN += "I";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"I";
                                plik.close();
                            }
                            int resJ = strcmp(buf, "74");
                            if(resJ == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "J";
                                showSN += "J";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"J";
                                plik.close();
                            }
                            int resK = strcmp(buf, "75");
                            if(resK == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "K";
                                showSN += "K";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"K";
                                plik.close();
                            }
                            int resL = strcmp(buf, "76");
                            if(resL == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "L";
                                showSN += "L";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"L";
                                plik.close();
                            }
                            int resM = strcmp(buf, "77");
                            if(resM == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "M";
                                showSN += "M";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"M";
                                plik.close();
                            }
                            int resN = strcmp(buf, "78");
                            if(resN == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "N";
                                showSN += "N";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"N";
                                plik.close();
                            }
                            int resO = strcmp(buf, "79");
                            if(resO == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "O";
                                showSN += "O";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"O";
                                plik.close();
                            }
                            int resP = strcmp(buf, "80");
                            if(resP == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "P";
                                showSN += "P";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"P";
                                plik.close();
                            }
                            int resQ = strcmp(buf, "81");
                            if(resQ == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "Q";
                                showSN += "Q";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"Q";
                                plik.close();
                            }
                            int resR = strcmp(buf, "82");
                            if(resR == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "R";
                                showSN += "R";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"R";
                                plik.close();
                            }
                            int resS = strcmp(buf, "83");
                            if(resS == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "S";
                                showSN += "S";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"S";
                                plik.close();
                            }
                            int resT = strcmp(buf, "84");
                            if(resT == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "T";
                                showSN += "T";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"T";
                                plik.close();
                            }
                            int resU = strcmp(buf, "85");
                            if(resU == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "U";
                                showSN += "U";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"U";
                                plik.close();
                            }
                            int resV = strcmp(buf, "86");
                            if(resV == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "V";
                                showSN += "V";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"V";
                                plik.close();
                            }
                            int resW = strcmp(buf, "87");
                            if(resW == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "W";
                                showSN += "W";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"W";
                                plik.close();
                            }
                            int resX = strcmp(buf, "88");
                            if(resX == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "X";
                                showSN += "X";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"X";
                                plik.close();
                            }
                            int resY = strcmp(buf, "89");
                            if(resY == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "Y";
                                showSN += "Y";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"Y";
                                plik.close();
                            }
                            int resZ = strcmp(buf, "90");
                            if(resZ == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "Z";
                                showSN += "Z";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"Z";
                                plik.close();
                            }
                            int resSa = strcmp(buf, "97");
                            if(resSa == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "a";
                                showSN += "a";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"a";
                                plik.close();
                            }
                            int resSb = strcmp(buf, "98");
                            if(resSb == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "b";
                                showSN += "b";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"b";
                                plik.close();
                            }
                            int resSc = strcmp(buf, "99");
                            if(resSc == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "c";
                                showSN += "c";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"c";
                                plik.close();
                            }
                            int resSd = strcmp(buf, "100");
                            if(resSd == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "d";
                                showSN += "d";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"d";
                                plik.close();
                            }
                            int resSe = strcmp(buf, "101");
                            if(resSe == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "e";
                                showSN += "e";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"e";
                                plik.close();
                            }
                            int resSf = strcmp(buf, "102");
                            if(resSf == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "f";
                                showSN += "f";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"f";
                                plik.close();
                            }
                            int resSg = strcmp(buf, "103");
                            if(resSg == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "g";
                                showSN += "g";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"g";
                                plik.close();
                            }
                            int resSh = strcmp(buf, "104");
                            if(resSh == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "h";
                                showSN += "h";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"h";
                                plik.close();
                            }
                            int resSi = strcmp(buf, "105");
                            if(resSi == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "i";
                                showSN += "i";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"i";
                                plik.close();
                            }
                            int resSj = strcmp(buf, "106");
                            if(resSj == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "j";
                                showSN += "j";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"j";
                                plik.close();
                            }
                            int resSk = strcmp(buf, "107");
                            if(resSk == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "k";
                                showSN += "k";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"k";
                                plik.close();
                            }
                            int resSl = strcmp(buf, "108");
                            if(resSl == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "l";
                                showSN += "l";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"l";
                                plik.close();
                            }
                            int resSm = strcmp(buf, "109");
                            if(resSm == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "m";
                                showSN += "m";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"m";
                                plik.close();
                            }
                            int resSn = strcmp(buf, "110");
                            if(resSn == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "n";
                                showSN += "n";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"n";
                                plik.close();
                            }
                            int resSo = strcmp(buf, "111");
                            if(resSo == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "o";
                                showSN += "o";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"o";
                                plik.close();
                            }
                            int resSp = strcmp(buf, "112");
                            if(resSp == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "p";
                                showSN += "p";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"p";
                                plik.close();
                            }
                            int resSq = strcmp(buf, "113");
                            if(resSq == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "q";
                                showSN += "q";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"q";
                                plik.close();
                            }
                            int resSr = strcmp(buf, "114");
                            if(resSr == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "r";
                                showSN += "r";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"r";
                                plik.close();
                            }
                            int resSs = strcmp(buf, "115");
                            if(resSs == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "s";
                                showSN += "s";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"s";
                                plik.close();
                            }
                            int resSt = strcmp(buf, "116");
                            if(resSt == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "t";
                                showSN += "t";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"t";
                                plik.close();
                            }
                            int resSu = strcmp(buf, "117");
                            if(resSu == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "u";
                                showSN += "u";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"u";
                                plik.close();
                            }
                            int resSv = strcmp(buf, "118");
                            if(resSv == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "v";
                                showSN += "v";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"v";
                                plik.close();
                            }
                            int resSw = strcmp(buf, "119");
                            if(resSw == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "w";
                                showSN += "w";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"w";
                                plik.close();
                            }
                            int resSx = strcmp(buf, "120");
                            if(resSx == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "x";
                                showSN += "x";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"x";
                                plik.close();
                            }
                            int resSy = strcmp(buf, "121");
                            if(resSy == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "y";
                                showSN += "y";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"y";
                                plik.close();
                            }
                            int resSz = strcmp(buf, "122");
                            if(resSz == 0){
                                //MessageBox( NULL, "E", "Inf", MB_ICONINFORMATION );
                                //cout << "z";
                                showSN += "z";
                                plik.open("barcode001.txt", ios::out | ios::app);
                                plik<<"z";
                                plik.close();
                            }

                        }

            }else{
                endOfFile1 = true;

            if(flag01 == 0 ){
                InvalidateRect(hWnd, &rect0[0], true);
                flag10 = 3;
                flag01 = 1;

//---------------Notification for pcb-------------

//if(endOfFile0 == false){
//if(endOfFile0 == true){
//if( (strlen(showSN.c_str()) >  4) && flag01 == 0 ){
/*
if(endOfFile0 == true){
    LPSTR sTip = "COM 10";
    LPSTR sTytul = "Serial number"; //add
    LPSTR sOpis = "time"; //add
    NOTIFYICONDATA nid;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO; // add NIF_INFO
    nid.uCallbackMessage = CMSG_TRAY1;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    nid.dwInfoFlags = NIIF_INFO; // add
    lstrcpy(nid.szTip, sTip);
    lstrcpy( nid.szInfoTitle, sTytul ); //add
    lstrcpy( nid.szInfo, sOpis ); //add
    BOOL r;
    r = Shell_NotifyIcon(NIM_ADD, & nid);
    if(!r) MessageBox(hWnd, "No way, No icon...", "Leeee...", MB_ICONEXCLAMATION);
}
*/
//-----------------------------------------------

            }



            }
//-------------Current Barcode----------------
if(flagBLExist == true){

string fileStr = "barcodelist.bar";
char * file = new char[ fileStr.size() + 1 ];
strcpy(file, fileStr.c_str());
        //cout << "file: " << filenameStr << '\n';
        //char file[] = "C:\\XML\\EPSILON\\"+filenameStr;

char t[100] = "";
struct stat b;
char tCurrent[100] = "";

time_t czas00;
time( & czas00 );
strftime(tCurrent, 100, "%Y%m%d%H%M%S.000", localtime( & czas00 ));

if(!stat(file, & b)){
//strftime(t, 100, "%d%m%Y%H%M%S", localtime(& b.st_mtime));
strftime(t, 100, "%Y%m%d%H%M%S.000", localtime(& b.st_mtime));
int tInt = atoi(t);
int tCurrentInt = atoi(tCurrent);
//cout << tCurrentInt << " " << tInt << endl;
//cout << tCurrentInt - tInt << endl;
if((tCurrentInt == tInt) && flagSync == true){
flagSync = false;
InvalidateRect(hWnd, &rect0[1], true);
flagBLExistDraw = true;
//logSave("21:21:21", "test");
//Sleep(500);
}else{
flagSync = true;
}
}

}
//-------------Current Barcode / the end------

        }
        break;
    case WM_CREATE:
        {
            if( SetTimer( hWnd, ID_TIMER, 1, NULL ) == 0 )
                 MessageBox( hWnd, "Some issue of timer!", "Err!", MB_ICONSTOP );

            //static HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE); <-- original
            static HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);

            static HWND hWndGroupbox = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Communication",
                                        WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                                        10,
                                        10,
                                        215,
                                        90,
                                        hWnd,
                                        NULL,
                                        hInst,
                                        NULL);
            static HWND hWndGroupbox01 = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Intercepted barcode",
                                        WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                                        10,
                                        110,
                                        215,
                                        130,
                                        hWnd,
                                        NULL,
                                        hInst,
                                        NULL);
            static HWND hWndGroupbox02 = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Current barcode",
                                        WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                                        10,
                                        250,
                                        215,
                                        90,
                                        hWnd,
                                        NULL,
                                        hInst,
                                        NULL);
            static HWND hWndButton1 = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Connect",
                                        WS_VISIBLE | WS_CHILD,
                                        20,
                                        40,
                                        90,
                                        40,
                                        hWnd,
                                        (HMENU) IDB_BUTTON1,
                                        hInst,
                                        NULL);
            static HWND hWndButton2 = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Disconnect",
                                        WS_VISIBLE | WS_CHILD,
                                        125,
                                        40,
                                        90,
                                        40,
                                        hWnd,
                                        (HMENU) IDB_BUTTON2,
                                        hInst,
                                        NULL);
            static HWND hWndButton3 = CreateWindowEx(
                                        0,
                                        "BUTTON",
                                        "Release PCB -->",
                                        WS_VISIBLE | WS_CHILD,
                                        25,
                                        350,
                                        190,
                                        40,
                                        hWnd,
                                        (HMENU) IDB_BUTTON3,
                                        hInst,
                                        NULL);

            rect.left = 10;
            rect.top = 19;
            rect.right = 220;
            rect.bottom = 100;
            flag = 1;

            hNumPort = CreateFile("\\\\.\\COM10", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
            dcb.DCBlength = sizeof(dcb);

            //dcb.BaudRate = CBR_9600; // cognex
            dcb.BaudRate = CBR_115200; // datalogic
            dcb.fParity = FALSE;
            dcb.Parity = NOPARITY;
            //dcb.Parity = EVENPARITY;
            dcb.StopBits = ONESTOPBIT;
            dcb.ByteSize = 8;
            dcb.fDtrControl = DTR_CONTROL_ENABLE;
            dcb.fRtsControl = DTR_CONTROL_ENABLE;
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fDsrSensitivity = FALSE;
            dcb.fAbortOnError = FALSE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            dcb.fErrorChar = FALSE;
            dcb.fNull = FALSE;

            if( SetCommState(hNumPort, & dcb)){
                //MessageBox( NULL, "COM10", "Inf", MB_ICONINFORMATION );
                //flag = 1;
            }else{
                MessageBox( NULL, "COM 10", "Disconnected", MB_ICONERROR );
                flag = 2;
            }

        }
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam)){
            case IDB_BUTTON1:
                InvalidateRect(hWnd, &rect, FALSE);

                //if(SetTimer( hWnd, ID_TIMER, 1, NULL ) == 0) // ??????????????????????
                // MessageBox(hWnd, "Some issue of timer!", "Err!", MB_ICONSTOP); // ????????????????

                hNumPort = CreateFile("\\\\.\\COM10", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                dcb.DCBlength = sizeof(dcb);

                //dcb.BaudRate = CBR_9600; // cognex
                dcb.BaudRate = CBR_115200; // datalogic
                dcb.fParity = FALSE;
                dcb.Parity = NOPARITY;
                //dcb.Parity = EVENPARITY;
                dcb.StopBits = ONESTOPBIT;
                dcb.ByteSize = 8;
                dcb.fDtrControl = DTR_CONTROL_ENABLE;
                dcb.fRtsControl = DTR_CONTROL_ENABLE;
                dcb.fOutxCtsFlow = FALSE;
                dcb.fOutxDsrFlow = FALSE;
                dcb.fDsrSensitivity = FALSE;
                dcb.fAbortOnError = FALSE;
                dcb.fOutX = FALSE;
                dcb.fInX = FALSE;
                dcb.fErrorChar = FALSE;
                dcb.fNull = FALSE;

                if(SetCommState(hNumPort, & dcb)){
                    flag = 1;
                    MessageBox(NULL, "COM 10", "Connected", MB_ICONINFORMATION);
                }else{
                    flag = 2;
                    MessageBox(NULL, "COM 10", "Disconnected", MB_ICONERROR);
                }

            break;
            case IDB_BUTTON2:
                flag = 2;
                InvalidateRect(hWnd, &rect, FALSE);
                CloseHandle(hNumPort);

                    MessageBox(NULL, "COM 10", "Disconnected", MB_ICONERROR);
            break;
            case IDB_BUTTON3:
                //flag = 3;
                //InvalidateRect(hWnd, &rect, FALSE);
                fstream fileTemp;
                fileTemp.open("C:\\Defects\\ComportSignal\\temp.txt", std::ios_base::out | std::ios::app);
                    if(fileTemp.is_open()){
                        fileTemp<<"temp"<<endl;
                        fileTemp.close();
                    }
                    fileTemp.close();

                   // MessageBox(NULL, "PCB", "Release", MB_ICONERROR);
            break;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        Draw(hdc);



        SetBkMode(hdc, TRANSPARENT);
        //TextOut(hdc, 15, 130, "1.", 2);
        //InvalidateRect(hWnd, &rect0[0], FALSE);


        if(endOfFile0 == true){//ok true
            //InvalidateRect(hWnd, &rect0[0], FALSE);
           // Draw01(hdc);
            TextOut(hdc, 90, 130, showSN.c_str(), strlen(showSN.c_str()));
            TextOut(hdc, 90, 150, showSN01.c_str(), strlen(showSN01.c_str()));
            TextOut(hdc, 90, 170, showSN02.c_str(), strlen(showSN02.c_str()));
            TextOut(hdc, 90, 190, showSN03.c_str(), strlen(showSN03.c_str()));
            TextOut(hdc, 90, 210, showSN04.c_str(), strlen(showSN04.c_str()));

            time05 = time04;
            time04 = time03;
            time03 = time02;
            time02 = time01;


            SYSTEMTIME czas00;
            char tab[32];
            char tekst[128];

            SYSTEMTIME st;
            GetLocalTime(&czas00);
            if(czas00.wHour<10){
                strcpy(tekst, "0");
                itoa(czas00.wHour, tab, 10);
                strcat(tekst, tab);
            }else{
                itoa(czas00.wHour, tekst, 10);
            }
            strcat(tekst, ":");
            itoa(czas00.wMinute, tab, 10);
            if(czas00.wMinute<10) strcat(tekst, "0");
                strcat(tekst, tab);
                itoa(czas00.wSecond, tab, 10);
                strcat(tekst, ":");
            if(czas00.wSecond<10) strcat(tekst, "0");
                strcat(tekst, tab);
                time01 = tekst;

            TextOut(hdc, 20, 130, time01.c_str(), strlen(time01.c_str()));
            TextOut(hdc, 20, 150, time02.c_str(), strlen(time02.c_str()));
            TextOut(hdc, 20, 170, time03.c_str(), strlen(time03.c_str()));
            TextOut(hdc, 20, 190, time04.c_str(), strlen(time04.c_str()));
            TextOut(hdc, 20, 210, time05.c_str(), strlen(time05.c_str()));

        }
        //Draw01(hdc);
if(flagBLExistDraw == true){
ifstream read01("barcodelist.bar");
std::string rowStr = "";
std::string rowStr01 = "";
std::string rowStr02 = "";
std::string rowStr03 = "";
	if(read01.is_open()){
		//char row[10000];//maksymalnie 9999 znakw w wierszu
		while(!read01.eof()){
                getline(read01,rowStr);
                std::size_t found = rowStr.find("Barcode#1");
                 if(found!=std::string::npos){
                    rowStr01 = rowStr;
                    rowStr01.erase(0,11);
                 }
                std::size_t found01 = rowStr.find("#Board:");
                 if(found01!=std::string::npos){
                    rowStr02 = rowStr;
                    //rowStr02.erase(0,33);
                    rowStr02.erase(rowStr02.begin()+0, rowStr02.end()-21); // it need to improvement
                    rowStr03 = rowStr;
                    rowStr03.erase(0,8);
                    //rowStr03.erase(rowStr03.begin()+22, rowStr03.end());
                    rowStr03.erase(rowStr03.find(','));
                 }
		}
		read01.close();
		TextOut(hdc, 20, 270, rowStr01.c_str(), strlen(rowStr01.c_str()));
		TextOut(hdc, 20, 290, rowStr02.c_str(), strlen(rowStr02.c_str()));
		TextOut(hdc, 20, 310, rowStr03.c_str(), strlen(rowStr03.c_str()));
		//TextOut(hdc, 20, 150, rowStr01.c_str(), strlen(rowStr01[1].c_str()));
	}

flagBLExistDraw = false;
}



        EndPaint(hWnd, &ps);
        break;
    case WM_SIZE:
    {
        if(wParam == SIZE_MINIMIZED){
            ShowWindow(hWnd, SW_HIDE);

                LPSTR sTip = "COM 10";
                NOTIFYICONDATA nid;

                nid.cbSize = sizeof(NOTIFYICONDATA);
                nid.hWnd = hWnd;
                nid.uID = ID_TRAY1;
                nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                nid.uCallbackMessage = CMSG_TRAY1;
                nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
                lstrcpy(nid.szTip, sTip);

                BOOL r;
                r = Shell_NotifyIcon( NIM_ADD, & nid );
                if(!r) MessageBox(hWnd, "No way, No icon...", "Leeee...", MB_ICONEXCLAMATION);

        }else{
            ShowWindow(hWnd, SW_SHOW);

                NOTIFYICONDATA nid;
                nid.cbSize = sizeof(NOTIFYICONDATA);
                nid.hWnd = hWnd;
                nid.uID = ID_TRAY1;
                nid.uFlags = 0;
                Shell_NotifyIcon(NIM_DELETE, & nid);
        }
    }
    break;
    case CMSG_TRAY1:
    {
        switch(wParam){
            case ID_TRAY1: // dla naszej ikonki
                switch(LOWORD(lParam)){
                    case NIN_BALLOONUSERCLICK: // gdy user kliknie balonik
                        //MessageBox( hWnd, "Fajnie e klikne mj balonik.", ";-)", MB_ICONEXCLAMATION );
                        ShowWindow(hWnd, SW_RESTORE);
                    break;
                    case NIN_BALLOONTIMEOUT: // gdy balonik zgaœnie samoczynnie
                        //MessageBox( hWnd, "Dlaczego nie klikn mojego balonika, co ?!", ";-(", MB_ICONEXCLAMATION );
                    break;
                    case WM_LBUTTONDOWN:
                        ShowWindow(hWnd, SW_RESTORE);
                    break;
                    default:
                        //ShowWindow(hWnd, SW_RESTORE);
                    break;
                }
            break;
        }
        //if(wParam == ID_TRAY1)
        //if(lParam == WM_LBUTTONDOWN)
        //   ShowWindow(hWnd, SW_RESTORE);
    }
    break;

    case WM_DESTROY:
        {
            if(IsIconic(hWnd)){
                NOTIFYICONDATA nid;
                nid.cbSize = sizeof(NOTIFYICONDATA);
                nid.hWnd = hWnd;
                nid.uID = ID_TRAY1;
                nid.uFlags = 0;
                Shell_NotifyIcon(NIM_DELETE, & nid);
            }
                CloseHandle(hNumPort);
                PostQuitMessage(0);
                return 0;
        }
        break;
                case IDB_BUTTON2:
                flag = 2;
                InvalidateRect(hWnd, &rect, FALSE);
                CloseHandle(hNumPort);

                    MessageBox(NULL, "COM 10", "Disconnected", MB_ICONERROR);
            break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}
