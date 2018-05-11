#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>


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
    
	char *REDO = argv[1];
    re_processamento(REDO);

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
    
    
    ////// HISTORICO DE COMANDOS PARA SELECCIONARMOS N COMANDO (Exerc. 2.2.1)
    history =fopen("history.txt","wr+");
    
    
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

			// ABRIR RESULTN.TXT
            char firstDollar[50];
            char append[50];
            char contadorString[5];
            strcpy (firstDollar, "result");
            strcpy (append, ".txt");
            sprintf(contadorString,"%d",contador); // int to string
            strcat(firstDollar, contadorString);
            strcat(firstDollar,append);
            // ABRIR RESULTN.TXT
			result1 =fopen(firstDollar,"wr+");

            int p=fork();
            if(p==0){
                dup2(fileno(result1),1); //STDOUT_FILENO
				fclose(result1);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
            }
            else{
                wait(0);
                fclose(result1);

                fputs(">>>\n",out);
                FILE * result2;
                result2 =fopen(firstDollar,"r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n",out);  
                }
            
        }

        //---------------------------------------------------------------COMANDO COM PIPE----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------
        if(strncmp(dollarPipe,line,strlen(dollarPipe))==0){     
            char *b =  trim(line + 3);
            contador++;
			printf("comando nº: %d \t %s\n",contador,b);
            
            // ABRIR RESULTN.TXT           MUDAR OS NOMES DISTO!!!!
            char filename[50];
            char append[50];
            char contadorString[5];
            strcpy (filename, "result");
            strcpy (append, ".txt");
            sprintf(contadorString,"%d",contador); // int to string
            strcat(filename, contadorString);
            strcat(filename,append);
            // ABRIR RESULTN.TXT
            
            FILE * resultN;
            resultN =fopen(filename,"wr+"); //abre o resultN.txt
            int d=fork();
            if(d==0){
				
				// ABRIR RESULTN.TXT
            	char filename2[50];
            	char append2[50];
            	char contadorString2[5];
            	strcpy (filename2, "result");
            	strcpy (append2, ".txt");
            	sprintf(contadorString2,"%d",contador-1); // int to string
            	strcat(filename2, contadorString2);
            	strcat(filename2,append2);
            	// ABRIR RESULTN.TXT

                result1 =fopen(filename2,"r");
                dup2(fileno(result1),0);//STDIN_FILENO
                dup2(fileno(resultN),1); //STDOUT_FILENO
                fclose(result1);
                fclose(resultN);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
            }
            else{
                wait(0);
                fclose(result1);
                fputs(">>>\n",out);
                FILE * result2;
                result2 =fopen(filename,"r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n",out);  
            }


        
        }

		
        //---------------------------------------------------------------NUMERO DE COMANDO ( $n| )-------------------------------------------------------
        /* Compile regular expression */
        reti = regcomp(&regex, "$[1-9][0-9]*|", 0);
        if( reti ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }

		/* Execute regular expression */
        reti = regexec(&regex, line, 0, NULL, 0);
        if( !reti ){
                //printf("\n Match %s \n",line);

				char *b =  trim(line + 4);
				int number = atoi(&line[1]);
				contador++;
				printf("comando nº: %d \t %s\t com STDIN do comando %d\n",contador,b,number);
				
				// ABRIR RESULTN.TXT       Para colocar o STDOUT
				char filename[50];
				char append[50];
				char contadorString[5];
				strcpy (filename, "result");
				strcpy (append, ".txt");
				sprintf(contadorString,"%d",contador); // int to string
				strcat(filename, contadorString);
				strcat(filename,append);
				// ABRIR RESULTN.TXT		Para colocar o STDOUT
				
				FILE * resultN;
				resultN =fopen(filename,"wr+"); //abre o resultN.txt
				int d=fork();
				if(d==0){
					
					// ABRIR RESULTN.TXT de onde se vai ler o STDIN
					char filename2[50];
					char append2[50];
					char contadorString2[5];
					strcpy (filename2, "result");
					strcpy (append2, ".txt");
					sprintf(contadorString2,"%d",number); // A CHAVE ESTA AQUI! vai buscar o N de "$N|"  int number = atoi(&line[1]);
					strcat(filename2, contadorString2);
					strcat(filename2,append2);
					// ABRIR RESULTN.TXT		de onde se vai ler o STDIN

					result1 =fopen(filename2,"r");
					dup2(fileno(result1),0);//STDIN_FILENO
					dup2(fileno(resultN),1); //STDOUT_FILENO
					fclose(result1);
					fclose(resultN);
					execl("/bin/sh", "/bin/sh", "-c", b, NULL);
				}
				else{
					wait(0);
					fclose(result1);
					fputs(">>>\n",out);
					FILE * result2;
					result2 =fopen(filename,"r");
					//escrever para out.txt
					while ((read2 = getline(&line2, &len2, result2)) != -1){
							fputs(line2,out);
						} 
					fputs("<<<\n",out);
					//result1 =fopen("result1.txt","w"); // ELIMINA CONTEUDOS DO FICHEIRO PARA EXECUTAR PROXIMO COMANDO  
				}

        }
		









        if (line) free (line);
        line = NULL;
        //if (line2) free (line2);
        //line2 = NULL;
    }

    
                   

    fclose(fp);
    fclose(result1);
    fclose(history);
	fputs("\n",out);	// se não meter isto depois no re-processamento algo bate mal !!!
	rename("out.txt",argv[1]);
    exit(EXIT_SUCCESS);
	
    
}