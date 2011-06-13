#ifndef _LIBMAP_H_
#define _LIBMAP_H_


//data types
	typedef struct{
		int size;
		struct libmap_node *root;
	}libmap_map;
	
	typedef struct libmap_node{
		void *key;
		void *value;
		struct libmap_node *left,*right,*parent;
	}libmap_node;
	
	typedef struct{
		libmap_node *prev,*node,*next;
		libmap_map *m;
		int direction;
		int constant;
	}libmap_iter;


//private internals
	int _libmap_node_add(libmap_node *,libmap_node *,int (*)(const void *,const void *));
	int _libmap_node_destroy(libmap_node **);
	int _libmap_node_next(libmap_node *,libmap_node **);
	int _libmap_node_prev(libmap_node *,libmap_node **);
	int _libmap_node_remove(libmap_node *,int,libmap_node **);


//public interfaces
	int libmap_create(libmap_map *);
	int libmap_destroy(libmap_map *);
	
	int libmap_add(libmap_map *,void *,void *,int (*)(const void *,const void *));
	int libmap_remove(libmap_map *,void *,int (*)(const void *,const void *),void **,void **);
	
	int libmap_get(libmap_map *,void *,int (*)(const void *,const void *),void **);
	int libmap_set(libmap_map *,void *,void *,int (*)(const void *,const void *),void **);
	
	int libmap_size(libmap_map *);
	
	int libmap_iter_begin(libmap_map *,libmap_iter *);
	int libmap_iter_rbegin(libmap_map *,libmap_iter *);
	int libmap_iter_end(libmap_map *,libmap_iter *);
	int libmap_iter_rend(libmap_map *,libmap_iter *);
	int libmap_iter_next(libmap_iter *);
	
	int libmap_iter_key(libmap_iter *,void **);
	int libmap_iter_get(libmap_iter *,void **);
	int libmap_iter_set(libmap_iter *,void *,void **);
	int libmap_iter_remove(libmap_iter *,void **,void **);


#endif
