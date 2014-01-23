// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include <stddef.h>
#include <error.h>
#include "structs.h"
#include "command.h"
#include "infix_expression_generator.h"
#include "infix_error_checker.h"
#include "postfix_expression_generator.h"
#include "binary_expression_tree_generator.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  //error (1, 0, "command reading not yet implemented");
    struct expression ** infix_exp;
    struct expression ** postfix_exp;
    size_t len;
    infix_exp = generate_infix_expr(get_next_byte, get_next_byte_argument, &len);
    infix_error_checker(infix_exp, len);
    postfix_exp = generate_postfix_expr(infix_exp, len);
    return generate_expression_tree(postfix_exp, len);
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
    if (s->cmdind != s->length) {
        return s->commands[s->cmdind++];
    } else {
        return NULL;
    }
}
