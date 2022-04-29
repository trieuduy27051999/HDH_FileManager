#ifndef MYSTRUCT_H
#define MYSTRUCT_H

#include<iostream>
#include<string>
#include <stdio.h>
using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef wchar_t WBYTE;

#pragma pack(1)
typedef struct BIOS_Paramenter_Block{
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

typedef struct RootEntry 
{
    BYTE FileName[11];
    BYTE fileAttrributes;
    BYTE unused[10];
    WORD modifiedTime;
    WORD modifiedDate;
    WORD firstClusterAddress;
    DWORD sizeOfFile;
} root;

typedef struct LongFileName 
{
    BYTE entryNo;
    BYTE fileName_part1[10];
    BYTE fileAttributes;
    BYTE reversed;
    BYTE checkSum;
    BYTE fileName_part2[12];
    WORD firstClusterAddress;
    BYTE fileName_part3[4];
} LFN;

typedef struct FileInfo 
{
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

#endif

#pragma pack()