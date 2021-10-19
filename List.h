#ifndef List_H
#define List_H

#define MAX_INT_STR_LENGTH 21

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Node {
    int pid;
    char *comm;
    struct Node *prev, *next;
};

struct List
{
    struct Node *head, *end;
    unsigned int size;
};

struct Node *NodeInit(int pid, char *comm);
struct List* ListInit(int pid, char *comm);
void AddNode(struct List *plist, struct Node* node);
void RemoveNode(struct Node* node, struct List **plist);
void ApplyList(struct List *plist);
void ApplyNode(struct Node *node);

#endif