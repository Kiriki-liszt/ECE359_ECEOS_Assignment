/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

void strip_down( char *cmmd, char **args, int *halt );
int execute( char **args, int halt );

int main(void) {

	char *args[MAX_LINE/2 + 1] = {'\0', };	/* command line (of 80) has max of 40 arguments */
	char history[5][MAX_LINE] = {'\0', };
	char cmmd[MAX_LINE] = {'\0', };
	char orig[MAX_LINE] = {'\0', };
	pid_t pid;
	int should_run = 1;	
	int halt = 0;
	int i;

	/********************************************************/
	/* After reading user input, the steps are:		*/
	/* (1) fork a child process				*/
	/* (2) the child process will invoke execvp()		*/
	/* (3) if command included &, parent will invoke wait()	*/
	/********************************************************/

		
   	while (should_run) {

		printf("osh>");
		fflush(stdout);

		halt = 0;
		for ( i = 0 ; i < MAX_LINE/2 + 1 ; i++ ) args[i] = NULL;
		for ( i = 0 ; i < MAX_LINE ; i++ ) { cmmd[i] = '\0'; orig[i] = '\0'; }

		fgets(cmmd, sizeof(cmmd), stdin);
		i = 0; while ( cmmd[i] != '\n' ) { i++; } cmmd[i]='\0';
		strcpy(orig, cmmd);
		strip_down( cmmd, args, &halt );

		if ( !args[0] ) printf("No command entered\n");
			
		else if ( !strcmp(args[0], "") ) printf("No command entered\n");

		else if ( !strcmp(args[0], "exit") && !args[1] ) {
			printf("Exiting now... goodbye\n");
			break;
		}

		else if ( !strcmp(args[0], "history") && !args[1] ) {
			if(!strcmp(history[0],"")) printf("Empty\n");
			else { for ( i = 5 ; i > 0 ; i-- )
				printf("%d - %s \n", i, history[i-1]);
			}
		}

		else if ( args[0][0]=='!' && args[0][2]=='\0' && !args[1] ) {
			if ( args[0][1]=='!' ) {
				printf("Execute from most recent history\n");
				strcpy( cmmd, history[0] );
				strcpy( orig, cmmd);
				strip_down( cmmd, args, &halt );
				if( args[0][0] == '\0' ) { printf("Nothing from history!\n"); continue; }
				printf("Executing : %s\n", orig);
				if ( execute( args, halt ) ) return 0;
				for ( i = 4 ; i > 0 ; i-- ) strcpy(history[i],history[i-1]); 
				strcpy(history[0], orig);
			}
			else if ( args[0][1] > '0' && args[0][1] < '6' ) {
				printf("Execute from history - %d\n", args[0][1] - '0');
				strcpy( cmmd, history[args[0][1] - '1'] );
				strcpy( orig, cmmd);
				strip_down( cmmd, args, &halt );
				if( args[0][0] == '\0' ) { printf("Nothing from history\n"); continue; }
				printf("Executing : %s\n", orig);
				if ( execute( args, halt ) ) return 0;
				for ( i = 4 ; i > 0 ; i-- ) strcpy(history[i],history[i-1]); 
				strcpy(history[0], orig);
			}
			else printf("False history command\n\n");
		}
		
		else {
			if ( execute( args, halt ) ) return 0;
			for ( i = 4 ; i > 0 ; i-- ) strcpy(history[i],history[i-1]); 
			strcpy(history[0], orig);
		}

	}
	return 0;
}



void strip_down( char *cmmd, char **args, int *halt ) {

	int c=0, a=0;

	while ( cmmd[c] != '\0' ) {
		while (cmmd[c] == ' ' || cmmd[c] == '\t') { cmmd[c] = '\0'; c++; }
		args[a] = &cmmd[c];
		while (cmmd[c] != ' ' && cmmd[c] != '\t' && cmmd[c] != '\0' && cmmd[c] != '\n') c++;
		if (cmmd[c] == '\n') cmmd[c] = '\0';
		if (args[a][0] == '&' && args[a][1] == '\0' ) { *halt = 1; a--;}
		a++;
	}
	cmmd[c] = '\0';
	args[a] = '\0';
	return;
}

int execute( char **args, int halt ) {
	pid_t pid = fork();

	if(pid < 0) {
		printf("Error while fork\n");
		return 1;
	}
	else if (pid == 0) {
		if ( execvp(args[0], args) ) {
                printf("Error while execvp\n");
		return 1;
		}		
	}	
	else { 
		if(halt==0) {
			printf("  Child now runs in Fore-Ground\n");
			while(wait(NULL) != pid);
			printf("  Fore-Ground ended\n\n");
		}
		else printf("  Child now runs in Back-Ground\n\n");
	}
	return 0;
}

