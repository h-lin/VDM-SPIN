#ifndef __AST_H__
#define __AST_H__

typedef enum _AST_type {
  AST_vdm_null, AST_bool_true, AST_bool_false, AST_int, AST_str, AST_quote,
  AST_record, AST_tuple,
  AST_set, AST_set_empty,
  AST_seq, AST_seq_empty,
  AST_map, AST_map_empty,
  AST_elem_list, AST_elem,
  AST_map_elem_list, AST_map_elem
} AST_type;

struct AST {
  AST_type        ast_type;  /* AST type of this node */
  struct AST     *parent;    /* back pointer for parent. NULL for root */
  int	       	  nth;       /* this AST is the nth child of the parent */
  int             num_child; /* num of children */
  union {
    struct AST  **child;     /* array of pointers to children */
    char         *name;      /* name of this node */
	int           num;       /* integer constant */
  } u;
};

struct AST *create_AST ( AST_type ast_type, int argp_len, ...);
int free_AST ( struct AST *ast);

#endif /* __AST_H__ */
