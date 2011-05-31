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

int libmap_next(map_iter *mi){
	if(mi->direction==0){
		mi->direction=1;
		int ret=libmap_prev(mi);
		mi->direction=0;
		return ret;
	}
	libmap_node *node=mi->node;
	if(node->right!=NULL){//go to the right...
		node=node->right;
		while(node->left!=NULL){//then as far to the left as we can.
			node=node->left;
		}
		mi->node=node;
		return 1;
	}

	if(node->parent!=NULL && node->parent->left==node){//if we are a left-child, go to our parent
		node=node->parent;
		mi->node=node;
		return 1;
	}else{//re are a right-child
		while(node->parent!=NULL && node->parent->right==node){
			node=node->parent;
		}
		if(node->parent!=NULL){
			node=node->parent;
			mi->node=node;
			return 1;
		}else{
			return 0;
		}
	}

	return 0;
}




int libmap_prev(map_iter *mi){
	if(mi->direction==0){
		mi->direction=1;
		int ret=libmap_next(mi);
		mi->direction=0;
		return ret;
	}
	libmap_node *node=mi->node;
	if(node->left!=NULL){//go to the left...
		node=node->left;
		while(node->right!=NULL){//then as far to the right as we can.
			node=node->right;
		}
		mi->node=node;
		return 1;
	}

	if(node->parent!=NULL && node->parent->right==node){//if we are a right-child, go to our parent
		node=node->parent;
		mi->node=node;
		return 1;
	}else{//re are a left-child
		while(node->parent!=NULL && node->parent->left==node){
			node=node->parent;
		}
		if(node->parent!=NULL){
			node=node->parent;
			mi->node=node;
			return 1;
		}else{
			return 0;
		}
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
				*value=node->value;
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
				*oldvalue=node->value;
				node->value=value;
				return 1;
		}
	}
	return 0;
}






#endif
