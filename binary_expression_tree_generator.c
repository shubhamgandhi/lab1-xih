#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "command-internals.h"
#include "command.h"
#include "alloc.h"
#include "binary_expression_tree_generator.h"
#define INIT_LENGTH 2

int
append_element (command_t parent, command_t ele) {
    
    if (parent->type == SIMPLE_COMMAND) {
        return 0;
    }
    
    if (parent->type == SUBSHELL_COMMAND) {
        if (parent->u.subshell_command) {
            return append_element(parent->u.subshell_command, ele);
        } else {
            parent->u.subshell_command = ele;
        }
    }
    
    if (parent->u.command[1] == NULL) {
        parent->u.command[1] = ele;
        return 1;
    }
    
    int r = append_element(parent->u.command[1], ele);
    if (r == 1) {
        return r;
    }
    
    if (parent->u.command[0] == NULL) {
        parent->u.command[0] = ele;
        return 1;
    }
    
    return append_element(parent->u.command[0], ele);
}

command_stream_t
generate_expression_tree (struct expression ** postfix_expr, size_t array_len)
{
    command_stream_t stream = checked_malloc(sizeof(struct command_stream));
    command_t *commands = checked_malloc(INIT_LENGTH * sizeof(command_t));
    stream->commands = commands;
    stream->length = 0;
    stream->maxlen = INIT_LENGTH;
    stream->cmdind = 0;
    
    size_t i, k;
    for (i = 0; i < array_len; i++) {
        command_t root = NULL;
        int j;
        char **in_ptr = NULL;
        char **out_ptr = NULL;
        for (j = (int) postfix_expr[i]->length - 1 ; j >= 0; j--) {
            
            if (postfix_expr[i]->expr[j].type == OPERAND) {
                
                size_t *spaces_pos = checked_malloc(INIT_LENGTH * sizeof(size_t));
                size_t spaces_array_len = 0;
                size_t spaces_array_maxlen = INIT_LENGTH;
                char ** words = checked_malloc(INIT_LENGTH * sizeof(char *));
                size_t words_len = 0;
                size_t words_maxlen = INIT_LENGTH;
                k = 0;
                
                spaces_pos[spaces_array_len] = -1;
                spaces_array_len++;
                if (spaces_array_len == spaces_array_maxlen) {
                    size_t temp = spaces_array_maxlen * sizeof(size_t);
                    spaces_pos = checked_grow_alloc(spaces_pos, &temp);
                    spaces_array_maxlen = temp / sizeof(size_t);
                }
                
                while (postfix_expr[i]->expr[j].content[k] != '\0') {
                    if (postfix_expr[i]->expr[j].content[k] == ' '
                        || postfix_expr[i]->expr[j].content[k] == '\t')
                    {
                        spaces_pos[spaces_array_len] = k;
                        spaces_array_len++;
                        if (spaces_array_len == spaces_array_maxlen) {
                            size_t temp = spaces_array_maxlen * sizeof(size_t);
                            spaces_pos = checked_grow_alloc(spaces_pos, &temp);
                            spaces_array_maxlen = temp / sizeof(size_t);
                        }
                    }
                    k++;
                }
                
                spaces_pos[spaces_array_len] = k;
                spaces_array_len++;
                if (spaces_array_len == spaces_array_maxlen) {
                    size_t temp = spaces_array_maxlen * sizeof(size_t);
                    spaces_pos = checked_grow_alloc(spaces_pos, &temp);
                    spaces_array_maxlen = temp / sizeof(size_t);
                }
                
                for (k = 0; k < spaces_array_len - 1; k++) {
                    int this_space_pos = (int) spaces_pos[k];
                    int next_space_pos = (int) spaces_pos[k + 1];
                    size_t substr_length = next_space_pos - this_space_pos;
                    
                    if (substr_length > 1) {
                        char *word = checked_malloc(substr_length * sizeof(char));
                        memcpy(word, &postfix_expr[i]->expr[j].content[this_space_pos + 1], substr_length - 1);
                        word[substr_length] = '\0';
                        words[words_len] = word;
                        words_len++;
                        if (words_len == words_maxlen) {
                            size_t temp = words_maxlen * sizeof(char *);
                            words = checked_grow_alloc(words, &temp);
                            words_maxlen = temp / sizeof(char *);
                        }
                    }
                }
                
                command_t cmd = checked_malloc(sizeof(struct command));
                cmd->status = -1;
                cmd->type = SIMPLE_COMMAND;
                if (in_ptr) {
                    cmd->input = *in_ptr;
                } else {
                    cmd->input = 0;
                }
                
                if (out_ptr) {
                    cmd->output = *out_ptr;
                } else {
                    cmd->output = 0;
                }
                in_ptr = NULL;
                out_ptr = NULL;
                cmd->u.word = checked_malloc(sizeof(char *) * INIT_LENGTH);
                size_t u_word_len = 0;
                size_t u_word_maxlen = INIT_LENGTH;
                
                for (k = 0; k < words_len; k++) {
                    cmd->u.word[u_word_len] = words[k];
                    u_word_len++;
                    if (u_word_len == u_word_maxlen) {
                        size_t temp = u_word_maxlen * sizeof(char *);
                        cmd->u.word = checked_grow_alloc(cmd->u.word, &temp);
                        u_word_maxlen = temp / sizeof(char *);
                    }
                }
                
                cmd->u.word[u_word_len] = NULL;
                
                if (root == NULL) {
                    root = cmd;
                    continue;
                }
                
                append_element(root, cmd);
                
            } else {
                command_t cmd = checked_malloc(sizeof(struct command));
                cmd->status = -1;
                cmd->input = 0;
                cmd->output = 0;
                if (!strcmp(postfix_expr[i]->expr[j].content, "||")) {
                    cmd->type = OR_COMMAND;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, "&&")) {
                    cmd->type = AND_COMMAND;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, "|")) {
                    cmd->type = PIPE_COMMAND;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, ";")) {
                    cmd->type = SEQUENCE_COMMAND;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, "<")) {
                    in_ptr = &postfix_expr[i]->expr[j - 1].content;
                    j--;
                    continue;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, ">")) {
                    out_ptr = &postfix_expr[i]->expr[j - 1].content;
                    j--;
                    continue;
                } else if (!strcmp(postfix_expr[i]->expr[j].content, ")")) {
                    cmd->type = SUBSHELL_COMMAND;
                    cmd->u.subshell_command = NULL;
                    if (in_ptr) {
                        cmd->input = *in_ptr;
                    } else {
                        cmd->input = 0;
                    }
                    
                    if (out_ptr) {
                        cmd->output = *out_ptr;
                    } else {
                        cmd->output = 0;
                    }
                    
                    in_ptr = NULL;
                    out_ptr = NULL;
                    
                    if (root == NULL) {
                        root = cmd;
                        continue;
                    }
                    append_element(root, cmd);
                    continue;
                }
                
                cmd->u.command[0] = NULL;
                cmd->u.command[1] = NULL;
                if (root == NULL) {
                    root = cmd;
                    continue;
                }
                
                append_element(root, cmd);
            }
        }
        stream->commands[stream->length] = root;
        stream->length++;
        if (stream->length == stream->maxlen) {
            size_t temp = stream->maxlen * sizeof(command_t);
            stream->commands = checked_grow_alloc(stream->commands, &temp);
            stream->maxlen = temp / sizeof(command_t);
        }
    }
    return stream;
}