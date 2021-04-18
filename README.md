# 530-Assignment-2

Nickolas Kokenis, cssc3766
Jenna Coffman, cssc3702
CS530, Spring 2021

Project #2, Linkage Editor

----Input Files----
main.c
Makefile
README

----Output Files----
output.st
objoutput.obj

----Compile Instructions----
> Navigate to the directory where the project is stored ~/al/
> Compile the project by typing 'make' into the command line

----Operation Instructions----
> After compiling the program, you can run it using the instruction below:
led <filename>: 
    > optional: add more file names with extension .ls 

----Design Decisions----
> Utilized jagged 2d arrays for storing and referring to the different lines in the file
> Dynamically allocated many of the arrays used since there was not a standardized length for many areas
----Extra Features----
> Includes 3 functions: assemble, buildSymTab, and buildObjFile
    > assemble: The assemble function is responsible for reading the file and storing its contents in a 2d array to be parsed later on
    > buildSymTab: The buildSymTab function is responsible for reading the 2d array from the assemble function and creating an ESTAB
    > buildObjFile: The buildObjFile function is responsible for reading the 2d array from the assemble function and creating the object file (records)

----Known Bugs/Deficiencies----
N/A
         
----Lessons Learned----
> Improved knowledge of linkage editors
> Gave a more indepth look at how records are formed and how they interact with each other and the file
> Learned how to better plan for a project with an SDD as well as how to collaborate on a project