#ifndef DLIB_OWNER_H_
# define DLIB_OWNER_H__

#include <string.h>

#ifndef DLIB_OWNER_SIZE
# define DLIB_OWNER_SIZE 8
#endif // DLIB_OWNER_SIZE
/**
 * @brief 
 *   simple memory manager, usually use in function
 */
typedef struct dlib_owner_t {
  struct {
    void* data;
    dlib_map_i* del;
  } data[DLIB_OWNER_SIZE];
  int size;
} dlib_owner_t;
#define DLIB_OWNER_NEW(owner) dlib_owner_t owner; \
    memset(&owner, 0, sizeof(owner))

/**
 * @brief 
 *   push a data to owner
 *
 * @param self
 * @param data
 * @param del
 *   point to the data destroyer
 *
 * @return 
 *   data on succ, NULL on the size == DLIB_OWNER_SIZE
 */
void* dlib_opush(dlib_owner_t* self, void* data, dlib_map_i* del);

/**
 * @brief
 *   free specific data from owner
 *
 * @param self
 * @param data
 * @param do_del
 *   1  delete, others not
 */
void* dlib_opop(dlib_owner_t* self, void* data, int do_del);
/* do not delete */
void* dlib_orelease(dlib_owner_t* self, void* data);
/* delete */
void dlib_ofree(dlib_owner_t* self, void* data);

/**
 * @brief
 *   free all data in owner, usually use for err handle
 *
 * @param self
 */
void dlib_oclear(dlib_owner_t* self);
int dlib_oreturn(int ret, dlib_owner_t* self);
void* dlib_opreturn(void* ret, dlib_owner_t* self);

#endif /* DLIB_OWNER_H_ */
