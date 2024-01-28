#include "../../libs/neutron/neutron.h"
#include "../../libs/neutron/stream.h"

#undef STREAM_SEEK_SET
#undef STREAM_SEEK_END
#undef STREAM_SEEK_CUR


#include <Windows.h>

// this is the allocated buffer for the flash emulation  Maps to
// a file on windows.

// defined in krypton.c on windows.  Same as used for the registry.


typedef struct _win_image_stream_t {
  stream_t base;
  uint32_t image_length;

  uint32_t pos;
  } win_image_stream_t;

static typeid_t flash_file_type;

static result_t flash_file_etherealize(handle_t handle)
  {
  result_t result;
  win_image_stream_t* file;
  if (failed(result = is_typeof(handle, &flash_file_type, (void**)&file)))
    return result;

  return result;
  }

static typeid_t flash_file_type =
  {
  .name = "ffile",
  .base = &stream_type,
  .etherealize = flash_file_etherealize
  };


extern result_t create_or_open_mmap(const char* filename, uint32_t size,
  uint8_t fill_byte, bool* created,
  HANDLE* hndl, HANDLE* mm_hndl, uint8_t** buffer);

static HANDLE ff_handle;
static HANDLE mm_handle;
static uint8_t *flash_buffer;

static result_t erase_image(flash_disk_t *fs, uint32_t base, uint32_t length)
  {
  // just sanity check
  if (base > (1024 * 4096) ||
    (base + length) > (1024 * 4096))
    return e_bad_parameter;

  return s_ok;
  }

static result_t get_flash_address(win_image_stream_t *fs, uint32_t offset, const void **image)
  {
  *image = flash_buffer + offset;

  return s_ok;
  }

static result_t write_buffer(win_image_stream_t*ffs, const void *buffer, uint32_t offset, uint16_t size)
  {
  result_t result;
  if (offset > (1024 * 4096) ||
    (offset + size) > (1024 * 4096))
    return e_bad_parameter;

  uint8_t *dst;
  if (failed(result = get_flash_address(ffs, offset, (const void **)&dst)))
    return result;

  memcpy(dst, buffer, size);
  return s_ok;
  }

static result_t read_buffer(win_image_stream_t* ffs, uint32_t offset, uint16_t size, void* buffer)
  {
  result_t result;
  if (offset > (1024 * 4096) ||
    (offset + size) > (1024 * 4096))
    return e_bad_parameter;

  uint8_t* dst;
  if (failed(result = get_flash_address(ffs, offset, (const void**)&dst)))
    return result;

  memcpy(buffer, dst, size);

  return s_ok;
  }


///////////////////////////////////////////////////////////////////////////////
//
// Flash file
static result_t flash_file_stream_eof(handle_t stream)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  return ff->pos >= ff->image_length ? s_ok : s_false;
  }

static result_t flash_file_stream_read(handle_t stream, void* buffer, uint32_t size, uint32_t* read)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  if (buffer == 0 || size == 0)
    return e_bad_parameter;

  const void* image;
  if (failed(result = get_flash_address(ff, ff->pos, &image)))
    return result;

  // assume max read size
  size_t copy_size = ff->image_length - ff->pos;
  if (copy_size > size)
    copy_size = size;

  memcpy(buffer, image, copy_size);

  ff->pos += copy_size;

  if (read != 0)
    *read = (uint16_t)copy_size;

  return s_ok;
  }

static result_t flash_file_stream_write(handle_t stream, const void* buffer, uint32_t size)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  if (buffer == 0 || size == 0)
    return e_bad_parameter;

  if (failed(result = write_buffer(ff, buffer, ff->pos, size)))
    return result;

  ff->pos += size;

  if (ff->image_length < ff->pos)
    ff->image_length = ff->pos;

  return s_ok;
  }

static result_t flash_file_stream_rename(handle_t stream, const char* name)
  {
  return e_not_supported;
  }

static result_t flash_file_stream_getpos(handle_t stream, uint32_t* pos)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  if (pos == 0)
    return e_bad_parameter;

  *pos = ff->pos;
  return s_ok;
  }

static result_t flash_file_stream_setpos(handle_t stream, int32_t pos, uint32_t whence)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  if (pos >= ff->image_length)
    return e_bad_parameter;

  ff->pos = pos;

  return s_ok;
  }

static result_t flash_file_stream_length(handle_t stream, uint32_t* length)
  {
  result_t result;
  win_image_stream_t* ff;
  if (failed(result = is_typeof(stream, &flash_file_type, (void**)&ff)))
    return result;

  if (length == 0)
    return e_bad_parameter;

  *length = ff->image_length;

  return s_ok;
  }

static result_t flash_file_stream_truncate(handle_t stream, uint32_t length)
  {
  return e_invalid_operation;
  }

static result_t flash_file_stream_delete(handle_t stream)
  {
  return e_invalid_operation;
  }

static const char *win_flash_str = "win-flash";

result_t create_image(const char *name, uint32_t size, handle_t* fd)
  {
  result_t result;
  // we allocate a 4mb memory mapped file with 16kb blocks to emulate the 512k/1mb PIC flash memory
  // allows for a 4:1 code density.  Easy changed.

  if (failed(result = create_or_open_mmap(name, 1024 * 1024, 0, 0, &ff_handle, &mm_handle, &flash_buffer)))
    return result;

  win_image_stream_t *ff;

  if(failed(result = neutron_calloc(sizeof(win_image_stream_t), 1, (void **)&ff)))
    return result;

  ff->base.base.type = &flash_file_type;
  ff->base.eof = flash_file_stream_eof;
  ff->base.getpos = flash_file_stream_getpos;
  ff->base.length = flash_file_stream_length;
  ff->base.read = flash_file_stream_read;
  ff->base.setpos = flash_file_stream_setpos;
  ff->base.truncate = flash_file_stream_truncate;
  ff->base.write = flash_file_stream_write;

  ff->image_length = 1024 * 1024;

  *fd = ff;

  return s_ok;
  }
