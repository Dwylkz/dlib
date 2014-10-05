#include "dlib_comm.h"
#include "dlib_array.h"

#include <stdlib.h>

dlib_array_t* dlib_anew()
{
  dlib_array_t* bud = calloc(sizeof(dlib_array_t), 1);
  if (bud == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_0;
  }
  return bud;
err_0:
  return 0;
}

int dlib_afree(void* self)
{
  free(((dlib_array_t*)self)->data);
  free(self);
}

int dlib_apush(dlib_array_t* self, void* data)
{
  if (self->size == self->volume) {
    int volume = (self->volume+1)*2-1;
    void** data = realloc(self->data, sizeof(void*)*volume);
    if (data == NULL) {
      DLIB_ERR("%s", dlib_syserr());
      goto err_0;
    }

    self->data = data;
    self->volume = volume;
  }

  self->data[self->size++] = data;
  return 0;
err_0:
  return -1;
}

int dlib_amap(dlib_array_t* self, dlib_map_i* mapper)
{
  int ret = 0;
  for (int i = 0; i < self->size; i++)
    ret |= mapper(self->data[i]);
  return ret;
}
