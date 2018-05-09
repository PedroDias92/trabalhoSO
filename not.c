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
    char * line2 = NULL;
    size_t len2 = 0;
    ssize_t read2;
    fp = fopen(argv[1], "r");
    char *dollar="$ ";
    char *dollarPipe="$| ";
    FILE * result;
    result =fopen("result.txt","wr+");
    FILE * result2;
    result2 =fopen("result.txt","wr+");
    FILE * bla;
    bla =fopen("zla.txt","wr+");
    FILE *out;
    out = fopen("out.txt", "wr+");


    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
        fputs(line,out);
        if(strncmp(dollar,line,strlen(dollar))==0){
			char *b =  trim(line + 2);
            //printf("%s",b);
            int p=fork();
            if(p==0){
                dup2(fileno(result),1); //STDOUT_FILENO
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
            }
            else{
                wait(0);
                fclose(result);
                fputs("\n>>>\n",out);

                FILE * result2;
                result2 =fopen("result.txt","r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("\n<<<\n",out);  
                }
        }
        //If dollarPipe 
            
        if(strncmp(dollarPipe,line,strlen(dollarPipe))==0){     
            char *b =  trim(line + 3);
            printf("%s",b);
            int d=fork();
            if(d==0){
                result =fopen("result.txt","r");
                dup2(fileno(result),0);//STDIN_FILENO
                dup2(fileno(bla),1); //STDOUT_FILENO
                fclose(result);
                fclose(bla);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
            }
            else{
                wait(0);
                fclose(result);
                fputs("\n>>>\n",out);

                FILE * result2;
                result2 =fopen("result.txt","r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("\n<<<\n",out);  
                }
        printf("OLSSSSSS2\n");  
        }  
        

        
    }

    
                   

    fclose(fp);
    
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
	
    
}