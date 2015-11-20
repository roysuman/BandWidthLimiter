#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<stdbool.h>
#include "safe_malloc.h"
#include "rb_tree_helper.h"


typedef struct red_black_node_struct {
  void* key;
  void* info;
  int red; /* if red=0 then the node is black */
  struct red_black_node_struct* left;
  struct red_black_node_struct* right;
  struct red_black_node_struct* parent;
  bool visited:1;
} red_black_node_struct;


/* Compare(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
typedef struct red_black_tree_struct {
  int (*compare)(const void* a, const void* b); 
  void (*destroy_key)(void* a);
  void (*destroy_info)(void* a);
  void (*print_key)(const void* a);
  void (*print_info)(void* a);
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when rb_tree_create is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  red_black_node_struct* root;             
  red_black_node_struct* nil;              
  red_black_node_struct* current_traverse_point;              
} red_black_tree_struct;

red_black_tree_struct* rb_tree_create(int  (*compFunc)(const void*, const void*),
			     void (*dest_func)(void*), 
			     void (*info_dest_func)(void*), 
			     void (*print_func)(const void*),
			     void (*print_info)(void*));
red_black_node_struct * rb_tree_insert(red_black_tree_struct*, void* key, void* info);
void rb_tree_print(red_black_tree_struct*);
void rb_delete(red_black_tree_struct* , red_black_node_struct* );
void rb_tree_destroy(red_black_tree_struct*);
red_black_node_struct* tree_predecessor(red_black_tree_struct*,red_black_node_struct*);
red_black_node_struct* tree_successor(red_black_tree_struct*,red_black_node_struct*);
red_black_node_struct* rb_exact_query(red_black_tree_struct*, void*);
//stk_stack * rbEnumerate(red_black_tree_struct* tree,void* low, void* high);
void null_function(void*);
void * start ( red_black_tree_struct*);
//void * end( red_black_tree_struct*);
void * next ( red_black_tree_struct*);
#endif
