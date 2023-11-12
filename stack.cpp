#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stack.h"

/*
extern int malocc;
extern void* mallocEX(size_t size);
extern void freeEX(void* ptr);
#define malloc(size) mallocEX(size)
#define free(size) freeEX(size)
*/

void initialize(Stack* stack) {
    stack->top = -1;
}

void push(Stack* stack, int value) {
    if (stack->top < MAX_SIZE - 1) {
        stack->arr[++stack->top] = value;
    }
    else {
        //error
        return;
    }
}

int pop(Stack* stack) {
    if (stack->top >= 0) {
        return stack->arr[stack->top--];
    }
    else {
        return -1;
    }
}

int peek(Stack* stack) {
    if (stack->top >= 0) {
        return stack->arr[stack->top];
    }
    else {
        return -1;
    }
}

//ChainStack

typedef struct Node {
    int data;
    struct Node* next;
} Node;

int cs_create(void) {
    Node* stack = (Node*)malloc(sizeof(Node));
    stack->data = 0;
    stack->next = NULL;
    return (int)stack;
}

void cs_push(int* stack, int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = (Node*)*stack;
    *stack = (int)newNode;
}

int cs_pop(int* stack) {
    if (*stack == 0)
        return -1;
    Node* topNode = (Node*)*stack;
    int ret = topNode->data;
    *stack = (int)topNode->next;
    free(topNode);
    return ret;
}

int cs_peek(int stack) {
    if (stack == 0)
        return -1;
    Node* topNode = (Node*)stack;
    return topNode->data ? topNode->data: -1;
}

void cs_free(int stack) {
    Node* cur = (Node*)stack;
    Node* temp;
    while (cur != NULL) {
        temp = cur->next;
        free(cur);
        cur = temp;
    }
}