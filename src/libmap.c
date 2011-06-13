#ifndef _LIBMAP_C_
#define _LIBMAP_C_

#include <stdlib.h>
#include <stdio.h>
#include "libmap.h"


int libmap_create(libmap_map *m){
	//printf("create()\n");
	m->size=0;
	m->root=NULL;
	return 1;
}

int libmap_size(libmap_map *m){
	if(m==NULL)return 0;
	return m->size;
}


int _libmap_node_add(libmap_node *node,libmap_node *pair,int (*comparator)(const void *,const void *)){
	switch(comparator(pair->key,node->key)){
		case -1:
			if(node->left==NULL){
				//printf("adding to the left\n");
				node->left=pair;
				node->left->parent=node;
				return 1;
			}else{
				//printf("recursing to the left\n");
				return _libmap_node_add(node->left,pair,comparator);
			}
			break;
		case 1:
			if(node->right==NULL){
				//printf("adding to the right\n");
				node->right=pair;
				node->right->parent=node;
				return 1;
			}else{
				//printf("recursing to the right\n");
				return _libmap_node_add(node->right,pair,comparator);
			}
			break;
		default:
			//printf("duplicate key?\n");
			return 0;
	}
	return 0;
}




int libmap_add(libmap_map *m,void *key,void *value,int (*comparator)(const void *,const void *)){
	//printf("add()\n");
	if(m==NULL || key==NULL || comparator==NULL)return 0;

	libmap_node *temp=malloc(sizeof(libmap_node));
	temp->key=key;
	temp->value=value;
	temp->left=NULL;
	temp->right=NULL;
	temp->parent=NULL;

	if(m->root==NULL){
		//printf("adding to root\n");
		m->root=temp;
		m->size++;
		return 1;
	}

	int ret=_libmap_node_add(m->root,temp,comparator);
	if(ret==0){
		free(temp);
		temp=NULL;
	}else{
		m->size++;
	}
	return ret;
}


int _libmap_node_destroy(libmap_node **node){
	libmap_node *temp=*node;
	//printf("destroying (%s:%s)\n",temp->key,temp->value);
	if(temp->left!=NULL){
		//printf("going left on (%s)\n",temp->key);
		_libmap_node_destroy(&(temp->left));
//		free(temp->left);
		temp->left=NULL;
	}
	if(temp->right!=NULL){
		//printf("going right on (%s)\n",temp->key);
		_libmap_node_destroy(&(temp->right));
//		free(temp->right);
		temp->right=NULL;
	}


	if(temp->parent!=NULL){
		if(temp->parent->left==temp){
			temp->parent->left=NULL;
		}else if(temp->parent->right==temp){
			temp->parent->right=NULL;
		}
	}
	temp->parent=NULL;

	temp->key=NULL;
	temp->value=NULL;

	free(temp);
	temp=NULL;
	return 1;
}



int libmap_destroy(libmap_map *m){
	m->size=0;
	if(m->root!=NULL){
		_libmap_node_destroy(&(m->root));
//		free(m->root);
//		m->root=NULL;
		return 1;
	}
	return 0;
}



int libmap_iter_begin(libmap_map *m,libmap_iter *mi){
	if(m==NULL)return 0;
	if(m->root==NULL)return 0;

	libmap_node *temp=m->root;
	while(temp->left!=NULL){
		temp=temp->left;
	}
	mi->node=temp;
	_libmap_node_next(mi->node,&(mi->next));
	_libmap_node_prev(mi->node,&(mi->prev));
	mi->direction=1;//forwards
	mi->m=m;
	return 1;
}


int libmap_iter_end(libmap_map *m,libmap_iter *mi){
	if(m==NULL)return 0;
	if(m->root==NULL)return 0;

	libmap_node *temp=m->root;
	while(temp->right!=NULL){
		temp=temp->right;
	}
	mi->node=temp;
	_libmap_node_next(mi->node,&(mi->next));
	_libmap_node_prev(mi->node,&(mi->prev));
	mi->direction=1;//forwards
	mi->m=m;
	return 1;
}


int libmap_iter_rend(libmap_map *m,libmap_iter *mi){
	int ret=libmap_iter_begin(m,mi);
	libmap_node *temp=NULL;
	temp=mi->prev;
	mi->prev=mi->next;
	mi->next=temp;
	mi->direction=0;//backwards
	return ret;
}

int libmap_iter_rbegin(libmap_map *m,libmap_iter *mi){
	int ret=libmap_iter_end(m,mi);
	libmap_node *temp=NULL;
	temp=mi->prev;
	mi->prev=mi->next;
	mi->next=temp;
	mi->direction=0;//backwards
	return ret;
}

