/* Create and destroy argument vectors (argv's)
   Copyright (C) 1992, 2001 Free Software Foundation, Inc.
   Written by Fred Fish @ Cygnus Support

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */


/*  Create and destroy argument vectors.  An argument vector is simply an
    array of string pointers, terminated by a NULL pointer. */

/*  Routines imported from standard C runtime libraries. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#define strdup _strdup
#endif

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include "argv.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8	/* Number of args + NULL in initial argv */

char **dupargv(char **argv)
  {
  int argc;
  char **copy;

  if (argv == NULL)
    return NULL;

  /* the vector */
  for (argc = 0; argv[argc] != NULL; argc++);
  copy = (char **)malloc((argc + 1) * sizeof(char *));
  if (copy == NULL)
    return NULL;

  /* the strings */
  for (argc = 0; argv[argc] != NULL; argc++)
    {
    int len = strlen(argv[argc]);
    copy[argc] = (char *)malloc(len + 1);
    if (copy[argc] == NULL)
      {
      freeargv(copy);
      return NULL;
      }
    strcpy(copy[argc], argv[argc]);
    }
  copy[argc] = NULL;
  return copy;
  }

void freeargv(char **vector)
  {
  register char **scan;

  if (vector != NULL)
    {
    for (scan = vector; *scan != NULL; scan++)
      {
      free(*scan);
      }
    free(vector);
    }
  }

char **buildargv(const char *input)
  {
  char *arg;
  char *copybuf;
  int squote = 0;
  int dquote = 0;
  int bsquote = 0;
  int argc = 0;
  int maxargc = 0;
  char **argv = NULL;
  char **nargv;

  if (input != NULL)
    {
    copybuf = (char *)alloca(strlen(input) + 1);
    /* Is a do{}while to always execute the loop once.  Always return an
 argv, even for null strings.  See NOTES above, test case below. */
    do
      {
      /* Pick off argv[argc] */
      while (isblank(*input))
        {
        input++;
        }
      if ((maxargc == 0) || (argc >= (maxargc - 1)))
        {
        /* argv needs initialization, or expansion */
        if (argv == NULL)
          {
          maxargc = INITIAL_MAXARGC;
          nargv = (char **)malloc(maxargc * sizeof(char *));
          }
        else
          {
          maxargc *= 2;
          nargv = (char **)realloc(argv, maxargc * sizeof(char *));
          }
        if (nargv == NULL)
          {
          if (argv != NULL)
            {
            freeargv(argv);
            argv = NULL;
            }
          break;
          }
        argv = nargv;
        argv[argc] = NULL;
        }
      /* Begin scanning arg */
      arg = copybuf;
      while (*input != EOS)
        {
        if (isspace(*input) && !squote && !dquote && !bsquote)
          {
          break;
          }
        else
          {
          if (bsquote)
            {
            bsquote = 0;
            *arg++ = *input;
            }
          else if (*input == '\\')
            {
            bsquote = 1;
            }
          else if (squote)
            {
            if (*input == '\'')
              {
              squote = 0;
              }
            else
              {
              *arg++ = *input;
              }
            }
          else if (dquote)
            {
            if (*input == '"')
              {
              dquote = 0;
              }
            else
              {
              *arg++ = *input;
              }
            }
          else
            {
            if (*input == '\'')
              {
              squote = 1;
              }
            else if (*input == '"')
              {
              dquote = 1;
              }
            else
              {
              *arg++ = *input;
              }
            }
          input++;
          }
        }
      *arg = EOS;
      argv[argc] = strdup(copybuf);
      if (argv[argc] == NULL)
        {
        freeargv(argv);
        argv = NULL;
        break;
        }
      argc++;
      argv[argc] = NULL;

      while (isspace(*input))
        {
        input++;
        }
      } while (*input != EOS);
    }
  return (argv);
  }

int writeargv(char **argv, FILE *f)
  {
  int status = 0;

  if (f == NULL)
    return 1;

  while (*argv != NULL)
    {
    const char *arg = *argv;

    while (*arg != EOS)
      {
      char c = *arg;

      if (isspace(c) || c == '\\' || c == '\'' || c == '"')
        if (EOF == fputc('\\', f))
          {
          status = 1;
          goto done;
          }

      if (EOF == fputc(c, f))
        {
        status = 1;
        goto done;
        }
      arg++;
      }

    if (EOF == fputc('\n', f))
      {
      status = 1;
      goto done;
      }
    argv++;
    }

done:
  return status;
  }

