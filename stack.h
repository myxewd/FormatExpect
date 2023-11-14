#pragma once
#ifndef _stack_h
#define _stack_h

#define MAX_SIZE nestDepth
struct Stack {
    int arr[MAX_SIZE];
    int top;
};

void initialize(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);
int peek(Stack* stack);

int cs_create(void);
void cs_push(int* stack, int data);
int cs_pop(int* stack);
int cs_peek(int stack);
void cs_free(int* stack);


#endif