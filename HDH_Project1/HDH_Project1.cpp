#include <windows.h>
#include<iostream>
#include<vector>
#include <array>
#include<sstream>
#include <codecvt>
#pragma warning(disable : 4996)
#include<string>
#include <stdio.h>
using namespace std;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef wchar_t WBYTE;
#define MAX_LENGTH 510
#pragma pack(1)
typedef struct BIOS_Paramenter_Block {
    BYTE BS_jmpBoot[3];
    BYTE BS_OEMName[8];
    WORD BPB_BytsPerSec;
    BYTE BPB_SecPerClus;
    WORD BPB_RsvdSecCnt;
    BYTE BPB_NumFATs;
    WORD BPB_RootEntCnt;
    WORD BPB_TotSec16;
    BYTE BPB_Media;
    WORD BPB_FATSz16;
    WORD BPB_SecPerTrk;
    WORD BPB_NumHeads;
    DWORD BPB_HiddSec;
    DWORD BPB_TotSec32;
    DWORD BPB_FATSz32;
    WORD BPB_ExtFlags;
    WORD BPB_FSVer;
    DWORD BPB_RootClus;
    WORD BPB_FSInfo;
    WORD BPB_BkBootSec;
    BYTE BPB_Reserved[12];
    BYTE BS_DrvNum;
    BYTE BS_Reserved1;
    BYTE BS_BootSig;
    DWORD BS_VolID;
    BYTE BS_VolLab[11];
    BYTE BS_FilSysType[8];
    BYTE BS_Bootcode[420];
    WORD BS_Signature;

} BPB;

typedef struct RootEntry {
    BYTE FileName[11];
    BYTE fileAttrributes;
    BYTE unused[10];
    WORD modifiedTime;
    WORD modifiedDate;
    WORD firstClusterAddress;
    DWORD sizeOfFile;

} root;

typedef struct LongFileName {
    BYTE entryNo;
    BYTE fileName_part1[10];
    BYTE fileAttributes;
    BYTE reversed;
    BYTE checkSum;
    BYTE fileName_part2[12];
    WORD firstClusterAddress;
    BYTE fileName_part3[4];
} LFN;

typedef struct FileInfo {
    wstring FileName;
    string fileAttributes;
    string modifiedTime;
    string modifiedDate;
    WORD firstClusterAddress;
    DWORD sizeOfFile;
    FileInfo() {
        FileName = L"";
        fileAttributes = "";
        modifiedTime = "";
        modifiedDate = "";
        firstClusterAddress = 0;
        sizeOfFile = 0;
    }
    void Print() {
        wcout << "File Name: " << FileName << endl;
        cout << "File Attributes : " << fileAttributes << endl;
        cout << "Modified Time : " << modifiedTime << endl;
        cout << "Modified Date : " << modifiedDate << endl;
        cout << "First Cluster Address: " << firstClusterAddress << endl;
        cout << "Size of File : " << sizeOfFile << endl;
        
    }
} File;
#pragma pack()
string DateToString(WORD Date) {
    WORD nYear = (Date >> 9) +1980;
    WORD nMonth = (Date << 7 );
    nMonth = nMonth >> 12;
    WORD nDay = (Date << 11);
    nDay = nDay >> 11;
    string ret = "";
    if (nDay < 10) {
        ret += ('0' + to_string(nDay) + '/');
    }
    else {
        ret += (to_string(nDay) + '/');
    }
    if (nMonth < 10) {
        ret += ('0' + to_string(nMonth) + '/');
    }
    else {
        ret += (to_string(nMonth) + '/');
    }
    ret += to_string(nYear);
    return ret;
}
string TimeToString(WORD Time) {
    WORD nHour = Time >> 11;
    WORD nMinute = (Time << 5);
    nMinute = nMinute >> 10;
    WORD nSecond = Time << 12;
    nSecond = nSecond >> 12;
    string ret = "";
    if (nHour < 10) {
        ret += ('0' + to_string(nHour) + ':');
    }
    else {
        ret += (to_string(nHour) + ':');
    }
    if (nMinute < 10) {
        ret += ('0' + to_string(nMinute) + ':');
    }
    else {
        ret += (to_string(nMinute) + ':');
    }
    if (nSecond < 10) {
        ret += ('0' + to_string(nSecond) );
    }
    else {
        ret +=  to_string(nSecond);
    }
    
    return ret;
   
}
bool is_empty_sector(BYTE sector[512]) {
    static const BYTE zero[512] = { 0 };
    return !memcmp(zero, sector, 512);
}
int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        
    }
}

