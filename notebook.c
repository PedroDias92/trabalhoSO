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
    
    ///// PARA APANHAR SINAIS ////
    printf("\nPressione Control C para cancelar escrita para o notebook.\n\n");
    sleep(2);
    ////
    
    FILE * fp;
    FILE * result1;
    FILE * result2;
    FILE * history;
    

    char *comands[10];
    char * line = NULL;
    char * line2 = NULL;
    size_t bufsize = 32;
    size_t len,len2= 0;
    ssize_t read,read2;
    fp = fopen(argv[1], "r");
    char *dollar="$ ";
    char *dollarNumber="$";
    char *dollarPipe="$| ";
    //result1 =fopen("result1.txt","wr+");
    
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

                fputs("\n>>>\n",out);
                FILE * result2;
                result2 =fopen(firstDollar,"r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n\n",out);  
                }
            
        }

        //COMANDO COM PIPE
        if(strncmp(dollarPipe,line,strlen(dollarPipe))==0){     
            char *b =  trim(line + 3);
            contador++;
            
            // ABRIR RESULTN.TXT
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
                fputs("\n>>>\n",out);
                FILE * result2;
                result2 =fopen(filename,"r");
                //escrever para out.txt
                while ((read2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n\n",out);
                //result1 =fopen("result1.txt","w"); // ELIMINA CONTEUDOS DO FICHEIRO PARA EXECUTAR PROXIMO COMANDO  
            }


        
        }
		/**
        //NUMERO DE COMANDO ( $n| )
        if(strncmp(dollarNumber,line,strlen(dollarNumber))==0){ 
            int tempCommand,count=0;  
            char *b =  trim(line + 1); // Para apanhar o número do comando que queremos
            if ((b[0]>='0')&&(b[0]<='9')){
                tempCommand = b[0] - '0';  // passei o char para int
                printf("%s",line);
                fputs("\n>>>\n",out);
                
                if (tempCommand>contComandos + contador-1){
                    // Se o nr do comando que demos for superior ao historico de comandos que temos, cancela o processo
                    printf("Nr. comando inválido. A cancelar...");exit(EXIT_FAILURE);
                    // TEMOS DE VOLTAR À VERSAO ORIGINAL DO NOTEBOOK NESTE SITIO.
                }
                else{
                    while (count==tempCommand){
                        read2 = getline(&line2, &len2, history);
                        if (read2 == -1) break;
                        printf("%s",line2);
                        count++;
                    }
                }
                fputs(line2,out);
                fputs("<<<\n\n",out);
                
            }
        }
		**/
        if (line) free (line);
        line = NULL;
        //if (line2) free (line2);
        //line2 = NULL;
    }

    
                   

    printf("Numero de comandos com pipes lidos: %d\n",contador-1);
    printf("Numero de comandos totais lidos: %d\nConcluído.\n",contComandos + contador-1);
    fclose(fp);
    fclose(result1);
    fclose(history);
    exit(EXIT_SUCCESS);
	
    
}