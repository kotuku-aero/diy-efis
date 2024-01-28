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

result_t krypton_create_flash_disk(memid_t key, flash_params_t *params, bool* init_mode, flash_disk_t** out)
  {
  *init_mode = false;

  memset(params, 0, sizeof(flash_params_t));

  char filename[REG_STRING_MAX];
  uint16_t len = REG_STRING_MAX;
  if(failed(reg_get_string(key, "filename", filename, &len)))
    strcpy(filename, "flash.img");

  if(failed(reg_get_uint32(key, "page-size", &params->page_size)))
    params->page_size = 4096;

  if(failed(reg_get_uint32(key, "block-size", &params->block_size)))
    params->block_size = 4096 * 64;

  if(failed(reg_get_uint32(key, "block-count", &params->block_count)))
    params->block_count = 4096;

  if(failed(reg_get_uint32(key, "block-cycle", &params->block_cycles)))
    params->block_cycles = 100;

  if(failed(reg_get_uint32(key, "cache-size", &params->cache_size)))
    params->cache_size = 8192;

  if(failed(reg_get_uint32(key, "lookahead-size", &params->lookahead_size)))
    params->lookahead_size = 128;

  if(failed(reg_get_uint32(key, "name-max", &params->name_max)))
    params->name_max = 32;

  if(failed(reg_get_uint32(key, "file-max", &params->file_max)))
    params->file_max = 1024 * 1024 * 1024;

  reg_get_uint32(key, "metadata-max", &params->metadata_max);
  // check this...
  params->read_size = 4096;
  params->prog_size = 4096;

  // how big the file needs to be
  uint32_t disk_size = params->block_count * params->block_size;
  win_flash_disk_t* disk;
  result_t result;
  if (failed(result = neutron_calloc( 1, sizeof(win_flash_disk_t), (void**)&disk)))
    return result;

  disk->ff_handle = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(disk->ff_handle == INVALID_HANDLE_VALUE)
    {
    if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
      disk->ff_handle = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
      *init_mode = true;

      if(disk->ff_handle == INVALID_HANDLE_VALUE)
        return e_unexpected;
      }
    }

  // set up the filesystem types
  memcpy(&disk->params, params, sizeof(flash_params_t));

  disk->base.erase = flash_erase;
  disk->base.write = flash_write;
  disk->base.read = flash_read;
  disk->base.write_oob = flash_write_oob;
  disk->base.read_oob = flash_read_oob;
  disk->base.sync = flash_sync;
  disk->base.copy_page = flash_copy_page;

  *out = (flash_disk_t *) disk;

  return s_ok;
  }