#ifndef SECTOR_H
#define SECTOR_H
#include "mystruct.h"
#include <windows.h>
#include <vector>

bool is_empty_sector(BYTE sector[512]);
int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512]);
void showBootSectorInformation(BPB _bpb);
vector<FileInfo> showRootDirectoryOrSubFolder(vector<DWORD> clusters, BPB _bpb, LPCWSTR  drive);
DWORD ReadNextEntryOfFAT32(DWORD cluster, WORD First_Sector_Fat, LPCWSTR  drive);
WORD convertClusterToSector(BPB _bpb, WORD cluster);
void PrintDataOfArchive(vector<DWORD> cluster, DWORD size, BPB _bpb, LPCWSTR  drive);

#endif 

