/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include "neutron.h"
#include "stream.h"

static filesystem_t* mounts[26];

const typeid_t filesystem_type =
  {
  .name = "filesystem_t"
  };

result_t filesystem_init()
  {
  memset(mounts, 0, sizeof(filesystem_t *) * 26);

  return s_ok;
  }

/**
 * @brief Split the path into mount point and path
 * @param full_path  full path to be split
 * @param mount      located mount
 * @param path       query part
 * @return 
*/
static result_t locate_drive(const char *full_path, filesystem_t** mount, const char **path)
  {
  if (full_path == 0)
    return e_path_not_found;

  if (mount == 0 || path == 0)
    return e_bad_parameter;

  *mount = 0;

  if(full_path[0] == 0 || full_path[1] != ':')
    return e_bad_parameter;

  char drive = toupper(*full_path);

  if(drive < 'A' || drive > 'Z')
    return e_bad_parameter;

  drive -= 'A';

  if(mounts[drive] == 0)
    return e_path_not_found;

  *mount = mounts[drive];
  *path = full_path + 2;

  return s_ok;
  }

result_t mount(char drive, filesystem_t *file_system)
  {
  drive = toupper(drive);

  if(drive < 'A' || drive > 'Z')
    return e_bad_parameter;

  drive -= 'A';

  if(mounts[drive] != 0)
    return e_bad_parameter;

  mounts[drive] = file_system;
  return file_system->mount(file_system);
  }

result_t umount(handle_t hndl)
  {
  filesystem_t *fs;
  result_t result;
  if (failed(result = is_typeof(hndl, &filesystem_type, (void **)&fs)))
    return result;

  return fs->unmount(fs);
  }

extern result_t fs_sync(handle_t hndl)
  {
  filesystem_t* fs;
  result_t result;
  if (failed(result = is_typeof(hndl, &filesystem_type, (void**)&fs)))
    return result;

  return fs->fssync(fs);
  }

result_t stream_open(const char *full_path, uint32_t flags, handle_t *stream)
  {
  if (full_path == 0 || stream == 0)
    return e_bad_parameter;

  // get the file system to use.
  result_t result;
  filesystem_t *fs;
  const char *path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;
  return fs->open(fs, path, flags, stream);
  }

result_t create_directory(const char *full_path)
  {
  if (full_path == 0)
    return e_bad_pointer;
  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

  return fs->mkdir(fs, path);
  }

result_t remove_directory(const char *full_path)
  {
  if (full_path == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

  return fs->rmdir(fs, path);
  }

typedef struct _dirp_t {
  base_t base;
  handle_t fs_dirp;
  filesystem_t *mount;
  } dirp_t;

static typeid_t dirp_type = { "dirp_t" };

result_t open_directory(const char *dirname, handle_t *dirp)
  {
  if (dirname == 0 || dirp == 0)
    return e_bad_pointer;


  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(dirname, &fs, &path)))
    return result;

  handle_t fs_dirp;
  if (failed(result = fs->opendir(fs, path, &fs_dirp)))
    return result;

  dirp_t *dir;
  if (failed(result = neutron_malloc(sizeof(dirp_t), (void **)&dir)))
    return result;

  dir->base.type = &dirp_type;
  dir->fs_dirp = fs_dirp;
  dir->mount = fs;

  *dirp = (handle_t) dir;

  return s_ok;
  }

result_t read_directory(handle_t hndl, stat_t *stat)
  {
  result_t result;
  dirp_t *dirp;
  if (failed(result = is_typeof(hndl, &dirp_type, (void **) &dirp)))
    return result;

  return dirp->mount->readdir(dirp->mount, dirp->fs_dirp, stat);
  }

result_t rewind_directory(handle_t hndl)
  {
  result_t result;
  dirp_t *dirp;
  if (failed(result = is_typeof(hndl, &dirp_type, (void **)&dirp)))
    return result;

  return dirp->mount->rewinddir(dirp->mount, dirp->fs_dirp);
  }

result_t freespace(const char *full_path, uint32_t *space)
  {
  if (full_path == 0 || space == 0)
    return e_bad_pointer;


  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

  return fs->freespace(fs, path, space);
  }

result_t totalspace(const char *full_path, uint32_t *space)
  {
  if (full_path == 0 || space == 0)
    return e_bad_pointer;


  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

  return fs->totalspace(fs, path, space);
  }

result_t stream_stat(const char* full_path, stat_t* st)
  {
  if(full_path == 0 || st == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

   return fs->stat(fs, path, st);
  }

result_t stream_rename(const char* old_filename, const char* new_filename)
  {
  if(old_filename == 0 || new_filename == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  filesystem_t* old_fs;
  const char* old_path;
  if (failed(result = locate_drive(old_filename, &old_fs, &old_path)))
    return result;

  filesystem_t* new_fs;
  const char* new_path;
  if (failed(result = locate_drive(old_filename, &new_fs, &new_path)))
    return result;

  if(old_fs != new_fs)
    return e_bad_parameter;     // the paths must match.


  return old_fs->rename(old_fs, old_path, new_path);
  }

result_t stream_delete(const char* full_path)
  {
  if(full_path == 0)
    return e_bad_pointer;


  // get the file system to use.
  result_t result;
  filesystem_t* fs;
  const char* path;
  if (failed(result = locate_drive(full_path, &fs, &path)))
    return result;

  return fs->remove(fs, path);
  }
