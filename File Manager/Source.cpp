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
#include "FAT32.h"

using namespace std;

#define MAX_LENGTH 510

void Menu() {
    cout << "Lua Chon Format cua USB:\n";
    cout << "1. FAT32" << endl;
    cout << "2. NTFS" << endl;
    cout << "Khac.Thoat chuong trinh" << endl;
}

void Menu1() {
    cout << "Lua Chon:\n";
    cout << "1. Xem thong tin boot sector FAT32" << endl;
    cout << "2.Xem thong tin thu muc goc " << endl;
    cout << "Khac.Thoat chuong trinh" << endl;
}
void Menu2() {
    cout << "Lua Chon:\n";
    cout << "1. Xem thong tin partition boot sector NTFS" << endl;
    cout << "2.Xem thong tin thu muc goc " << endl;
    cout << "Khac.Thoat chuong trinh" << endl;
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

    int checkReadable = ReadSector(drivepath, 0, sector);
    if (checkReadable == -1) return 0;

    
    int formattype = 1;
    int choose = 1;
    do {
        Menu();
        cout << "Nhap lua chon ";
        cin >> formattype;
        switch (formattype)
        {
        case 1: {
            BPB _bpb;
            memcpy(&_bpb, sector, 512);
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
                    //int continuecase = 1;
                    do {
                        cout << "Lua Chon:\n";
                        cout << "1. Xem noi dung tap tin/ thu muc" << endl;
                        cout << "2. Quay lai Menu" << endl;
                        cout << "Khac. Thoat chuong trinh " << endl;
                        cout << "Nhap lua chon ";
                        cin >> choose;
                        switch (choose) {
                        case 1: {
                            exploreFAT32(_bpb, drivepath, container);
                        }
                        case 2: {
                            break;
                        }
                        default: return 0;
                        }
                        break;
                    } while (true);
                    break;
                }
                default: {
                    return 0;
                }
                }
            } while (true);
        }
        case 2: {
            NTFS _bpb;
            memcpy(&_bpb, sector, 512);
            do {
                Menu2();
                cout << "Nhap lua chon ";
                cin >> choose;
                switch (choose)
                {
                case 1: {
                    showPartitionBootSectorInformation(_bpb);
                    break;
                }
                default:
                    return 0;
                }
            } while (true);
        }
        default:
            return 0;
        }
    } while (true);
    



    return 0;


}