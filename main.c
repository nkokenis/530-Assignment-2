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

    char address[4] = {};
    char label[16] = {};
    char instruction[16] = {};
    char operand[64] = {};
    char objcode[8] = {};

    int numLines = 0;

    int index = 0;
    int len = 0;

    int newWhiteSpace = 0;

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
            if(newWhiteSpace == 0)
            {
                printf("%d",index);
                len = 0;
                index++;
                newWhiteSpace = 1;
            }
        }
        else if(ch == '\n')
        {
            printf("%d\n",index);
            numLines++;
            len = 0;
            index = 0;
            newWhiteSpace = 0;
        }
        else
        {
            if(index == 0)
            {
                address[len] = ch;
                newWhiteSpace = 0;
                len++;
            }
            else if(index == 1)
            {
                label[len] = ch;
                newWhiteSpace = 0;
                len++;
            }
            else if(index == 2)
            {
                instruction[len] = ch;
                newWhiteSpace = 0;
                len++;
            }
            else if(index == 3)
            {
                operand[len] = ch;
                newWhiteSpace = 0;
                len++;
            }
            else if(index == 4)
            {
                objcode[len] = ch;
                newWhiteSpace = 0;
                len++;
            }
        }
    }
}

void parseInstruction(char* instruction)
{
    printf("%s\n", instruction);
}