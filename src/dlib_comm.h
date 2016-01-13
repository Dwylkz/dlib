#ifndef DLIB_COMM_H_
# define DLIB_COMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define DLIB_SIZE_NAME 64
#define DLIB_DEBUG 1

#define DLIB_MSG(file, prefix, fmt,...) \
    do {\
      fprintf(file, "%s:%d:%d:%s: ", __FILE__, __LINE__, getpid(), prefix);\
      fprintf(file, fmt,##__VA_ARGS__);\
      fprintf(file, "\n");\
    } while(0);
#define DLIB_ERR(fmt,...) DLIB_MSG(stderr, "ERR", fmt,##__VA_ARGS__)
#ifdef DLIB_DEBUG
#define DLIB_DBG(fmt,...) DLIB_MSG(stderr, "DBG", fmt,##__VA_ARGS__)
#else
#define DLIB_DBG(fmt,...) ;
#endif
#define DLIB_INFO(fmt,...) DLIB_MSG(stdout, "INFO", fmt,##__VA_ARGS__)

/**
 * @brief 
 *   print system error message
 *
 * @return
 *   pointer to systen error message
 */
const char* dlib_syserr();

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
typedef int dlib_cmd_i(int, char**);
typedef struct dlib_cmd_t {
  const char* name;
  const char* usage;
  dlib_cmd_i* cmd;
} dlib_cmd_t;
#define DLIB_CMD_DEFINE(name, usage) {#name, usage, name}
#define DLIB_CMD_NULL {NULL, NULL, NULL}
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
int dlib_subcmd_mutiplex(int argc, char** argv, dlib_cmd_i* cmd);

/**
 * @brief
 *   load file to memory
 *
 * @param filename
 *
 * @return
 *   mem buffer pointer
 */
char* dlib_loadfile(const char* filename);

/**
 * @brief
 *   gen a random number in [lower, upper)
 *
 * @param lower
 * @param upper
 *
 * @return
 *   a random number in [lower, upper)
 */
uint32_t dlib_rand_num(const uint32_t lower, const uint32_t upper);
/**
 * @brief
 *   gen a random string which each charactor is in [lower, upper)
 *   and has len length
 *
 * @param nmemb
 * @param len
 * @param lower
 * @param upper
 *
 * @return
 *   a random string which each charactor is in [lower, upper)
 */
char* dlib_rand_str(const uint32_t len, const uint32_t lower, const uint32_t upper);

/**
 * @brief
 *
 * @param fmt
 *   printf style format
 * @param ...
 *
 * @return 
 *   pointer to formated string
 */
char* dlib_fmtstr(const char* fmt, ...);

/**
 * @brief
 *   mapper function interface
 *
 * @param 
 *
 * @return
 *   0 on succ
 *   others on error
 */
typedef int (dlib_map_i)(void*);
/**
 * @brief
 *   apply mapper to [first, last)
 *
 * @param first
 * @param last
 * @param mapper
 *
 * @return
 *   0 on succ
 *   others on error
 */
int dlib_map(void* first, void* last, dlib_map_i* mapper);
/**
 * @brief
 *   a wrapper of free
 *
 * @param self
 *
 * @return
 *   always return 0
 */
int dlib_free(void* self);
/* wrapper of close */
int dlib_close(void* self);
/* wrapper of fclose */
int dlib_fclose(void* self);

/**
 * @brief
 *   compare function interface
 *
 * @param 
 *
 * @return
 *   <0 if lhs < rhs
 *   0 if lhs == rhs
 *   >0 if lhs > rhs
 */
typedef int (dlib_comp_i)(const void*, const void*);
int dlib_int_comp(const void* lhs, const void* rhs);
int dlib_str_comp(const void* lhs, const void* rhs);

/**
 * @brief 
 *   hash function interface
 *
 * @param 
 *
 * @return 
 *   hash code
 */
typedef uint32_t (dlib_hash_i)(void*);
uint32_t dlib_int_hash(void* self);
uint32_t dlib_str_hash(void* self);

ssize_t dlib_so_read(int fd, void* buf, size_t size);
ssize_t dlib_so_readline(int fd, char* buf, size_t size);
ssize_t dlib_so_write(int fd, void* buf, size_t size);

#endif // DLIB_COMM_H_
