#ifndef LIST_H
#define LIST_H
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)
typedef struct list_head_ list_head;
struct list_head_{
	list_head *next;
	list_head *prev;
};
#define INIT_LIST_HEAD(list_ptr)\
	do{\
		(list_ptr)->next = ( list_ptr);\
		(list_ptr)->prev = (list_ptr );\
	}while(0)
static inline void 
__add_list__( list_head *new,
	     list_head *prev,
	     list_head *next){

	next->prev= new;
	new->next = next;
	new->prev= prev;
	prev->next = new;
	return;
}

static inline void 
list_add_tail( list_head *new,
	       list_head *head){

	__add_list__(new,head->prev,head);
	return;
}	

#define list_for_each(pos,head)\
	for ( pos = (head)->next; pos != head ; pos =(pos)->next)

#define list_for_each_safe(pos, n, head) \
        for (pos = (head)->next, n = pos->next; pos != (head); \
                pos = n, n = pos->next)
#define get_member_struct(ptr,type,struct_member)({	\
		const typeof( ( (type *)0)->struct_member) *__mptr = (ptr);	\
		(type *)( (char*)__mptr - (( (size_t) &((type*)0)->struct_member) ));})

static inline void 
__delete_list__( list_head *next, 
		list_head *prev ){


	prev->next = next;
	next->prev = prev;
	return;
}

static inline void 
delete_list( list_head *entry){
	
	__delete_list__(entry->next , entry->prev);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
	return;
}
#endif
