
#ifndef __binary_expression_tree_generator__
#define __binary_expression_tree_generator__

#include "command.h"
#include "structs.h"
#include <stddef.h>

command_stream_t
generate_expression_tree (struct expression ** postfix_expr, size_t array_len);

#endif
