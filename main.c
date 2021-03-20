#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Global variables
int numLines = 0;

//Function declarations
struct LineData* assemble(char* listingFilePath);
void parseInstruction(char* instruction);

struct LineData
{
    char address[4];
    char label[16];
    char instruction[16];
    char operand[64];
    char objcode[8];
    int lineNumber;
};


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        //There is less than 1 argument
        printf("Program needs at least one argument.\n");
        return 0;
    }

    for(int i = 1; i < argc; i++)
    {
        struct LineData* data = assemble(argv[i]);
        //Call other functions after assembling
        free(data);
    }

    return 0;
}

struct LineData* assemble(char* listingFilePath)
{
    //Create a FILE pointer using the file path provided
    FILE* listingFile = fopen(listingFilePath,"r");
    if(listingFile == NULL)
    {
        //The file does not exist or couldn't be found
        printf("File not found.");
        return NULL;
    }

    int index = 0;
    int len = 0;
    int lineNum = 0;

    while(1)
    {
        char ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            break;
        }
        else
        {
            numLines++;
        }
    }
    struct LineData* data = malloc(sizeof(struct LineData) * numLines);
    rewind(listingFile);

    //Iterate through the file, character by character
    while(1)
    {
        //Get the next character in the listing file
        char ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            printf("%d",index);
            break;
        }

        if(ch == ' ')
        {
            printf("%d",index);
            len = 0;
            index++;
            while(fgetc(listingFile) == ' ');
        }
        else if(ch == '\n')
        {
            printf("%d\n",index);
            data[lineNum].lineNumber = lineNum;
            lineNum++;
            len = 0;
            index = 0;
        }
        else
        {
            if(index == 0)
            {
                data[lineNum].address[len] = ch;
                len++;
            }
            else if(index == 1)
            {
                data[lineNum].label[len] = ch;
                len++;
            }
            else if(index == 2)
            {
                data[lineNum].instruction[len] = ch;
                len++;
            }
            else if(index == 3)
            {
                data[lineNum].operand[len] = ch;
                len++;
            }
            else if(index == 4)
            {
                data[lineNum].objcode[len] = ch;
                len++;
            }
        }
    }
    return data;
}

void parseInstruction(char* instruction)
{
    printf("%s\n", instruction);
}