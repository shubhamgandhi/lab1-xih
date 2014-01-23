
#ifndef CStack_stack_h
#define CStack_stack_h
#define STACK_INIT_SIZE 2
#include <stddef.h>

typedef struct
{
    void *elements;
    size_t elemsize;
    size_t max_len;
    size_t len;
} stack;

void init_stack (stack *s, size_t size);
void pop_stack (stack *s, void *dst);
void push_stack (stack *s,const void *src);
void destroy_stack (stack *s);
void stack_top (stack *s, void *dst);
int is_empty(stack *s);


#endif