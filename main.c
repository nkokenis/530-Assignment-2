#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Nicholas Kokenis, Jenna Coffman
 * CSSC3766, CSSC3702
 * Project #2
 * main.c
 */





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
void buildSymTab(int rows, int cols, char data[rows][cols], char* listingFilePath);

/**
 * Function Name:
 * Build Object File
 * 
 * Function Description: 
 * The buildObjFile function is responsible for reading the 2d array
 * from the assemble function and creating the object file (records)
 */
void buildObjFile(int rows, int cols, char data[rows][cols], char* listingFilePath);





int main(int argc, char** argv)
{
    //Checks to see if the user entered the correct number of arguments
    if(argc < 2)
    {
        printf("Program needs at least one argument.\n");
        return 0;
    }

    //Begins the linkage editor for each listing file
    int i;
    for(i = 1; i < argc; i++)
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

    //Sets the constant variable for the maximum number of characters per line
    const int MAX_LENGTH = 60;

    //Gets the number of lines in the file, not counting lines containing comments
    int numLines = 0;
    int commentCheck = 0;
    char ch;
    while(1)
    {
        ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            break;
        }

        if(ch == '\n')
        {
            commentCheck = 0;
        }
        else if(commentCheck == 0 && ch == '.')
        {
            commentCheck = 1;
        }
        else if(commentCheck == 0 && ch != '.')
        {
            commentCheck = 1;
            numLines++;
        }
    }
    rewind(listingFile);

    //Creates a dynamically allocated 2d array to store the line contents
    char data[numLines][MAX_LENGTH];

    //Dynamically allocates memory for our 2d array
    int lineNum = 0;
    int index = 0;
    while(1)
    {
        ch = fgetc(listingFile);
        if(ch == '.')
        {
            if(index != 0)
            {
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
            index = 0;
            lineNum++;
        }
        else
        {
            data[lineNum][index] = ch;
            index++;
        }
    }

    //Calls our "Build Symbol Table" and "Build Object File" functions
    buildSymTab(numLines, MAX_LENGTH, data, listingFilePath);
    buildObjFile(numLines, MAX_LENGTH, data, listingFilePath);
}

void buildSymTab(int rows, int cols, char data[rows][cols], char* listingFilePath)
{
    char* temp;
    temp = strchr(listingFilePath, '.');
    *temp = '\0';
    strcat(listingFilePath, ".st");

    //Creates a file pointer for our output
    FILE *outputFile;
    outputFile = fopen(listingFilePath,"w");

    //Constant value for max symbol length
    const int MAX_SYMBOL_LENGTH = 8;
    const int MAX_ADDRESS_LENGTH = 4;

    //Keeps track of the line number and index position within the line
    int lineNum = 0;
    int index = 0;

    //Keeps track of the line number and position of the EXTDEF directive
    int extDefPos = 0;
    int extDefLine = 0;

    //Gets the number of externally defined symbols in the file
    int extDefNumLines = 0;
    char ch;
    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        if(strstr(data[lineNum],"EXTDEF") != NULL)
        {
            extDefLine = lineNum;
            char* src = data[extDefLine];
            char* dst = strstr(src, "EXTDEF");
            extDefPos = dst - src;
            extDefPos += 6;
            for(index = extDefPos; index < cols; index++)
            {
                ch = data[extDefLine][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == cols - 1))
                    {
                        extDefNumLines++;
                    }
                }
            }
        }
    }

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char extDefSymbols[extDefNumLines][MAX_SYMBOL_LENGTH];

    //Dynamically allocates space for our 2d array
    int extDefLineNum = 0;
    int extDefIndex = 0;
    for(index = extDefPos; index <= cols; index++)
    {
        ch = data[extDefLine][index];
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
    char outputArr[MAX_SYMBOL_LENGTH];
    int outputIndex = 0;
    for(index = 8; data[0][index] != ' '; index++)
    {
        outputArr[outputIndex++] = data[0][index];
    }
    fprintf(outputFile,"%-8s",outputArr);

    //Prints the address of the control section for the ESTAB
    char outputAddress[MAX_ADDRESS_LENGTH];
    outputIndex = 0;
    for(index = 0; index < 4; index++)
    {
        outputAddress[outputIndex++] = data[0][index];
    }
    fprintf(outputFile,"%*s",16,"");
    fprintf(outputFile,"%-12s",outputAddress);

    //Prints the length of the control section for the ESTAB
    char startAddr[MAX_ADDRESS_LENGTH];
    char endAddr[MAX_ADDRESS_LENGTH];
    for(index = 0; index < 4; index++)
    {
        startAddr[index] = data[0][index];
        endAddr[index] = data[rows - 1][index];
    }
    int start = (int)strtol(startAddr, NULL, 16);
    int end = (int)strtol(endAddr, NULL, 16);
    int length = end - start;
    length = length + 3;
    fprintf(outputFile,"%04X", length);

    //Finds all external symbols within the file and prints them to the ESTAB
    for(lineNum = 0; lineNum < rows; lineNum++)
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
                    fprintf(outputFile,"\n%*s",8,"");
                    fprintf(outputFile,"%-8s",extDefSymbols[extDefLineNum]);
                    fprintf(outputFile,"%*s",8,"");

                    //Gets the address of the symbol
                    for(index = 0; index < 4; index++)
                    {
                        char ch = data[lineNum][index];
                        fprintf(outputFile,"%c",ch);
                    }

                    //Remove the symbol from the list since it's been found
                    for(index = 0; index < MAX_SYMBOL_LENGTH; index++)
                    {
                        extDefSymbols[extDefLineNum][index] = "";
                    }
                }
            }
        }
    }
    fclose(outputFile);
}

