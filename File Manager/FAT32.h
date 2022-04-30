#ifndef FAT32_H
#define FAT32_H

#include "mystruct.h"
#include "sector.h"
#include <windows.h>
#include <vector>
void exploreFAT32(BPB _bpb, LPCWSTR drivepath, vector<FileInfo> container)
{
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
            
}

#endif // !FAT32_H

