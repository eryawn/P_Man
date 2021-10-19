#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>

struct Node *NodeInit(int pid, char *comm) {
    if(comm == NULL) {
        comm = "(null!)";
    }
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node -> pid = pid;
    node -> comm = comm;
    node -> next = NULL;
    node -> prev = NULL;
    return node;
}
struct List* ListInit(int pid, char *comm) {
    if(comm == NULL) {
        comm = "(null!)";
    }
    struct Node* node = NodeInit(pid, comm);
    struct List* list = (struct List*) malloc(sizeof(struct List));
    list -> head = node;
    list -> end = node;
    list -> size = 1;
    return list;
}

void AddNode(struct List *plist, struct Node* node) {
    if(plist == NULL || node == NULL) {
        return;
    }
    plist -> end -> next = node;
    node -> prev = plist -> end;
    plist -> end = node;
    plist -> size += 1;
}

void ApplyNode(struct Node *node) {
    if (node == NULL) {
        printf("Node empty.\n"); 
        return;
    }

    //access the process folder
    int pid = node -> pid;
    ssize_t len, dlen = 128;
    char* addr = malloc(dlen);
    char path[MAX_INT_STR_LENGTH];
    sprintf(path, "/proc/%d/cwd", pid);

    if (fopen(path, "r") == NULL) {
        printf("%d: %s\n", pid, node->comm);
        return;
    }
    while((len = readlink(path, addr, dlen)) == dlen) {
        dlen *= 2;
        addr = realloc(addr, dlen);
    }
    addr[len] = '\0';

    printf("%d:    %s", pid, (char *) addr);
    printf("%s\n", node->comm);
    free(addr);
}

void ApplyList(struct List *plist){
    if(plist == NULL) {
        printf("Total background jobs: 0.\n");
        return;
    }
    struct Node* node = plist -> head;    
    while (node != NULL) {
        ApplyNode(node);
        node = node -> next;
    }
    printf("Total background jobs: %u.\n", plist -> size);
}

void RemoveNode(struct Node *node, struct List **plist) {
    if (node == NULL || *plist == NULL) {
        return;
    }

    struct List *l_temp = *plist;
    l_temp -> size -= 1;
    if(l_temp -> size == 0) {
       *plist = NULL;
        free(node);
        free(*plist);
        return;
    }
    //head
    if(l_temp -> head -> pid == node -> pid) {
        l_temp -> head = node -> next;
        node -> next -> prev = NULL;
        free(node);
        return;
    }
    //end
    else if(l_temp -> end -> pid == node -> pid) {
        l_temp -> end = node -> prev;
        node -> prev -> next = NULL;
        free(node);
        return;
    }

    node -> prev -> next = node -> next;
    node -> next -> prev = node -> prev;
    free(node);
}



