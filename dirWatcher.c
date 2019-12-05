//Autor: aglahir
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <string.h>

void synchronizeAllData()
{
    int fileCount = 0;
    char ** outNames;
    struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return ; 
    } 
  
    while ((de = readdir(dr)) != NULL) 
        if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
            printf("%s\n", de->d_name);
  
    closedir(dr);
}

int main(void) 
{ 
    synchronizeAllData();
    return 0; 
}