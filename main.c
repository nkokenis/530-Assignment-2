#include <stdio.h>

//Function definitions
void assemble(char* listingFilePath);

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

    char buffer[64];
    int index;

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
            index++;
        }
        else
        {
            buffer[index] = ch;
        }

        if(ch != ' ')
        {
            printf("%c",ch);
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