#include <stdio.h>

#include "stack.h"




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