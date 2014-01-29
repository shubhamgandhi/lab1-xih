#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alloc.h"
#include "infix_expression_generator.h"
#include "structs.h"

static char *trim(char *str) {
    
    char *string = malloc(strlen(str) + 1);
    strcpy(string, str);
    while (*string == '\t' || *string == ' ') {
        string++;
    }
    if (*string == 0) {
        return string;
    }
    char *s;
    for (s = string; *s != 0; s++) {}
    s--;
    while (*s == ' ' || *s == '\t') {
        s--;
    }
    s++;
    *s = 0;
    return string;
    
}

struct expression **
generate_infix_expr (int (*get_next_byte) (void *),
                     void *get_next_byte_argument,
                     size_t *array_length)
{
    enum status {
        NEWLINE,
        PARSING_OPERAND,
        PARSING_OPERATOR,
        PARSING_PARENTHESIS,
        IGNORING_COMMENTS,
        PARSING_SPACES
    };
    
    struct expression **expressions = checked_malloc(sizeof(struct expression *) * INIT_LENGTH); // expression array
    size_t expressions_maxlen = INIT_LENGTH; // expression array max length
    size_t expressions_len = 0; // expression array current length
    size_t working_line_num = 1;
    char next_byte = get_next_byte(get_next_byte_argument);
    char prev_byte = '\n';
    enum status st = NEWLINE; // parsing status
    enum status prev_st;
    char *str_ptr;
    size_t str_len;
    size_t str_maxlen;
    struct expression *expr_ptr = checked_malloc(sizeof(struct expression));
    expr_ptr->expr = checked_malloc(sizeof(struct expr_element) * INIT_LENGTH);
    expr_ptr->lineno = working_line_num;
    expr_ptr->length = 0;
    expr_ptr->maxlen = INIT_LENGTH;
    int in_subshell = 0;
    
    while (next_byte != EOF) {
    START:
        if (next_byte == '\n') {
            if (in_subshell > 0) {
                next_byte = ' ';
                working_line_num++;
                goto START;
            }
            
            if (st == NEWLINE || st == PARSING_OPERATOR || st == PARSING_SPACES) {
                working_line_num++;
            } else if (st == PARSING_OPERAND) {
                st = NEWLINE;
                prev_st = PARSING_OPERAND;
                
                expr_ptr->expr[expr_ptr->length].type = OPERAND;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->lineno=working_line_num;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                expressions[expressions_len] = expr_ptr;
                expressions_len++;
                if (expressions_len == expressions_maxlen) {
                    size_t temp = expressions_maxlen * sizeof(struct expression *);
                    expressions = checked_grow_alloc(expressions, &temp);
                    expressions_maxlen = temp / sizeof(struct expression *);
                }
                working_line_num++;
                expr_ptr = checked_malloc(sizeof(struct expression));
                expr_ptr->expr = checked_malloc(sizeof(struct expr_element) * INIT_LENGTH);
                expr_ptr->lineno = working_line_num;
                expr_ptr->length = 0;
                expr_ptr->maxlen = INIT_LENGTH;
            } else if (st == PARSING_PARENTHESIS) {
                st = NEWLINE;
                prev_st = PARSING_PARENTHESIS;
                
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->lineno=working_line_num;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                expressions[expressions_len] = expr_ptr;
                expressions_len++;
                if (expressions_len == expressions_maxlen) {
                    size_t temp = expressions_maxlen * sizeof(struct expression *);
                    expressions = checked_grow_alloc(expressions, &temp);
                    expressions_maxlen = temp / sizeof(struct expression *);
                }
                working_line_num++;
                expr_ptr = checked_malloc(sizeof(struct expression));
                expr_ptr->expr = checked_malloc(sizeof(struct expr_element) * INIT_LENGTH);
                expr_ptr->lineno = working_line_num;
                expr_ptr->length = 0;
                expr_ptr->maxlen = INIT_LENGTH;
            } else {
                st = prev_st;
                prev_st = IGNORING_COMMENTS;
                working_line_num++;
            }
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if (st == IGNORING_COMMENTS) {
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if (next_byte == ' '
            || next_byte == '\t')
        {
            if (st == NEWLINE || st == PARSING_SPACES) {
                
            } else if (st == PARSING_OPERATOR) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                prev_st = st;
                st = PARSING_SPACES;
                
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERAND) {
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_PARENTHESIS) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                prev_st = st;
                st = PARSING_SPACES;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            }
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if ((next_byte >= 'A' && next_byte <= 'Z')
            || (next_byte >= 'a' && next_byte <= 'z')
            || (next_byte >= '0' && next_byte <= '9')
            || next_byte == '!'
            || next_byte == '%'
            || next_byte == '+'
            || next_byte == ','
            || next_byte == '-'
            || next_byte == '_'
            || next_byte == '.'
            || next_byte == '/'
            || next_byte == ':'
            || next_byte == '@'
            || next_byte == '^')
        {
            if (st == NEWLINE) {
                prev_st = st;
                st = PARSING_OPERAND;
                
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERAND || st == PARSING_SPACES) {
                str_ptr[str_len] = next_byte;
                str_len++;
                prev_st = st;
                st = PARSING_OPERAND;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERATOR) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                prev_st = st;
                st = PARSING_OPERAND;
                
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            } else if (st == PARSING_PARENTHESIS) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                prev_st = st;
                st = PARSING_OPERAND;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            }
            
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if (next_byte == '|'
            || next_byte == '&'
            || next_byte == ';'
            || next_byte == '<'
            || next_byte == '>'
            )
        {
            if (st == NEWLINE) {
                prev_st = st;
                st = PARSING_OPERATOR;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERATOR) {
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERAND) {
                expr_ptr->expr[expr_ptr->length].type = OPERAND;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                prev_st = st;
                st = PARSING_OPERATOR;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            } else if (st == PARSING_PARENTHESIS) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                
                prev_st = st;
                st = PARSING_OPERATOR;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            } else if (st == PARSING_SPACES) {
                
                free(str_ptr);
                prev_st = st;
                st = PARSING_OPERATOR;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            }
            
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if (next_byte == '('
            || next_byte == ')') {
            
            if (next_byte == '(') {
                in_subshell++;
            } else {
                in_subshell--;
            }
            
            if (st == NEWLINE) {
                prev_st = st;
                st = PARSING_PARENTHESIS;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERATOR) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                prev_st = st;
                st = PARSING_PARENTHESIS;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            } else if (st == PARSING_OPERAND) {
                expr_ptr->expr[expr_ptr->length].type = OPERAND;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                prev_st = st;
                st = PARSING_PARENTHESIS;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            } else if (st == PARSING_PARENTHESIS) {
                expr_ptr->expr[expr_ptr->length].type = OPERATOR;
                str_ptr[str_len] = '\0';
                expr_ptr->expr[expr_ptr->length].content = str_ptr;
                expr_ptr->length++;
                if (expr_ptr->length == expr_ptr->maxlen) {
                    size_t temp = expr_ptr->maxlen * sizeof(struct expression);
                    expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
                    expr_ptr->maxlen = temp / sizeof(struct expression);
                }
                prev_st = st;
                st = PARSING_PARENTHESIS;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
                
            } else if (st == PARSING_SPACES) {
                
                free(str_ptr);
                prev_st = st;
                st = PARSING_PARENTHESIS;
                str_ptr = checked_malloc(sizeof(char) * INIT_LENGTH);
                str_len = 0;
                str_maxlen = INIT_LENGTH;
                str_ptr[str_len] = next_byte;
                str_len++;
                if (str_len == str_maxlen) { // model for checked grow realloc
                    size_t temp = str_maxlen * sizeof(char);
                    str_ptr = checked_grow_alloc(str_ptr, &temp);
                    str_maxlen = temp / sizeof(char);
                }
            }
            
            prev_byte = next_byte;
            next_byte = get_next_byte(get_next_byte_argument);
            continue;
        }
        
        if (next_byte == '#') {
            if (prev_byte == ' '
                || prev_byte == '\n'
                || prev_byte == '\t')
            {
                prev_st = st;
                st = IGNORING_COMMENTS;
                prev_byte = next_byte;
                next_byte = get_next_byte(get_next_byte_argument);
                continue;
            }
        }
        fprintf(stderr, "%zu: unexpected characters.\n", working_line_num);
        exit(1);
    }
    
    if (st == PARSING_OPERAND) {
        expr_ptr->expr[expr_ptr->length].type = OPERAND;
        str_ptr[str_len] = '\0';
        expr_ptr->expr[expr_ptr->length].content = str_ptr;
        expr_ptr->length++;
        if (expr_ptr->length == expr_ptr->maxlen) {
            size_t temp = expr_ptr->maxlen * sizeof(struct expression);
            expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
            expr_ptr->maxlen = temp / sizeof(struct expression);
        }
        expressions[expressions_len] = expr_ptr;
        expressions_len++;
        if (expressions_len == expressions_maxlen) {
            size_t temp = expressions_maxlen * sizeof(struct expression *);
            expressions = checked_grow_alloc(expressions, &temp);
            expressions_maxlen = temp / sizeof(struct expression *);
        }
    } else if (st == PARSING_OPERATOR || st == PARSING_PARENTHESIS) {
        expr_ptr->expr[expr_ptr->length].type = OPERATOR;
        str_ptr[str_len] = '\0';
        expr_ptr->expr[expr_ptr->length].content = str_ptr;
        expr_ptr->length++;
        if (expr_ptr->length == expr_ptr->maxlen) {
            size_t temp = expr_ptr->maxlen * sizeof(struct expression);
            expr_ptr->expr = checked_grow_alloc(expr_ptr->expr, &temp);
            expr_ptr->maxlen = temp / sizeof(struct expression);
        }
        expressions[expressions_len] = expr_ptr;
        expressions_len++;
        if (expressions_len == expressions_maxlen) {
            size_t temp = expressions_maxlen * sizeof(struct expression *);
            expressions = checked_grow_alloc(expressions, &temp);
            expressions_maxlen = temp / sizeof(struct expression *);
        }
    } else if (st == PARSING_SPACES) {
        expressions[expressions_len] = expr_ptr;
        expressions_len++;
        if (expressions_len == expressions_maxlen) {
            size_t temp = expressions_maxlen * sizeof(struct expression *);
            expressions = checked_grow_alloc(expressions, &temp);
            expressions_maxlen = temp / sizeof(struct expression *);
        }
    }
    
    size_t i, j;
    char *temp;
    for (i = 0; i < expressions_len; i++) {
        for (j = 0; j < expressions[i]->length; j++) {
            temp = checked_malloc(strlen(expressions[i]->expr[j].content) + 1);
            strcpy(temp, expressions[i]->expr[j].content);
            free(expressions[i]->expr[j].content);
            expressions[i]->expr[j].content = trim(temp);
        }
    }
    *array_length = expressions_len;
    return expressions;
}

