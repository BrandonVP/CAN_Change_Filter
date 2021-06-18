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

#define MAXLINE 250 //every line with end with \n 
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
}CANMSG;

int inputLine(FILE* fp, CANMSG e)
{
    return 1;
}

void parseFile(FILE* fptr, char* delimiter) 
{
    char buffer[MAXLINE] = { 0 };
    char* linestatus = NULL;
    char* token = NULL;
    int count = 0;
    while ((linestatus = fgets(buffer, MAXLINE, fptr)) != NULL) {
        int len = strlen(buffer);
        buffer[len - 2] = '\0'; //removing \n from string   
        token = strtok(buffer, " ");
        count = 0;
        while (token != NULL) 
        {
            printf("%s  ", token); //or store  
            token = strtok(NULL, delimiter);
            count++;
        }
        printf("\n");
    }
}

int findIDs(char* filename)
{
    CANMSG buff[100];
    FILE* fp; // declaration of file pointer
    char buffer[1000]; // variable to read the content
    fopen_s(&fp, filename, "r");// opening of file
    if (!fp)// checking for error
        return 1;

    char s[2] = " ";
    //parseFile(fp, s);
    
    inputLine(fp, buff[0]);
    printf("%h", buff[0].id);

    while (fgets(buffer, 1000, fp) != NULL)
    {
      
    }
    //    printf("%s", buffer);
    

    fclose(fp); // closing file
    return 0;
}

bool isIDInList(int id)
{
    // Check if an element exists in list
    // Create a list Iterator
    list<int>::iterator it;
    // Fetch the iterator of element with value 'the'
    it = find(IDBuffer.begin(), IDBuffer.end(), id);
    // Check if iterator points to end or not
    if (it != IDBuffer.end())
    {
        // It does not point to end, it means element exists in list
        cout << id + " exists in list " << endl;
        return true;
    }
    else
    {
        // It points to end, it means element does not exists in list
        cout << id +" does not exists in list" << endl;
        return false;
    }
}

int fileLength(char* filename) {
    int line_count = 0;
    FILE* fp;
    char str[MAX_STRING_LENGTH];
    //	char *filename = nodeinfo_location;
    fopen_s(&fp, filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        return 1;
    }
    // Counts lines until a NULL is found
    while (fgets(str, MAX_STRING_LENGTH, fp) != NULL) {
        line_count++;
    }
    // Value returned is used to determine structure size
    return line_count;
}

int main(int argc, char* argv[])
{
    CANMSG buffer;
    char filename[] = "AutoStopStart01.txt";

    printf("File length: %d\n", fileLength(filename));
   
    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }

    while (fscanf(ptr, "%*s %*s %x %d", &buffer.id, &buffer.length) != EOF)
    {
        //printf("%x\n", buffer.id);
        //printf("%d\n", buffer.length);

        for (int i = 0; i < 8; i++)
        {
            (i < buffer.length) ? (fscanf(ptr, "%x", &buffer.data[i])) : (buffer.data[i] = 0);
            //printf("%x ", buffer.data[i]);
        }
        //printf("\n");
        if (!isIDInList(buffer.id))
        {
            //IDBuffer.push_back(buffer.id);
        }
    }
    IDBuffer.sort();

    for (int x : IDBuffer)
    {
        printf("%x\n", x);
    }
}
