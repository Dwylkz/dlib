#ifndef DLIB_ARRAY_H_
# define DLIB_ARRAY_H_

/**
 * @brief
 *   dynamic size array
 */
typedef struct dlib_array_t {
  void** data;
  int size;
  int volume;
} dlib_array_t;

/**
 * @brief
 *   create an array
 *
 * @return
 *   a pointer to new array on succ, NULL on error
 */
dlib_array_t* dlib_anew();
/**
 * @brief
 *   release an array
 */
int dlib_afree(void* self);
/**
 * @brief
 *   push a data after array's tail
 *
 * @param self
 * @param data
 *
 * @return
 *   0 on succ, others on error
 */
int dlib_apush(dlib_array_t* self, void* data);
/**
 * @brief
 *   apply mapper to all elements in self
 *
 * @param self
 * @param mapper
 *
 * @return
 *   0 on succ, others on error
 */
int dlib_amap(dlib_array_t* self, dlib_map_i* mapper);

#endif // DLIB_ARRAY_H_
