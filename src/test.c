#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dlib_comm.h>

int echo(int argc, char** argv)
{
  for (int i = 1; i < argc; i++)
    dlib_errmsg("%s\n", argv[i]);
  return 0;
}

int owner(int argc, char** argv)
{
  dlib_owner_t owner = DLIB_OWNER_NULL;
  char* s = "hehe";

  dlib_opush(&owner, strdup(s), free);
  dlib_opush(&owner, strdup(s), free);
  dlib_opush(&owner, strdup(s), free);
  dlib_opush(&owner, strdup(s), free);

  goto err_0;

  dlib_opush(&owner, strdup(s), free);
  dlib_opush(&owner, strdup(s), free);

  dlib_opush(&owner, strdup(s), free);

  char* b = strdup(s);
  dlib_opush(&owner, b, free);
  dlib_opop(&owner, b);
  dlib_opop(&owner, (void*)0x1);

  dlib_opush(&owner, strdup(s), free);

  printf("%s\n", s);

  dlib_oclear(&owner);
  return 0;
err_0:
  dlib_oclear(&owner);
  return 0;
}

int loadfile(int argc, char** argv)
{
  if (argc != 2)
    return -1;

  dlib_owner_t owner = DLIB_OWNER_NULL;

  char* foo = dlib_loadfile(argv[1]);
  dlib_opush(&owner, foo, free);
  puts(foo);

  dlib_oclear(&owner);
  return 0;
}

int main(int argc, char** argv)
{
  const dlib_cmd_t cmds[] = {
    {"loadfile", "<file>", loadfile},
    {"echo", "*<string>", echo},
    {"owner", "", owner},
    DLIB_CMD_NULL
  };
  return dlib_subcmd(argc, argv, cmds);
}
