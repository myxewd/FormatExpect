#pragma once
#ifndef _stack_h
#define _stack_h

#define MAX_SIZE 200
struct Stack {
    int arr[MAX_SIZE];
    int top;
};

void initialize(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);
int peek(Stack* stack);
#endif