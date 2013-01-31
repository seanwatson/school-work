/*******************************************************************************
 *
 * File         : hash_table.c
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>
 * Last Updated : March 29, 2012
 *
 * See function descriptions in hash_table.h.
 * 
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "hash_table.h"
#include "assembler.h"
hash_table_t* hash_table_create()
{
	hash_table_t* table = (hash_table_t*)malloc(sizeof(hash_table_t));

	if (! table)
		return NULL;

	table->buckets = calloc(DEFAULT_TABLE_CAPACITY, sizeof(hash_bucket_t*));

	if (! table->buckets)
	{
		free(table);
		return NULL;
	}

	table->capacity = DEFAULT_TABLE_CAPACITY;
	table->size = 0;

	return table;
}

void hash_table_destroy(hash_table_t* table)
{
	int i;
	hash_bucket_t* current;
	hash_bucket_t* next;

	for (i = 0; i < table->capacity; i++)
	{
		current = table->buckets[i];

		while (current) {
			free(current->key);
			free(current->data);
			next = current->next;
			free(current);
			current = next;
		}
	}

	free(table->buckets);
	free(table);
}

int hash(hash_table_t* table, const char* key)
{
	unsigned int hash = 0;
	unsigned int len = strlen(key);
	unsigned int i;

//	printf("Key: %s", key);
	
	for (i = 0; i < len; i++)
	{
		hash = (hash << 5) | (hash >> 27);
		hash += (unsigned int) *key;
		key++;
	}
	
//	printf(", Position: %d\n", hash % table->capacity);
	
	return (int)hash % table->capacity;
}

hash_bucket_t* hash_table_get_bucket(hash_table_t* table, const char* key)
{
	int hc = hash(table, key);
	hash_bucket_t* bucket = table->buckets[hc];

	while (bucket) {

		//printf("Key: %s, Bucket Key: %s\n", key, bucket->key);

		if (! strcmp(bucket->key, key)) {
			return bucket;
		}

		bucket = bucket->next;
	}

	return NULL;
}

int hash_table_set(hash_table_t* table, const char* key, void* data)
{
	int hc;
	hash_bucket_t* bucket;
	
	if (key == NULL)
		return HT_INVALID_KEY;
		
 	bucket = hash_table_get_bucket(table, key);

	if (bucket) {
//		printf("Bucket key: %s\n", bucket->key);
		bucket->data = data;
		return HT_SUCCESS;
	}
	

	
	bucket = malloc(sizeof(hash_bucket_t));

	if (!bucket)
		return HT_MEM_ERROR;

	if (!(bucket->key = strdup(key))) {
		free(bucket);
		return HT_MEM_ERROR;
	}
//	if(!strcmp(bucket->key, "stw")){
//		printf("SETTING\nBucket key: %s\n", bucket->key);
//		printf("Data Opcode: %d\n\n", ((perm_symtab_entry_t*)data)->opcode);
//	}
//	if(table->buckets[hc]){
//		printf("Collision with %s\n", table->buckets[hc]->key);
//	}
 	hc = hash(table, key);
	bucket->data = data;
	bucket->next = table->buckets[hc];
//	if(bucket->next){
//		printf("Just moved %s\n", bucket->next->key);
//	}
	table->buckets[hc] = bucket;
	table->size++;

	return HT_SUCCESS;
}

void* hash_table_get(hash_table_t* table, const char *key)
{
	hash_bucket_t* bucket;
		
	if (key == NULL)
		return NULL;
		
	bucket = hash_table_get_bucket(table, key);

	if (bucket){
	//	if(!strcmp(bucket->key, "stw")){
	//		printf("GETTING\nBucket key: %s\n", bucket->key);
		//	printf("Next bucket: %d\n", (int) bucket->next);
	//		printf("Data Opcode: %d\n", ((perm_symtab_entry_t*)bucket->data)->opcode);
	//	}
		return bucket->data;
	}
	else
		return NULL;
}

void** hash_table_values(hash_table_t* table)
{
	void** values;
	hash_bucket_t* bucket;
	int i;
	int idx;
	
	if (table->size == 0)
		return NULL;
		
	values = (void**)malloc(table->size * sizeof(void*));
	
	for (i = 0, idx = 0; i < table->capacity; ++i)
	{
		bucket = table->buckets[i];

		while (bucket) {
			values[idx++] = bucket->data;
			bucket = bucket->next;
		}	
	}
	
	return values;
}
