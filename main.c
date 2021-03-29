#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Global variables
int gNumLines;

//Function declarations
char** assemble(char* listingFilePath);
void buildSymTab(char** data);
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
        char** data = assemble(argv[i]);
        buildSymTab(data);
        free(data);
    }

    return 0;
}

char** assemble(char* listingFilePath)
{
    //Creates a FILE pointer using the file path provided
    FILE* listingFile = fopen(listingFilePath,"r");
    if(listingFile == NULL)
    {
        printf("File not found.");
        return NULL;
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
    gNumLines = numLines;
    rewind(listingFile);

    //Keeps track of the index as we iterate thru the line
    int index = 0;

    //Keeps track of the line number we are currently on
    int lineNum = 0;

    //Creates a dynamically allocated 2d array to store the line contents
    char** data = malloc(sizeof(char*) * numLines);

    //Creates a 1d array to store the size of each row in our 2d array
    int rowSize[numLines];

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
                printf("%c ", data[lineNum][index]);
            }
        }
    }
    return data;
}

void buildSymTab(char** data)
{
    //char symbol[16] = {};
    //int symbolIndex = 0;
    for(int i = 0; i < gNumLines; i++)
    {
        if(strstr(data[i],"EXTDEF") != NULL)
        {
            printf("\nFOUND EXTDEF\n");
            /*
            char* src = data[i];
            char* dst = strstr(data[i],"EXTDEF");
            int pos = dst - src;
            pos += 6;
            while(pos < sizeof(data[i]))
            {
                char ch = data[i][pos];
                if(ch != ' ')
                {
                    if(ch == ',')
                    {
                        //Parse symbol name
                        symbolIndex = 0;
                        parseInstruction(symbol);
                        memset(symbol, 0, sizeof(symbol));
                    }
                    else
                    {
                        //Fill the array character by character
                        symbol[symbolIndex] = ch;
                        symbolIndex++;
                    }
                }
                pos++;
            }
            */
        }
    }
}

void parseInstruction(char* instruction)
{
    printf("%s\n", instruction);
}