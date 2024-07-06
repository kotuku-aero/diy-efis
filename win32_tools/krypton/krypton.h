#ifndef __krypton_h__
#define __krypton_h__
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

#include "../../libs/neutron/neutron.h"
#include "../../libs/neutron/stream.h"
#include "../../libs/muon/cli.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct _krypton_parameters_t {
    neutron_parameters_t neutron_params;
    bool factory_reset;
    uint16_t config_blocks;   // number of config blocks (32 bytes)
    const char *config_path;  // path to the config database
    bool init_gdi;            // needs gdi
    uint16_t width;           // gdi width
    uint16_t height;          // gdi height
    bool touch_tools;         // add a touch emulator toolbar to the screen
    bool init_slcan;          // initialize a serial can device
    const char *slcan_device; // device name to open
    // Only used by ion version.
    const char* fs_root;      // file system root to use for the local fs
    uint16_t debug_port;
    uint16_t console_port;
    const char* startup_app;  // optional app to start
    bool init_ion;            // needs interpreter
    bool init_muon;           // needs muon
    uint32_t flash_fs_blocks; // size of flash file system
    const char *ffs_base;     // name of ffs file
    const char *ffs_mount;    // ffs mount point
    } krypton_parameters_t;

/**
 * @function krypton_init(int argc, char **argv, krypton_parameters_t *init_params)
 * @brief initialize the windows krypton library
 * @param argc argument count
 * @param argv startup arguments
 * @param init_params other parameters.  The init will fill in most.
 * @return s_ok if arguments are ok
*/
extern result_t krypton_init(int argc, char **argv, krypton_parameters_t *init_params);
/**
 * @brief run the krypton command interpreter loop
 * @param console parameters with the console stream
 * @return 
*/
extern result_t run_krypton(stream_t *console, const char *prompt, cli_node_t *cli_root);

extern result_t krypton_fs_init(const char* root_path, filesystem_t** fs);

#ifdef __cplusplus
}
#endif

#endif
