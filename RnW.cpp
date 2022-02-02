#include <iostream>
#include <ctime>
#include <cstdio>
#include <clocale>
#include <string>
#include <windows.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

void killProcessByName(const char *filename);
bool pathValidator(string &pathToValid);

int main(){
    bool flagRename = "false";
    bool flagIncoming = "false";
    char file[] = "SMEMASignal1.txt";
    char t[100] = "";
    struct stat b;
    char tCurrent[100] = "";
    int synchronize = 0;

//-----------Only one instance--------------------------------------
	HANDLE hMutex = CreateMutex( NULL, FALSE, "RnWMutex" );
	if( hMutex && GetLastError() != 0 ){
		PostQuitMessage( 0 );
		return 0;
	}
//-----------Only one instance / The End------------------------------
//-------------------initialize---------------------------------------
    int amountTemp = 0;
    for(const auto& entry : fs::directory_iterator("C:\\Defects\\ComportSignal\\")){ ///source
        const auto filenameStr0 = entry.path().filename().string();
        if(entry.is_regular_file()){
            amountTemp++;
            if(amountTemp>1){
                string pathToRemove0 = "C:\\Defects\\ComportSignal\\"+filenameStr0;
                fs::remove(pathToRemove0);
            }
        }
    }
    string pathToFile0 = "C:\\Defects\\BuyOffControl.exe";
    if(pathValidator(pathToFile0) == true){
        fs::rename("C:\\Defects\\BuyOffControl.exe", "C:\\Defects\\BuyOffControl1.exe");
    }
//-----------------The End initialize-------------------------------

    for(;;){
        //cout<<synchronize<<endl;
        int amount = 0;
        time_t timeCurrent;
        time(& timeCurrent);
        strftime(tCurrent, 100, "%Y%m%d%H%M%S", localtime(& timeCurrent));
        //cout << tCurrent << endl;
        if(!stat(file, & b)){
            strftime(t, 100, "%Y%m%d%H%M%S", localtime(& b.st_mtime));
            int tInt = atoi(t);
            int tCurrentInt = atoi(tCurrent);
            //cout << tCurrentInt << " " << tInt << endl;
            if(tCurrentInt == tInt){
                //cout << "OK" << endl;
                char buffer[64];
                time_t timeTM;
                time(& timeTM);
                tm timeTM0 = * localtime(& timeTM);

                setlocale(LC_ALL, "English");
                strftime(buffer, sizeof(buffer), "%c", & timeTM0);
                string dateTime = buffer;
                //cout << dateTime << endl;
                fstream handle;
                string recipe;
                handle.open("C:\\cpi\\data\\names.txt");

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

                if(barcode.length() > 6 && barcode.length() < 20){
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
                            flagIncoming = true;
                            if(synchronize<=1){
                                synchronize++;
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
                    Sleep(500);
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
        }
        for(const auto& entry : fs::directory_iterator("C:\\Defects\\ComportSignal\\")){ ///source
            const auto filenameStr = entry.path().filename().string();
            if(entry.is_regular_file()){
                //cout << "file: " << filenameStr << '\n';
                amount++;
                if(amount>1){
                    //cout << "to remove" << endl;
                    string pathToRemove = "C:\\Defects\\ComportSignal\\"+filenameStr;
                    flagRename = true;
                    Sleep(500);
                    fs::remove(pathToRemove);
                }
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
                synchronize--;
            }
        }

        if(flagIncoming == true && synchronize == 2){
            string pathToFile1 = "C:\\Defects\\BuyOffControl1.exe";
            if(pathValidator(pathToFile1) == true){
                Sleep(500);
                fs::rename("C:\\Defects\\BuyOffControl1.exe", "C:\\Defects\\BuyOffControl.exe"); /// turn on the Bye Off Control
                Sleep(500);
            }
            flagIncoming = false;
        }else{
            flagIncoming = false;
        }
    }
    return 0;
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
