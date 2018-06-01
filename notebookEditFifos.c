/**
 * $ gcc notebook.c -o notebook
 * $ ./notebook comandos.txt
Passos do programa:
1- Cria uma nova directoria "tmp" (temporaria)
2- Cria um ficheiro "out.txt" , e por cada linha lida do ficheiro de input (neste caso "comandos.txt") copia a linha para o ficheiro "out.txt".
3- Por cada linha (comando) começado $ ou $| ou $n| executa o comando atraves um "exec" (usando de um processo filho) cria um ficheiro com o nome generico resultN.txt (em que N é o numero do comando) e
 copia o resultado deste ficheiro para o "out.txt" acrescentadndo >>> e <<< no inicio e fim . 
 Todos os ficheiros do tipo "resultN.txt" encontran-se na directoria "tmp"
4- No fim caso não ocorra nenhum erro na execução dos comandos ou o utilizador não termine o programa (CTR-C) 
o ficheiro de input é substituido pelo "out.txt"
5- todo o conteudo da directoria "tmp" é eliminado
**/


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
#include <dirent.h> //para eliminar os conteudos de "/tmp"
#define MAX_BUF 1024



char filename[50];
char fifoName[50]; // 31 Maio


//Para apanhar o CTRL-C
static volatile int running = 1;
void handler(int dummy){
    running = 0;
}

void strcatFifo(int contador){ // 31 Maio
    char append[50]; 
    char contadorString[5];
    strcpy (fifoName, "tmp/result");
    sprintf(contadorString,"%d",contador);
    strcat(fifoName, contadorString);
}

void strcatFilename(int contador){
    char append[50];
    char contadorString[5];
    strcpy (filename, "tmp/result");
    strcpy (append, ".txt");
    sprintf(contadorString,"%d",contador); // int to string
    strcat(filename, contadorString);
    strcat(filename,append);
}

//Remove os ficheiros da directoria "tmp"
void removeFilesFromTmp(char *folder){
    // These are data types defined in the "dirent" header
        DIR *theFolder = opendir(folder);
        struct dirent *next_file;
        char filepath[257];

        while ( (next_file = readdir(theFolder)) != NULL )
        {
            // build the path for each file in the folder
            sprintf(filepath, "%s/%s", folder, next_file->d_name);
            remove(filepath);
        }
        closedir(theFolder);
}

//Separa os comandos dos $'s
char * trim(char * s) {
    int l = strlen(s);
    while(isspace(s[l - 1])) --l;
    while(* s && isspace(* s)) ++s, --l;
    return strndup(s, l);
}

//Caso algum processo filho de erro
int forkError(int status,char *b){
    int result=1;
    if(WIFEXITED(status))
                {
                    if(WEXITSTATUS(status) != 0)
                    {
                        printf("ERRO no comando %s\n",b);
                        printf("fork: %s\n", strerror(errno));
                        result=0;
                    }
                }
                else if (WIFSIGNALED(status))
                {
                    printf("terminated because it didn't catch signal number %d\n", WTERMSIG(status));
                    result=0;
                }
                else
                {
                    printf("ERRO no comando %s\n",b);
                    printf("fork: %s\n", strerror(errno));
                    result=0;
                }

    return result;
}


//Elimina todo o conteudo entre >>> e <<< do ficheiro de input
void re_processamento(char * file){
    char * line = NULL;
    size_t bufsize = 32;
    size_t len=0;
    ssize_t readL=2;
    FILE * fp; 
    fp = fopen(file, "r");
    FILE *REDO;
    REDO = fopen("REDO.txt", "wr+");
    
    
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((readL = getline(&line, &len, fp)) != -1){
        if(strncmp(">>>\n",line,strlen(">>>\n"))==0){
            readL = getline(&line, &len, fp);
            while(strncmp("<<<\n",line,strlen("<<<\n"))!=0)
                readL = getline(&line, &len, fp);
        }
        if(strncmp("<<<\n",line,strlen("<<<\n"))==0){
            readL = getline(&line, &len, fp);
        }
        //printf("%s",line);
        fputs(line,REDO);
        
        
    }
    fclose(fp);
    fclose(REDO);
    rename("REDO.txt",file);

    if (line) free (line);
        line = NULL;
}

