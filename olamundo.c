#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int main(){
	char *comands[2];
	comands[0]="ls";
	comands[1]="head -1";
	int filefd = open("resul.txt", O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, 0666);
	int filefd2 = open("resul2.txt", O_WRONLY|O_CREAT|O_APPEND|O_TRUNC, 0666);
	int childbuff;
	int fd[2];
	pipe(fd);
	int p=fork();
	
	int n;
	
	if(p==0){
		char foo[1024];
		close(fd[0]);
		dup2(fd[1],1); //STDOUT_FILENO
		freopen("resul.txt", "a+", fd[1]); 
		execl("/bin/sh", "/bin/sh", "-c", "ls | tee resul.txt", NULL);
	}
	else{
		close(fd[1]);
		dup2(fd[0],0); //STDERR_FILENO
		execl("/bin/sh", "/bin/sh", "-c", "head -1 | tee resul2.txt", NULL);
	}
	
	return 0;
}