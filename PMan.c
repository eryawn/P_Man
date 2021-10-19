#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include "List.h"

void bg_entry(char **argv, struct List **plist);
void bglist_entry(struct List* plist);
void bgsig_entry(int pid, int type, struct List* plist);
void check_zombieProcess(struct List **plist);
void pstat_entry(int pid);

int main(){
	struct List *plist = NULL;

	while(1){
        
		char* cmd = readline("PMan: > ");
		check_zombieProcess(&plist);

		/* parse the input cmd (e.g., with strtok)
		 */
		const char* s = " ";
		char* token;
		char* token_list[100];
		int count = 0;
		int pid = 0;		
		token = strtok(cmd, s);
			
		while (token != NULL) {		
			token = strtok(NULL, s);
			token_list[count] = token;
			count++;
		}
		if (token_list[0] != NULL) {
			pid = atoi(token_list[0]);
		}		
        
        //bg entry
		if (strcmp(cmd, "bg") == 0){			
			if (token_list[0] != NULL){
				bg_entry(token_list, &plist);
			}
			else {
				printf("Invalid command.\n");
			}			
		}
        //bglist entry
		else if(strcmp(cmd, "bglist") == 0){
			bglist_entry(plist);
		}
        //bgkill, stop, start entry
		else if(strcmp(cmd, "bgkill") == 0 || strcmp(cmd, "bgstop") == 0 || strcmp(cmd, "bgstart") == 0){
			if (pid > 0) {
				if (strcmp(cmd, "bgkill") == 0) {
					bgsig_entry(pid, 0, plist);
				}
				else if (strcmp(cmd, "bgstop") == 0) {
					bgsig_entry(pid, 1, plist);
				}
				else if (strcmp(cmd, "bgstart") == 0) {
					bgsig_entry(pid, 2, plist);
				}
				else {
					printf("Invalid pid.\n");
				}
			}
			else {
				printf("Invalid pid.\n");
			}
		}
        //pstat entry
		else if(strcmp(cmd, "pstat") == 0){
			if (pid > 0) {
				pstat_entry(pid);
			}
			else {
				printf("Invalid pid.\n");
			}
		}
		else {
			printf("Invalid command.\n");
		}
	}

	return 0;
}

void bg_entry(char **argv, struct List **plist){
	pid_t pid;
	char *temp = (char *) malloc(strlen(&argv[0][1]));
	strcpy(temp, &argv[0][1]);
	pid = fork();
	
    //background procss
	if(pid == 0){
		if(execvp(argv[0], argv) < 0){
			perror("Error on execvp");
		}   
		exit(EXIT_SUCCESS);	
	}
	//store information of the background child process in data structures
	else if(pid > 0) {
		if (*plist == NULL) {
			*plist = ListInit(pid, temp);
            usleep(200000);
			return;
		}
		struct Node* node = NodeInit(pid, temp);
		AddNode(*plist, node);
        usleep(200000);
	}
	else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}

void bglist_entry(struct List* plist) {
	ApplyList(plist);
}

void bgsig_entry(int pid, int type, struct List* plist) {
    int retVal = 0;
    if (pid > 0) {
		//kill
		if (type == 0) {
			retVal = kill((pid_t) pid, SIGTERM);
        	sleep(1);
			if (retVal == -1) {
				printf("kill failed.\n");
				//exit(EXIT_FAILURE);
			}
		}
		//stop
		else if (type == 1) {
			retVal = kill((pid_t) pid, SIGSTOP);
        	sleep(1);
			if (retVal == -1) {
            	printf("stop failed.\n");
				//exit(EXIT_FAILURE);
			}
		}
		//start
		else if (type == 2) {
			retVal = kill((pid_t) pid, SIGCONT);
        	sleep(1);
			if (retVal == -1) {
				printf("start failed.\n");
				//exit(EXIT_FAILURE);
			}
		}
    }
    else {
        printf("Invalid pid.\n");
    }
}

void check_zombieProcess(struct List** plist){
	int status;
	int retVal = 0;
	struct Node* current = NULL;
	struct Node* temp = NULL;
	if (*plist == NULL) {
		return;
	}

	//check every process in list
	current = (*plist) -> head;
	while(current != NULL) {
		temp = current -> next;
		retVal = waitpid(current -> pid, &status, WNOHANG);	
		if(retVal > 0) {
			//remove the background process from data structure
			RemoveNode(current, plist);
		}
		else if(retVal == 0){}
		else{
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
		current = temp;
	}
	return ;
}

void pstat_entry(int pid){
	//extract stats from /proc/%d/stat
	char file[100]; 
	sprintf(file, "/proc/%d/stat", pid);
    FILE *handle = fopen(file, "r");
		
	if(handle == NULL){
		printf("Process %d does not exist. \n", pid);
		return;
	}

	char state;
	float utime, stime;
	unsigned int flags;
    char comm[100], buffer[1000];
	unsigned long long int starttime, vsize; 
	unsigned long int minflt, cminflt, mafflt, cmajflt;	
	int unused, ppid ,pgroup, session, tty_nr, tpgid, vol, nonvol;
    long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss;
		
	fscanf(handle, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %f %f %ld %ld %ld %ld %ld %ld %llu %llu %ld",
        &unused,comm,&state,&ppid,&pgroup,&session,&tty_nr,&tpgid,&flags,
		&minflt,&cminflt,&mafflt,&cmajflt,&utime,&stime,&cutime,&cstime,
		&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss);
        
    utime /= sysconf(_SC_CLK_TCK);
    stime /= sysconf(_SC_CLK_TCK);
	                
    //extract stats from /proc/%d/status
	char file1[100];
	sprintf(file1, "/proc/%d/status", pid);
	FILE *handle1 = fopen(file1, "r");

	while(fscanf(handle1, "%s", buffer) != EOF){
        if(strcmp(buffer, "voluntary_ctxt_switches:") == 0){
            fscanf(handle1, "%d", &vol);
	    }else if(strcmp(buffer, "nonvoluntary_ctxt_switches:") == 0){
            fscanf(handle1, "%d", &nonvol);
	    }
	 }
	
    printf("comm: %s\n", comm);
	printf("state: %c\n", state);
	printf("utime: %f seconds\n",utime);
	printf("stime: %f seconds\n",stime);
	printf("rss: %ld\n",rss);
    printf("voluntary_ctxt_switches: %d\n", vol);
    printf("nonvoluntary_ctxt_switches: %d\n", nonvol);
	fclose(handle);
    fclose(handle1);
}
