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

/*
 * inserts a key-value pair to the trie, replacing an existing value
 * the key can be empty (key_len == 0)
 * return 1 on success, 0 on error and sets errno
 */
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


/*
 * if key is in the trie, returns a non-zero int and, if pvalue is not NULL,
 * writes the corresponding value of the key to *pvalue
 * if the key is not in the trie, returns zero
 * the key may be empty (key_len == 0)
 */
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


/* array of zeroes for use with memcmp */
static unsigned char zeros[256] = {0};

/* 
 * removes the key from the trie if it exists and returns its value
 * returns NULL if key is not in the trie
 * deallocates all nodes that become unnecessary
 */
void *trie_remove(struct trie *t, size_t key_len, unsigned char key[key_len])
{
	struct trie_node *n = t->root, *last_valid = NULL;
}


void trie_delete(struct trie *t)
{
	/* the val pointer is unused and is repurposed as a "parent" pointer;
	 * this allows traversing and deallocating nodes without recursive
	 * function calls or heap-allocation of additional storage
	 * additionally, the is_terminal value is used to record the next
	 * position to check in the children array of a node
	 */

	struct trie_node *curr_node = t->root;

	if (t->root) {
		t->root->val = NULL;
		t->root->is_terminal = 0;
	}
	
	while (curr_node) {
		if (curr_node->is_terminal >= 256) {
			curr_node = (struct trie_node *) curr_node->val;
			(*(t->pfree))(curr_node->children[curr_node->is_terminal]);
			curr_node->is_terminal++;
		} else if (curr_node->children[curr_node->is_terminal] == 0) {
			curr_node->is_terminal++;
		} else {
			curr_node->children[curr_node->is_terminal]->val = curr_node;
			curr_node = curr_node->children[curr_node->is_terminal];
		}
	}


	
}