void showBootSectorInformation(BPB _bpb) {
    
    printf("FAT32 USB disk information \n");
    printf("OEM Name : %s\n", _bpb.BS_OEMName);
    printf("Bytes per sector : %d \n", _bpb.BPB_BytsPerSec);
    printf("Sector per cluster : %d \n", _bpb.BPB_SecPerClus);
    printf("Number of sector for BootSector: %d \n", _bpb.BPB_RsvdSecCnt);
    printf("Number of sector in Volumn : %d \n", _bpb.BPB_TotSec32);
    printf("Number of Fats : %d \n", _bpb.BPB_NumFATs);
    printf("Number of sector in FATS : %d\n", _bpb.BPB_FATSz32);
    printf("First sector of FAT: %d \n", _bpb.BPB_RsvdSecCnt);
    printf("First sector of RDET: %d \n", _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs* _bpb.BPB_FATSz32);
    printf("First sector of RDET: %d \n", _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs * _bpb.BPB_FATSz32);
    
   
}

vector<FileInfo> showRootDirectoryOrSubFolder(vector<DWORD> clusters, BPB _bpb) {
    BYTE sector[512];
    vector<FileInfo> container;
    bool noEntry = false;
    int Index = 0;
    int nMaxLFNEntryForFile(0);
    WORD first_sector_RDET = _bpb.BPB_NumFATs * _bpb.BPB_FATSz32 + _bpb.BPB_RsvdSecCnt;
    WORD numSector;
    memset(sector, 0, 512);
    for (int i = 0; i < clusters.size(); i++) {
        numSector = clusters[i] * _bpb.BPB_SecPerClus + first_sector_RDET;
        int IND = 0;
        do {
            memset(sector, 0, 512);
            ReadSector(L"\\\\.\\E:", (IND + numSector) * 512, sector);
            if (is_empty_sector(sector)) {
                break;
            }


            BYTE* pByteRDET;
            pByteRDET = sector;
            wstring sLongFileName;
            wstring sLongFileNameTemp;
            BYTE RDETEntry[32];
            for (int index = 0; index < (512 / 32); index++) {


                memset(RDETEntry, 0, 32);
                memcpy(RDETEntry, pByteRDET, 32);


                if (RDETEntry[0] == 0x00) {
                    noEntry = true;
                    break;
                }
                else {
                    if (RDETEntry[11] == 0x0f) {

                        if (RDETEntry[0] == 0xE5) {
                            pByteRDET += 32;
                            continue;
                        }
                        wstring sFileName = L"";


                        nMaxLFNEntryForFile = int(RDETEntry[0] - 0x40);
                        
                        sLongFileName = L"";
                        sLongFileNameTemp = L"";
                        for (int i = nMaxLFNEntryForFile; i > 0; i--)
                        {
                           
                            sLongFileNameTemp = sLongFileName;

                            LFN stLFN = { 0 };
                            memcpy(RDETEntry, pByteRDET, 32);
                            index++;
                            memcpy(&stLFN, RDETEntry, 32);
                            wstring szNamePart1(reinterpret_cast<wchar_t*>(stLFN.fileName_part1), 10 / sizeof(wchar_t));


                            sLongFileName = szNamePart1;

                            bool flag = false;
                            for (int i = 0; i < 12; i++) {
                                if (stLFN.fileName_part2[i] != 0xff) {
                                    flag = true;
                                    break;
                                }
                            }
                            if (flag) {
                                wstring szNamePart2(reinterpret_cast<wchar_t*>(stLFN.fileName_part2), 12 / sizeof(wchar_t));

                                sLongFileName += szNamePart2;
                            }
                            flag = false;
                            for (int i = 0; i < 4; i++) {
                                if (stLFN.fileName_part3[i] != 0xff) {
                                    flag = true;
                                    break;
                                }
                            }
                            if (flag) {
                                wstring szNamePart3(reinterpret_cast<wchar_t*>(stLFN.fileName_part3), 4 / sizeof(wchar_t));

                                sLongFileName += szNamePart3;
                            }
                            sLongFileName += sLongFileNameTemp;
                            pByteRDET += 32;

                        }
                        root nSFN = { 0 };
                        memcpy(RDETEntry, pByteRDET, 32);
                        memcpy(&nSFN, RDETEntry, 32);
                        FileInfo file;

                        file.FileName = sLongFileName;
                        if (nSFN.fileAttrributes & 0x01) {
                            file.fileAttributes = "Read Only File";
                        }
                        if (nSFN.fileAttrributes & 0x02) {
                            file.fileAttributes = "Hidden File";
                        }
                        if (nSFN.fileAttrributes & 0x04) {
                            file.fileAttributes = "System File";
                        }
                        if (nSFN.fileAttrributes & 0x08) {
                            file.fileAttributes = "Volume Label";
                        }
                        if (nSFN.fileAttrributes & 0x10) {
                            file.fileAttributes = "Directory";
                        }
                        if (nSFN.fileAttrributes & 0x20) {
                            file.fileAttributes = "Archive";
                        }
                        if (nSFN.fileAttrributes & 0x40) {
                            file.fileAttributes = "Device";
                        }

                        file.modifiedDate = DateToString(nSFN.modifiedDate);
                        file.modifiedTime = TimeToString(nSFN.modifiedTime);
                        file.firstClusterAddress = nSFN.firstClusterAddress - 2;
                        file.sizeOfFile = nSFN.sizeOfFile;
                        container.push_back(file);

                        pByteRDET += 32;







                    }
                    else {
                        root nSFN = { 0 };
                        memcpy(&nSFN, RDETEntry, 32);
                        FileInfo file;
                        wstring s(nSFN.FileName, nSFN.FileName + 11);
                        file.FileName = s;
                        if (nSFN.fileAttrributes & 0x01) {
                            file.fileAttributes = "Read Only File";
                        }
                        if (nSFN.fileAttrributes & 0x02) {
                            file.fileAttributes = "Hidden File";
                        }
                        if (nSFN.fileAttrributes & 0x04) {
                            file.fileAttributes = "System File";
                        }
                        if (nSFN.fileAttrributes & 0x08) {
                            file.fileAttributes = "Volume Label";
                        }
                        if (nSFN.fileAttrributes & 0x10) {
                            file.fileAttributes = "Directory";
                        }
                        if (nSFN.fileAttrributes & 0x20) {
                            file.fileAttributes = "Archive";
                        }
                        file.modifiedDate = DateToString(nSFN.modifiedDate);
                        file.modifiedTime = TimeToString(nSFN.modifiedTime);
                        file.firstClusterAddress = nSFN.firstClusterAddress - 2;
                        file.sizeOfFile = nSFN.sizeOfFile;
                        container.push_back(file);

                        pByteRDET += 32;
                    }

                }




            }
            if (noEntry == true) {
                break;
            }
            IND++;
        }while (IND < _bpb.BPB_SecPerClus && is_empty_sector(sector) == false);
        
    }
    
    
    return container;
}

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
DWORD ReadNextEntryOfFAT32(DWORD cluster,WORD First_Sector_Fat) {
    WORD entryNum = cluster * 4;
    WORD numSec =  First_Sector_Fat + entryNum /512;
    entryNum = entryNum % 512;
    DWORD ANS;
    BYTE Check[4];
    BYTE sector[512];
    ReadSector(L"\\\\.\\E:", numSec * 512, sector);
    BYTE* pSector = sector;
    pSector += entryNum;
    memcpy(&ANS, pSector, 4);
    
    if (ANS == 0xffffff8 || ANS == 0xfffffff || ANS == 0xFFFF || ANS == 0xFFF8 || ANS == 0xFFF || ANS == 0xFF8) {
        return -1;
    }
    return ANS;
    
    
    
    
    

}
WORD convertClusterToSector(BPB _bpb, WORD cluster) {
    return _bpb.BPB_NumFATs * _bpb.BPB_FATSz32 + _bpb.BPB_RsvdSecCnt + cluster * _bpb.BPB_SecPerClus;
}
void PrintDataOfArchive(vector<DWORD> cluster, DWORD size,BPB _bpb) {
    DWORD current_sector = 0;
    DWORD index;
    BYTE sector[512];
    for (auto clus : cluster) {
        index = 0;
        current_sector = clus * _bpb.BPB_SecPerClus + _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs * _bpb.BPB_FATSz32;
        while (index < _bpb.BPB_SecPerClus && size >= _bpb.BPB_BytsPerSec) {
            memset(sector, 0, 512);
            BYTE tmp[513];
            ReadSector(L"\\\\.\\E:", (current_sector +index) * 512, sector);
            memcpy(tmp, sector, 512);
            tmp[512] = '\0';
            printf("%s", tmp);
            size -= _bpb.BPB_BytsPerSec;
            index++;
        }
        if (size > 0&&size< _bpb.BPB_BytsPerSec) {
            memset(sector, 0, 512);
            ReadSector(L"\\\\.\\E:", (current_sector + index) * 512, sector);
            BYTE* tmp = new BYTE[size+1];

            memcpy(tmp, sector, size);
            tmp[size] = '\0';
            printf("%s", tmp);
            delete tmp;
        }

    }
}
int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);
    
    BYTE sector[512];
    memset(sector, 0, 512);
   
    ReadSector(L"\\\\.\\E:", 0, sector);
    

    
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
                    DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt);
                    currentCluster = nextCluster;
                }
                vector<FileInfo> container = showRootDirectoryOrSubFolder(clusters_root,_bpb);

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
                                DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt);
                                currentCluster = nextCluster;
                            }
                            container = showRootDirectoryOrSubFolder(clusters_sub, _bpb);
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
                                    DWORD nextCluster = ReadNextEntryOfFAT32(currentCluster, _bpb.BPB_RsvdSecCnt);
                                    currentCluster = nextCluster;
                                }
                                wcout << L"\nData of Txt :" << f.FileName << endl;;
                                PrintDataOfArchive(clusters_file, f.sizeOfFile, _bpb);
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