void buildObjFile(int rows, int cols, char data[rows][cols], char* listingFilePath)
{
    char* temp;
    temp = strchr(listingFilePath, '.');
    *temp = '\0';
    strcat(listingFilePath, ".obj");

    //Creates a file pointer for our output
    FILE *outputFile;
    outputFile = fopen(listingFilePath,"w");

    //Constant variables
    const int MAX_ADDRESS_LENGTH = 4;
    const int MAX_SYMBOL_LENGTH = 8;

    //Keeps track of the line number and index position within the file
    int lineNum = 0;
    int index = 0;





    /**
     * ------------------------------------------------------------
     * Header Record
     * ------------------------------------------------------------
     */

    //Prints the first column in the record
    fprintf(outputFile,"H");

    //Prints the control section of the program
    for(index = 8; data[0][index] != ' '; index++)
    {
        fprintf(outputFile,"%c",data[0][index]);
    }

    //Prints the starting address of the program
    fprintf(outputFile,"0000");
    for(index = 0; index < 4; index++)
    {
        fprintf(outputFile,"%c",data[0][index]);
    }

    //Prints the length of the program
    char startAddr[MAX_ADDRESS_LENGTH];
    char endAddr[MAX_ADDRESS_LENGTH];
    for(index = 0; index < 4; index++)
    {
        startAddr[index] = data[0][index];
        endAddr[index] = data[rows - 1][index];
    }
    int start = (int)strtol(startAddr, NULL, 16);
    int end = (int)strtol(endAddr, NULL, 16);
    int length = end - start;
    int lengthEOF = length + 3;
    fprintf(outputFile,"%04X", lengthEOF);





    /**
     * ------------------------------------------------------------
     * Define Record
     * ------------------------------------------------------------
     */

    //Prints the first column of the record
    fprintf(outputFile,"\nD");

    //Keeps track of the position and line number of the EXTDEF directive
    int extDefPos = 0;
    int extDefLine = 0;

    //Gets the number of externally defined symbols
    int extDefNumLines = 0;
    char ch;
    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        if(strstr(data[lineNum],"EXTDEF") != NULL)
        {
            extDefLine = lineNum;
            char* src = data[extDefLine];
            char* dst = strstr(src, "EXTDEF");
            extDefPos = dst - src;
            extDefPos += 6;
            for(index = extDefPos; index < cols; index++)
            {
                ch = data[extDefLine][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == cols - 1))
                    {
                        extDefNumLines++;
                    }
                }
            }
        }
    }

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char extDefSymbols[extDefNumLines][MAX_SYMBOL_LENGTH];

    //Dynamically allocates space for our 2d array
    int extDefLineNum = 0;
    int extDefIndex = 0;
    for(index = extDefPos; index <= cols; index++)
    {
        ch = data[extDefLine][index];
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
    for(lineNum = 0; lineNum < rows; lineNum++)
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
                    fprintf(outputFile,"%s",extDefSymbols[extDefLineNum]);

                    //Gets the address of the symbol
                    for(index = 0; index < 4; index++)
                    {
                        char ch = data[lineNum][index];
                        fprintf(outputFile,"%c",ch);
                    }

                    //Remove the symbol from the list since it's been found
                    for(index = 0; index < MAX_SYMBOL_LENGTH; index++)
                    {
                        extDefSymbols[extDefLineNum][index] = "";
                    }
                }
            }
        }
    }





    /**
     * ------------------------------------------------------------
     * Refer Record
     * ------------------------------------------------------------
     */
    fprintf(outputFile,"\nR");
    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        if(strstr(data[lineNum],"EXTREF") != NULL)
        {
            char* src = data[lineNum];
            char* dst = strstr(src, "EXTREF");
            int pos = dst - src;
            pos += 6;
            for(index = pos; index < cols; index++)
            {
                ch = data[lineNum][index];
                if(ch != ' ')
                {
                    if(ch == ',')
                    {
                        fprintf(outputFile," ");
                    }
                    else
                    {
                        fprintf(outputFile,"%c",ch);
                    }
                }
            }
        }
    }





    /**
     * ------------------------------------------------------------
     * Text Record
     * ------------------------------------------------------------
     */

    //Data constants for the maximum length (in bytes) of a line and the index of object code in the line
    const int OBJ_CODE_INDEX = 52;
    const int MAX_LINE_LEN = 30;

    //Keeps track of object code when filling the array
    int x = 0;
    int lineLength = 0;

    //Keeps track of object code when calculating the length
    int y = 0;
    int lineLength2 = 0;

    //
    int globalAddressCounter = 0;
    int count = 0;

    //Creates our text records
    while((globalAddressCounter <= length) && (count < 2))
    {
        lineLength = 0;
        lineLength2 = 0;

        //Prints the first column of the record
        fprintf(outputFile,"\nT");

        //Prints the starting address of the record
        fprintf(outputFile,"00");
        int i;
        for(i = 0; i < 4; i++)
        {
            fprintf(outputFile,"%c",data[x][i]);
        }

        //Prints the length of the record
        while(y < rows - 1)
        {
            if(data[y][OBJ_CODE_INDEX] == ' ')
            {
                //If there's no object code in this line, do the following
                char startAddr[MAX_ADDRESS_LENGTH];
                char endAddr[MAX_ADDRESS_LENGTH];
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
                //Calculates the length (in bytes) of the object code in this line
                char startAddr[MAX_ADDRESS_LENGTH];
                char endAddr[MAX_ADDRESS_LENGTH];
                for(index = 0; index < 4; index++)
                {
                    startAddr[index] = data[y][index];
                    endAddr[index] = data[y + 1][index];
                }
                int start = (int)strtol(startAddr, NULL, 16);
                int end = (int)strtol(endAddr, NULL, 16);
                lineLength2 = lineLength2 + (end - start);
                globalAddressCounter = globalAddressCounter + (end - start);
                if(!(lineLength2 <= MAX_LINE_LEN))
                {
                    lineLength2 -= end - start;
                    globalAddressCounter = globalAddressCounter - (end - start);
                    break;
                }

            }
            y++;
        }
        if(count == 1)
        {
            fprintf(outputFile,"03");
        }
        else
        {
            fprintf(outputFile,"%02X",lineLength2);
        }

        //Prints the object code for the record
        while(x < rows - 1)
        {
            if(data[x][OBJ_CODE_INDEX] != ' ')
            {
                //There is object code
                //How long is the object code in this line?
                char startAddr[MAX_ADDRESS_LENGTH];
                char endAddr[MAX_ADDRESS_LENGTH];
                for(index = 0; index < 4; index++)
                {
                    startAddr[index] = data[x][index];
                    endAddr[index] = data[x + 1][index];
                }
                int start = (int)strtol(startAddr, NULL, 16);
                int end = (int)strtol(endAddr, NULL, 16);
                lineLength = lineLength + (end - start);
                if(lineLength <= MAX_LINE_LEN)
                {
                    //Add the object code to the record
                    int i;
                    for(i = OBJ_CODE_INDEX - 1; i < cols; i++)
                    {
                        fprintf(outputFile,"%c",data[x][i]);
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
            fprintf(outputFile,"454F46");
        }
        if(globalAddressCounter == length)
        {
            count++;
        }
    }





    /**
     * ------------------------------------------------------------
     * Modification Record
     * ------------------------------------------------------------
     */

    //Gets the number of externally defined symbols
    int extRefNumLines = 0;
    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        if(strstr(data[lineNum],"EXTREF") != NULL)
        {
            extDefLine = lineNum;
            char* src = data[extDefLine];
            char* dst = strstr(src, "EXTREF");
            extDefPos = dst - src;
            extDefPos += 6;
            for(index = extDefPos; index < cols; index++)
            {
                ch = data[extDefLine][index];
                if(ch != ' ')
                {
                    if(ch == ',' || (index == cols - 1))
                    {
                        extRefNumLines++;
                    }
                }
            }
        }
    }

    //Creates a dynamically allocated 2d array to store the externally defined symbols
    char extRefSymbols[extRefNumLines][MAX_SYMBOL_LENGTH];

    //Dynamically allocates space for our 2d array
    int extRefLineNum = 0;
    int extRefIndex = 0;
    for(index = extDefPos; index <= cols; index++)
    {
        ch = data[extDefLine][index];
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

    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        int count = 0;
        for(index = OBJ_CODE_INDEX - 1; index < cols; index++)
        {
            count++;
        }
        if(count == 8)
        {
            fprintf(outputFile,"\nM");
            fprintf(outputFile,"00");
            char startAddr[MAX_ADDRESS_LENGTH];
            char endAddr[MAX_ADDRESS_LENGTH];
            int i;
            for(i = 0; i < 4; i++)
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
            fprintf(outputFile,"%04X",ans);
            fprintf(outputFile,"05+");
            for(i = 8; data[0][i] != ' '; i++)
            {
                fprintf(outputFile,"%c",data[0][i]);
            }
        }
        else
        {
            for(extRefLineNum = 0; extRefLineNum < extRefNumLines; extRefLineNum++)
            {
                if(strstr(data[lineNum],extRefSymbols[extRefLineNum]) != NULL && strstr(data[lineNum],"EXTREF") == NULL && 
                   strstr(data[lineNum],"BASE") == NULL)
                {
                    fprintf(outputFile,"\nM");
                    fprintf(outputFile,"00");
                    char startAddr[MAX_ADDRESS_LENGTH];
                    char endAddr[MAX_ADDRESS_LENGTH];
                    int i;
                    for(i = 0; i < 4; i++)
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
                    fprintf(outputFile,"%04X",ans);
                    if(strstr(data[lineNum],"WORD") != NULL || strstr(data[lineNum],"BYTE") != NULL ||
                       strstr(data[lineNum],"RESW") != NULL || strstr(data[lineNum],"RESB") != NULL ||
                       strstr(data[lineNum],"EQU") != NULL)
                    {
                        fprintf(outputFile,"06");
                    }
                    else
                    {
                        fprintf(outputFile,"05");
                    }
                    if(strstr(data[lineNum],"-") != NULL)
                    {
                        fprintf(outputFile,"-");
                    }
                    else
                    {
                        fprintf(outputFile,"+");
                    }
                    fprintf(outputFile,"%s",extRefSymbols[extRefLineNum]);

                    //Remove the symbol from the list since it's been found
                    for(index = 0; index < MAX_SYMBOL_LENGTH; index++)
                    {
                        extRefSymbols[extRefLineNum][index] = "";
                    }
                }
            }
        }
    }





    /**
     * ------------------------------------------------------------
     * End Record
     * ------------------------------------------------------------
     */

    fprintf(outputFile,"\nE");

    //Finds the end directive and the symbol of the first executable instruction
    char endArr[MAX_SYMBOL_LENGTH];
    int endArrIndex = 0;
    for(lineNum = 0; lineNum < rows; lineNum++)
    {
        if(strstr(data[lineNum],"END") != NULL)
        {
            char* src = data[lineNum];
            char* dst = strstr(src, "END");
            int pos = dst - src;
            pos += 3;
            for(index = pos; index < cols; index++)
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
    if(sizeof endArr == 0)
    {
        fprintf(outputFile,"%s",startAddr);
    }
    else
    {
        for(lineNum = 0; lineNum < rows; lineNum++)
        {
            if(strstr(data[lineNum],endArr) != NULL)
            {
                fprintf(outputFile,"00");
                for(index = 0; index < 4; index++)
                {
                    fprintf(outputFile,"%c",data[lineNum][index]);
                }
                lineNum = rows;
            }
        }
    }
    fclose(outputFile);
}
