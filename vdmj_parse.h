#include <stdio.h>
#include <stdlib.h> /* for atoi */
#include <string.h> /* for strlen, memcpy */
#include "ast.h"
#include "xml.h"
#define YYSTYPE  struct AST*

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

extern int yyparse (void);
extern char *yytext;

static int yyerror (char *mesg);
struct AST *ast_root;
char *argv1;

int parse_flag=0;

YY_BUFFER_STATE buffer;
extern const char* AST_type_str[];
