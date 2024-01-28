#ifndef __argv_h__
#define __argv_h__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
/*

@deftypefn Extension char** dupargv (char **@var{vector})

Duplicate an argument vector.  Simply scans through @var{vector},
duplicating each argument until the terminating @code{NULL} is found.
Returns a pointer to the argument vector if successful.  Returns
@code{NULL} if there is insufficient memory to complete building the
argument vector.

@end deftypefn

*/

extern char **dupargv(char **argv);

  /*

@deftypefn Extension void freeargv (char **@var{vector})

Free an argument vector that was built using @code{buildargv}.  Simply
scans through @var{vector}, freeing the memory for each argument until
the terminating @code{NULL} is found, and then frees @var{vector}
itself.

@end deftypefn

*/

extern void freeargv(char **vector);

/*

@deftypefn Extension char** buildargv (char *@var{sp})

Given a pointer to a string, parse the string extracting fields
separated by whitespace and optionally enclosed within either single
or double quotes (which are stripped off), and build a vector of
pointers to copies of the string for each field.  The input string
remains unchanged.  The last element of the vector is followed by a
@code{NULL} element.

All of the memory for the pointer array and copies of the string
is obtained from @code{malloc}.  All of the memory can be returned to the
system with the single function call @code{freeargv}, which takes the
returned result of @code{buildargv}, as it's argument.

Returns a pointer to the argument vector if successful.  Returns
@code{NULL} if @var{sp} is @code{NULL} or if there is insufficient
memory to complete building the argument vector.

If the input is a null string (as opposed to a @code{NULL} pointer),
then buildarg returns an argument vector that has one arg, a null
string.

@end deftypefn

The memory for the argv array is dynamically expanded as necessary.

In order to provide a working buffer for extracting arguments into,
with appropriate stripping of quotes and translation of backslash
sequences, we allocate a working buffer at least as long as the input
string.  This ensures that we always have enough space in which to
work, since the extracted arg is never larger than the input string.

The argument vector is always kept terminated with a @code{NULL} arg
pointer, so it can be passed to @code{freeargv} at any time, or
returned, as appropriate.

*/

extern char **buildargv(const char *input);

/*

@deftypefn Extension int writeargv (const char **@var{argv}, FILE *@{file})

Write each member of ARGV, handling all necessary quoting, to the file
named by FILE, separated by whitespace.  Return 0 on success, non-zero
if an error occurred while writing to FILE.

@end deftypefn

*/

extern int writeargv(char **argv, FILE *f);
/*

@deftypefn Extension void expandargv (int *@var{argcp}, char ***@var{argvp})

The @var{argcp} and @code{argvp} arguments are pointers to the usual
@code{argc} and @code{argv} arguments to @code{main}.  This function
looks for arguments that begin with the character @samp{@@}.  Any such
arguments are interpreted as ``response files''.  The contents of the
response file are interpreted as additional command line options.  In
particular, the file is separated into whitespace-separated strings;
each such string is taken as a command-line option.  The new options
are inserted in place of the option naming the response file, and
@code{*argcp} and @code{*argvp} will be updated.  If the value of
@code{*argvp} is modified by this function, then the new value has
been dynamically allocated and can be deallocated by the caller with
@code{freeargv}.  However, most callers will simply call
@code{expandargv} near the beginning of @code{main} and allow the
operating system to free the memory when the program exits.

@end deftypefn

*/

extern void expandargv(int *argcp, char ***argvp);


#ifdef __cplusplus
  }
#endif

#endif

