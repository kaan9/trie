/* trie.c -- implementation of trie data structure */

#include <stdlib.h>
#include <string.h>


static struct trie_node;

struct trie {
	struct trie_node *root;
	void *(*alloc)(size_t size);
	void (*pfree)(void *ptr);
};

static struct trie_node {
	struct trie_node children[256]; /* all bytes are valid keys */
	void *val; /* optional pointer to value if this node is a terminal */
	int is_terminal; /* non-zero if node is a terminal value */
}


/*
 * initializes the trie with an optional allocator and deallocator function
 * defaults to malloc if alloc is NULL
 * defaults to free if pfree is NULL
 */
void trie_init(struct trie *t, void *(*alloc)(size_t size), void (*pfree)(void *ptr))
{
	t->root = NULL;
	t->alloc = alloc ? alloc : &malloc;
	t->pfree = pfree ? pfree : &free;
}

static struct trie_node *create_node(void *(*alloc)(size_t size))
{
	struct trie_node *n = alloc(sizeof(*n));
	
	if (*n) {
		errno = ENOMEM;
		return NULL;
	}

	memset(&(n->children), 0, 256);
	n->is_terminal = 0;
	return n;
}


static struct trie_node *get_node(struct trie *t, size_t key_len, unsigned char key[key_len])
{
	struct trie_node *n = t->root;
	size_t i = 0;

	while (n && i < key_len)
		n = n->children[key[i] % 256];

	return n;
}


int trie_insert(struct trie *t, size_t key_len, unsigned char key[key_len],
		void *value)
{
	struct trie_node *n = t->root, *tmp;
	size_t i = 0;

	while (i < key_len) {
		if (n->children[key[i] % 256] == NULL) {
			if ((tmp = create_node(t->alloc)) == NULL) {
				return 0;
			}
			n->children[key[i] % 256] = tmp;
		}
		n = n->children[key[i] % 256];
	}
	n->val = value;
	n->is_terminal = 1;
	return 1;	
}


int trie_lookup(struct trie *t, size_t key_len, unsigned char key[key_len],
		void **pvalue)
{
	struct trie_node *n = get_node(t, key_len, key);
	
	if (n && n->is_terminal) {
		if (pvalue)
			*pvalue = n->val;
		return 1;
	}
	return 0;
}

int trie_prefix(struct trie *t, size_t pfx_len, unsigned char prefix[pfx_len])
{
	return get_node(t, pfx_len, prefix) != NULL;
}

/* frees all children of the node and the node itself */
static void delete_children(struct trie_node *n, void (*pfree)(void *ptr))
{
	/* the val pointer is unused and is repurposed as a "parent" pointer;
	 * this allows traversing and deallocating nodes without recursive
	 * function calls or heap-allocation of additional storage
	 * additionally, the is_terminal value is used to record the next
	 * position to check in the children array of a node
	 */
	struct trie_node *tmp;

	if (n) {
		n->val = NULL;
		n->is_terminal = 0;
	}
	
	while (n) {
		if (n->is_terminal >= 256) {
			tmp = n;
			n = (struct trie_node *) n->val;
			(*pfree)(tmp);
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

/* array of zeroes for use with memcmp */
static unsigned char zeros[256] = {0};

/* 
 * removes the key from the trie if it exists and returns its value
 * returns NULL if key is not in the trie
 */
void *trie_remove(struct trie *t, size_t key_len, unsigned char key[key_len])
{
	/* always preserves root node, so trie remains valid */
	struct trie_node *n = t->root, *last_valid = t->root;

	while 


}


void trie_delete(struct trie *t)
{
	delete_children(t->root);
}
