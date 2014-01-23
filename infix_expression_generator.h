/** MODULE: Convert a script to a infix expression
    Author: Han, Xi
    Date: Jan. 12, 2014
 */

#ifndef __infix_expression_generator_h__
#define __infix_expression_generator_h__
#define INIT_LENGTH 2
#include "structs.h"

// IN: script stream
// OUT: infix expressions representation of the script
struct expression **
generate_infix_expr (int (*get_next_byte) (void *),
                     void *get_next_byte_argument,
                     size_t *array_len);

#endif