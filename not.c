//gcc notebook.c -o notebook
//./notebook comandos.txt
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

char * trim(char * s) {
    int l = strlen(s);

    while(isspace(s[l - 1])) --l;
    while(* s && isspace(* s)) ++s, --l;

    return strndup(s, l);
}

int main(int argc, char ** argv){
    //printf("%s",argv[1]);
	char *comands[10];


    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int p=fork();
    fp = fopen(argv[1], "r");
    char *dollar="$ ";
    //int result = open("result.txt", O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, 0666);
    FILE * result;
    result =fopen("result.txt","wr+");
    FILE *out;
    out = fopen("out.txt", "w+");


    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
        fputs(line,out);
        if(strncmp(dollar,line,strlen(dollar))==0){
			char *b =  trim(line + 2);
            printf("%s",b);
            if(p==0){
                dup2(fileno(result),1); //STDOUT_FILENO
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
            }
            while ((read = getline(&line, &len, result)) != -1){
                fputs(line,out);
            }
        }
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
	
}