/*
struct expression **
generate_infix_expr_v2 (int (*get_next_byte) (void *),
                     void *get_next_byte_argument,
                     size_t *array_length)
{
    char current_byte = get_next_byte(get_next_byte_argument); // Setup the current byte cursor.
    char next_byte = get_next_byte(get_next_byte_argument); // Setup the byte ahead.
    size_t line_number = 1; // Setup the line number indicator.
    
    // Setup the expressions array.
    struct expression ** expressions_array = checked_malloc(INIT_LENGTH * sizeof(struct expression *));
    size_t expression_array_length = 0;
    size_t expression_array_max_length = INIT_LENGTH;
    
    // Setup the string buffer and the expression structure.
    char *string_buffer = checked_malloc(INIT_LENGTH * sizeof(char));
    size_t string_buffer_length = 0;
    size_t string_buffer_max_length = INIT_LENGTH;
    
    struct expression *expression = checked_malloc(sizeof(struct expression));
    expression->expr = checked_malloc(INIT_LENGTH * sizeof(struct expr_element));
    expression->length = 0;
    expression->maxlen = INIT_LENGTH;
    expression->lineno = line_number;
    
    enum status {
        // TO-DO: add status enumeration.
        NEW_EXPRESSION,
        
    };
    // TO-DO: add parser code.
    enum status parsing_status = NEW_EXPRESSION;
    
    while (current_byte != EOF) {
        switch (parsing_status) {
            case NEW_EXPRESSION: {
                switch (current_byte) {
                    case ' ':
                    case '\t':
                    {
                        break; // Silently ingore the character.
                    }
                        
                    case '\n':
                    {
                        line_number++; // 
                    }
                        
                }
                break;
            }
            default:
                break;
        }
    }
    return expressions_array;
    
}
*/