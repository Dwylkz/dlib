#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dlib_comm.h>
#include <dlib_array.h>
#include <dlib_hash.h>

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

  dlib_opush(&owner, strdup(s), dlib_free);
  dlib_opush(&owner, strdup(s), dlib_free);
  dlib_opush(&owner, strdup(s), dlib_free);
  dlib_opush(&owner, strdup(s), dlib_free);

  goto err_0;

  dlib_opush(&owner, strdup(s), dlib_free);
  dlib_opush(&owner, strdup(s), dlib_free);

  dlib_opush(&owner, strdup(s), dlib_free);

  char* b = strdup(s);
  dlib_opush(&owner, b, dlib_free);
  dlib_opop(&owner, b, 1);
  dlib_opop(&owner, (void*)1, 1);

  dlib_opush(&owner, strdup(s), dlib_free);

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
  dlib_opush(&owner, foo, dlib_free);

  dlib_oclear(&owner);
  return 0;
}

int array(int argc, char** argv)
{
  dlib_owner_t owner = DLIB_OWNER_NULL;

  char* bud = "hehe";
  dlib_array_t* a = dlib_anew();
  dlib_opush(&owner, a, dlib_afree);

  for (int i = 0; i < 1000000; i++)
    dlib_apush(a, strdup(bud));

  dlib_amap(a, dlib_free);

  dlib_oclear(&owner);
  return 0;
}

int utils(int argc, char** argv)
{
  char* foo = dlib_fmtstr("%s hehe", "hello world");
  puts(foo);
  free(foo);
  return 0;
}

int pstr(void* self)
{
  printf("%s\n", (char*)self);
  return 0;
}

int hash(int argc, char** argv)
{
  int N, L;
  if (argc < 3 || sscanf(argv[1], "%d", &N)+sscanf(argv[2], "%d", &L) != 2)
    return -1;

  char** strs = calloc(N, sizeof(char*));
  for (int i = 0; i < N; i++)
    strs[i] = dlib_rand_str(L, 'a', 'z'+1);

  dlib_hash_t* h = dlib_hnew(dlib_str_hash, dlib_int_comp);
  if (h == NULL)
    return -1;

  for (int i = 0; i < N; i++)
    if (dlib_hsearch(h, strs[i]) == NULL)
      puts("fuck");

  for (int i = 0; i < N; i++)
    dlib_hremove(h, strs[i]);

  for (int i = 0; i < N; i++)
    dlib_hfind(h, strs[i]);

  // printf("size=%d\n", h->size);

  dlib_map(strs, strs+N, dlib_free);
  free(strs);

  dlib_hfree(h);
  return 0;
}

int main(int argc, char** argv)
{
  const dlib_cmd_t cmds[] = {
    DLIB_CMD_DEFINE(hash, "<nmemb> <len>"),
    DLIB_CMD_DEFINE(utils, ""),
    DLIB_CMD_DEFINE(array, ""),
    DLIB_CMD_DEFINE(loadfile, "<file>"),
    DLIB_CMD_DEFINE(echo, "*<string>"),
    DLIB_CMD_DEFINE(owner, ""),
    DLIB_CMD_NULL
  };
  clock_t start = clock();
  int ret = dlib_subcmd(argc, argv, cmds);
  printf("use %fs\n", 1.0*(clock()-start)/CLOCKS_PER_SEC);
  return ret;
}
