#ifndef DLIB_HASH_H_
# define DLIB_HASH_H_

#include "dlib_comm.h"

/**
 * @brief
 *   dynamic size hash table
 */
typedef struct dlib_hash_t {
  struct node_t** head;
  struct node_t* data;
  struct node_t* freed;
  int size;
  int volume;
  dlib_hash_i* hash; 
  dlib_comp_i* comp;
} dlib_hash_t;
/**
 * @brief
 *   create a new hash table
 *
 * @param hash
 *   hash function
 * @param comp
 *   compare function
 *
 * @return
 *   new hash table on succ, NULL on error
 */
dlib_hash_t* dlib_hnew(dlib_hash_i* hash, dlib_comp_i* comp);
/**
 * @brief 
 *
 * @param self
 *
 * @return 
 *   always 0
 */
int dlib_hfree(void* self);
/**
 * @brief
 *   search for data, if data isn't in table, data would be inserted
 *   into it.
 *
 * @param self
 * @param data
 *
 * @return
 *   data on succ, NULL on error
 */
void* dlib_hsearch(dlib_hash_t* self, void* data);
/**
 * @brief
 *   search for data in table, compared to hsearch, data would
 *   not be inserted into table if it doesn't in table
 *
 * @param self
 * @param data
 *
 * @return
 *   data on succ, NULL on error
 */
void* dlib_hfind(dlib_hash_t* self, void* data);
/**
 * @brief
 *   remove data from table
 *
 * @param self
 * @param data
 */
void dlib_hremove(dlib_hash_t* self, void* data);
/**
 * @brief
 *   apply function to all elements in table
 *
 * @param self
 * @param mapper
 *
 * @return
 *   0 on succ, others on error
 */
int dlib_hmap(dlib_hash_t* self, dlib_map_i* mapper);

#endif // DLIB_HASH_H_
