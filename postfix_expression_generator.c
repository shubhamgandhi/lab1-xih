#include <string.h>
#include "structs.h"
#include "alloc.h"
#include "stack.h"
#include <stdlib.h>
#include "postfix_expression_generator.h"

int
get_precedence (const char *op)
{
    if (!strcmp(op, ";")) {
        return 1;
    } else if (!strcmp(op, "&&")
               ||!strcmp(op, "||"))
    {
        return 2;
    } else if (!strcmp(op, "|")) {
        return 3;
    } else if (!strcmp(op, "<")
               ||!strcmp(op, ">"))
    {
        return 4;
    }
    
    return 0;
}

struct expression **
generate_postfix_expr (struct expression ** infix_expr, size_t array_len)
{
    int i, j;
    struct expression ** postfix_expr = checked_malloc(array_len * sizeof(struct expression *));
    for (i = 0; i < (int) array_len; i++) {
        stack *s = checked_malloc(sizeof(stack));
        init_stack(s, sizeof(struct expr_element));
        struct expr_element * working_postfix_expression = malloc(infix_expr[i]->length * sizeof(struct expr_element));
        size_t postfix_expr_idx = 0;
        for (j = 0; j < (int) infix_expr[i]->length; j++) {
            if (infix_expr[i]->expr[j].type == OPERAND) {
                working_postfix_expression[postfix_expr_idx] = infix_expr[i]->expr[j];
                postfix_expr_idx++;
            } else if (!strcmp(infix_expr[i]->expr[j].content, " ")) {
                continue;
            } else if (!strcmp(infix_expr[i]->expr[j].content, "(")) {
                push_stack(s, &infix_expr[i]->expr[j]);
            } else if (!strcmp(infix_expr[i]->expr[j].content, ")")) {
                struct expr_element temp_ele;
                stack_top(s, &temp_ele);
                while (strcmp(temp_ele.content, "(")) {
                    pop_stack(s, &working_postfix_expression[postfix_expr_idx]);
                    postfix_expr_idx++;
                    stack_top(s, &temp_ele);
                }
                memcpy(&working_postfix_expression[postfix_expr_idx], &infix_expr[i]->expr[j], sizeof(struct expr_element));
                postfix_expr_idx++;
                pop_stack(s, &temp_ele);
            } else {
                struct expr_element temp_ele;
                stack_top(s, &temp_ele);
                while (!is_empty(s) && strcmp(temp_ele.content, "(")
                       && get_precedence(infix_expr[i]->expr[j].content)
                       <= get_precedence(temp_ele.content)) {
                    pop_stack(s, &working_postfix_expression[postfix_expr_idx]);
                    postfix_expr_idx++;
                    stack_top(s, &temp_ele);
                }
                push_stack(s, &infix_expr[i]->expr[j]);
            }
        }
        
        while (!is_empty(s)) {
            pop_stack(s, &working_postfix_expression[postfix_expr_idx]);
            postfix_expr_idx++;
        }
        
        postfix_expr[i] = checked_malloc(sizeof(struct expression));
        postfix_expr[i]->expr = working_postfix_expression;
        postfix_expr[i]->length = postfix_expr_idx;
        postfix_expr[i]->lineno = infix_expr[i]->lineno;
        postfix_expr[i]->maxlen = infix_expr[i]->maxlen;
        
        destroy_stack(s);
        free(s);
    }
    return postfix_expr;
}

