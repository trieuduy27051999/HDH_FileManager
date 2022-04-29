#include "sector.h"

string DateToString(WORD Date) {
    WORD nYear = (Date >> 9) + 1980;
    WORD nMonth = (Date << 7);
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
        ret += ('0' + to_string(nSecond));
    }
    else {
        ret += to_string(nSecond);
    }

    return ret;

}

bool is_empty_sector(BYTE sector[512])
{
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

void showBootSectorInformation(BPB _bpb)
{
    printf("FAT32 USB disk information \n");
    printf("OEM Name : %s\n", _bpb.BS_OEMName);
    printf("Bytes per sector : %d \n", _bpb.BPB_BytsPerSec);
    printf("Sector per cluster : %d \n", _bpb.BPB_SecPerClus);
    printf("Number of sector for BootSector: %d \n", _bpb.BPB_RsvdSecCnt);
    printf("Number of sector in Volumn : %d \n", _bpb.BPB_TotSec32);
    printf("Number of Fats : %d \n", _bpb.BPB_NumFATs);
    printf("Number of sector in FATS : %d\n", _bpb.BPB_FATSz32);
    printf("First sector of FAT: %d \n", _bpb.BPB_RsvdSecCnt);
    printf("First sector of RDET: %d \n", _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs * _bpb.BPB_FATSz32);
    printf("First sector of DATA: %d \n", _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs * _bpb.BPB_FATSz32);
}

vector<FileInfo> showRootDirectoryOrSubFolder(vector<DWORD> clusters, BPB _bpb, LPCWSTR  drive)
{
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
            ReadSector(drive, (IND + numSector) * 512, sector);
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
        } while (IND < _bpb.BPB_SecPerClus && is_empty_sector(sector) == false);

    }


    return container;
}

DWORD ReadNextEntryOfFAT32(DWORD cluster, WORD First_Sector_Fat, LPCWSTR  drive)
{
    WORD entryNum = cluster * 4;
    WORD numSec = First_Sector_Fat + entryNum / 512;
    entryNum = entryNum % 512;
    DWORD ANS;
    BYTE Check[4];
    BYTE sector[512];
    ReadSector(drive, numSec * 512, sector);
    BYTE* pSector = sector;
    pSector += entryNum;
    memcpy(&ANS, pSector, 4);

    if (ANS == 0xffffff8 || ANS == 0xfffffff || ANS == 0xFFFF || ANS == 0xFFF8 || ANS == 0xFFF || ANS == 0xFF8) {
        return -1;
    }
    return ANS;
}

WORD convertClusterToSector(BPB _bpb, WORD cluster) 
{
    return _bpb.BPB_NumFATs * _bpb.BPB_FATSz32 + _bpb.BPB_RsvdSecCnt + cluster * _bpb.BPB_SecPerClus;
}

void PrintDataOfArchive(vector<DWORD> cluster, DWORD size, BPB _bpb, LPCWSTR  drive)
{
    DWORD current_sector = 0;
    DWORD index = 0;
    BYTE sector[512];
    for (auto clus : cluster) {
        current_sector = clus * _bpb.BPB_SecPerClus + _bpb.BPB_RsvdSecCnt + _bpb.BPB_NumFATs * _bpb.BPB_FATSz32;
        while (index < _bpb.BPB_SecPerClus && size >= _bpb.BPB_BytsPerSec) {
            memset(sector, 0, 512);
            ReadSector(drive, (current_sector + index) * 512, sector);
            BYTE tmp[513];
            memcpy(tmp, sector, 512);
            tmp[512] = '\0';
            printf("%s", sector);
            size -= _bpb.BPB_BytsPerSec;
        }
        if (size > 0) {
            memset(sector, 0, 512);
            ReadSector(drive, (current_sector + index) * 512, sector);
            BYTE* tmp = new BYTE[size+1];
            memcpy(tmp, sector, size);
            tmp[size] = '\0';
            printf("%s", tmp);
        }

    }
}