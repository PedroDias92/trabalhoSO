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
	//pid_t p;
    int filefd = open("resul.txt", O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, 0666);
	int filefd2 = open("resul2.txt", O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, 0666);
	
    int final = fopen("final.txt","w+");

	int i=0;
	FILE * fp; 
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int contador=0;
	//Declaração dos pipes
	int childbuff;
	int fd[2];
	pipe(fd);
	int p=fork();
    
    char *dollar="$ ";
    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
		//printf("%s\n",line);
        //fputs(line,newfile);
		if(strncmp(dollar,line,strlen(dollar))==0){
			char *b =  trim(line + 3);
			//printf("%s\n",b);
            contador++;
            comands[i++]=b;
            
		}
    }
	

	
	if(p==0){
		char foo[1024];
		close(fd[0]);
		dup2(fd[1],1); //STDOUT_FILENO
		strcat(comands[0]," | tee resul.txt");
		execl("/bin/sh", "/bin/sh", "-c", comands[0], NULL);
	}
	else{
		close(fd[1]);
		dup2(fd[0],0); //STDERR_FILENO
		strcat(comands[1]," | tee resul2.txt");
		execl("/bin/sh", "/bin/sh", "-c", comands[1], NULL);
	}
	
    int flag=0;
	while ((read = getline(&line, &len, fp)) != -1) {
		fputs(line,final);
		//if(flag==1){
		//	int result = fopen("result.txt","wr+");
		//	while ((read = getline(&line, &len, result)) != -1) {
		//		fputs(line,final);
		//	flag=0;
		//	}
		//}
		if(strncmp(dollar,line,strlen(dollar))==0){
			flag=1;
		}
    }
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
	
	
	
	
}


/**
 * Tentar fazer um ciclo a partir do codigo em baixo. e depois executar um o array comands
 * 
 * 
 if(contador==3)
	{
		pid = fork();

		if(pid == 0)
		{
			close(pipefd1[0]);
			dup2(pipefd1[1], 1);
			execvp(arrayStrings2[0], arrayStrings2);
			exit(0);
		}
		close(pipefd1[1]);

		pid=fork();
		if(pid == 0)
		{
			close(pipefd1[1]);
			close(pipefd2[0]);
			dup2(pipefd1[0],0);
			dup2(pipefd2[1],1);
			execvp(arrayStrings3[0], arrayStrings3);
			exit(0);
		}
		close(pipefd2[1]);
		close(pipefd1[0]);

		pid = fork();
		if(pid==0)
		{
			close(pipefd2[1]);
			dup2(pipefd2[0],0);
			close(out[0]);
			dup2(out[1], 1);
			execvp(arrayStrings4[0], arrayStrings4);
			exit(0);
		}
		else
		{
			char foo[1024];
			close(out[1]);
			read(out[0], foo, sizeof(foo));
			printf("%s\n", foo);
			strcpy(tarefas[idx].output, foo);
			memset(foo, '\0', 1024);
			wait(0);
		}
	}

	//Encadeamento caso sejam dois comandos
	if(contador==2)
	{
		pid = fork();
		if(pid==0)
		{
			close(pipefd1[0]);
			dup2(pipefd1[1], 1);
			execvp(arrayStrings2[0], arrayStrings2);
			exit(0);
		}
		close(pipefd1[1]);

		pid=fork();
		if(pid==0)
		{
			close(pipefd1[1]);
			dup2(pipefd1[0],0);
			close(out[0]);
			dup2(out[1], 1);
			execvp(arrayStrings3[0], arrayStrings3);
			exit(0);
		}
		else
		{
			close(pipefd1[0]);
			char foo[1024];
			close(out[1]);
			read(out[0], foo, sizeof(foo));
			printf("%s\n", foo);
			strcpy(tarefas[idx].output, foo);
			memset(foo, '\0', 1024);
			wait(0);
		}
	}

	//Execução caso seja só um comando
	if(contador==1)
	{
		pid=fork();
		if(pid==0)
		{
			close(out[0]);
			dup2(out[1], 1);
			execvp(arrayStrings2[0], arrayStrings2);
			exit(0);
		}
		else
		{
			char foo[1024];
			close(out[1]);
			read(out[0], foo, sizeof(foo));
			printf("%s\n", foo);
			strcpy(tarefas[idx].output, foo);
			memset(foo, '\0', 1024);			
			wait(0);
		}
	}

    **/