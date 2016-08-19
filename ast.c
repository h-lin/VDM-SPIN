#include <stdio.h>
#include <stdarg.h>  /* for variable-length args */
#include <stdlib.h>  /* for malloc */
#include <string.h>  /* for strcpy */
#include "ast.h"

struct AST *
create_AST (AST_type ast_type, int argp_len, ...)
{
  va_list     argp;
  int         i;
  struct AST *ast;
  char    	 *var;

  ast = malloc (sizeof (struct AST));
  ast->ast_type = ast_type;

  va_start (argp, argp_len);
  switch (ast_type) {
  case AST_int:
    ast->num_child = 0;
	ast->u.num = va_arg (argp, int);
	break;
  case AST_str: case AST_quote:
    ast->num_child = 0;
    var = va_arg (argp, char *);
    ast->u.name = malloc (strlen (var) + 1);
    strcpy (ast->u.name, var);
    break;
  default:
    ast->num_child = argp_len;
	if (argp_len==0) break;  /* no child */
  	ast->u.child = malloc (sizeof(struct AST *) * argp_len);
  	for (i=0; i<argp_len; i++) {
      struct AST *child = va_arg (argp, struct AST *);
	  ast->u.child [i] = child;
	  if (child != NULL) {
        child->parent = ast;
        child->nth    = i;
      }
    }
    break;
  }
  va_end (argp);

  return ast;
}

int
free_AST(struct AST* ast)
{
  int i;

  if (ast->num_child>0)
    for (i=0; i<ast->num_child; i++)
      free_AST(ast->u.child[i]);
  switch (ast->ast_type) {
    case AST_str: case AST_quote:
      free(ast->u.name);
      break;
    default:
      break;
  }
  free(ast);


  return 1;
}
