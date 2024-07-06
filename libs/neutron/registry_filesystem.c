#include "../neutron/neutron.h"

typedef struct _registry_fs_t {
  filesystem_t fs;
  memid_t root;
  } registry_fs_t;

  
static typeid_t registry_fs_type =
  {
  .name = "rfs",
  .base = &filesystem_type
  };
 
static result_t registry_fs_mount(filesystem_t *fs) 
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
      
  return s_ok; 
  }

static result_t registry_fs_unmount(filesystem_t *fs) 
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  return s_ok;
  }

static result_t registry_fs_fssync(filesystem_t *fs) 
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  return s_ok; 
  }

static result_t registry_fs_sync(filesystem_t *fs, handle_t fshndl)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  return s_ok; 
  }

static result_t registry_fs_freespace(filesystem_t *fs, const char *path, uint32_t *space)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
  
  // TODO
  *space = 512 * 1024;

  return s_ok; 
  }

static result_t registry_fs_totalspace(filesystem_t *fs, const char *path, uint32_t *space) 
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
  
  *space = 512 * 1024;

  return s_ok; 
  }

/*
 * char *realpath(const char *path);
 *
 * resolve a path.  No concept of working directory so all paths assumed
 * to be rooted at base of fs.  Replaces / with a \0 so paths are
 * in a canonical form
 */
static result_t canonical_path(const char *path, uint16_t *num_names, char **names)
  {
  result_t result;
  char *str;
  if(failed(result = neutron_malloc(strlen(path)+1, (void **) &str)))
    return result;
  
  char *out = str;
  const char *in = path;
  *num_names = 0;
  
  while(*in != 0)
    {
    if(*in == '/')
      {
      if(out > str)
        {
        // separator
        *out++ = 0;
        *num_names = *num_names + 1;
        }
        // otherwise ignore as leading /
      }
    else
      {
      *out++ = *in;
      }
    
    *in++;
    }
  
  if(out > str)
    *num_names = *num_names + 1;
  
  *out = 0;
  
  *names = str;
  
  return s_ok;
  }

static result_t registry_fs_open(filesystem_t *fs, 
                                 const char *path,
                                 uint32_t flags,
                                 handle_t *fd) 
  {
  if (fs == 0 || path == 0 || fd == 0)
    return e_bad_parameter;

  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  uint16_t num_names;
  char *names;
  // search for the file, given the path
  if(failed(result = canonical_path(path, &num_names, &names)))
    return result;
  
  const char *current_name = names;
  memid_t dir = reg_fs->root;
  for(uint16_t i = 0; i < num_names; i++)
    {
    if(i == num_names -1)
      {
      // must be a stream
      handle_t stream;
      if (failed(result = reg_stream_open(dir, current_name, flags, &stream)))
        {
        if(result == e_not_found && (flags & STREAM_O_CREAT) != 0)
          {
          // create the file
          if (failed(result = reg_stream_create(dir, current_name, flags, &stream)))
            break;
          }
        }
        break;
      
      *fd = stream;
      }
    else
      {
      if(failed(result = reg_open_key(dir, current_name, &dir)))
        break;
      }
    
    current_name += strlen(current_name) + 1;
    }
  
  neutron_free(names);

  return result;
  }


static result_t registry_fs_mkdir(filesystem_t *fs, const char *path)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  uint16_t num_names;
  char *names;
  // search for the file, given the path
  if(failed(result = canonical_path(path, &num_names, &names)))
    return result;
  
  const char *current_name = names;
  memid_t dir = reg_fs->root;
  for(uint16_t i = 0; i < num_names; i++)
    {
    if(i == num_names -1)
      {
      // must be a stream
      memid_t new_dir;
      result = reg_create_key(dir, current_name, &new_dir);
      break;
      }
    else
      {
      if(failed(result = reg_open_key(dir, current_name, &dir)))
        break;
      }
    }
  
  neutron_free(names);

  return result;
  }

static result_t registry_fs_rmdir(filesystem_t *fs, const char *path)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;

  uint16_t num_names;
  char *names;
  // search for the file, given the path
  if(failed(result = canonical_path(path, &num_names, &names)))
    return result;
  
  const char *current_name = names;
  memid_t dir = reg_fs->root;
  for(uint16_t i = 0; i < num_names; i++)
    {
    if(failed(result = reg_open_key(dir, current_name, &dir)))
     break;
    
    if(i == num_names -1)
      result = reg_delete_key(dir);
    }
  
  neutron_free(names);
  
  return result;
  }

