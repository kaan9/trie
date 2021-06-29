#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "trie.h"

int main(void)
{
	struct trie t;
	void *pval;

	trie_init(&t, NULL, NULL);


	trie_insert(&t, 3, "cat", "CAT");
	trie_insert(&t, 6, "cattle", "CATTLE");
	trie_insert(&t, 4, "joke", NULL);
	trie_insert(&t, 3, "kit", "KIT");
	trie_insert(&t, 3, "kin", "KIN");
	trie_insert(&t, 0, NULL, "NULL");


	assert(trie_lookup(&t, 3, "cat",&pval));
	assert(strcmp(pval, "CAT") == 0);
	assert(trie_lookup(&t, 6, "cattle",&pval));
	assert(strcmp(pval, "CATTLE") == 0);
	assert(trie_lookup(&t, 4, "joke",&pval));
	assert(pval == NULL);
	assert(trie_lookup(&t, 3, "kit",&pval));
	assert(strcmp(pval, "KIT") == 0);
	assert(trie_lookup(&t, 3, "kin", NULL));
	assert(trie_lookup(&t, 0, NULL,&pval));
	assert(strcmp(pval, "NULL") == 0);


	assert(!trie_lookup(&t, 3, "abc", NULL));
	assert(!trie_lookup(&t, 2, "ca", NULL));
	assert(!trie_lookup(&t, 2, "catt", NULL));
	assert(!trie_lookup(&t, 5, "joker",&pval));
	assert(!trie_lookup(&t, 6, "joker", NULL));


	assert(trie_has_prefix(&t, 2, "ca"));
	assert(!trie_has_prefix(&t, 1, "a"));
	assert(!trie_has_prefix(&t, 1, "b"));
	assert(trie_has_prefix(&t, 4, "joke"));
	assert(!trie_has_prefix(&t, 5, "joker"));

	/* FIXME: longest_prefix not implemented correctly
	assert(trie_longest_prefix(&t, 4, "joke") == 4);
	assert(trie_longest_prefix(&t, 4, "joker") == 4);
	assert(trie_longest_prefix(&t, 3, "jok") == 0);
	printf("%d\n", trie_longest_prefix(&t, 6, "kitten"));
	assert(trie_longest_prefix(&t, 6, "kitten") == 3);
	*/


	trie_delete(&t);

	return 0;
}
