#ifndef _LIBMAP_C_
#define _LIBMAP_C_

#include <stdlib.h>
#include <stdio.h>
#include "libmap.h"


int libmap_create(map *m){
	printf("create()\n");
	m->size=0;
	m->root=NULL;
	return 1;
}

int libmap_size(map *m){
	if(m==NULL)return 0;
	return m->size;
}


int libmap_node_add(libmap_node *node,libmap_node *pair,int (*comparator)(const void *,const void *)){
	switch(comparator(pair->key,node->key)){
		case -1:
			if(node->left==NULL){
				printf("adding to the left\n");
				node->left=pair;
				node->left->parent=node;
				return 1;
			}else{
				printf("recursing to the left\n");
				return libmap_node_add(node->left,pair,comparator);
			}
			break;
		case 1:
			if(node->right==NULL){
				printf("adding to the right\n");
				node->right=pair;
				node->right->parent=node;
				return 1;
			}else{
				printf("recursing to the right\n");
				return libmap_node_add(node->right,pair,comparator);
			}
			break;
		default:
			printf("duplicate key?\n");
			return 0;
	}
	return 0;
}




int libmap_add(map *m,void *key,void *value,int (*comparator)(const void *,const void *)){
	printf("add()\n");
	if(m==NULL || key==NULL || comparator==NULL)return 0;

	libmap_node *temp=malloc(sizeof(libmap_node));
	temp->key=key;
	temp->value=value;
	temp->left=NULL;
	temp->right=NULL;
	temp->parent=NULL;

	if(m->root==NULL){
		printf("adding to root\n");
		m->root=temp;
		m->size++;
		return 1;
	}

	int ret=libmap_node_add(m->root,temp,comparator);
	if(ret==0){
		free(temp);
		temp=NULL;
	}else{
		m->size++;
	}
	return ret;
}


void libmap_node_destroy(libmap_node *node){
	node->key=NULL;
	node->value=NULL;
	if(node->left!=NULL){
		libmap_node_destroy(node->left);
		free(node->left);
		node->left=NULL;
	}
	if(node->right!=NULL){
		libmap_node_destroy(node->right);
		free(node->right);
		node->right=NULL;
	}
}



void libmap_destroy(map *m){
	m->size=0;
	if(m->root!=NULL){
		libmap_node_destroy(m->root);
		free(m->root);
		m->root=NULL;
	}
}



int libmap_begin(map *m,map_iter *mi){
	if(m==NULL)return 0;
	if(m->root==NULL)return 0;

	libmap_node *temp=m->root;
	while(temp->left!=NULL){
		temp=temp->left;
	}
	mi->node=temp;
	mi->direction=1;//forwards
	return 1;
}


int libmap_end(map *m,map_iter *mi){
	if(m==NULL)return 0;
	if(m->root==NULL)return 0;

	libmap_node *temp=m->root;
	while(temp->right!=NULL){
		temp=temp->right;
	}
	mi->node=temp;
	mi->direction=1;//forwards
	return 1;

}


int libmap_rend(map *m,map_iter *mi){
	int ret=libmap_begin(m,mi);
	mi->direction=0;//backwards
	return ret;
}

int libmap_rbegin(map *m,map_iter *mi){
	int ret=libmap_end(m,mi);
	mi->direction=0;//backwards
	return ret;
}

int libmap_node_next(libmap_node *node,libmap_node **next){
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


int libmap_next(map_iter *mi){
	libmap_node *next;
	if(mi->direction==1){
		if(libmap_node_next(mi->node,&next)){
			mi->node=next;
			return 1;
		}
		return 0;
	}else{
		if(libmap_node_prev(mi->node,&next)){
			mi->node=next;
			return 1;
		}
		return 0;
	}

	return 0;
}





int libmap_node_prev(libmap_node *node,libmap_node **prev){
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






int libmap_prev(map_iter *mi){
	libmap_node *prev;
	if(mi->direction==1){
		if(libmap_node_prev(mi->node,&prev)){
			mi->node=prev;
			return 1;
		}
		return 0;
	}else{
		if(libmap_node_next(mi->node,&prev)){
			mi->node=prev;
			return 1;
		}
		return 0;
	}

	return 0;
}



void *libmap_key(map_iter *mi){
	if(mi==NULL || mi->node==NULL)return NULL;
	return mi->node->key;
}

void *libmap_value(map_iter *mi){
	if(mi==NULL || mi->node==NULL)return NULL;
	return mi->node->value;
}



int libmap_get(map *m,void *key,int (*comparator)(const void *,const void *),void **value){
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


int libmap_set(map *m,void *key,void *value,int (*comparator)(const void *,const void *),void **oldvalue){
	if(m==NULL || key==NULL || m->root==NULL)return 0;
	libmap_node *node=m->root;
	void *temp=NULL;
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


//int libmap_remove(map *m,void *key,int (*comparator)(const void *,const void *),void **oldkey,void **oldvalue){
//
//}

//int libmap_node_remove(libmap_node *node,int direction){
//
//	
//
//}
//
//
//int libmap_remove(map_iter *mi,void **oldkey,void **oldvalue){
//	*oldkey=mi->node->key;
//	*oldvalue=mi->node->value;
//
//	return libmap_node_remove(mi->node,mi->direction);
//
//}








#endif
