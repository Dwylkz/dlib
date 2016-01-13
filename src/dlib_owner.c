#include "dlib_comm.h"

#include "dlib_owner.h"

void* dlib_opush(dlib_owner_t* self, void* data, dlib_map_i* del)
{
  if (self->size == DLIB_OWNER_SIZE) {
    DLIB_ERR("the owner is going to overflow");
    return NULL;
  }

  self->data[self->size].data = data;
  self->data[self->size].del = del;
  self->size++;
  return data;
}

void* dlib_opop(dlib_owner_t* self, void* data, int do_del)
{
  for (int i = 0; i < DLIB_OWNER_SIZE; i++) {
    if (self->data[i].data == data) {
      dlib_map_i* del = self->data[i].del;
      self->data[i] = self->data[self->size-1];
      self->size--;
      if (do_del == 1) {
        del(data);
        return NULL;
      }
      return data;
    }
  }

  DLIB_INFO("data %p is not under control", data);
  return NULL;
}

void dlib_ofree(dlib_owner_t* self, void* data)
{
  dlib_opop(self, data, 1);
}

void* dlib_orelease(dlib_owner_t* self, void* data)
{
  return dlib_opop(self, data, 0);
}

void dlib_oclear(dlib_owner_t* self)
{
  for (int i = 0; i < self->size; i++) {
    self->data[i].del(self->data[i].data);
    self->data[i].data = 0;
    self->data[i].del = 0;
  }
  self->size = 0;
}

int dlib_oreturn(int ret, dlib_owner_t* self)
{
  dlib_oclear(self);
  return ret;
}

void* dlib_opreturn(void* ret, dlib_owner_t* self)
{
  dlib_oclear(self);
  return ret;
}
