#include "dlib_hash.h"

typedef struct node_t {
  void* data;
  struct node_t* to;
  uint32_t code;
} node_t;

dlib_hash_t* dlib_hnew(dlib_hash_i* hash, dlib_comp_i* comp)
{
  dlib_hash_t* bud = calloc(sizeof(dlib_hash_t), 1);
  if (bud == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_0;
  }

  bud->hash = hash;
  bud->comp = comp;
  return bud;
err_0:
  return NULL;
}

int dlib_hfree(void* self)
{
  free(((dlib_hash_t*)self)->data);
  free(((dlib_hash_t*)self)->head);
  free(self);
  return 0;
}

static int resize(dlib_hash_t* self)
{
  DLIB_OWNER_NEW(owner);

  int volume = (self->volume+1)*2-1;
  node_t* data = calloc(volume, sizeof(node_t));
  if (data != NULL)
    dlib_opush(&owner, data, dlib_free);

  node_t** head = calloc(volume, sizeof(node_t*));
  if (head != NULL)
    dlib_opush(&owner, head, dlib_free);

  if (data == NULL || head == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_0;
  }

  node_t* freed = data+volume-1;
  for (int i = 1; i < volume; i++)
    data[i].to = data+i-1;

  for (int i = 0; i < self->size; i++) {
    uint32_t at = self->data[i].code%volume;
    node_t* bud = freed;
    freed = freed->to;

    *bud = self->data[i];
    bud->to = head[at];
    head[at] = bud;
  }

  self->volume = volume;
  free(self->data);
  self->data = data;
  free(self->head);
  self->head = head;
  self->freed = freed;
  return 0;
err_0:
  dlib_oclear(&owner);
  return -1;
}

static node_t* nnew(dlib_hash_t* self, void* data, uint32_t code)
{
  if (self->freed == NULL && resize(self) != 0) {
    DLIB_ERR("resize failed");
    goto err_0;
  }

  node_t* bud = self->freed;
  self->freed = self->freed->to;

  bud->data = data;
  bud->code = code;

  self->size++;
  return bud;
err_0:
  return NULL;
}

static int nfree(dlib_hash_t* self, void* old)
{
  ((node_t*)old)->to = self->freed;
  self->freed = old;
  self->size--;
  return 0;
}

static node_t** hash(dlib_hash_t* self, void* data, int do_insert)
{
  if (self->volume == 0 && resize(self) != 0) {
    DLIB_ERR("resize failed");
    goto err_0;
  }

  uint32_t code = self->hash(data);
  int at = code%self->volume;
  for (node_t** i = self->head+at; *i; i = &(*i)->to)
    if (self->comp(data, (*i)->data) == 0)
      return i;

  if (do_insert == 0)
    return NULL;

  node_t* bud = nnew(self, data, code);
  if (bud == NULL) {
    DLIB_ERR("nnew failed");
    goto err_0;
  }

  at = code%self->volume;
  bud->to = self->head[at];
  self->head[at] = bud;
  return self->head+at;
err_0:
  return NULL;
}

void* dlib_hsearch(dlib_hash_t* self, void* data)
{
  node_t** found = hash(self, data, 1);
  if (found == NULL) {
    DLIB_ERR("hash failed");
    return NULL;
  }
  return (*found)->data;
}

void* dlib_hfind(dlib_hash_t* self, void* data)
{
  node_t** found = hash(self, data, 0);
  if (found == NULL)
    return NULL;
  return (*found)->data;
}

void dlib_hremove(dlib_hash_t* self, void* data)
{
  node_t** found = hash(self, data, 0);
  if (found == NULL)
    return ;

  node_t* old = *found;
  *found = (*found)->to;
  nfree(self, old);
}

int dlib_hmap(dlib_hash_t* self, dlib_map_i* mapper)
{
  int ret = 0;
  for (int i = 0; i < self->volume; i++)
    for (node_t* j = self->head[i]; j; j = j->to)
      ret |= mapper(j->data);
  return ret;
}
