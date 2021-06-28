/* trie.h -- trie data structure -- MIT license, copyright kaan*/
#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

struct trie;

/*
 * initializes the trie with an optional allocator and deallocator function
 * defaults to malloc if alloc is NULL
 * defaults to free if pfree is NULL
 */
void trie_init(struct trie *t, void *(*alloc)(size_t size), void (*pfree)(void *ptr));

/*
 * inserts a key-value pair to the trie, replacing an existing value
 * the key can be empty (key_len == 0)
 * return 1 on success, 0 on error and sets errno
 */
int trie_insert(struct trie *t, size_t key_len, unsigned char key[key_len],
		void *value);

/*
 * if key is in the trie, returns a non-zero int and, if pvalue is not NULL,
 * writes the corresponding value of the key to *pvalue
 * if the key is not in the trie, returns zero
 * the key may be empty (key_len == 0)
 */
int trie_lookup(struct trie *t, size_t key_len, unsigned char key[key_len],
		void **pvalue);

/*
 * if prefix is the prefix of some key in the trie, returns a non-zero int
 * otherwise returns 0
 */
int trie_prefix(struct trie *t, size_t pfx_len, unsigned char prefix[pfx_len]);

/* 
 * removes the key from the trie if it exists and returns its value
 * returns NULL if key is not in the trie
 */
void *trie_remove(struct trie *t, size_t key_len, unsigned char key[key_len]);

/*
 * deinitializes and deallocates the trie, removing all keys from the trie
 * does not free the trie struct itself
 * does not deallocate any values of (key,val) pairs
 * does not use recursion or heap-allocation
 * a call to trie_init is necessary to reuse the trie after trie_delete
 */
void trie_delete(struct trie *t);

#endif
