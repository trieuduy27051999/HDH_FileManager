#include <windows.h>
#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <codecvt>
#include <string>
#include <stdio.h>

#include "mystruct.h"
#include "sector.h"

using namespace std;

#define MAX_LENGTH 510

void Menu1() {
    cout << "Lua Chon:\n";
    cout << "1. Xem thong tin boot sector FAT32" << endl;
    cout << "2.Xem thong tin thu muc goc " << endl;
    cout << "Khac.Thoat chuong trinh" << endl;
}
void Menu2() {
    cout << "Lua Chon:\n";
    cout << "1. Xem noi dung tap tin/ thu muc" << endl;
    cout << "2. Quay lai Menu" << endl;
    cout << "Khac. Thoat chuong trinh " << endl;
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);

    BYTE sector[512];
    memset(sector, 0, 512);

    //Get USB drive letter
    cout << "Drive letter of USB?: ";
    char path[] = "\\\\.\\?:";
    scanf_s("%c", path + 4, 1);
    cout << "Drive path is: " << path <<"\n";

    //Convert char to LPCWSTR
    wchar_t wtext[20];
    mbstowcs(wtext, path, strlen(path) + 1);//Plus null
    LPCWSTR drivepath = wtext;

    ReadSector(drivepath, 0, sector);

    BPB _bpb;
    memcpy(&_bpb, sector, 512);

    int choose = 1;
    do {
        Menu1();
        cout << "Nhap lua chon ";
        cin >> choose;
        switch (choose) {
        case 1: {
            showBootSectorInformation(_bpb);
            break;
        }
        case 2: {
            vector<DWORD> clusters_root;
            DWORD currentCluster = 0;
            while (currentCluster != -1) {
                clusters_root.push_back(currentCluster);
                DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt, drivepath);
                currentCluster = nextCluster;
            }
            vector<FileInfo> container = showRootDirectoryOrSubFolder(clusters_root, _bpb, drivepath);

            for (int i = 0; i < container.size(); i++) {
                cout << "Number " << i << endl;
                container[i].Print();
            }
            do {
                Menu2();
                cout << "Nhap lua chon ";
                cin >> choose;
                switch (choose) {
                case 1: {
                    int stt = -1;
                    do {
                        cout << "Nhap so thu tu thu muc/tap tin de xem noi dung (STT phai hop le) \n";
                        cin >> stt;
                        system("cls");
                    } while (stt < 0 && stt >= container.size());
                    FileInfo f = container[stt];

                    if (f.fileAttributes == "Directory") {
                        container.clear();
                        vector<DWORD> clusters_sub;
                        DWORD currentCluster = f.firstClusterAddress;
                        while (currentCluster != -1) {
                            clusters_sub.push_back(currentCluster);
                            DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt, drivepath);
                            currentCluster = nextCluster;
                        }
                        container = showRootDirectoryOrSubFolder(clusters_sub, _bpb, drivepath);
                        for (int i = 0; i < container.size(); i++) {
                            cout << "Number " << i << endl;
                            container[i].Print();
                        }


                    }
                    if (f.fileAttributes == "Archive") {
                        for (int i = 0; i < container.size(); i++) {
                            cout << "Number " << i << endl;
                            container[i].Print();
                        }
                        int pos = f.FileName.find(L".txt");
                        if (pos != wstring::npos) {
                            vector<DWORD> clusters_file;
                            DWORD currentCluster = f.firstClusterAddress;
                            while (currentCluster != -1) {
                                clusters_file.push_back(currentCluster);
                                DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt, drivepath);
                                currentCluster = nextCluster;
                            }
                            wcout << L"\nData of Txt :" << f.FileName << endl;;
                            PrintDataOfArchive(clusters_file, f.sizeOfFile, _bpb, drivepath);
                            cout << endl;
                        }
                        else {
                            cout << "\n Need compatible software to open ! \n";
                        }

                    }
                    break;


                }
                case 2:break;
                default:
                    return 0;
                }
            } while (true);

        }
        default: {
            return 0;
        }
        }
    } while (true);



    return 0;


}