int main(int argc, char ** argv){
    signal(SIGINT, handler);  //CTRL-C
	char *REDO = argv[1];
    re_processamento(REDO);   //Elimina todo o conteudo entre >>> e <<< do ficheiro de input

    //cria a pasta /tmp/ com os resultsN
    struct stat st = {0};

    if (stat("tmp", &st) == -1) {
        mkdir("tmp", 0700);
    }

    FILE * fp;
    FILE * result1;
    FILE * result2;

    
	regex_t regex;
    int reti;
    int descritorFifo[2];
    char *comands[10];
    char * line = NULL;
    char * line2 = NULL;
    size_t bufsize = 32;
    size_t len,len2= 0;
    ssize_t readL,readL2; // 31 Maio : TAVA A FAZER CONFLITO COM READS
    char buf[MAX_BUF]; // 31 Maio
    fp = fopen(argv[1], "r");
    char *dollar="$ ";
    char *dollarPipe="$| ";
    
    int flagErrorFork=1;
    
    FILE *out;
    out = fopen("tmp/out.txt", "wr+");
    int contador=0,contComandos=0;

    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((readL = getline(&line, &len, fp)) != -1 && flagErrorFork==1 && (running)) {
        fputs(line,out);

        //---------------------------------------------COMANDO SIMPLES--------------------------------------------
        //-------------------------------------------------------------------------------------------------------
        if(strncmp(dollar,line,strlen(dollar))==0 && (running)){
			contador++;
			contComandos++;
			char *b =  trim(line + 2);
			printf("comando nº: %d \t %s\n",contador,b);

            strcatFilename(contador);

            // 31 Maio
            strcatFifo(contador);
            int fifoCheck = 0;
            fifoCheck = mkfifo (fifoName,0666);
            if (fifoCheck == 1) {
                perror("Erro na criação do FIFO");
                exit(-1);
            } 
            // Cria direito

            int p=fork();
            if(p==0){
                descritorFifo[1] = open(fifoName,O_RDONLY); // Parece que demora tempo a fazer isto
				dup2(descritorFifo[1],1);
                close(descritorFifo[1]);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
                exit(-1);
                // já reparei que ele consegue escrever no fifo após isto.
            }   
            // O problema tá aqui!
            else{
                int status;
                wait(&status); //Não sei se é por causa disto
                
                flagErrorFork=forkError(status,b);
                if( flagErrorFork == 0){ 
                    break;
                }
                printf("->passou pelo flagErrorFork\n"); // O wait de lá de cima parece atrasar isto.
                descritorFifo[0] = open(fifoName,O_RDONLY); // ISTO Não tá a funcionar! Não faço ideia pq
                printf("->passou pelo open\n");
                
                //FILE *resultFifo = fdopen(descritorFifo[0], "w"); // Isto era a testar se depois podia fazer getlines.

                fputs(">>>\n",out);

                while (read(descritorFifo[0], &buf, sizeof(buf)) > 0){
                    fputs(buf,out);
                    fputs("\0",out);
                } 
                
                printf("passou pelo while\n");

                //write(descritorFifo, argv[1], strlen(argv[1]));
                fputs("<<<\n",out);  
                printf("Lido pelo fifo %s\n", fifoName);
                close(descritorFifo[0]);
            }
            
        }
        
        //---------------------------------------------------------------COMANDO COM PIPE----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------
        if(strncmp(dollarPipe,line,strlen(dollarPipe))==0 && (running)){     
            char *b =  trim(line + 3);
            contador++;
			printf("comando nº: %d \t %s\n",contador,b);


            strcatFilename(contador);            
            FILE * resultN;
            resultN =fopen(filename,"wr+"); //abre o resultN.txt
            int d=fork();
            if(d==0){
				// ABRIR RESULTN.TXT
               	strcatFilename(contador-1);
            	
                result1 =fopen(filename,"r");
                dup2(fileno(result1),0);//STDIN_FILENO
                dup2(fileno(resultN),1); //STDOUT_FILENO
                fclose(result1);
                fclose(resultN);
                execl("/bin/sh", "/bin/sh", "-c", b, NULL);
                exit(-1);
            }
            else{
                //wait(0);
                int status;
                wait(&status); 
                
                flagErrorFork=forkError(status,b);
                if( flagErrorFork == 0){ // 0 deu erro no fork
                    break;
                }

                fclose(result1);
                fputs(">>>\n",out);
                FILE * result2;
                result2 =fopen(filename,"r");
                //escrever para out.txt
                while ((readL2 = getline(&line2, &len2, result2)) != -1){
                        fputs(line2,out);
                    } 
                fputs("<<<\n",out);  
            }


        
        }
        //sleep(2);
		
        //---------------------------------------------------------------NUMERO DE COMANDO ( $n| )-------------------------------------------------------
        /* Compile regular expression */
        reti = regcomp(&regex, "$[1-9][0-9]*|", 0);
        if( reti ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }

		/* Execute regular expression */
        reti = regexec(&regex, line, 0, NULL, 0);
        if( !reti && (running)){
                //printf("\n Match %s \n",line);

				char *b =  trim(line + 4);
				int number = atoi(&line[1]);
				contador++;
				printf("comando nº: %d \t %s\t com STDIN do comando %d\n",contador,b,number);
				
				strcatFilename(contador);
				// ABRIR RESULTN.TXT		Para colocar o STDOUT
				
				FILE * resultN;
				resultN =fopen(filename,"wr+"); //abre o resultN.txt
				int d=fork();
				if(d==0){
                    strcatFilename(number); // A CHAVE ESTA AQUI! vai buscar o N de "$N|"  int number = atoi(&line[1]);
					result1 =fopen(filename,"r");
					dup2(fileno(result1),0);//STDIN_FILENO
					dup2(fileno(resultN),1); //STDOUT_FILENO
					fclose(result1);
					fclose(resultN);
					execl("/bin/sh", "/bin/sh", "-c", b, NULL);
                    exit(-1);
				}
				else{
					//wait(0);
                    int status;
                    wait(&status); 	
                    
                    flagErrorFork=forkError(status,b);
                    if( flagErrorFork == 0){ // 0 deu erro no fork
                        break;
                    }
					fclose(result1);
					fputs(">>>\n",out);
					FILE * result2;
					result2 =fopen(filename,"r");
					//escrever para out.txt
					while ((readL2 = getline(&line2, &len2, result2)) != -1){
							fputs(line2,out);
						} 
					fputs("<<<\n",out);
					//result1 =fopen("result1.txt","w"); // ELIMINA CONTEUDOS DO FICHEIRO PARA EXECUTAR PROXIMO COMANDO  
				}

        }


        if (line) free (line);
        line = NULL;
    }
                  
    fclose(fp);
    fclose(result1);
	fputs("\n",out);	// se não meter isto depois no re-processamento algo bate mal !!!
	//sleep(2);
    if(flagErrorFork==1 && (running)){   //se flag == 1 e não houve um CTR-C ,então tudo correu bem
        rename("tmp/out.txt",argv[1]);
        //removeFilesFromTmp("tmp");
        
    }   
    else{                   //senão elimina todo o conteudo da pasta tmp é apagado
        removeFilesFromTmp("tmp");
        
    }
    exit(EXIT_SUCCESS);
	
    
}
