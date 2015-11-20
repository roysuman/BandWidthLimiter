
#include "red_black_tree.h"

red_black_tree_struct* 
rb_tree_create( int (*compFunc) (const void*,const void*),
		void (*dest_func) (void*),
		void (*info_dest_func) (void*),
		void (*print_func) (const void*),
		void (*print_info)(void*)) {

	red_black_tree_struct* new_tree;
	red_black_node_struct* temp;
	
	new_tree=(red_black_tree_struct*) safe_malloc(sizeof(red_black_tree_struct));
	new_tree->compare=  compFunc;
	new_tree->destroy_key= dest_func;
	new_tree->print_key= print_func;
	new_tree->print_info= print_info;
	new_tree->destroy_info= info_dest_func;

	temp=new_tree->nil= (red_black_node_struct*) safe_malloc(sizeof(red_black_node_struct));
	temp->parent=temp->left=temp->right=temp;
	temp->red=0;
	temp->key=0;
	temp=new_tree->root= (red_black_node_struct*) safe_malloc(sizeof(red_black_node_struct));
	temp->parent=temp->left=temp->right=new_tree->nil;
	temp->key=0;
	temp->red=0;
	return(new_tree);
}


static void 
left_rotate(red_black_tree_struct* tree, 
	   red_black_node_struct* x) {
	red_black_node_struct* y;
	red_black_node_struct* nil=tree->nil;
	y=x->right;
	x->right=y->left;

	if (y->left != nil) y->left->parent=x; 
	y->parent=x->parent;   
	if( x == x->parent->left) {
		x->parent->left=y;
	}else {
		x->parent->right=y;
	}
	y->left=x;
	x->parent=y;
	return;
}



static void 
right_rotate(red_black_tree_struct* tree, 
		red_black_node_struct* y) {
	red_black_node_struct* x;
	red_black_node_struct* nil = tree->nil;

	x=y->left;
	y->left = x->right;
	if ( nil != x->right )
		x->right->parent=y; 
	x->parent=y->parent;
	if( y == y->parent->left) {
		y->parent->left=x;
	} else {
		y->parent->right=x;
	}
	x->right=y;
	y->parent=x;
	return;
}


static void 
tree_insert_help(red_black_tree_struct* tree, 
		red_black_node_struct* z) {
	red_black_node_struct* x;
	red_black_node_struct* y;
	red_black_node_struct* nil=tree->nil;

	z->left=z->right=nil;
	y=tree->root;
	x=tree->root->left;
	while( x != nil) {
		y=x;
		if (1 == tree->compare(x->key,z->key)) { 
			x=x->left;
		}else { 
			x=x->right;
		}
	}
	z->parent=y;
	if ( (y == tree->root) ||
			(1 == tree->compare(y->key,z->key))) { 
		y->left=z;
	} else {
		y->right=z;
	}
	return;
}


red_black_node_struct * rb_tree_insert(red_black_tree_struct* tree, void* key, void* info) {
  red_black_node_struct * y;
  red_black_node_struct * x;
  red_black_node_struct * newNode;

  x=(red_black_node_struct*) safe_malloc(sizeof(red_black_node_struct));
  x->key=key;
  x->info=info;

  tree_insert_help(tree,x);
  newNode=x;
  x->red=1;
  while(x->parent->red) { 
    if (x->parent == x->parent->parent->left) {
      y=x->parent->parent->right;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->right) {
	  x=x->parent;
	  left_rotate(tree,x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	right_rotate(tree,x->parent->parent);
      } 
    } else { 
      y=x->parent->parent->left;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->left) {
	  x=x->parent;
	  right_rotate(tree,x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	left_rotate(tree,x->parent->parent);
      } 
    }
  }
  tree->root->left->red=0;
  return(newNode);
}

  
red_black_node_struct* tree_successor(red_black_tree_struct* tree,red_black_node_struct* x) { 
  red_black_node_struct* y;
  red_black_node_struct* nil=tree->nil;
  red_black_node_struct* root=tree->root;

  if (nil != (y = x->right)) { 
    while(y->left != nil) { 
      y=y->left;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->right) { 
      x=y;
      y=y->parent;
    }
    if (y == root) return(nil);
    return(y);
  }
}


red_black_node_struct* tree_predecessor(red_black_tree_struct* tree, red_black_node_struct* x) {
  red_black_node_struct* y;
  red_black_node_struct* nil=tree->nil;
  red_black_node_struct* root=tree->root;

  if (nil != (y = x->left)) { 
    while(y->right != nil) { 
      y=y->right;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->left) { 
      if (y == root) return(nil); 
      x=y;
      y=y->parent;
    }
    return(y);
  }
}


static void inorder_tree_print(red_black_tree_struct* tree, red_black_node_struct* x) {
  red_black_node_struct* nil=tree->nil;
  red_black_node_struct* root=tree->root;
  if (x != tree->nil) {
    inorder_tree_print(tree,x->left);
    printf("info=");
    tree->print_info(x->info);
    printf("  key="); 
    tree->print_key(x->key);
    printf("  l->key=");
    if( x->left == nil) printf("NULL"); else tree->print_key(x->left->key);
    printf("  r->key=");
    if( x->right == nil) printf("NULL"); else tree->print_key(x->right->key);
    printf("  p->key=");
    if( x->parent == root) printf("NULL"); else tree->print_key(x->parent->key);
    printf("  red=%i\n",x->red);
    inorder_tree_print(tree,x->right);
  }
}

//get the starting position of rb
 
