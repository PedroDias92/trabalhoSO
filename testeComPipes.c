#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>


char * trim(char * s) {
    int l = strlen(s);
    while(isspace(s[l - 1])) --l;
    while(* s && isspace(* s)) ++s, --l;
    return strndup(s, l);
}


void re_processamento(char * filename){
    char * line = NULL;
    size_t bufsize = 32;
    size_t len=0;
    ssize_t read=2;
    FILE * fp; 
    fp = fopen(filename, "r");
    FILE *REDO;
    REDO = fopen("REDO.txt", "wr+");
    
    
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1){
        if(strncmp(">>>\n",line,strlen(">>>\n"))==0){
            read = getline(&line, &len, fp);
            while(strncmp("<<<\n",line,strlen("<<<\n"))!=0)
                read = getline(&line, &len, fp);
        }
        if(strncmp("<<<\n",line,strlen("<<<\n"))==0){
            read = getline(&line, &len, fp);
        }
        //printf("%s",line);
        fputs(line,REDO);
        
        
    }
    fclose(fp);
    fclose(REDO);
    rename("REDO.txt",filename);

    if (line) free (line);
        line = NULL;
        //if (line2) free (line2);
        //line2 = NULL;
}

int main(int argc, char ** argv){


    FILE * fp;
    FILE * result1;
    FILE * result2;
    FILE * history;
    
	regex_t regex;
    int reti;

    char *comands[10];
    char * line = NULL;
    char * line2 = NULL;
    size_t bufsize = 32;
    size_t len,len2= 0;
    ssize_t read,read2;
    fp = fopen(argv[1], "r");
    char *dollar="$ ";
    char *dollarPipe="$| ";
    
    
    
    FILE *out;
    out = fopen("out.txt", "wr+");
    int contador=0,contComandos=0;

    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        fputs(line,out);

        //COMANDO SIMPLES
        if(strncmp(dollar,line,strlen(dollar))==0){
			contador++;
			contComandos++;
			char *b =  trim(line + 2);
			printf("comando nº: %d \t %s\n",contador,b);
			//result1 =fopen(firstDollar,"wr+");

            
            int p=fork();
            if(p==0){
                printf("estou no filho\n");
                mkfifo("fifo",0666);
                int fifo = open("fifo",O_WRONLY|O_RDONLY);
                //dup2(fileno(result1),1); //STDOUT_FILENO
                dup2(fifo,1);
				close(fifo);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
                //exit(-1);
            }
            else{
                wait(0);
                fputs(">>>\n",out);
                FILE * result2;
                result2 =fopen("fifo","r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n",out);  
                }
            
        }
    }


    
    exit(EXIT_SUCCESS);
	
    
}
