#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"
#include "stack.h"

void
init_stack (stack *s, size_t ele_size)
{
    s->elemsize = ele_size;
    s->len = 0;
    s->max_len = STACK_INIT_SIZE;
    s->elements = checked_malloc(STACK_INIT_SIZE * ele_size);
}

void
destroy_stack (stack *s)
{
    free(s->elements);
}

int is_empty (stack *s)
{
    return s->len == 0;
}

void
push_stack (stack *s, const void *src)
{
    void *dst;
    if (s->len == s->max_len) {
        size_t temp = s->max_len * s->elemsize;
        s->elements = checked_grow_alloc(s->elements, &temp);
        s->max_len = temp / s->elemsize;
    }
    
    dst = (char *)s->elements + s->len * s->elemsize;
    memcpy(dst, src, s->elemsize);
    s->len++;
}

void
pop_stack (stack *s, void *dst)
{
    const void *src;
    s->len--;
    src = (const char *)s->elements + s->len * s->elemsize;
    memcpy(dst, src, s->elemsize);
}

void
stack_top (stack *s, void *dst)
{
    const void *src;
    s->len--;
    src = (const char *)s->elements + s->len * s->elemsize;
    memcpy(dst, src, s->elemsize);
    s->len++;
}