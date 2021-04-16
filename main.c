#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Function Name:
 * Assemble
 * 
 * Function Description: 
 * The assemble function is responsible for reading the file and
 * storing its contents in a 2d array to be parsed later on
 */
void assemble(char* listingFilePath);
/**
 * Function Name:
 * Build Symbol Table
 * 
 * Function Description: 
 * The buildSymTab function is responsible for reading the 2d array
 * from the assemble function and creating an ESTAB
 */
void buildSymTab(char** data, int* cols, int numLines);
/**
 * Function Name:
 * Build Object File
 * 
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

    //Begins assembling each listing file provided as input to the program
    for(int i = 1; i < argc; i++)
    {
        assemble(argv[i]);
    }

    return 0;
}

void assemble(char* listingFilePath)
{
    FILE* listingFile = fopen(listingFilePath,"r");
    //Checks to make sure the file exists within the current directory
    if(listingFile == NULL)
    {
        printf("File not found.");
        return;
    }

    //Gets the number of lines in the file
    int numLines = 0;
    char ch;
    int pos = 0;
    while(1)
    {
        ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            break;
        }

        if(ch == '\n')
        {
            pos = 0;
        }
        else if(pos == 0 && ch == '.')
        {
            pos = 1;
        }
        else if(pos == 0 && ch != '.')
        {
            pos = 1;
            numLines++;
        }
        else
        {
            //Default case
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
    pos = 0;
    while(1)
    {
        ch = fgetc(listingFile);
        if(ch == '.')
        {
            if(index != 0)
            {
                data[lineNum] = (char*)malloc(sizeof(char) * (index));
                cols[lineNum] = index;
                index = 0;
                lineNum++;
            }
            while(ch != '\n')
            {
                ch = fgetc(listingFile);
            }
            continue;
        }

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
        if(ch == '.')
        {
            if(index != 0)
            {
                printf("\n");
                index = 0;
                lineNum++;
            }
            while(ch != '\n')
            {
                ch = fgetc(listingFile);
            }
            continue;
        }

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

    //Calls our "Build Symbol Table" and "Build Object File" functions
    printf("\n-----------------------------\n            ESTAB\n-----------------------------\n");
    buildSymTab(data, cols, numLines);
    printf("\n-----------------------------\n            OBJFL\n-----------------------------\n");
    buildObjFile(data, cols, numLines);

    //Frees the allocated memory for our arrays
    free(data);
    free(cols);
}

void buildSymTab(char** data, int* cols, int numLines)
{
    //Keeps track of the line number and index position within the line
    int lineNum = 0;
    int index = 0;

    //Keeps track of the position of the EXTDEF directive in the file
    int pos = 0;

    //Keeps track of which line EXTDEF is in
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

    //Prints the control section for the ESTAB
    char* outputArr = malloc(sizeof(char) * 32);
    int outputIndex = 0;
    for(index = 8; data[0][index] != ' '; index++)
    {
        outputArr[outputIndex++] = data[0][index];
    }
    printf("%-8s",outputArr);
    free(outputArr);

    //Prints the address of the control section for the ESTAB
    char outputAddress[4] = {};
    outputIndex = 0;
    for(index = 0; index < 4; index++)
    {
        outputAddress[outputIndex++] = data[0][index];
    }
    printf("%*s",16,"");
    printf("%-12s",outputAddress);

    //Prints the length of the control section for the ESTAB
    char* startAddr = malloc(sizeof(char) * 4);
    char* endAddr = malloc(sizeof(char) * 4);
    for(index = 0; index < 4; index++)
    {
        startAddr[index] = data[0][index];
        endAddr[index] = data[numLines - 1][index];
    }
    int start = (int)strtol(startAddr, NULL, 16);
    int end = (int)strtol(endAddr, NULL, 16);
    int length = end - start;
    length = length + 3;
    printf("%04X", length);
    free(startAddr);
    free(endAddr);

    //Finds all external symbols within the file and prints them to the ESTAB
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
                    //Gets the name of the symbol
                    printf("\n%*s",8,"");
                    printf("%-8s",extDefSymbols[extDefLineNum]);
                    printf("%*s",8,"");

                    //Gets the address of the symbol
                    for(index = 0; index < 4; index++)
                    {
                        char ch = data[lineNum][index];
                        printf("%c",ch);
                    }

                    //Remove the symbol from the list since it's been found
                    extDefSymbols[extDefLineNum] = "\n";
                }
            }
        }
    }

    //Frees the memory allocated for extDefSymbols
    free(extDefSymbols);
}

void buildObjFile(char** data, int* cols, int numLines)
{
    /**
     * Header Record
     */
    printf("H");

    //Prints the name of the program
    char* outputArr = malloc(sizeof(char) * 32);
    int outputIndex = 0;
    int index;
    for(index = 8; data[0][index] != ' '; index++)
    {
        outputArr[outputIndex++] = data[0][index];
    }
    printf("%s",outputArr);
    free(outputArr);

    //Prints the starting address of the program
    printf("0000");
    for(index = 0; index < 4; index++)
    {
        printf("%c",data[0][index]);
    }

    //Prints the length of the program
    char* startAddr = malloc(sizeof(char) * 4);
    char* endAddr = malloc(sizeof(char) * 4);
    for(index = 0; index < 4; index++)
    {
        startAddr[index] = data[0][index];
        endAddr[index] = data[numLines - 1][index];
    }
    int start = (int)strtol(startAddr, NULL, 16);
    int end = (int)strtol(endAddr, NULL, 16);
    int length = end - start;
    int lengthEOF = length + 3;
    printf("%04X", lengthEOF);



    /**
     * Define Record
     */
    printf("\nD");
    //Keeps track of the line number and index position within the line
    int lineNum = 0;
    index = 0;

    //Keeps track of the position of the EXTDEF directive in the file
    int pos = 0;

    //Keeps track of which line EXTDEF is in
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

    //Finds all external symbols within the file and prints them to the ESTAB
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
                    //Gets the name of the symbol
                    printf("%s",extDefSymbols[extDefLineNum]);

                    //Gets the address of the symbol
                    for(index = 0; index < 4; index++)
                    {
                        char ch = data[lineNum][index];
                        printf("%c",ch);
                    }

                    //Remove the symbol from the list since it's been found
                    extDefSymbols[extDefLineNum] = "\n";
                }
            }
        }
    }






    /**
     * Refer Record
     */
    printf("\nR");
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"EXTREF") != NULL)
        {
            char* src = data[lineNum];
            char* dst = strstr(src, "EXTREF");
            int pos = dst - src;
            pos += 6;
            for(index = pos; index < cols[lineNum]; index++)
            {
                ch = data[lineNum][index];
                if(ch != ' ')
                {
                    printf("%c",ch);
                }
            }
        }
    }





    /**
     * ---------------------------------------------------------------------------------------------
     * Text Record
     * ---------------------------------------------------------------------------------------------
     */
    lineNum = 0;
    int x = 0;
    const int OBJ_CODE_INDEX = 52;
    const int MAX_LINE_LEN = 30;
    int lineLength = 0;

    int y = 0;
    int lineLength2 = 0;

    int globalAddressCounter = 0;
    int count = 0;

    char strt[4] = {'0','0','0','0'};
    char endd[4] = {'0','0','0','0'};

    while((globalAddressCounter <= length) && (count < 2))
    {
        lineLength = 0;
        lineLength2 = 0;
        //Print T
        printf("\nT");
        //Print the starting address of the record
        printf("00");
        for(int i = 0; i < 4; i++)
        {
            printf("%c",data[x][i]);
        }

        //Print the length of the record
        while(y < numLines - 1)
        {
            if(cols[y] < OBJ_CODE_INDEX)
            {
                //No object code
                char* startAddr = malloc(sizeof(char) * 4);
                char* endAddr = malloc(sizeof(char) * 4);
                for(index = 0; index < 4; index++)
                {
                    startAddr[index] = data[y][index];
                    endAddr[index] = data[y + 1][index];
                }
                int start = (int)strtol(startAddr, NULL, 16);
                int end = (int)strtol(endAddr, NULL, 16);
                globalAddressCounter = globalAddressCounter + (end - start);
                
            }
            else
            {
                //There is object code
                //How long is the object code in this line?
                char* startAddr = malloc(sizeof(char) * 4);
                char* endAddr = malloc(sizeof(char) * 4);
                for(index = 0; index < 4; index++)
                {
                    startAddr[index] = data[y][index];
                    endAddr[index] = data[y + 1][index];
                }
                int start = (int)strtol(startAddr, NULL, 16);
                int end = (int)strtol(endAddr, NULL, 16);
                lineLength2 = lineLength2 + (end - start);
                globalAddressCounter = globalAddressCounter + (end - start);
                if(lineLength2 <= MAX_LINE_LEN)
                {
                    //Add the object code to the record

                }
                else
                {
                    //Do not add the object code to the record, this indicates the end of a record
                    lineLength2 -= end - start;
                    globalAddressCounter = globalAddressCounter - (end - start);
                    break;
                }

            }
            y++;
        }
        if(count == 1)
        {
            printf("03");
        }
        else
        {
            printf("%02X",lineLength2);
        }

        //Print the object code for the record
        while(x < numLines - 1)
        {
            if(cols[x] < OBJ_CODE_INDEX)
            {
                //No object code
                
            }
            else
            {
                //There is object code
                //How long is the object code in this line?
                char* startAddr = malloc(sizeof(char) * 4);
                char* endAddr = malloc(sizeof(char) * 4);
                for(index = 0; index < 4; index++)
                {
                    startAddr[index] = data[x][index];
                    endAddr[index] = data[x + 1][index];
                }
                int start = (int)strtol(startAddr, NULL, 16);
                int end = (int)strtol(endAddr, NULL, 16);
                lineLength = lineLength + (end - start);
                //printf("%d",lineLength);
                if(lineLength <= MAX_LINE_LEN)
                {
                    //Add the object code to the record
                    for(int i = OBJ_CODE_INDEX - 1; i < cols[x]; i++)
                    {
                        printf("%c",data[x][i]);
                    }

                }
                else
                {
                    //Do not add the object code to the record, this indicates the end of a record
                    break;
                }

            }
            x++;
        }
        lineNum++;

        if(count == 1)
        {
            printf("454F46");
        }

        if(globalAddressCounter == length)
        {
            count++;
        }
    }





    /**
     * Modification Record
     */

    //Gets the number of externally defined symbols
    int extRefNumLines = 0;
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"EXTREF") != NULL)
        {
            dataLine = lineNum;
            char* src = data[dataLine];
            char* dst = strstr(src, "EXTREF");
            pos = dst - src;
            pos += 6;
            for(index = pos; index < cols[dataLine]; index++)
            {
                ch = data[dataLine][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == cols[dataLine] - 1))
                    {
                        extRefNumLines++;
                    }
                }
            }
        }
    }

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char** extRefSymbols = malloc(sizeof(char*) * extRefNumLines);

    //Dynamically allocates space for our 2d array
    int extRefLineNum = 0;
    int extRefIndex = 0;
    for(index = pos; index < cols[dataLine]; index++)
    {
        ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(index == cols[dataLine] - 1)
            {
                extRefIndex++;
                extRefSymbols[extRefLineNum] = (char*)malloc(sizeof(char) * (extRefIndex));
            }
            else if(ch == ',')
            {
                extRefSymbols[extRefLineNum] = (char*)malloc(sizeof(char) * (extRefIndex));
                extRefIndex = 0;
                extRefLineNum++;
            }
            else
            {
                extRefIndex++;
            }
        }
    }

    //Fills our 2d array with all the externally defined symbols
    extRefIndex = 0;
    extRefLineNum = 0;
    for(index = pos; index <= cols[dataLine]; index++)
    {
        ch = data[dataLine][index];
        if(ch != ' ')
        {
            if(ch == ',')
            {
                extRefIndex = 0;
                extRefLineNum++;
            }
            else
            {
                extRefSymbols[extRefLineNum][extRefIndex] = ch;
                extRefIndex++;
            }
        }
    }

    //Finds all external symbols within the file and prints them to the ESTAB
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        int count = 0;
        for(index = OBJ_CODE_INDEX - 1; index < cols[lineNum]; index++)
        {
            count++;
        }
        //printf("%d",count);
        if(count == 8)
        {
            //Mod record
            printf("\nM");
            //Address
            printf("00");
            char* startAddr = malloc(sizeof(char) * 4);
            char* endAddr = malloc(sizeof(char) * 4);
            for(int i = 0; i < 4; i++)
            {
                startAddr[i] = data[lineNum][i];
                if(i != 3)
                {
                    endAddr[i] = '0';
                }
                else
                {
                    endAddr[i] = '1';
                }
            }
            int start = (int)strtol(startAddr, NULL, 16);
            int end = (int)strtol(endAddr, NULL, 16);
            int ans = start + end;
            printf("%04X",ans);
            //Length of the field to be modified
            printf("05+");
            for(int i = 8; data[0][i] != ' '; i++)
            {
                printf("%c",data[0][i]);
            }
        }
        else
        {
            for(extRefLineNum = 0; extRefLineNum < extRefNumLines; extRefLineNum++)
            {
                if(strstr(data[lineNum],extRefSymbols[extRefLineNum]) != NULL && strstr(data[lineNum],"EXTREF") == NULL)
                {
                    //Mod record
                    printf("\nM");
                    //Address
                    printf("00");
                    char* startAddr = malloc(sizeof(char) * 4);
                    char* endAddr = malloc(sizeof(char) * 4);
                    for(int i = 0; i < 4; i++)
                    {
                        startAddr[i] = data[lineNum][i];
                        if(i != 3)
                        {
                            endAddr[i] = '0';
                        }
                        else
                        {
                            endAddr[i] = '1';
                        }
                    }
                    int start = (int)strtol(startAddr, NULL, 16);
                    int end = (int)strtol(endAddr, NULL, 16);
                    int ans = start + end;
                    printf("%04X",ans);
                    //Length of the field to be modified
                    if(strstr(data[lineNum],"WORD") != NULL || strstr(data[lineNum],"BYTE") != NULL ||
                       strstr(data[lineNum],"RESW") != NULL || strstr(data[lineNum],"RESB") != NULL ||
                       strstr(data[lineNum],"EQU") != NULL)
                    {
                        printf("06");
                    }
                    else
                    {
                        printf("05");
                    }
                    if(strstr(data[lineNum],"-") != NULL)
                    {
                        printf("-");
                    }
                    else
                    {
                        printf("+");
                    }
                    printf("%s",extRefSymbols[extRefLineNum]);

                    //Remove the symbol from the list since it's been found
                    extRefSymbols[extRefLineNum] = "\n";
                }
            }
        }
    }





    /**
     * End Record
     */
    printf("\nE");

    //Finds the end directive and the symbol of the first executable instruction
    char* endArr = malloc(sizeof(char) * 16);
    int endArrIndex = 0;
    for(lineNum = 0; lineNum < numLines; lineNum++)
    {
        if(strstr(data[lineNum],"END") != NULL)
        {
            char* src = data[lineNum];
            char* dst = strstr(src, "END");
            int pos = dst - src;
            pos += 3;
            for(index = pos; index < cols[lineNum]; index++)
            {
                char ch = data[lineNum][index];
                if(ch != ' ')
                {
                    endArr[endArrIndex++] = ch;
                }
            }
        }
    }

    //Finds the symbol of the first executable instruction and appends its address to the end record
    printf("00");
    if(sizeof endArr == 0)
    {
        printf("%s",startAddr);
    }
    else
    {
        for(lineNum = 0; lineNum < numLines; lineNum++)
        {
            if(strstr(data[lineNum],endArr) != NULL)
            {
                for(index = 0; index < 4; index++)
                {
                    printf("%c",data[lineNum][index]);
                }
                lineNum = numLines;
            }
        }
    }
    free(endArr);

    //Frees the allocated memory for the start and end addresses
    free(startAddr);
    free(endAddr);
}