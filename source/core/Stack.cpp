#include "Stack.h"

void
Stack::reset()
{
    stackPointer = 0;
    for(auto& s : stack) { s = 0; }
}

void
Stack::stackPush(const u16 address)
{
    stack[stackPointer++] = address;
}

const u16
Stack::stackPop()
{
    return stack[--stackPointer];
}