void expandargv(int *argcp, char ***argvp)
  {
  /* The argument we are currently processing.  */
  int i = 0;
  /* Non-zero if ***argvp has been dynamically allocated.  */
  int argv_dynamic = 0;
  /* Loop over the arguments, handling response files.  We always skip
     ARGVP[0], as that is the name of the program being run.  */
  while (++i < *argcp)
    {
    /* The name of the response file.  */
    const char *filename;
    /* The response file.  */
    FILE *f;
    /* An upper bound on the number of characters in the response
 file.  */
    long pos;
    /* The number of characters in the response file, when actually
 read.  */
    size_t len;
    /* A dynamically allocated buffer used to hold options read from a
 response file.  */
    char *buffer;
    /* Dynamically allocated storage for the options read from the
 response file.  */
    char **file_argv;
    /* The number of options read from the response file, if any.  */
    size_t file_argc;
    /* We are only interested in options of the form "@file".  */
    filename = (*argvp)[i];
    if (filename[0] != '@')
      continue;
    /* Read the contents of the file.  */
    f = fopen(++filename, "r");
    if (!f)
      continue;
    if (fseek(f, 0L, SEEK_END) == -1)
      goto error;
    pos = ftell(f);
    if (pos == -1)
      goto error;
    if (fseek(f, 0L, SEEK_SET) == -1)
      goto error;
    buffer = (char *)malloc(pos * sizeof(char) + 1);
    len = fread(buffer, sizeof(char), pos, f);
    if (len != (size_t)pos
      /* On Windows, fread may return a value smaller than POS,
         due to CR/LF->CR translation when reading text files.
         That does not in-and-of itself indicate failure.  */
      && ferror(f))
      goto error;
    /* Add a NUL terminator.  */
    buffer[len] = '\0';
    /* Parse the string.  */
    file_argv = buildargv(buffer);
    /* If *ARGVP is not already dynamically allocated, copy it.  */
    if (!argv_dynamic)
      {
      *argvp = dupargv(*argvp);
      if (!*argvp)
        {
        fputs("\nout of memory\n", stderr);
        exit(1);
        }
      }
    /* Count the number of arguments.  */
    file_argc = 0;
    while (file_argv[file_argc] && *file_argv[file_argc])
      ++file_argc;
    /* Now, insert FILE_ARGV into ARGV.  The "+1" below handles the
 NULL terminator at the end of ARGV.  */
    *argvp = ((char **) realloc(*argvp, (*argcp + file_argc + 1) * sizeof(char *)));
    memmove(*argvp + i + file_argc, *argvp + i + 1, (*argcp - i) * sizeof(char *));
    memcpy(*argvp + i, file_argv, file_argc * sizeof(char *));
    /* The original option has been replaced by all the new
 options.  */
    *argcp += file_argc - 1;
    /* Free up memory allocated to process the response file.  We do
 not use freeargv because the individual options in FILE_ARGV
 are now in the main ARGV.  */
    free(file_argv);
    free(buffer);
    /* Rescan all of the arguments just read to support response
 files that include other response files.  */
    --i;
  error:
    /* We're all done with the file now.  */
    fclose(f);
    }
  }

#ifdef MAIN

/* Simple little test driver. */

static const char *const tests[] =
  {
    "a simple command line",
    "arg 'foo' is single quoted",
    "arg \"bar\" is double quoted",
    "arg \"foo bar\" has embedded whitespace",
    "arg 'Jack said \\'hi\\'' has single quotes",
    "arg 'Jack said \\\"hi\\\"' has double quotes",
    "a b c d e f g h i j k l m n o p q r s t u v w x y z 1 2 3 4 5 6 7 8 9",

    /* This should be expanded into only one argument.  */
    "trailing-whitespace ",

    "",
    NULL
  };

int
main(void)
  {
  char **argv;
  const char *const *test;
  char **targs;

  for (test = tests; *test != NULL; test++)
    {
    printf("buildargv(\"%s\")\n", *test);
    if ((argv = buildargv(*test)) == NULL)
      {
      printf("failed!\n\n");
      }
    else
      {
      for (targs = argv; *targs != NULL; targs++)
        {
        printf("\t\"%s\"\n", *targs);
        }
      printf("\n");
      }
    freeargv(argv);
    }

  return 0;
  }

#endif