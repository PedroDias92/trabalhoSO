#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <errno.h>
#include <sys/stat.h> // para criar o directorio /tmp


main(){
    struct stat st = {0};

    if (stat("tmp", &st) == -1) {
        mkdir("tmp", 0700);
    }
    FILE * result1;
    // ABRIR RESULTN.TXT
    char firstDollar[50];
    char append[50];
    char contadorString[5];
    strcpy (firstDollar, "tmp/result");
    strcpy (append, ".txt");
    sprintf(contadorString,"%d",22); // int to string
    strcat(firstDollar, contadorString);
    strcat(firstDollar,append);
    // ABRIR RESULTN.TXT
	result1 =fopen(firstDollar,"wr+");
}
