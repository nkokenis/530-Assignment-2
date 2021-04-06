#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Function Description: 
 * The assemble function is responsible for reading the file and
 * storing its contents in a 2d array to be parsed later on
 */
void assemble(char* listingFilePath);
/**
 * Function Description: 
 * The buildSymTab function is responsible for reading the 2d array
 * from the assemble function and creating an ESTAB
 */
void buildSymTab(char** data, int* cols, int numLines);
/**
 * Function Description: 
 * The buildObjFile function is responsible for reading the 2d array
 * from the assemble function and creating the object file (records)
 */
void buildObjFile(char** data, int* cols, int numLines);

int main(int argc, char** argv)
{
    //Checks to see if the user entered the correct number of arguments
    if(argc < 2)
    {
        printf("Program needs at least one argument.\n");
        return 0;
    }

    //Begins assembling each listing file in the argument list
    for(int i = 1; i < argc; i++)
    {
        assemble(argv[i]);
    }

    return 0;
}

void assemble(char* listingFilePath)
{
    FILE* listingFile = fopen(listingFilePath,"r");
    if(listingFile == NULL)
    {
        printf("File not found.");
        return;
    }

    //Gets the number of lines in the file
    int numLines = 0;
    char ch;
    while(1)
    {
        ch = fgetc(listingFile);
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

    //Creates a dynamically allocated 2d array to store the line contents
    char** data = malloc(sizeof(char*) * numLines);

    //Creates a dynamically allocated 1d array to store the size of each row in our 2d array
    int* cols = malloc(sizeof(int) * numLines);

    //Dynamically allocates memory for our 2d array
    int lineNum = 0;
    int index = 0;
    while(1)
    {
        ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            data[lineNum] = (char*)malloc(sizeof(char) * (index));
            cols[lineNum] = index;
            break;
        }
        else if(ch == '\n')
        {
            data[lineNum] = (char*)malloc(sizeof(char) * (index));
            cols[lineNum] = index;
            index = 0;
            lineNum++;
        }
        else
        {
            index++;
        }
    }
    rewind(listingFile);

    //Fills our 2d array with the file contents
    lineNum = 0;
    index = 0;
    while(1)
    {
        ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            break;
        }
        else if(ch == '\n')
        {
            printf("\n");
            index = 0;
            lineNum++;
        }
        else
        {
            printf("%c",ch);
            data[lineNum][index] = ch;
            index++;
        }
    }

    printf("\n-----------------------------\n            ESTAB\n-----------------------------\n");
    buildSymTab(data, cols, numLines);
    printf("\n-----------------------------\n            OBJFL\n-----------------------------\n");
    buildObjFile(data, cols, numLines);

    free(data);
    free(cols);
}

