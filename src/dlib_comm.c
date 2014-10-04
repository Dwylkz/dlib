#include "dlib_comm.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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

int dlib_opush(dlib_owner_t* self, void* data, void (*del)(void*))
{
  int id = 0;
  while (id < DLIB_OWNER_SIZE) {
    if (self->data[id].data == 0)
      break;
    id++;
  }

  if (id == DLIB_OWNER_SIZE) {
    DLIB_ERR("the owner is going to overflow");
    return -1;
  }

  self->data[id].data = data;
  self->data[id].del = del;
  return 0;
}

void dlib_opop(dlib_owner_t* self, void* data)
{
  for (int i = 0; i < DLIB_OWNER_SIZE; i++) {
    if (self->data[i].data == data) {
      self->data[i].del(data);
      self->data[i].data = 0;
      self->data[i].del = 0;
      return ;
    }
  }

  DLIB_INFO("data %p is not under control", data);
}

void dlib_oclear(dlib_owner_t* self)
{
  for (int i = 0; i < DLIB_OWNER_SIZE; i++) {
    if (self->data[i].data != 0) {
      self->data[i].del(self->data[i].data);
      self->data[i].data = 0;
      self->data[i].del = 0;
    }
  }
}
