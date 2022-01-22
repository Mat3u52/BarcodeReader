#include <iostream>

#include <ctime>
#include <cstdio>
#include <clocale>
#include <string>
#include <windows.h>
#include <fstream>

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#include <ctime>

#include <cstdlib>

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

#include <filesystem>

namespace fs = std::filesystem;
using namespace std;
void killProcessByName(const char *filename);
bool pathValidator(string &pathToValid);

int main(){
//int WinMain(HINSTANCE hInstance,
//					HINSTANCE hPrevInstance,
//					LPTSTR    lpCmdLine,
//					int       nCmdShow){
//-----------Only one instance--------------------------------------
	HANDLE hMutex = CreateMutex( NULL, FALSE, "RnWMutex" );
	if( hMutex && GetLastError() != 0 ){
		//MessageBox( NULL, "The app already exists.",
		//	"HotShot", MB_ICONEXCLAMATION );
		PostQuitMessage( 0 );
		return 0;
	}

//-----------Only one instance / the end------------------------------
    bool flagRename = "false";
    bool flagIncoming = "false"; /// <-- add
    char file[] = "SMEMASignal1.txt";

    char t[100] = "";
    struct stat b;
    char tCurrent[100] = "";

    int synchronize = 0; /// <-- add

    for(;;){
    //cout<<synchronize<<endl;
        int amount = 0;
        time_t czas;
        time( & czas );
        strftime(tCurrent, 100, "%Y%m%d%H%M%S", localtime( & czas ));

        //cout << tCurrent << endl;

        if(!stat(file, & b)){
            //strftime(t, 100, "%d%m%Y%H%M%S", localtime(& b.st_mtime));
            strftime(t, 100, "%Y%m%d%H%M%S", localtime(& b.st_mtime));
            int tInt = atoi(t);
            int tCurrentInt = atoi(tCurrent);
            //cout << tCurrentInt << " " << tInt << endl;
            if(tCurrentInt == tInt ){
                //cout << "OK" << endl;



    char bufor[ 64 ];
    time_t czas;
    time( & czas );
    tm czasTM = * localtime( & czas );

    setlocale( LC_ALL, "English" );
    strftime( bufor, sizeof( bufor ), "%c", & czasTM );
    //printf( "[Zawartosc bufora]: \"%s\"\n", bufor );

    string dateTime = bufor;
    //cout << dateTime << endl;


fstream handle;
string recipe;

handle.open("C:\\cpi\\data\\names.txt");
//handle.open("names.txt"); // at home

if(handle.good()==false){
    exit(0);
}else{
    getline(handle, recipe);
}
handle.close();
    int recipeL = recipe.length();
    recipe.erase(recipeL-4,4);
    recipe.erase(0,2);
    //cout << recipe << endl;

fstream handleB;
string barcode;

handleB.open("barcode001.txt");

if(handleB.good()==false){
    exit(0);
}else{
    getline(handleB, barcode);
}
handleB.close();

//cout << barcode << endl;

                //Sleep(2000);
                //Sleep(500);

if(barcode.length() > 6 && barcode.length() < 20){

//flagIncoming = true; /// <-- add
//if(synchronize<=1){ /// <-- add
//    synchronize++; /// <-- add
//}

if(barcode.compare("NOREAD") != 0){
    fstream plik;
    plik.open("barcodelist.bar", ios::out);
    plik<<"#Board: ";
    plik<<recipe<<", ";
    plik<<dateTime<<"\n";
    plik<<"\n#Number Of Panel	Barcode\n";
    plik<<"Barcode#1	*"<<barcode<<"\n";
    plik<<"#End";
    plik.close();


if(barcode.compare("Barcode0") != 0){
    flagIncoming = true; /// <-- add
    if(synchronize<=1){ /// <-- add
        synchronize++; /// <-- add
    }
}

}else{
    fstream plik;
    plik.open("barcodelist.bar", ios::out);
    plik<<"#Board: ";
    plik<<recipe<<", ";
    plik<<dateTime<<"\n";
    plik<<"\n#Number Of Panel	Barcode\n";
    plik<<"Barcode#1	*Barcode0\n";
    plik<<"#End";
    plik.close();
}
                Sleep(500); // in work
                //Sleep(1000); // in work
                //Sleep(20000);
}else{
killProcessByName("Mondelbrot.exe");
Sleep(2000);
ShellExecute(NULL, "open", "Mandelbrot.exe", NULL, NULL, 0);
}
//----Start clear barcode001 file
fstream clearFile;
clearFile.open("barcode001.txt", ios::out);
clearFile<<"NOREAD";
clearFile.close();
//----The end clesr file

            }
        }else{
            printf("Lack of the source to file. \n");
        }







for(const auto& entry : fs::directory_iterator("C:\\Defects\\ComportSignal\\")){ ///source
        const auto filenameStr = entry.path().filename().string();
            if(entry.is_directory()){
                cout << "dir:  " << filenameStr << '\n';
                //i++;
                //int filenameInt = atoi(filenameStr.c_str());
                //i = filenameInt;

            }else if (entry.is_regular_file()){
                //cout << "file: " << filenameStr << '\n';
                amount++;
                if(amount>1){
                    //cout << "to remove" << endl;
                    string pathToRemove = "C:\\Defects\\ComportSignal\\"+filenameStr;
                    flagRename = true;
                    Sleep(500);
                    fs::remove(pathToRemove);

                    //if(synchronize>=1){
                    //    synchronize--; /// <-- add
                    //}
                }
            }else{
                cout << "?? "<< filenameStr << '\n';
            }
}
    if(flagRename == true){
        string pathToFile = "C:\\Defects\\BuyOffControl.exe";
        if(pathValidator(pathToFile) == true){
            Sleep(500);
            fs::rename("C:\\Defects\\BuyOffControl.exe", "C:\\Defects\\BuyOffControl1.exe");
            Sleep(500);
        }
        flagRename = false;

        if(synchronize>=1){
            synchronize--; /// <-- add
        }

        //Sleep(3000); /// <-- add
    }
    //else{ /// <-- add
    //    flagRename = false; /// <-- add
   // }

    if(flagIncoming == true && synchronize == 2){ /// <-- add
        string pathToFile1 = "C:\\Defects\\BuyOffControl1.exe";
        if(pathValidator(pathToFile1) == true){
            Sleep(500);
            fs::rename("C:\\Defects\\BuyOffControl1.exe", "C:\\Defects\\BuyOffControl.exe"); /// turn on the Bye Off Control
            Sleep(500);
        }
        //flagRename = false;
        flagIncoming = false; /// <-- add
    }else{ /// <--  add
        flagIncoming = false; /// <-- add
    }












    }

    return 0;
}

void killProcessByName(const char *filename){
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

bool pathValidator(string &pathToValid){
    struct stat st;
    char pathChar[pathToValid.length()+1];
    strcpy(pathChar, pathToValid.c_str());

    if(stat(pathChar, &st) == 0){
        return true;
    }else{
        return false;
    }

    return false;
}
