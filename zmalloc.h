#ifndef ZMALLOC_H
#define ZMALLOC_H


#define zmalloc(size) malloc(size) 
#define zfree(p) free(p)
#define zrealloc(ptr,size) realloc(ptr,size)
#define zcalloc(x) calloc(x,1)
#define zstrdup strdup


#endif
