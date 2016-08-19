#include <stdio.h>
#include <assert.h>
#include "ast.h"
#include "xml.h"

static void output_tag (char *tag, int indent);
static int dump_xml_sub (struct AST *ast, int indent);

int
dump_xml (struct AST *ast)
{
    printf ("<?xml version=\"1.0\"?>\n");
    //printf ("<!DOCTYPE qhasm_btor SYSTEM \"qhasm_btor.dtd\">\n");
    dump_xml_sub (ast, 0);
}

static void
output_tag (char *tag, int indent)
{
    printf ("%*s%s\n", indent * 2, "", tag);
}

/*
  AST_right_im, AST_right_var
*/

const char* AST_type_str[] = {
  "AST_vdm_null", "AST_bool_true", "AST_bool_false", "AST_int", "AST_str", "AST_quote",
  "AST_record", "AST_tuple",
  "AST_set", "AST_set_empty",
  "AST_seq", "AST_seq_empty",
  "AST_map", "AST_map_empty",
  "AST_elem_list", "AST_elem",
  "AST_map_elem_list", "AST_map_elem",
};


static int
dump_xml_sub (struct AST *ast, int indent)
{
  int i;

  switch (ast->ast_type) {
    case AST_bool_true:
      printf ("%*s<%s>", indent*2, "", AST_type_str[ast->ast_type]);
      printf ("%s", "true");
      printf ("</%s>\n", AST_type_str[ast->ast_type]);
      break;
    case AST_bool_false:
      printf ("%*s<%s>", indent*2, "", AST_type_str[ast->ast_type]);
      printf ("%s", "false");
      printf ("</%s>\n", AST_type_str[ast->ast_type]);
      break;
    case AST_int:
      printf ("%*s<%s>", indent*2, "", AST_type_str[ast->ast_type]);
      printf ("%d", ast->u.num);
      printf ("</%s>\n", AST_type_str[ast->ast_type]);
      break;
    case AST_str: case AST_quote:
      printf ("%*s<%s>", indent*2, "", AST_type_str[ast->ast_type]);
      printf ("%s", ast->u.name);
      printf ("</%s>\n", AST_type_str[ast->ast_type]);
      break;
    default:
      if (ast->num_child==0) {
        printf ("%*s<%s>", indent*2, "", AST_type_str[ast->ast_type]);
        printf ("</%s>\n", AST_type_str[ast->ast_type]);
      } else {
        printf ("%*s<%s>\n", indent*2, "", AST_type_str[ast->ast_type]);
        for (i=0; i<ast->num_child; i++)
          dump_xml_sub (ast->u.child [i], indent + 1);
        printf ("%*s</%s>\n", indent*2, "", AST_type_str[ast->ast_type]);
      }
	  break;
    }
}
