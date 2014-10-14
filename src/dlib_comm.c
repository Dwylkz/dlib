#include "dlib_comm.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

const char* dlib_syserr()
{
  return strerror(errno);
}

void dlib_errmsg(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

int dlib_subcmd(int argc, char** argv, const dlib_cmd_t* cmds)
{
  if (argc < 2)
    goto err_0;

  for (int i = 0; cmds[i].name; i++)
    if (strncmp(argv[1], cmds[i].name, strlen(cmds[i].name)) == 0)
      return cmds[i].cmd(argc-1, argv+1);

  goto err_0;
  return 0;
err_0:
  dlib_errmsg("<usage> = \"%s\" <command>\n", argv[0]);
  for (int i = 0; cmds[i].name; i++) {
    dlib_errmsg("<command> =/ <%s>\n", cmds[i].name);
    dlib_errmsg("<%s> = \"%s\" %s\n", cmds[i].name, cmds[i].name, cmds[i].usage);
  }
  return -1;
}

char* dlib_loadfile(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_0;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_1;
  }

  long size = ftell(file);
  if (size == -1) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_1;
  }

  rewind(file);

  char* foo = calloc(size+1, 1);
  if (foo == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_1;
  }
  
  if (fread(foo, 1, size, file) != size) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_2;
  }

  fclose(file);
  return foo;
err_2:
  free(foo);
err_1:
  fclose(file);
err_0:
  return NULL;
}

uint32_t dlib_rand_num(const uint32_t lower, const uint32_t upper)
{
  static int is_init = 1;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex);

  if (is_init == 1) {
    srand((uint32_t)time(NULL));
    is_init = 0;
  }
  int ret = rand()%(upper-lower)+lower;

  pthread_mutex_unlock(&mutex);
  return ret;
}

char* dlib_rand_str(const uint32_t len, const uint32_t lower, const uint32_t upper)
{
  char* bud = calloc(len+1, 1);
  if (bud == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    return NULL;
  }

  for (int i = 0; i < len; i++)
    bud[i] = dlib_rand_num(lower, upper);
  return bud;
}

char* dlib_fmtstr(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int size = vsnprintf(NULL, 0, fmt, ap);
  if (size < 0) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_0;
  }

  char* foo = calloc(size+1, 1);
  if (foo == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_1;
  }

  va_start(ap, fmt);
  if (vsnprintf(foo, size+1, fmt, ap) < 0) {
    DLIB_ERR("%s", dlib_syserr());
    goto err_2;
  }

  va_end(ap);
  return foo;
err_2:
  free(foo);
err_1:
  va_end(ap);
err_0:
  return NULL;
}

int dlib_int_comp(const void* lhs, const void* rhs)
{
  return *((int*)lhs)-*((int*)rhs);
}

int dlib_str_comp(const void* lhs, const void* rhs)
{
  return strcmp(lhs, rhs);
}

uint32_t dlib_int_hash(void* self)
{
  return *((int*)self);
}

uint32_t dlib_str_hash(void* self)
{
  uint32_t seed = 0;
  for (char* i = self; *i; i++)
    seed = seed*3+*i;
  return seed;
}

int dlib_opush(dlib_owner_t* self, void* data, dlib_map_i* del)
{
  if (self->size == DLIB_OWNER_SIZE) {
    DLIB_ERR("the owner is going to overflow");
    return -1;
  }

  self->data[self->size].data = data;
  self->data[self->size].del = del;
  self->size++;
  return 0;
}

void dlib_opop(dlib_owner_t* self, void* data, int do_del)
{
  for (int i = 0; i < DLIB_OWNER_SIZE; i++) {
    if (self->data[i].data == data) {
      if (do_del == 1)
        self->data[i].del(data);
      self->data[i] = self->data[self->size-1];
      self->size--;
      return ;
    }
  }

  DLIB_INFO("data %p is not under control", data);
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

int dlib_map(void* first, void* last, dlib_map_i* mapper)
{
  int ret = 0;
  void** begin = first;
  void** end = last;
  while (begin < end)
    ret |= mapper(*begin++);
  return ret;
}

int dlib_free(void* self)
{
  free(self);
  return 0;
}
