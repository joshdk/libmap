#ifndef _LIBMAP_H_
#define _LIBMAP_H_


typedef struct{
	int size;
	struct libmap_node *root;
}map;

typedef struct{
	struct libmap_node *node;
}map_iter;

typedef struct libmap_node{
	void *key;
	void *value;
	struct libmap_node *left,*right,*parent;
}libmap_node;


int libmap_create(map *);
int libmap_size(map *);
int libmap_node_add(libmap_node *,libmap_node *,int (*)(const void *,const void *));
int libmap_add(map *,void *,void *,int (*)(const void *,const void *));
void libmap_node_destroy(libmap_node *);
void libmap_destroy(map *);
int libmap_begin(map *,map_iter *);
int libmap_end(map *,map_iter *);
int libmap_next(map_iter *);
int libmap_prev(map_iter *);
void *libmap_key(map_iter *);
void *libmap_value(map_iter *);
void *libmap_get(map *,void *,int (*)(const void *,const void *));
void *libmap_set(map *,void *,void *,int (*)(const void *,const void *));




#endif
