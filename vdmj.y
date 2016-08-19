%token DIGIT STR QUOTE RECORD EOL TRUE FALSE MK vdm_err
/*
%left '+' '-'
%left '*' '.'
%left '<' '>'
*/
%{
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

%}

%%

root
  : vdm_type  { ast_root = $1; }
  ;

vdm_type
  : '(' ')'             { $$ = create_AST (AST_vdm_null, 0); }
  | TRUE                { $$ = create_AST (AST_bool_true, 0); }
  | FALSE               { $$ = create_AST (AST_bool_false, 0); }
  | DIGIT               { $$ = create_AST (AST_int, 1, atoi (yytext)); }
  | STR                 { $$ = create_AST (AST_str, 1, yytext); }
  | QUOTE               { $$ = create_AST (AST_quote, 1, yytext); }
  | RECORD '(' vdm_elem_list ')'  { $$ = create_AST (AST_record, 1, $3); }
  | MK '(' vdm_elem_list ')'       { $$ = create_AST (AST_tuple, 1, $3); }
  | '{' vdm_elem_list '}'          { $$ = create_AST (AST_set, 1, $2); }
  | '{' '}'                        { $$ = create_AST (AST_set_empty, 0); }
  | '[' vdm_elem_list ']'          { $$ = create_AST (AST_seq, 1, $2); }
  | '[' ']'                        { $$ = create_AST (AST_seq_empty, 0); }
  | '{' vdm_map_elem_list '}'      { $$ = create_AST (AST_map, 1, $2); }
  | '{' '|' '-' '>' '}'            { $$ = create_AST (AST_map_empty, 0); }
  ;

vdm_elem_list
  : vdm_type ',' vdm_elem_list   { $$ = create_AST (AST_elem_list, 2, $1, $3); }
  | vdm_type                     { $$ = create_AST (AST_elem, 1, $1); }
  ;

vdm_map_elem_list
  : vdm_type '|' '-' '>' vdm_type ',' vdm_map_elem_list   { $$ = create_AST (AST_map_elem_list, 3, $1, $5, $7); }
  | vdm_type '|' '-' '>' vdm_type                         { $$ = create_AST (AST_map_elem, 2, $1, $5); }
  ;

%%

static int yyerror (char *mesg) {
  //printf (";error! line: %d\n", _line);
  fprintf (stderr, "yyerror: %s\n", mesg);
  return 1;
}


/*
int main (int argc, char *argv [])
{
  argv1 = argv [1];
  extern FILE* yyin;
  char tmp_str[128];
  int nbytes=127;
  YY_BUFFER_STATE buffer;

  if (argc==2) { // filename specified
    yyin = fopen(argv1, "r");
    if (yyin == 0) {    // make sure file is successfully opened
      printf("ERROR: file open failed\n");
      exit(0);
    }
    yyparse ();
    fclose(yyin);

    dump_xml(ast_root); // dump for test
  }
  if (argc==1) { // no filename
    while(1) {
      memset(tmp_str, '\0', sizeof(tmp_str));
      printf("Enter a string for parsing...(\"exit\" to terminate)\n> ");
      fgets(tmp_str, nbytes, stdin);
      if (strncmp(tmp_str,"exit", 4) == 0) {
        printf("terminating...\n");
        break;
      }
      tmp_str[strlen(tmp_str)-1] = '\0';
      buffer = yy_scan_string(tmp_str);
      yyparse();
      yy_delete_buffer(buffer);
      dump_xml(ast_root); // dump for test
      free_AST(ast_root);
    }
  }

  return 1;
}
*/
