#include "Stack.h"

Stack::Stack()
    : stackPointer()
{
}

void
Stack::reset()
{
    stackPointer = 0;
    for(auto& s : stack) { s = 0; }
}

void
Stack::stackPush(const u16 address)
{
    if(stackPointer >= STACK_SIZE) [[unlikely]] {
        return; // スタックオーバー
    }
    stack[stackPointer++] = address;
}

const u16
Stack::stackPop()
{
    if(stackPointer==0) [[unlikely]] {
        return 0;   // スタックアンダー
    }
    return stack[--stackPointer];
}
