#include "../../libs/neutron/neutron.h"

#undef STREAM_SEEK_SET
#undef STREAM_SEEK_END
#undef STREAM_SEEK_CUR


#include <Windows.h>


extern result_t create_or_open_mmap(const char* filename, uint32_t size,
  uint8_t fill_byte, bool* created,
  HANDLE* hndl, HANDLE* mm_hndl, uint8_t** buffer);


typedef struct _win_flash_disk_t {
  flash_disk_t base;

  flash_params_t params;

  HANDLE ff_handle;

  } win_flash_disk_t;

static typeid_t flash_file_type;

static result_t  flash_disk_etherealize(handle_t handle)
  {
  result_t result;
  flash_disk_t* file;
  if (failed(result = is_typeof(handle, &flash_file_type, (void**)&file)))
    return result;

  // todo: cleanup

  return result;
  }

static typeid_t flash_file_type =
  {
  .name = "flash_disk",
  .base = &filesystem_type,
  .etherealize = flash_disk_etherealize
  };

static result_t flash_write(const flash_disk_t* _fs, uint32_t block, uint32_t page, uint32_t size, const void* buffer)
  {
  win_flash_disk_t* fs = (win_flash_disk_t*)_fs;

  // calc offset
  uint32_t offset = block * fs->params.block_size;
  offset += page * fs->params.page_size;

  SetFilePointer(fs->ff_handle, offset, 0, FILE_BEGIN);

  if(!WriteFile(fs->ff_handle, buffer, size, 0, 0))
    return e_unexpected;

  return s_ok;
  }

static result_t flash_read(const flash_disk_t* _fs, uint32_t block, uint32_t page, uint32_t size, void* buffer)
  {
  win_flash_disk_t* fs = (win_flash_disk_t*)_fs;

  uint32_t offset = block * fs->params.block_size;
  offset += page * fs->params.page_size;

  SetFilePointer(fs->ff_handle, offset, 0, FILE_BEGIN);

  if(!ReadFile(fs->ff_handle, buffer, size, 0, 0))
    return e_unexpected;

  return s_ok;
  }

static result_t flash_erase(const flash_disk_t* _fs, uint32_t block)
  {
  win_flash_disk_t* fs = (win_flash_disk_t*)_fs;
  result_t result;

  uint8_t page[4096];

  memset(page, 0xFF, fs->params.page_size);

  for (uint32_t i = 0; i < 64; i++)
    {
    if (failed(result = flash_write(fs, block, i, 4096, page)))
      return result;
    }

  return s_ok;
  }

static result_t flash_write_oob(const flash_disk_t* fs, uint32_t block, uint32_t page, uint32_t size, const void* buffer)
  {
  return e_not_implemented;
  }

static result_t flash_read_oob(const flash_disk_t* fs, uint32_t block, uint32_t page, uint32_t size, void* buffer)
  {
  return e_not_implemented;
  }

static result_t flash_sync(const flash_disk_t* fs)
  {
  return s_ok;
  }

static result_t flash_copy_page(const flash_disk_t* _fs,
  uint32_t src_block, uint32_t src_page,
  uint32_t dst_block, uint32_t dst_page,
  uint32_t offset, uint16_t size, const void* buffer)
  {
  return e_not_implemented;
  }
