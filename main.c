#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Function declarations
void assemble(char* listingFilePath);
void parseInstruction(char* instruction);

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Program needs at least one argument.\n");
        return 0;
    }

    //Parses through each file
    for(int i = 1; i < argc; i++)
    {
        assemble(argv[i]);
    }

    return 0;
}

void assemble(char* listingFilePath)
{
    //Creates a FILE pointer using the file path provided
    FILE* listingFile = fopen(listingFilePath,"r");
    if(listingFile == NULL)
    {
        printf("File not found.");
        return;
    }

    //Keeps track of the number of lines in the file
    int numLines = 0;

    //Gets the number of lines in the file
    while(1)
    {
        char ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            numLines++;
            break;
        }
        if(ch == '\n')
        {
            numLines++;
        }
    }
    rewind(listingFile);

    //Keeps track of the index as we iterate thru the line
    int index = 0;

    //Keeps track of the line number we are currently on
    int lineNum = 0;

    //Creates a dynamically allocated 2d array to store the line contents
    char** data = malloc(sizeof(char*) * numLines);

    //Creates a 1d array to store the size of each row in our 2d array
    int* rowSize = malloc(sizeof(int) * numLines);

    //Dynamically allocates our 2d array by creating a "jagged" array
    while(1)
    {
        char ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            data[lineNum] = (char*)malloc(sizeof(char) * (index + 1));
            rowSize[lineNum] = index + 1;
            index = 0;
            lineNum++;
            break;
        }
        if(ch == '\n')
        {
            data[lineNum] = (char*)malloc(sizeof(char) * (index + 1));
            rowSize[lineNum] = index + 1;
            index = 0;
            lineNum++;
        }
        else
        {
            index++;
        }
    }
    rewind(listingFile);

    //Fills the array
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        for(index = 0; index < rowSize[lineNum]; index++)
        {
            char ch = fgetc(listingFile);
            if(feof(listingFile))
            {
                break;
            }
            if(ch == '\n')
            {
                printf("\n");
            }
            else
            {
                data[lineNum][index] = ch;
                printf("%c", data[lineNum][index]);
            }
        }
    }

    //Keeps track of the number of externally defined symbols
    int extDefNumLines = 0;

    //Keeps track of the position of EXTDEF
    int pos = 0;

    //Keeps track of which line in data[][] EXTDEF is found
    int dataLine = 0;

    //Gets the number of externally defined symbols
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"EXTDEF") != NULL)
        {
            dataLine = lineNum;
            char* src = data[lineNum];
            char* dst = strstr(src, "EXTDEF");
            pos = dst - src;
            pos += 6;
            for(index = pos; index < rowSize[lineNum]; index++)
            {
                char ch = data[lineNum][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == rowSize[lineNum] - 1))
                    {
                        extDefNumLines++;
                    }
                }
            }
        }
    }

    //Keeps track of the index as we iterate thru the externally defined symbols
    int extDefIndex = 0;

    //Keeps track of the symbol we're currently on
    int extDefLineNum = 0;

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char** extDefSymbols = malloc(sizeof(char*) * extDefNumLines);

    //Creates a 1d array to store the size of each row in our 2d array
    int* extDefRowSize = malloc(sizeof(int) * extDefNumLines);

    //Finds externally defined symbol names and allocates space in another 2d jagged array
    for(index = pos; index < rowSize[dataLine]; index++)
    {
        char ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(ch == ',' || (index == rowSize[dataLine] - 1))
            {
                extDefSymbols[extDefLineNum] = (char*)malloc(sizeof(char) * (extDefIndex + 1));
                extDefRowSize[extDefLineNum] = extDefIndex + 1;
                extDefIndex = 0;
                extDefLineNum++;
            }
            else
            {
                extDefIndex++;
            }
        }
    }

    extDefIndex = 0;
    extDefLineNum = 0;

    //Fills the array
    printf("\nExternally defined symbols are: ");
    for(index = pos; index < rowSize[dataLine]; index++)
    {
        char ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(ch == ',' || (index == rowSize[dataLine] - 1))
            {
                extDefIndex = 0;
                extDefLineNum++;
                printf(", ");
            }
            else
            {
                extDefSymbols[extDefLineNum][extDefIndex] = ch;
                printf("%c",extDefSymbols[extDefLineNum][extDefIndex]);
                extDefIndex++;
            }
        }
    }

    //Find all symbols within the file
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"WORD") != NULL || strstr(data[lineNum],"BYTE") != NULL ||
           strstr(data[lineNum],"RESW") != NULL || strstr(data[lineNum],"RESB") != NULL ||
           strstr(data[lineNum],"EQU") != NULL)
        {
            printf("\nRelative address of symbol ");
            //Gets the label
            for(index = 8; index < rowSize[lineNum]; index++)
            {
                char ch = data[lineNum][index];
                if(ch == ' ')
                {
                    break;
                }
                else
                {
                    printf("%c", ch);
                }
            }
            printf(" is ");
            //Gets the address
            for(index = 0; index < 4; index++)
            {
                char ch = data[lineNum][index];
                printf("%c", ch);
            }
        }
    }
}

void parseInstruction(char* instruction)
{
    printf("%s\n", instruction);
}