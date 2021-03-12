#include <stdio.h>
#include <string.h>

//Function definitions
void assemble(char* listingFilePath);
void parseInstruction(char* instruction);

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
        assemble(argv[i]);
    }

    return 0;
}

void assemble(char* listingFilePath)
{
    //Create a FILE pointer using the file path provided
    FILE* listingFile = fopen(listingFilePath,"r");
    if(listingFile == NULL)
    {
        //The file does not exist or couldn't be found
        printf("File not found.");
    }

    char line[8][64] = {};
    char buffer[64] = {};
    int index = 0;
    int lineIndex = 0;
    int nextWhiteSpace = 1;

    //Iterate through the file, character by character
    while(1)
    {
        //Get the next character in the listing file
        char ch = fgetc(listingFile);
        if(feof(listingFile))
        {
            break;
        }

        if(ch == ' ')
        {
            if(nextWhiteSpace == 1)
            {
                //Add the buffer to the line array
                lineIndex++;

                index = 0;
                nextWhiteSpace = 0;
                memset(buffer, 0, sizeof buffer);
            }
        }
        else if(ch == '\n')
        {
            lineIndex = 0;
        }
        else
        {
            nextWhiteSpace = 1;
            buffer[index] = ch;
            index++;
        }
        

        /*
        char buffer[30];
        //"%[^\n]"
        while(fscanf(listingFile, " ", buffer) != EOF)
        {
            printf("%s", buffer);
        }
        */
    }
}

void parseInstruction(char* instruction)
{
    printf("%s\n", instruction);
}