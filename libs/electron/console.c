/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "console.h"
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static int child_pid = 0;
static int child_stdin[2];
static int child_stdout[2];

int init_console()
  {

  return 0;
  }

static const char * shell = "/bin/bash";
static char * const argv[] = { "/bin/bash", "-i", 0 };

static const char *console_welcome = "diy-efis console 1.0\r\n";
static const char *console_exit = "diy-efis console closed\r\n";

int create_console(uint16_t channel_id)
  {
  // create the stdin pipe
  if(pipe(child_stdin) < 0)
    return -1;

  // create the stdout pipe
  if(pipe(child_stdout) < 0)
    {
    close(child_stdin[0]);
    close(child_stdin[1]);
    }

  // the child process will now read from stdin which is the wpipefd[0]
  // and write to stdout which is child_stdout[1]

  if(child_pid == 0)
    {
    switch(child_pid = fork())
      {
      case 0 :
        {
        // this the the child process running so close our duplicated
        // file handles
        close(child_stdin[1]);
        close(child_stdout[0]);
        dup2(child_stdin[0], STDIN_FILENO);
        dup2(child_stdout[1], STDOUT_FILENO);
        dup2(child_stdout[1], STDERR_FILENO);

        write(STDOUT_FILENO, console_welcome, strlen(console_welcome));
        close(child_stdout[1]);
        close(child_stdin[0]);

        // and execute our shell...
        execv(shell, argv);
        }
      case -1 :
        {
        close(child_stdin[0]);
        close(child_stdin[1]);
        close(child_stdout[0]);
        close(child_stdout[1]);

        return -1;
        }
      default :
        {
        // close the oposing ends of the pipes.
        close(child_stdin[0]);
        child_stdin[0] = 0;

        close(child_stdout[1]);
        child_stdout[1] = 0;
        }
        break;
      }
    }

  return 0;
  }

int close_console(uint16_t channel_id)
  {
  if(child_pid == 0)
    return -1;

  // terminate the process
  killpg(child_pid, SIGKILL);
  child_pid = 0;

  close(child_stdout[0]);
  child_stdout[0] = 0;

  close(child_stdin[1]);
  child_stdin[1] = 0;
  return 0;
  }

int send_console(uint16_t channel_id, char ch)
  {
  if(child_stdin[1] == 0)
    return -1;

  if(ch == '\r')
    ch = '\n';

  write(child_stdin[1], &ch, 1);
  return 0;
  }

int receive_console(uint16_t channel_id, char *ch, uint16_t len, uint16_t *chars_read)
  {
  *chars_read = 0;
  if(child_stdout[0] == 0)
    return -1;

  int result = -1;

  //Just a char by char read here
  if(len > 0 &&
      (result = read(child_stdout[0], ch, 1) > -1))
    {
    len--;
    *chars_read = *chars_read + 1;
//    if(*ch == '\n')
//      *ch = '\r';
    }

  return result;
  }