int _libmap_node_next(libmap_node *node,libmap_node **next){
	if(node==NULL || next==NULL){
		return 0;
	}
	*next=NULL;
	libmap_node *temp=node;
	if(temp->right!=NULL){//go to the right...
		temp=temp->right;
		while(temp->left!=NULL){//then as far to the left as we can.
			temp=temp->left;
		}
		*next=temp;
		return 1;
	}

	if(temp->parent!=NULL && temp->parent->left==temp){//if we are a left-child, go to our parent
		*next=temp->parent;
		return 1;
	}else{//re are a right-child
		while(temp->parent!=NULL && temp->parent->right==temp){
			temp=temp->parent;
		}
		if(temp->parent!=NULL){
			*next=temp->parent;
			return 1;
		}else{
			temp=NULL;
			return 0;
		}
	}
	temp=NULL;
	return 0;
}


int libmap_iter_next(libmap_iter *mi){
	mi->prev=mi->node;
	mi->node=mi->next;
	mi->next=NULL;
	if(mi->direction==1){//normal iterator
		if(mi->node==NULL){//no more nodes...
			return 0;
		}else{
			_libmap_node_next(mi->node,&(mi->next));
			return 1;
		}
	}else{//reverse iterator
		if(mi->node==NULL){//no more nodes...
			return 0;
		}else{
			_libmap_node_prev(mi->node,&(mi->next));
			return 1;
		}
	}

	return 0;
}





int _libmap_node_prev(libmap_node *node,libmap_node **prev){
	if(node==NULL || prev==NULL){
		return 0;
	}
	*prev=NULL;
	libmap_node *temp=node;
	if(temp->left!=NULL){//go to the left...
		temp=temp->left;
		while(temp->right!=NULL){//then as far to the right as we can.
			temp=temp->right;
		}
		*prev=temp;
		return 1;
	}

	if(temp->parent!=NULL && temp->parent->right==temp){//if we are a right-child, go to our parent
		*prev=temp->parent;
		return 1;
	}else{//re are a left-child
		while(temp->parent!=NULL && temp->parent->left==temp){
			temp=temp->parent;
		}
		if(temp->parent!=NULL){
			*prev=temp->parent;
			return 1;
		}else{
			temp=NULL;
			return 0;
		}
	}
	temp=NULL;
	return 0;
}






//int libmap_prev(libmap_iter *mi){
//	libmap_node *prev;
//	mi->next=mi->node;
//	mi->node=mi->prev;
//	mi->prev=NULL;
//	if(mi->direction==1){//normal iterator
//		if(mi->node==NULL){//no more nodes...
//			return 0;
//		}else{
//			_libmap_node_prev(mi->node,&(mi->prev));
//			return 1;
//		}
//	}else{//reverse iterator
//		if(mi->node==NULL){//no more nodes...
//			return 0;
//		}else{
//			_libmap_node_next(mi->node,&(mi->prev));
//			return 1;
//		}
//	}
//
//	return 0;
//}





int libmap_iter_key(libmap_iter *mi,void **key){
	if(mi==NULL || mi->node==NULL)return 0;
	*key=mi->node->key;
	return 1;
}

int libmap_iter_get(libmap_iter *mi,void **value){
	if(mi==NULL || mi->node==NULL)return 0;
	*value=mi->node->value;
	return 1;
}


int libmap_iter_set(libmap_iter *mi,void *value,void **oldvalue){
	if(mi==NULL || mi->node==NULL)return 0;
	if(oldvalue!=NULL){
		*oldvalue=mi->node->value;
	}
	mi->node->value=value;
	return 1;
}

int libmap_get(libmap_map *m,void *key,int (*comparator)(const void *,const void *),void **value){
	if(m==NULL || key==NULL || m->root==NULL)return 0;
	libmap_node *node=m->root;
	while(1){
		switch(comparator(key,node->key)){
			case -1:
				if(node->left==NULL){
					return 0;
				}else{
					node=node->left;
				}
				break;
			case 1:
				if(node->right==NULL){
					return 0;
				}else{
					node=node->right;
				}
				break;
			default:
				if(value!=NULL){
					*value=node->value;
				}
				return 1;
		}
	}
	return 0;
}


