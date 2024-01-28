#include "mfd_cli.h"
#include "../../libs/neutron/vector.h"
#include "../../libs/neutron/type_vector.h"

type_vector_t(stat);


/**
 * @brief Split a path
 * @param path      path to split
 * @param dirname   directory name, call neutron_free when done
 * @param basename  end path
*/
static void splitpath(const char* path, char** dirname, const char** basename)
  {
  char* split;
  neutron_strdup(path, 0, &split);

  char* sep = split+1;
  char* str = split;
  str++;
  while (*str != 0)
    {
    if (*str == '/')
      sep = str;

    str++;
    }

  if (*sep == '/')
    {
    *sep = 0;
    *basename = ++sep;
    }

  if(*str != 0)
    *str++ = 0;


  *dirname = split;
  }

static void split_path(const char* path, char* out)
  {
  while (*path != 0)
    {
    if (*path == '/')
      *out = 0;
    else
      *out = *path;

    out++;
    path++;
    }

  *out = 0;
  }

static result_t combine_path(const char* base, const char* tail, char* buffer, size_t len)
  {
  // create a canoncical base (remove all . and ..)
  char* new_path;
  char* buffer_start = buffer;

  size_t base_len = strlen(base) + 1;
  size_t tail_len = (tail != 0) ? strlen(tail) + 1 : 0;

  neutron_malloc(base_len + tail_len, (void**)&new_path);

  // stringify the paths
  char* str = new_path;
  if (tail == 0 || tail[0] != '/')
    {
    // only process base if the tail is not a
    // rooted path
    split_path(base, str);
    str += base_len;
    }

  if(tail != 0)
    {
    // append the tail path
    split_path(tail, str);
    str += tail_len;
    }

  // store end of the strings
  char* end = str;

  // new_path is now a list of directories starting at
  // the root
  len--;

  str = new_path;
  while (str < end && len > 1)
    {
    if (*str != 0)
      {
      // assume just copy path
      char* next_dir = str;
      if (strcmp(str, ".")== 0)
        {
        next_dir = 0;
        str += 1;
        }
      else if (strcmp(str, "..")== 0)
        {
        // scan backwards
        if ((buffer - buffer_start) > 3)
          {
          buffer--;   // back to the previous term
          buffer--;   // back to the name

          while (*buffer != '/' && buffer > buffer_start)
            buffer--;

          buffer++;
          *buffer = 0;
          }

        str += 2;
        next_dir = 0;
        }

      if (next_dir != 0)
        {
        // test for drive at start
        bool new_dir = next_dir[1] != ':';

        if(new_dir)
          *buffer++ = '/';

        len--;
        while (*str != 0)
          {
          *buffer++ = *str++;
          len--;
          }

        //if(!new_dir)
        //  *buffer++ = '/';

        *buffer = 0;
        }
      }
    str++;
    }

  neutron_free(new_path);

  *buffer = 0;
  return s_ok;
  }

static result_t list_directory(cli_t* context, const char* path, bool ignore_wildcard, stats_t** matches)
  {
  result_t result;
  if (matches == 0)
    return e_bad_parameter;

  // split the path
  char* dirname;
  const char* fname = 0;
  splitpath(path, &dirname, &fname);

  size_t fname_length = 0;
  if (fname != 0)
    fname_length = strlen(fname);

  handle_t dirp;

  if (failed(result = open_directory(dirname, &dirp)))
    {
    neutron_free(dirname);
    return result;
    }

  *matches = 0;
  stat_t st;
  do
    {
    if (failed(result = read_directory(dirp, &st)))
      {
      if (result == e_not_found)
        result = s_ok;
      break;
      }

    bool add_name = true;
    uint16_t i;
    size_t name_len = strlen(st.name);

    for (i = 0; i < fname_length && i < name_len; i++)
      {
      if (ignore_wildcard && (fname[i] == '*' || fname[i] == '?'))
        break;            // found a wildcard so use this name

      if (fname[i] != st.name[i])
        {
        add_name = false;
        break;        // no match, either length or character
        }
      }

    // full match see if we add it
    if (add_name)
      {
      if (*matches == 0)
        {
        if (failed(result = stats_create(matches)))
          return result;
        }

      // add the type


      if (failed(stats_push_back(*matches, &st)))
        return result;
      }

    } while (true);

    neutron_free(dirname);
    close_handle(dirp);

    return s_ok;
  }

// special command to simulate a message from the can bus.
result_t recv_can_id_type_val1_val2_val3_val4_action(cli_t* context,
  uint16_t can_id, uint16_t type, const char* val1, const char* val2, const char* val3, const char* val4)
  {
  canmsg_t msg;
  result_t result;

  if (failed(result = create_can_msg(&msg, can_id, type, val1, val2, val3, val4)))
    return result;

  // enqueue the message onto the can bus.
  publish_local(&msg, INDEFINITE_WAIT);
  return s_ok;
  }

