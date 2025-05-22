#include "exec.h"
#include "lexer.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static void usage (void);

int
main (int argc, char **argv)
{
  Lexer *lexer;
  Exec *exec;
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
              printf ("sizeof(Lexer) = %u\n", sizeof (Lexer));
              printf ("sizeof(Exec) = %u\n", sizeof (Exec));
              printf ("sizeof(Object) = %u\n", sizeof (Object));
              printf ("sizeof(Token) = %u\n", sizeof (Token));
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
    lexer = lexer_new (code);
    exec = exec_new (lexer);

    exec_run (exec);
    if (*exec->error)
      {
        printf ("wpp: %s\n", exec->error);
      }

    lexer_free (lexer);
    exec_free (exec);
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
