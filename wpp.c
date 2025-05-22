#include "exec.h"
#include "lexer.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char **argv)
{
  wppLexer *lexer;
  wppExec *exec;
  int fd, i;
  struct stat stat;
  char *code;
  char *filename = NULL;

  if (argc <= 1)
    {
      usage ();
      return 1;
    }

  for (i = 1; i < argc; i++)
    {
      char *arg = argv[i];
      if (arg[0] == '-')
        {
          if (!strcmp (arg, "--help"))
            {
              usage ();
              printf ("sizeof(wppLexer) = %u\n",
                      (unsigned int)sizeof (wppLexer));
              printf ("sizeof(wppExec) = %u\n",
                      (unsigned int)sizeof (wppExec));
              printf ("sizeof(wppObject) = %u\n",
                      (unsigned int)sizeof (wppObject));
              printf ("sizeof(wppToken) = %u\n",
                      (unsigned int)sizeof (wppToken));
              return 0;
            }
        }
      else
        filename = arg;
    }

  if (!filename)
    {
      puts ("wpp: no filename provided");
      return 1;
    }

  /* read entire file */
  fd = open (argv[1], 0, 0);
  if (fd == -1)
    {
      perror ("wpp: open");
      return 1;
    }

  if (fstat (fd, &stat) == -1)
    {
      perror ("wpp: fstat");
      return 1;
    }

  if (!stat.st_size)
    {
      puts ("wpp: stat.st_size: file is empty");
      return 1;
    }

  /* map the file contents */
  code = mmap (0, stat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  {
    lexer = wpp_lexer_new (code);
    exec = wpp_exec_new (lexer);

    wpp_exec_run (exec);
    if (*exec->error)
      {
        printf ("wpp: %s\n", exec->error);
      }

    wpp_lexer_free (lexer);
    wpp_exec_free (exec);
  }

  /* close & free up the file buffer and the file itself */
  munmap (code, stat.st_size);
  close (fd);
  return 0;
}

static void
usage (void)
{
  puts ("usage: wpp [filename] [options]...");
}