result_t shell_cp_src_dst_action(cli_t* context, const char* src_path, const char* dst_path)
  {
  result_t result;
  char new_src_path[256];
  if (src_path != 0 && src_path[0] != 0 && src_path[1] == ':')
    {
    strncpy(new_src_path, src_path, 255);
    new_src_path[255] = 0;
    }
  else
    // path combine
    combine_path(context->path, src_path, new_src_path, 256);

  char new_dst_path[256];
  if (dst_path != 0 && dst_path[0] != 0 && dst_path[1] == ':')
    {
    strncpy(new_dst_path, dst_path, 255);
    new_dst_path[255] = 0;
    }
  else
  {
    // path combine
    combine_path(context->path, dst_path, new_dst_path, 256);
  }

  stat_t st;

  if (failed(result = stream_stat(new_src_path, &st)))
    return result;

  if ((st.type & FILE_TYPE_DIR) != 0)
    {
    stream_puts(context->cfg.console_err, "Cannot copy from directory\r\n");
    return e_unexpected;
    }

  if (failed(result = stream_stat(new_dst_path, &st)))
    return result;

  if ((st.type & FILE_TYPE_DIR) != 0)
    {
    // get the filename from the src_path
    char *dirname;
    char *filename;
    splitpath(new_src_path, &dirname, &filename);

    char *path_copy;
    neutron_strdup(new_dst_path, 0, &path_copy);

    combine_path(path_copy, filename, new_dst_path, 256);

    neutron_free(path_copy);
    neutron_free(dirname);
    }

  handle_t src_handle;
  handle_t dst_handle;

  if(failed(result = stream_open(new_src_path, STREAM_O_RD, &src_handle)))
    return result;

  if (failed(result = stream_open(new_dst_path, STREAM_O_CREAT | STREAM_O_APPEND | STREAM_O_RDWR, &dst_handle)))
    {
    close_handle(src_handle);
    return result;
    }

  result = stream_copy(src_handle, dst_handle);

  close_handle(src_handle);
  close_handle(dst_handle);

  return s_ok;
  }

result_t shell_rm_name_action(cli_t* context, const char* path)
  {
  result_t result;
  char new_dir[256];
  if (path[0] == '/')
    {
    strncpy(new_dir, path, 255);
    new_dir[255] = 0;
    }
  else
    // path combine
    combine_path(context->path, path, new_dir, 256);

  return stream_delete(new_dir);
  }

result_t shell_mkdir_path_action(cli_t* context, const char* path)
  {
  result_t result;
  char new_dir[256];
  if (path[0] == '/')
    {
    strncpy(new_dir, path, 255);
    new_dir[255] = 0;
    }
  else
    // path combine
    combine_path(context->path, path, new_dir, 256);

  return create_directory(new_dir);
  }

result_t shell_rmdir_path_action(cli_t* context, const char* path)
  {
  result_t result;
  char new_dir[256];
  if (path[0] == '/')
    {
    strncpy(new_dir, path, 255);
    new_dir[255] = 0;
    }
  else
    // path combine
    combine_path(context->path, path, new_dir, 256);

  return remove_directory(new_dir);
  }

result_t shell_cd_path_action(cli_t* context, const char* path)
  {
  result_t result;
  char new_dir[256];

  // see if path is a rooted path this is starts with <char>:

   if (path != 0 && path[0] != 0 && path[1] == ':')
    {
    strncpy(new_dir, path, 255);
    new_dir[255] = 0;
    }
  else
    // path combine
    combine_path(context->path, path, new_dir, 256);

  stat_t st;
  if (failed(result = stream_stat(new_dir, &st)))
    return result;

  if ((st.type & FILE_TYPE_DIR) == 0)
    {
    stream_puts(context->cfg.console_err, "Not a directory");
    return e_not_file;
    }

  // save it
  strncpy(context->path, new_dir, 256);

  // TODO save the current directory

  char *prompt = context->prompt[context->root_level];

  // the prompt is either the full path or ...\<dirname>\<dirname>
  size_t dirlen = strlen(new_dir);

  *prompt = 0;
  const char* dirp = new_dir;

  int maxlen = MAX_PROMPT_LENGTH;

  if (dirlen > (maxlen - 2))
    {
    // this is the case where we have to prepend an elipsis
    strcpy(prompt, ".../");
    maxlen -= 4;

    // work backward on the dirname looking for the first directry marker
    dirp += dirlen - maxlen;

    while (*dirp != '/' && *dirp != 0)
      dirp++;
    }
  else
    strcpy(prompt, "");

  strncat(prompt, dirp, maxlen);

  return s_ok;
  }


result_t shell_ls_path_action(cli_t* context, const char* path)
  {
  uint16_t indent = 0;
  result_t result;
  stats_t* matches = 0;

  char search_path[256];
  if (path != 0 && path[0] != 0 && path[1] == ':')
    strncpy(search_path, path, 255);
  else
    // path combine
    combine_path(context->path, path, search_path, 256);

  size_t path_len = strlen(search_path);
  if (path_len > 1 && search_path[path_len - 1] != '/')
    {
    search_path[path_len] = '/';
    search_path[path_len +1] = 0;
    }

  // expand wildcards
  if (failed(result = list_directory(context, search_path, true, &matches)))
    return result;

  if(matches == 0)
    return s_ok;

  // work over each one.
  field_datatype dt = 0;
  memid_t child = 0;
  uint16_t i;

  // decide what to do
  uint16_t len = stats_count(matches);

  for (i = 0; i < len; i++)
    {

    if ((stats_begin(matches)[i].type & FILE_TYPE_FILE) != 0)
      stream_printf(context->cfg.console_out, "  %s (%d)\r\n", stats_begin(matches)[i].name, stats_begin(matches)[i].size);
    else
      stream_printf(context->cfg.console_out, "d %s\r\n", stats_begin(matches)[i].name);
    }

  stats_close(matches);

  return s_ok;
  }

result_t shell_exit_action(cli_t* context)
  {
  cli_submode_exit(context);

  return s_ok;
  }

result_t shell_action(cli_t* context)
  {
  strcpy(context->path, "F:/");
  return cli_submode_enter(context, 0, "fs");
  }

