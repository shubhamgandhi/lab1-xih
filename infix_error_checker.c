//
//  infix_error_checker.c
//  factory
//
//  Created by Xi Han on 1/13/14.
//  Copyright (c) 2014 Xi Han. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "infix_error_checker.h"

void
infix_error_checker (struct expression ** exps, size_t array_len)
{
    size_t i, j;
    for (i = 0; i < array_len; i++) {
        size_t linenum = exps[i]->lineno;
        int parenthesis_balance = 0;
        for (j = 0; j < exps[i]->length; j++) {
            if (exps[i]->expr[j].type == OPERATOR) {
                if (strcmp(exps[i]->expr[j].content, "||")
                    && strcmp(exps[i]->expr[j].content, "&&")
                    && strcmp(exps[i]->expr[j].content, "|")
                    && strcmp(exps[i]->expr[j].content, ";")
                    && strcmp(exps[i]->expr[j].content, "(")
                    && strcmp(exps[i]->expr[j].content, ")")
                    && strcmp(exps[i]->expr[j].content, "<")
                    && strcmp(exps[i]->expr[j].content, ">"))
                {
                    fprintf(stderr, "%zu: unexpected operator.\n", linenum);
                    exit(1);
                }
                
                if (!strcmp(exps[i]->expr[j].content, ";")) {
                    if (j == exps[i]->length - 1 || !strcmp(exps[i]->expr[j + 1].content, ")")) {
                        exps[i]->expr[j].content[0] = ' ';
                    }
                    if (j == 0 || (exps[i]->expr[j - 1].type != OPERAND
					     && exps[i]->expr[j - 1].content[0] != ')')) {
                        fprintf(stderr, "%zu: no word before a semicolon.\n", linenum);
                        exit(1);
                    }
                } else if (!strcmp(exps[i]->expr[j].content, "(")) {
                    parenthesis_balance++;
                } else if (!strcmp(exps[i]->expr[j].content, ")")) {
                    parenthesis_balance--;
                    if (parenthesis_balance < 0) {
                        fprintf(stderr, "%zu: imbalanced parenthesis.\n", linenum);
                        exit(1);
                    }
                } else if (!strcmp(exps[i]->expr[j].content, ">")
                           || !strcmp(exps[i]->expr[j].content, "<"))
                {
                    if (j == 0 || (exps[i]->expr[j - 1].type != OPERAND
                                   && strcmp(exps[i]->expr[j - 1].content, ")"))) {
                        fprintf(stderr, "%zu: no program.\n", linenum);
                        exit(1);
                    }
                    
                    if (j == exps[i]->length - 1 || exps[i]->expr[j + 1].type != OPERAND) {
                        fprintf(stderr, "%zu: no operand for redirection.\n", linenum);
                        exit(1);
                    }
                    
                    if (!strcmp(exps[i]->expr[j].content, ">")) {
                        size_t k = 1;
                        while ((j + k) < exps[i]->length - 1
                               && exps[i]->expr[j+k].type != OPERATOR) {
                            k++;
                        }
                        if (!strcmp(exps[i]->expr[j+k].content, ">")
                            || !strcmp(exps[i]->expr[j+k].content, "<")) {
                            fprintf(stderr, "%zu: wrong syntax for redirection.\n", linenum);
                            exit(1);
                        }
                    } else {
                        size_t k = 1;
                        while ((j + k) < exps[i]->length - 1
                               && exps[i]->expr[j+k].type != OPERATOR) {
                            k++;
                        }
                        if (!strcmp(exps[i]->expr[j+k].content, "<")) {
                            fprintf(stderr, "%zu: wrong syntax for redirection.\n", linenum);
                            exit(1);
                        }
                    }
                    //                    if (j == 0)
                } else {
                    if (j == 0 || j == exps[i]->length - 1) {
                        fprintf(stderr, "%zu: misplaced operator.\n", linenum);
                        exit(1);
                    }
                    if (strcmp(exps[i]->expr[j - 1].content, ")")
                        && strcmp(exps[i]->expr[j + 1].content, "("))
                    {
                        if (exps[i]->expr[j - 1].type == OPERATOR
                            || exps[i]->expr[j + 1].type == OPERATOR)
                        {
                            fprintf(stderr, "%zu: misplaced operator.\n", linenum);
                            exit(1);
                        }
                    }
                }
            }
        }
        if (parenthesis_balance != 0) {
            fprintf(stderr, "%zu: imbalanced parenthesis.\n", linenum);
            exit(1);
        }
    }
}
