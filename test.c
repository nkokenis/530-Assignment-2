#include <stdio.h>

int main()
{
    char address[4] = {};
    char* label = (char*)malloc(16);
    char* instruction = (char*)malloc(16);
    char* operand = (char*)malloc(64);
    char* objcode = (char*)malloc(8);

    char buffer[64] = {};
    int index = 0;
    int len = 0;

    while(1)
    {
        //Get the next character in the listing file
        char ch = "";

        if(ch == ' ')
        {
            len = 0;
            index++;
        }
        else if(ch == '\n')
        {
            //Parse line function
            memset(buffer, 0, sizeof buffer);
        }
        else
        {
            //buffer[index] = ch;

            if(index == 0)
            {
                address[len] = ch;
                len++;
            }
            else if(index == 1)
            {
                //point to label, add to that memory location
            }
        }

    return -1;
}