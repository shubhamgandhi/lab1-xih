//
//  structs.h
//  TimeTravelShell
//
//  Created by Xi Han on 1/12/14.
//  Copyright (c) 2014 Xi Han. All rights reserved.
//

#ifndef TimeTravelShell_structs_h
#define TimeTravelShell_structs_h
#include "command.h"
enum element_type
{
    OPERATOR,
    OPERAND
};

struct expr_element
{
    char *content;
    enum element_type type;
    
};

struct expression
{
    struct expr_element *expr;
    size_t lineno;
    size_t length;
    size_t maxlen;
};

struct command_stream
{
    command_t *commands;
    size_t length;
    size_t maxlen;
    size_t cmdind;
};

#endif