int libmap_set(libmap_map *m,void *key,void *value,int (*comparator)(const void *,const void *),void **oldvalue){
	if(m==NULL || key==NULL || m->root==NULL)return 0;
	libmap_node *node=m->root;
	while(1){
		switch(comparator(key,node->key)){
			case -1:
				if(node->left==NULL){
					return 0;
				}else{
					node=node->left;
				}
				break;
			case 1:
				if(node->right==NULL){
					return 0;
				}else{
					node=node->right;
				}
				break;
			default:
				if(oldvalue!=NULL){
					*oldvalue=node->value;
				}
				node->value=value;
				return 1;
		}
	}
	return 0;
}


//int libmap_remove(libmap_map *m,void *key,int (*comparator)(const void *,const void *),void **oldkey,void **oldvalue){
//
//}

int _libmap_node_remove(libmap_node *node,int direction,libmap_node **newroot){
	//printf("removing [%s]:[%s]\n",node->key,node->value);
	libmap_node *root=node;
	int ret=0;

	if(node->left!=NULL && node->right!=NULL){//we have 2 children
		//puts("2 children");
		libmap_node *prev;
		if(direction==1){
			_libmap_node_prev(node,&prev);
		}else{
			_libmap_node_next(node,&prev);
		}
//
		//printf("shifting [%s]:[%s]\n",prev->key,prev->value);
		node->key=prev->key;
		node->value=prev->value;
		ret=1;
		_libmap_node_remove(prev,direction,NULL);//shift the *previous* node
//		
//		
	}else if(node->left==NULL && node->right==NULL){//we have no children
		//puts("0 children");

		if(node->parent!=NULL){//we have a parent
			if(node->parent->left==node){
				node->parent->left=NULL;
			}else{
				node->parent->right=NULL;
			}
			node->parent=NULL;
		}else{
			root=NULL;
		}
		free(node);
		ret=1;

	}else{//we have one child
		//puts("1 child");

		if(node->parent!=NULL){//we have a parent
			if(node->parent->left==node){
//				node->parent->left=child;
				if(node->left!=NULL){
					node->parent->left=node->left;
				}else{
					node->parent->left=node->right;
				}
			}else{
//				node->parent->right=child;
				if(node->left!=NULL){
					node->parent->right=node->left;
				}else{
					node->parent->right=node->right;
				}
			}
//			child->parent=node->parent;
			if(node->left!=NULL){
				node->left->parent=node->parent;
			}else{
				node->right->parent=node->parent;
			}
			node->parent=NULL;
		}else{//root node
			if(node->left!=NULL){
				node->left->parent=NULL;
				root=node->left;
			}else{
				node->right->parent=NULL;
				root=node->right;
			}
		}
		node->left=NULL;
		node->right=NULL;
		free(node);
		ret=1;
		
	}

	if(newroot!=NULL){
		*newroot=root;
	}


	return ret;

}


int libmap_iter_remove(libmap_iter *mi,void **oldkey,void **oldvalue){
	if(oldkey!=NULL){
		*oldkey=mi->node->key;
	}
	if(oldvalue!=NULL){
		*oldvalue=mi->node->value;
	}
	int root=mi->node->parent==NULL;
	libmap_node *newroot=NULL;
	int ret=0;
	if((ret=_libmap_node_remove(mi->node,mi->direction,&newroot))){
		mi->m->size--;
	}
	mi->node=mi->prev;
	mi->prev=NULL;
	if(root){
		//printf("we deleted the root...\n");
		mi->m->root=newroot;
	}
	return ret;
}



int libmap_remove(libmap_map *m,void *key,int (*comparator)(const void *,const void *),void **oldkey,void **oldvalue){

	if(m==NULL || key==NULL || m->root==NULL)return 0;
	//printf("erasing: [%s]\n",key);
	libmap_node *node=m->root;
	int trip=1;
	while(trip){
		switch(comparator(key,node->key)){
			case -1:
				//printf("going left\n");
				if(node->left==NULL){
					return 0;
				}else{
					node=node->left;
				}
				break;
			case 1:
				//printf("going right\n");
				if(node->right==NULL){
					return 0;
				}else{
					node=node->right;
				}
				break;
			case 0:
				//printf("found it\n");
//				if(value!=NULL){
//					*value=node->value;
//				}
//				return 1;
				trip=0;
				break;
		}
	}
	int root=node->parent==NULL;
	int ret=0;
	if(oldkey!=NULL){
		*oldkey=node->key;
	}
	if(oldvalue!=NULL){
		*oldvalue=node->value;
	}
	libmap_node *newroot=NULL;
	if((ret=_libmap_node_remove(node,1,&newroot))){
		m->size--;
	}
	if(root){
		//printf("we deleted the root...\n");
		m->root=newroot;
	}
	return ret;

}




#endif