void*
start( red_black_tree_struct *tree){
#ifdef DEBUG
	fprintf(stdout,"RB Tree search start\n");
#endif
	tree->current_traverse_point = tree->root->left;
	while ( tree->current_traverse_point->left != tree->nil ){
		tree->current_traverse_point = tree->current_traverse_point->left;
	}
	if ( tree->current_traverse_point == tree->nil )
		return NULL;
	tree->current_traverse_point->visited = true;
	return tree->current_traverse_point;
}
/* 
void*
end( red_black_tree_struct *tree){
	red_black_node_struct *end_ = tree->root->left;
	red_black_node_struct* nil=tree->nil;
	if ( tree->root->left == tree->nil )
		return NULL;
	while ( end_->right != nil ){
		end_ = end_->right;
	}
	return end_;
}
*/

//TODO change the visited flag
//this function will work fine if it  getting called only once
void*
next( red_black_tree_struct *tree ){
	red_black_node_struct* nil=tree->nil;
	red_black_node_struct* root=tree->root;
	if ( tree->current_traverse_point->right != nil && !tree->current_traverse_point->right->visited ){
		tree->current_traverse_point = tree->current_traverse_point->right;
		if ( tree->current_traverse_point->left != nil ){
			while ( tree->current_traverse_point->left != nil ){
				tree->current_traverse_point = tree->current_traverse_point->left;
			}
		}
		tree->current_traverse_point->visited = true;
	}else if ( tree->current_traverse_point->visited ){
		while ( tree->current_traverse_point->visited ){
			if ( tree->current_traverse_point->parent == root )
				return NULL;
			tree->current_traverse_point = tree->current_traverse_point->parent;
		}
//		tree->current_traverse_point->visited = true;
	}
#ifdef DEBUG
	fprintf(stdout,"\nRB TREE NEXT TRAVERSE Next [ %d ]\n",*(int*)tree->current_traverse_point->key);
#endif
	tree->current_traverse_point->visited = true;
	return tree->current_traverse_point;
}

		

static void 
tree_dest_helper(red_black_tree_struct* tree, 
		red_black_node_struct* x) {

	red_black_node_struct* nil=tree->nil;
	if (x != nil) {
		tree_dest_helper(tree,x->left);
		tree_dest_helper(tree,x->right);
		tree->destroy_key(x->key);
		tree->destroy_info(x->info);
		free(x);
	}
	return;
}


void 
rb_tree_destroy(red_black_tree_struct* tree) {
	tree_dest_helper(tree,tree->root->left);
	free(tree->root);
	free(tree->nil);
	free(tree);
	return;
}



void 
rb_tree_print(red_black_tree_struct* tree) {
	inorder_tree_print(tree,tree->root->left);
	return;
}


  
red_black_node_struct* 
rb_exact_query(red_black_tree_struct* tree, 
		void* q) {

	if ( tree == NULL ) return NULL;
	red_black_node_struct* x=tree->root->left;
	red_black_node_struct* nil=tree->nil;
	int comp_val;

	if (x == nil) return(0);
	comp_val=tree->compare(x->key,(int*) q);
	while(0 != comp_val) {
		if (1 == comp_val) {
			x=x->left;
		} else {
			x=x->right;
		}
		if ( x == nil) return(0);
		comp_val=tree->compare(x->key,(int*) q);
	}
	return(x);
}


static void 
rb_delete_fix_up(red_black_tree_struct* tree, 
		red_black_node_struct* x) {

	red_black_node_struct* root=tree->root->left;
	red_black_node_struct* w;

	while( (!x->red) && (root != x)) {
		if (x == x->parent->left) {
			w=x->parent->right;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				left_rotate(tree,x->parent);
				w=x->parent->right;
			}
			if ( (!w->right->red) && (!w->left->red) ) { 
				w->red=1;
				x=x->parent;
			}else {
				if (!w->right->red) {
					w->left->red=0;
					w->red=1;
					right_rotate(tree,w);
					w=x->parent->right;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->right->red=0;
				left_rotate(tree,x->parent);
				x=root;
			}
		} else { 
			w=x->parent->left;
			if (w->red) {
				w->red=0;
				x->parent->red=1;
				right_rotate(tree,x->parent);
				w=x->parent->left;
			}
			if ( (!w->right->red) && (!w->left->red) ) { 
				w->red=1;
				x=x->parent;
			} else {
				if (!w->left->red) {
					w->right->red=0;
					w->red=1;
					left_rotate(tree,w);
					w=x->parent->left;
				}
				w->red=x->parent->red;
				x->parent->red=0;
				w->left->red=0;
				right_rotate(tree,x->parent);
				x=root;
			}
		}
	}
	x->red=0;
	return;
}



void 
rb_delete(red_black_tree_struct* tree, 
		red_black_node_struct* z){

	red_black_node_struct* y;
	red_black_node_struct* x;
	red_black_node_struct* nil=tree->nil;
	red_black_node_struct* root=tree->root;

	y= ((z->left == nil) || (z->right == nil)) ? z : tree_successor(tree,z);
	x= (y->left == nil) ? y->right : y->left;
	if (root == (x->parent = y->parent)) { 
		root->left=x;
	} else {
		if (y == y->parent->left) {
			y->parent->left=x;
		} else {
			y->parent->right=x;
		}
	}
	if (y != z) { 
		if (!(y->red)) rb_delete_fix_up(tree,x);
		tree->destroy_key(z->key);
		tree->destroy_info(z->info);
		y->left=z->left;
		y->right=z->right;
		y->parent=z->parent;
		y->red=z->red;
		z->left->parent=z->right->parent=y;
		if (z == z->parent->left) {
			z->parent->left=y; 
		} else {
			z->parent->right=y;
		}
		free(z); 
	} else {
		tree->destroy_key(y->key);
		tree->destroy_info(y->info);
		if (!(y->red)) rb_delete_fix_up(tree,x);
		free(y);
	}
	return;
}

