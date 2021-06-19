#define _CRT_SECURE_NO_WARNINGS
// CAN_Change_Filter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h> 
#include <string>

/*=========================================================
    END DEBUGGING
===========================================================*/
//#define DEBUG_FINDIDS
#define DEVMODE
#define DEBUG_XOR
/*=========================================================
    END DEBUGGING
===========================================================*/

#define MAXLINE 200 
#define MAX_STRING_LENGTH 100

using namespace std;

list<int> IDBuffer;

typedef struct
{
    int messageNum;
    float time;
    int id;
    int length;
    int data[8];
} CANMSG;

CANMSG buffer;

bool isIDInList(int id)
{
    return std::find(IDBuffer.begin(), IDBuffer.end(), id) != IDBuffer.end();
}

int findIDs(char* filename)
{
    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }

    // Msg Num, Time, ID, Length
    while (fscanf(ptr, "%*s %*s %x %d", &buffer.id, &buffer.length) != EOF)
    {
        #if defined DEBUG_FINDIDS
            printf("%x\n", buffer.id);
        #endif
        
        // Determine length and loop the the data array, an array shorter than 8 is padded with zeros
        for (int i = 0; i < 8; i++)
        {
            (i < buffer.length) ? (fscanf(ptr, "%x", &buffer.data[i])) : (buffer.data[i] = 0);

        #if defined DEBUG_FINDIDS
            printf("%x ", buffer.data[i]);
        #endif

        }

        #if defined DEBUG_FINDIDS
            printf("\n");
        #endif
        
        // Add id to list if not already
        if (!isIDInList(buffer.id))
        {
            IDBuffer.push_back(buffer.id);
        }
    }
}

// Find the length of a file
int fileLength(char* filename) {
    int line_count = 0;
    FILE* fp;
    char str[MAX_STRING_LENGTH];
    fopen_s(&fp, filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }
    // Counts lines until a NULL is found
    while (fgets(str, MAX_STRING_LENGTH, fp) != NULL) {
        line_count++;
    }
    return line_count;
}

// Call with false if you just want count
int printIDList(bool printMe = true)
{
    int count = 0;
    IDBuffer.sort();

    for (int x : IDBuffer)
    {
        if (printMe)
        {
            printf("%x\n", x);
        }
        count++;
    }
    return count;
}


int XORData(char* filename, int id)
{
    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }

    while (fscanf(ptr, "%*s %*s %x %d", &buffer.id, &buffer.length) != EOF)
    {
#if defined DEBUG_XOR
        if (buffer.id == 83 && id == 83)
        {
            printf("ID: %x\n", buffer.id);
            printf("Length: %x\n", buffer.length);
        }
#endif
        for (int i = 0; i < 8; i++)
        {
            (i < buffer.length) ? (fscanf(ptr, "%x", &buffer.data[i])) : (buffer.data[i] = 0);
#if defined DEBUG_XOR
            //printf("%x ", buffer.data[i]);
#endif
        }
#if defined DEBUG_XOR
        //printf("\n");
#endif
    }
}

void startXOR(char* filename)
{
    for (int id : IDBuffer)
    {
        printf("XOR ID: %x\n ", id);
        XORData(filename, id);
    }
}

// XOR: ^
int main(int argc, char* argv[])
{
#if defined DEVMODE
    char filename[] = "AutoStopStart01.txt";
#else 
    char filename[] = argv[1];
#endif
    
    printf("File length: %d\n", fileLength(filename));

    findIDs(filename);

    printf("Unique IDs: %d\n", printIDList(false));
    
    startXOR(filename);

}