typedef struct _dirp_t {
  base_t base;
  memid_t dir;
  memid_t child;
  bool search_dir;
  } krypton_dirp_t;

static typeid_t krypton_dirp_type;

static result_t dirp_etherealize(handle_t hndl)
  {
  result_t result;
  krypton_dirp_t *dirp;
  if(failed(result = is_typeof(hndl, &krypton_dirp_type, (void **)&dirp)))
    return result;
  
  // close
  memset(dirp, 0, sizeof(krypton_dirp_t));
  neutron_free(dirp);
  
  return s_ok;
  }
  
static typeid_t krypton_dirp_type =
  {
  .name = "rfdirp",
  .etherealize = dirp_etherealize
  };

static result_t registry_fs_opendir(filesystem_t *fs,
                                    const char *path,
                                    handle_t *handle)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
  
  // open the path first

  uint16_t num_names;
  char *names;
  // search for the file, given the path
  if(failed(result = canonical_path(path, &num_names, &names)))
    return result;
  
  const char *current_name = names;
  memid_t dir = reg_fs->root;
  for(uint16_t i = 0; i < num_names; i++)
    {
      if(failed(result = reg_open_key(dir, current_name, &dir)))
        break;
    }
  
  neutron_free(names);
  
  if(failed(result))
    return result;
  
  krypton_dirp_t *dirp;
  if(failed(result = neutron_malloc(sizeof(krypton_dirp_t), (void **)&dirp)))
    return result;
  
  dirp->base.type = &krypton_dirp_type;
  dirp->dir = dir;
  dirp->child = 0;
  dirp->search_dir = true;
  
  *handle = (handle_t) dirp;

  return s_ok; 
  }

static result_t registry_fs_readdir(filesystem_t *fs,
                                    handle_t handle,
                                    stat_t *st)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
  
  if(st)
    return e_bad_parameter;
  
  krypton_dirp_t *dirp;
  if(failed(result = is_typeof(handle, &krypton_dirp_type, (void **)&dirp)))
    return result;
  
  field_datatype ft = field_key;
  if(dirp->search_dir)
    {
    if(succeeded(result = reg_enum_key(dirp->dir, &ft, 0, 0, 0, st->name, &dirp->child)))
      {
      st->type = FILE_TYPE_DIR;
      return s_ok;
      }
    
    if(result != e_not_found)
      return result;            // some error.
    
    dirp->search_dir = false;
    dirp->child = 0;
    }
  
  ft = field_stream;
  
  if(succeeded(result = reg_enum_key(dirp->dir, &ft, 0, 0, 0, st->name, &dirp->child)))
    {
    st->type = FILE_TYPE_FILE;
    return s_ok;
    }
  
  return result;
  }

static result_t registry_fs_rewinddir(filesystem_t *fs, handle_t handle)
  {
  result_t result;
  registry_fs_t *reg_fs;
  if(failed(result = is_typeof(fs, &registry_fs_type, (void **) &reg_fs)))
    return result;
  
  
  if(handle == 0)
    return e_bad_parameter;
  
  krypton_dirp_t *dirp;
  if(failed(result = is_typeof(handle, &krypton_dirp_type, (void **)&dirp)))
    return result;
  
  dirp->child = 0;
  dirp->search_dir = true;
  return s_ok; 
  }

result_t create_registry_fs(memid_t root, filesystem_t **handle)
  {
  result_t result;
  // open the file system, and get the handle
  registry_fs_t *filesystem;
  if(failed(result = neutron_malloc(sizeof(registry_fs_t), (void **)&filesystem)))
    return result;

  filesystem->fs.freespace = registry_fs_freespace;
  filesystem->fs.fssync = registry_fs_fssync;
  filesystem->fs.mkdir = registry_fs_mkdir;
  filesystem->fs.mount = registry_fs_mount;
  filesystem->fs.open = registry_fs_open;
  filesystem->fs.opendir = registry_fs_opendir;
  filesystem->fs.readdir = registry_fs_readdir;
  filesystem->fs.rewinddir = registry_fs_rewinddir;
  filesystem->fs.rmdir = registry_fs_rmdir;
  filesystem->fs.sync = registry_fs_sync;
  filesystem->fs.totalspace = registry_fs_totalspace;
  filesystem->fs.unmount = registry_fs_unmount;

  
  filesystem->root = root;
  
  filesystem->fs.base.type = &registry_fs_type;

  *handle = &filesystem->fs;

  return s_ok;
  }