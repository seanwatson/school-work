/*******************************************************************************
 *
 * File         : hash_table.h
 * Author       : Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca
 * Last Updated : March 29, 2012
 * 
 * Functions and definitions for a hash table data structure.
 *
 ******************************************************************************/

#include <string.h>

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/

#define DEFAULT_TABLE_CAPACITY 101
#define HT_SUCCESS 0
#define HT_MEM_ERROR -1
#define HT_INVALID_KEY -2

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

// A bucket in the hash table (you won't work with this structure)
typedef struct hash_bucket {
	char *key;
	void *data;
	struct hash_bucket* next;
} hash_bucket_t;

// Hash table data structure
typedef struct {
	size_t capacity;		// Maximum capacity of the table
	size_t size;			// Number of elements currently in the table
	hash_bucket_t** buckets;	// Linked list of buckets in the table
} hash_table_t;

/*******************************************************************************
 * FUNCTION DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 *
 * hash_table_create
 * 
 * Initializes and returns a new, empty hash table.
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   An empty hash table.  WARNING: the memory for this function is dynamically
 *   allocated.  Callers should use the hash_table_destroy() function to free
 *   a hash table (along with all of its buckets and data values) when no longer
 *   needed.
 *
 ******************************************************************************/
hash_table_t* hash_table_create();

/*******************************************************************************
 *
 * hash_table_destroy
 * 
 * Destroys the given hash table, freeing the memory allocated to it, its buckets,
 * and the values stored within it.
 *
 * Parameters:
 *   table	The hash table to destroy
 *
 * Returns:
 *   Nothing
 *
 ******************************************************************************/
void hash_table_destroy(hash_table_t* table);

/*******************************************************************************
 *
 * hash_table_set
 * 
 * Associates the specified key with the specified data value in the hash table.
 *
 * Parameters:
 *   table	The hash table in which to perform the association
 *   key        The key to map to the specified value
 *   value      The value mapped by the key
 *
 * Returns:
 *   HT_SUCCESS (0), if the association succeeded; otherwise, a non-zero value
 *
 ******************************************************************************/
int hash_table_set(hash_table_t* table, const char* key, void* data);

/*******************************************************************************
 *
 * hash_table_get
 * 
 * Gets the value associated with the specified key in the hash table.
 *
 * Parameters:
 *   table	The hash table in which to perform the search
 *   key        The key for which to return a value
 *
 * Returns:
 *   The value associated with the given key, if found; otherwise, NULL
 *
 ******************************************************************************/
void* hash_table_get(hash_table_t* table, const char *key);

/*******************************************************************************
 *
 * hash_table_values
 * 
 * Returns an array containing all the values in the hash table.  One can 
 * determine the size of this array by checking the 'size' member of the 
 * hash table 
 *
 * Parameters:
 *   table	The hash table whose values are to be retrieved
 *
 * Returns:
 *   An array containing all the values in the hash table.  If the table is 
 *   empty, NULL is returned.
 *
 ******************************************************************************/
void** hash_table_values(hash_table_t* table);

#endif
