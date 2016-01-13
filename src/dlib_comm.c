#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "dlib_comm.h"
#include "dlib_owner.h"

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
    if (strncmp(argv[1], cmds[i].name, strnlen(argv[1], 32)) == 0)
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

int dlib_subcmd_mutiplex(int argc, char** argv, dlib_cmd_i* cmd)
{
  if (argc < 2)
    return -2;

  uint32_t times;
  if (sscanf(argv[1], "%u", &times) != 1)
    return -2;

  while (times--) {
    int pid = fork();
    if (pid == -1) {
      DLIB_ERR("fork failed: (%s)", dlib_syserr());
      return -1;
    }
    if (pid == 0) {
      exit(cmd(argc-1, argv+1));
    }
  }
  return 0;
}

char* dlib_loadfile(const char* filename)
{
  DLIB_OWNER_NEW(owner);
  FILE* file = dlib_opush(&owner,
                          fopen(filename, "r"),
                          dlib_fclose);
  if (file == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    return dlib_opreturn(NULL, &owner);
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    DLIB_ERR("%s", dlib_syserr());
    return dlib_opreturn(NULL, &owner);
  }

  long size = ftell(file);
  if (size == -1) {
    DLIB_ERR("%s", dlib_syserr());
    return dlib_opreturn(NULL, &owner);
  }

  rewind(file);

  char* foo = dlib_opush(&owner,
                         calloc(size+1, 1),
                         dlib_free);
  if (foo == NULL) {
    DLIB_ERR("%s", dlib_syserr());
    return dlib_opreturn(NULL, &owner);
  }
  
  if (fread(foo, 1, size, file) != size) {
    DLIB_ERR("%s", dlib_syserr());
    return dlib_opreturn(NULL, &owner);
  }

  return dlib_opreturn(dlib_orelease(&owner, foo),
                       &owner);;
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

int dlib_close(void* self)
{
  return close(*(int*)self);
}

int dlib_fclose(void* self)
{
  return fclose((FILE*)self);
}

ssize_t dlib_so_read(int fd, void* buf, size_t size)
{
  int ret = 0;
  int cnt = 0;

  while (1) {
    cnt++;
    ret = read(fd, buf, size);
    if (ret == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      DLIB_ERR("%d: read: msg=(%s)", errno, dlib_syserr());
      return errno;
    }

    break;
  }
  DLIB_DBG("read time: cnt=%d", cnt);
  return ret;
}
ssize_t dlib_so_readline(int fd, char* buf, size_t size)
{
  int ret = 0;
  int cnt = 0;

  int i = 0;
  while (i < size) {
    cnt++;
    ret = dlib_so_read(fd, buf+i, size);
    if (ret < 0) {
      DLIB_ERR("%d: so_read: fd=%d", ret, fd);
      return -1;
    }

    if (ret == 0)
      break;
    i += ret;
    if (buf[i-1] == '\n') {
      i--;
      break;
    }
  }
  buf[i] = '\0';
  DLIB_DBG("readline time: cnt=%d", cnt);
  return ret;
}
ssize_t dlib_so_write(int fd, void* buf, size_t size)
{
  int ret = 0;
  int cnt = 0;

  int i = 0;
  while (i < size) {
    cnt++;
    ret = write(fd, buf, size-i);
    if (ret == -1) {
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      DLIB_ERR("%d: wrtie: msg=(%s)", errno, dlib_syserr());
      return errno;
    }
    else if (ret == 0) {
      break;
    }
    i += ret;
  }

  DLIB_DBG("write time: cnt=%d", cnt);
  return i;
}
