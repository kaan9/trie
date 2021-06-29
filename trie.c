/* trie.c -- implementation of trie data structure */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

struct trie_node {
	struct trie_node *children[256]; /* all bytes are valid keys */
	void *val; /* optional pointer to value if this node is a terminal */
	int is_terminal; /* non-zero if node is a terminal value */
};


static struct trie_node *create_node(void *(*alloc)(size_t size))
{
	struct trie_node *n = alloc(sizeof(*n));
	
	if (!n) {
		errno = ENOMEM;
		return NULL;
	}

	memset(&(n->children), 0, sizeof(n->children));
	n->is_terminal = 0;
	return n;
}


void trie_init(struct trie *t, void *(*alloc)(size_t size), void (*pfree)(void *ptr))
{
	t->alloc = alloc ? alloc : &malloc;
	t->pfree = pfree ? pfree : &free;
	t->root = create_node(t->alloc);
}



/* should've been   get_node (trie *t, ...) (trie_node *n, int depth)
 * but C doesn't allow multiple return values
 */
static struct trie_node *get_node(struct trie *t, size_t key_len, unsigned char *key, size_t *depth)
{
	struct trie_node *n = t->root;
	size_t i;

	for (i = 0; n && i < key_len; i++) {
		n = n->children[key[i] % 256];
	}

	if (depth)
		*depth = i;
	return n;
}


int trie_insert(struct trie *t, size_t key_len, unsigned char *key,
		void *value)
{
	struct trie_node *n = t->root, *tmp;
	size_t i = 0;

	for (; i < key_len; i++) {
		if (n->children[key[i] % 256] == NULL) {
			if ((tmp = create_node(t->alloc)) == NULL)
				return 0;
			n->children[key[i] % 256] = tmp;
		}
		n = n->children[key[i] % 256];
	}
	n->val = value;
	n->is_terminal = 1;
	return 1;	
}


int trie_lookup(struct trie *t, size_t key_len, unsigned char *key,
		void **pvalue)
{
	struct trie_node *n = get_node(t, key_len, key, NULL);
	
	if (n && n->is_terminal) {
		if (pvalue)
			*pvalue = n->val;
		return 1;
	}
	return 0;
}

int trie_has_prefix(struct trie *t, size_t pfx_len, unsigned char *prefix)
{
	return get_node(t, pfx_len, prefix, NULL) != NULL;
}

size_t trie_longest_prefix(struct trie *t, size_t key_len, unsigned char *key)
{
	size_t len;
	struct trie_node *n;

	get_node(t, key_len, key, &len);
	printf("%s: len = %d\n", key, len);
	/*FIXME: not working properly */
	return trie_lookup(t, len, key, NULL) ? len : 0;
}

void trie_delete(struct trie *t)
{
	/* the val pointer is unused and is repurposed as a "parent" pointer;
	 * this allows traversing and deallocating nodes without recursive
	 * function calls or heap-allocation of additional storage
	 * additionally, the is_terminal value is used to record the next
	 * position to check in the children array of a node
	 */
	struct trie_node *n = t->root, *tmp;

	if (n) {
		n->val = NULL;
		n->is_terminal = 0;
	}
	
	while (n) {
		if (n->is_terminal >= 256) {
			tmp = n;
			n = (struct trie_node *) n->val;
			(*t->pfree)(tmp);
			if (n)
				n->is_terminal++;
		} else if (n->children[n->is_terminal] == 0) {
			n->is_terminal++;
		} else {
			n->children[n->is_terminal]->val = n;
			n = n->children[n->is_terminal];
		}
	}

}
