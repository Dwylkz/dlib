#ifndef DLIB_COMM_H_
# define DLIB_COMM_H_

#define DLIB_MSG(file, prefix, fmt,...) \
    do {\
      fprintf(file, "%s:%s:%d:%s: ", __FILE__, __func__, __LINE__, prefix);\
      fprintf(file, fmt,##__VA_ARGS__);\
      fprintf(file, "\n");\
    } while(0);
#define DLIB_ERR(fmt,...) DLIB_MSG(stderr, "ERR", fmt,##__VA_ARGS__)
#define DLIB_INFO(fmt,...) DLIB_MSG(stdout, "INFO", fmt,##__VA_ARGS__)

/**
 * @brief put message to stderr
 *
 * @param fmt
 * @param ...
 */
void dlib_errmsg(const char* fmt, ...);

/**
 * @brief 
 *   it's a structure for subcommand definition.
 *   cmd is a pointer to the command implement.
 *   name is command's name.
 *   usage is command's usage.
 */
typedef struct dlib_cmd_t {
  const char* name;
  const char* usage;
  int (*cmd)(int, char**);
} dlib_cmd_t;
const dlib_cmd_t DLIB_CMD_NULL = {0, 0, 0};
/**
 * @brief 
 *   it's a command proxy for main function
 *   which takes argv[1] as the sub-command
 *   name to run the specific command and do some arguments shifting.
 *
 *   for example:
 *     #include <stdio.h>
 *
 *     #include <dlib_comm.h>
 *
 *     int echo(int argc, char** argv)
 *     {
 *       for (char** i; *i; i++)
 *         puts(*i);
 *       return 0;
 *     }
 *
 *     int main(int argc, char** argv)
 *     {
 *       const dlib_cmd_t cmds[] = {
 *         {"echo", "*<string>", echo},
 *         DLIB_CMD_NULL
 *       };
 *       return dlib_subcmd(argc, argv, cmds);
 *     }
 *   
 *   the error message like this:
 *     <usage> = "main" <command>
 *     <command> =/ <echo>
 *     <echo> = "echo" *<string>
 *
 * @param argc
 * @param argv
 * @param cmds
 *
 * @return 
 *   it depends on which command you ran.
 */
int dlib_subcmd(int argc, char** argv, const dlib_cmd_t* cmds);

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
    void (*del)(void*);
  } data[DLIB_OWNER_SIZE];
} dlib_owner_t;
const dlib_owner_t DLIB_OWNER_NULL = {{{0, 0}}};
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
 *   0 on succ, -1 on the size == DLIB_OWNER_SIZE
 */
int dlib_opush(dlib_owner_t* self, void* data, void (*del)(void*));
/**
 * @brief
 *   free specific data from owner
 *
 * @param self
 * @param data
 */
void dlib_opop(dlib_owner_t* self, void* data);
/**
 * @brief
 *   free all data in owner, usually use for err handle
 *
 * @param self
 */
void dlib_oclear(dlib_owner_t* self);

#endif // DLIB_COMM_H_