void buildSymTab(char** data, int* cols, int numLines)
{
    //Keeps track of the line number and index positions within the line
    int lineNum = 0;
    int index = 0;

    //Keeps track of the position of the EXTDEF directive in the file
    int pos = 0;

    //Keeps track of which line EXTDEF is found in our 2d array
    int dataLine = 0;

    //Gets the number of externally defined symbols
    int extDefNumLines = 0;
    char ch;
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"EXTDEF") != NULL)
        {
            dataLine = lineNum;
            char* src = data[dataLine];
            char* dst = strstr(src, "EXTDEF");
            pos = dst - src;
            pos += 6;
            for(index = pos; index < cols[dataLine]; index++)
            {
                ch = data[dataLine][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == cols[dataLine] - 1))
                    {
                        extDefNumLines++;
                    }
                }
            }
        }
    }

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char** extDefSymbols = malloc(sizeof(char*) * extDefNumLines);

    //Dynamically allocates space for our 2d array
    int extDefLineNum = 0;
    int extDefIndex = 0;
    for(index = pos; index < cols[dataLine]; index++)
    {
        ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(index == cols[dataLine] - 1)
            {
                extDefIndex++;
                extDefSymbols[extDefLineNum] = (char*)malloc(sizeof(char) * (extDefIndex));
            }
            else if(ch == ',')
            {
                extDefSymbols[extDefLineNum] = (char*)malloc(sizeof(char) * (extDefIndex));
                extDefIndex = 0;
                extDefLineNum++;
            }
            else
            {
                extDefIndex++;
            }
        }
    }

    //Fills our 2d array with all the externally defined symbols
    extDefIndex = 0;
    extDefLineNum = 0;
    for(index = pos; index <= cols[dataLine]; index++)
    {
        ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(ch == ',')
            {
                extDefIndex = 0;
                extDefLineNum++;
            }
            else
            {
                extDefSymbols[extDefLineNum][extDefIndex] = ch;
                extDefIndex++;
            }
        }
    }

    //Creates an array to output the contents of the ESTAB
    char* outputArr = malloc(sizeof(char) * 64);
    int outputIndex = 0;

    //Print the control section of the ESTAB
    for(index = 8; data[0][index] != ' '; index++)
    {
        outputArr[outputIndex++] = data[0][index];
    }
    printf("%-10s",outputArr);
    memset(outputArr, 0, sizeof outputArr);
    outputIndex = 0;

    //Find all external symbols within the file and prints them to the ESTAB
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"WORD") != NULL || strstr(data[lineNum],"BYTE") != NULL ||
           strstr(data[lineNum],"RESW") != NULL || strstr(data[lineNum],"RESB") != NULL ||
           strstr(data[lineNum],"EQU") != NULL)
        {
            for(extDefLineNum = 0; extDefLineNum < extDefNumLines; extDefLineNum++)
            {
                if(strstr(data[lineNum],extDefSymbols[extDefLineNum]) != NULL)
                {
                    printf("\n%*s",8,"");
                    printf("%-8s",extDefSymbols[extDefLineNum]);
                    printf("%*s",8,"");
                    //Gets the address of the symbol
                    for(index = 0; index < 4; index++)
                    {
                        char ch = data[lineNum][index];
                        printf("%c",ch);
                    }
                }
            }
        }
    }
}

void buildObjFile(char** data, int* cols, int numLines)
{
    int index;
    //Header
    printf("H");
    //Creates an array to output the contents of the ESTAB
    char* outputArr = malloc(sizeof(char) * 64);
    int outputIndex = 0;

    //Print the control section of the ESTAB
    for(index = 8; data[0][index] != ' '; index++)
    {
        outputArr[outputIndex++] = data[0][index];
    }
    printf("%s",outputArr);
    memset(outputArr, 0, sizeof outputArr);
    outputIndex = 0;

    printf("00");
    for(index = 0; index < 4; index++)
    {
        printf("%c",data[0][index]);
    }

    char* arr1 = malloc(sizeof(char) * 4);
    char* arr2 = malloc(sizeof(char) * 4);

    for(index = 0; index < 4; index++)
    {
        arr1[index] = data[0][index];
        arr2[index] = data[numLines - 1][index];
    }

    int start = (int)strtol(arr1, NULL, 16);
    int end = (int)strtol(arr2, NULL, 16);

    int length = end - start;
    printf("%04X", length);
    //Text

    //Mod

    //End
    printf("\n");
    printf("E");

    char* endArr = malloc(sizeof(char) * 16);
    int endArrIndex = 0;

    for(int i = 0; i < numLines; i++)
    {
        if(strstr(data[i],"END") != NULL)
        {
            char* src = data[i];
            char* dst = strstr(src, "END");
            int pos = dst - src;
            pos += 3;
            for(int j = pos; j < cols[i]; j++)
            {
                char ch = data[i][j];
                if(ch != ' ')
                {
                    endArr[endArrIndex++] = ch;
                }
            }
        }
    }
    //Account for if the label is used before it is defined
    if(sizeof endArr == 0)
    {
        printf("%s",arr1);
    }
    else
    {
        for(int i = 0; i < numLines; i++)
        {
            if(strstr(data[i],endArr) != NULL)
            {
                for(int j = 0; j < 4; j++)
                {
                    printf("%c",data[i][j]);
                }
                i = numLines;
            }
        }
    }
}