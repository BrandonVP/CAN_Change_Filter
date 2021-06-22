/*
 Name:		CAN_Change_Filter.ino
 Created:	6/18/2021 1:15:46 PM
 Author:	Brandon Van Pelt
*/

// Disable Visual Studio's deprecated code warning
#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <iostream>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h> 
#include <string>

/*=========================================================
    START DEBUGGING
===========================================================*/
//#define DEBUG_FINDIDS // Useful if changing file read format
#define DEVMODE
/*=========================================================
    END DEBUGGING
===========================================================*/

#define MAX_STRING_LENGTH 100

using namespace std;

typedef struct
{
    int messageNum;
    float time;
    int id;
    int length;
    int data[8];
} CANMSG;

CANMSG buffer;
list<int> IDBuffer;
list<int> filteredIDBuffer;

int findChange(char*, int, int);

// Returns true if given id is already in a list
bool isIDInList(int id)
{
    return find(IDBuffer.begin(), IDBuffer.end(), id) != IDBuffer.end();
}

// Find all unique IDs and add them to list
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

// Find the length of the text file
int fileLength(char* filename) {
    int line_count = 0;
    FILE* fp;
    char str[MAX_STRING_LENGTH];
    fopen_s(&fp, filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return -1;
    }
    // Counts lines until a NULL is found
    while (fgets(str, MAX_STRING_LENGTH, fp) != NULL) {
        line_count++;
    }
    return line_count;
}

// Call with false if you only want the count of unique IDs
int printIDList(bool printMe = true)
{
    int count = 0;
    IDBuffer.sort();

    for (int x : IDBuffer)
    {
        if (printMe)
        {
            printf("%03x\n", x);
        }
        count++;
    }
    return count;
}

// 
void XORData(char* filename, int id)
{
    int OR_Data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int XOR_Data1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // n
    int XOR_Data2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // n + 1
    int count = 0;
    bool firstTime = true;
    bool pushID = false;
    bool printResult = false;

    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return;
    }

    while (fscanf(ptr, "%*s %*s %x %d", &buffer.id, &buffer.length) != EOF)
    {
        for (int i = 0; i < 8; i++)
        {
            (i < buffer.length) ? (fscanf(ptr, "%x", &buffer.data[i])) : (buffer.data[i] = 0);
        }
        if (buffer.id == id)
        {
            count++;
            if (firstTime)
            {
                for (int i = 0; i < 8; i++)
                {
                    XOR_Data2[i] = buffer.data[i];
                }
                firstTime = false;
            }
            else
            {
                for (int i = 0; i < 8; i++)
                {
                    XOR_Data1[i] = XOR_Data2[i];
                }
                for (int i = 0; i < 8; i++)
                {
                    XOR_Data2[i] = buffer.data[i];
                }
                for (int i = 0; i < 8; i++)
                {
                    OR_Data[i] = OR_Data[i] | (XOR_Data1[i] ^ XOR_Data2[i]);
                }
            }
        }
    }

    // Determine which bytes in the array have a change of 1 bit then find what time that change happened.
    for (int i = 0; i < 8; i++)
    {
        if (OR_Data[i] == 0x1 || OR_Data[i] == 0x2 || OR_Data[i] == 0x4 || OR_Data[i] == 0x8 || OR_Data[i] == 0x10 || OR_Data[i] == 0x20 || OR_Data[i] == 0x40 || OR_Data[i] == 0x80)
        {
            printResult = true;
            pushID = true;
            printf("\n");
            printf("Found change at data[%d]\n", i);
            findChange(filename, id, i);
        }
    }
    if (pushID)
    {
        filteredIDBuffer.push_back(id);
    }
    if (printResult)
    {
        printf("ID: %04x Data: %02x %02x %02x %02x %02x %02x %02x %02x MSGS: %04d\n\n", id, OR_Data[0], OR_Data[1], OR_Data[2], OR_Data[3], OR_Data[4], OR_Data[5], OR_Data[6], OR_Data[7], count);
    }
}

// Start the XOR(^) process
void startXOR(char* filename)
{
    for (int id : IDBuffer)
    {
        XORData(filename, id);
    }
}

//
void printFilteredIDBuffer()
{
    int count = 0;
    printf("IDs with 1 bit difference:\n");
    for (int x : filteredIDBuffer)
    {
        printf("ID: %03x\n", x);
        count++;
    }
    printf("Count: %d\n", count);
}

//Find the times for an alternating bit
int findChange(char* filename, int id, int arrayPos)
{
    int count = 0;
    bool firstTime = true;
    int temp = 0;

    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }

    while (fscanf(ptr, "%*s %f %x %d", &buffer.time, &buffer.id, &buffer.length) != EOF)
    {
        for (int i = 0; i < 8; i++)
        {
            (i < buffer.length) ? (fscanf(ptr, "%x", &buffer.data[i])) : (buffer.data[i] = 0);
        }

        if (buffer.id == id && firstTime)
        {
            temp = buffer.data[arrayPos];
            firstTime = false;
            printf("First value assigned: %02x ", buffer.data[arrayPos]);
            printf("Time: %f\n", buffer.time);
        }
        else if (buffer.id == id && temp != buffer.data[arrayPos])
        {
            temp = buffer.data[arrayPos];
            printf("Value changed to: %02x ", buffer.data[arrayPos]);
            printf("Time: %f\n", buffer.time);
            count++;
        }
    }
    printf("Count: %d\n", count);
}

// Call with filename to filter
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

    printFilteredIDBuffer();
}
