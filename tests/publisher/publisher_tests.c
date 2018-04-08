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
// Basic can aerospace framework support files
#include "../../libs/electron/electron.h"
#include "../../libs/proton/proton.h"
#include "../../libs/muon/muon.h"
#include "../../libs/neutron/stream.h"
#include "../../libs/neutron/slcan.h"

#define CAN_TX_BUFFER_LEN 1024
#define CAN_RX_BUFFER_LEN 1024

#include <sys/stat.h>
#include "../cunit/headers/automated.h"
#include "../cunit/headers/cuerror.h"

static bool is_init = false;
static const char *ini_path = "test.reg";

static int suite_init()
  {
  if (!is_init)
    {
    bool factory_reset = false;

    // delete the registry if it exists
    struct stat st;
    if (stat(ini_path, &st) == 0)
      unlink(ini_path);

   electron_init(ini_path, factory_reset);

    neutron_parameters_t init_params;
    init_params.node_id = (uint8_t)mfd_node_id;
    init_params.node_type = unit_pi;
    init_params.hardware_revision = 0x11;
    init_params.software_revision = 0x10;
    init_params.tx_length = CAN_TX_BUFFER_LEN;
    init_params.tx_stack_length = 4096;
    init_params.rx_length = CAN_RX_BUFFER_LEN;
    init_params.rx_stack_length = 4096;
    init_params.publisher_stack_length = 4096;

    // create the publisher key
    reg_create_key(0, "neutron", 0);

    neutron_init(&init_params, false);
    }
  return 0;
  }

static int suite_clean()
  {
  return 0;
  }

static int test_publisher_setup()
  {
  CU_ASSERT(succeeded(reg_set_uint16(0, "test", 0)));
  CU_ASSERT(succeeded(define_datapoint(id_differential_pressure, 250, CANAS_DATATYPE_SHORT, 8, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_static_pressure, 250, CANAS_DATATYPE_SHORT, 8, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_def_utc, 1000, CANAS_DATATYPE_UCHAR3, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_gps_aircraft_latitude, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_gps_aircraft_longitude, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_gps_groundspeed, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_track, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_magnetic_heading, 1000, CANAS_DATATYPE_FLOAT, 0, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_indicated_airspeed, 250, CANAS_DATATYPE_FLOAT, 0, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_true_airspeed, 250, CANAS_DATATYPE_FLOAT, 0, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_wind_speed, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_wind_direction, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_gps_valid, 5000, CANAS_DATATYPE_USHORT, 0, true, true)));
  CU_ASSERT(succeeded(define_datapoint(id_air_time, 1000, CANAS_DATATYPE_UINT32, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_heading_angle, 1000, CANAS_DATATYPE_FLOAT, 0, false, true)));
  CU_ASSERT(succeeded(define_datapoint(id_qnh, 1000, CANAS_DATATYPE_SHORT, 0, false, true)));

  return 0;
  }

static CU_TestInfo publisher_setup_tests[] = {
    { "setup publisher", test_publisher_setup },
    CU_TEST_INFO_NULL
  };

static CU_SuiteInfo suites[] = {
    { "setup_registry", suite_init, suite_clean, NULL, NULL, publisher_setup_tests },
    CU_SUITE_INFO_NULL,
  };

void AddTests(void)
  {
  /* Register suites. */
  if (CU_register_suites(suites) != CUE_SUCCESS)
    {
    fprintf(stderr, "suite registration failed - %s\n",
      CU_get_error_msg());
    exit(EXIT_FAILURE);
    }
  }

static void print_example_results(void)
  {
  fprintf(stdout, "\n\nExpected Test Results:"
    "\n\n  Error Handling  Type      # Run   # Pass   # Fail"
    "\n\n  ignore errors   suites%9u%9u%9u"
    "\n                  tests %9u%9u%9u"
    "\n                  asserts%8u%9u%9u"
    "\n\n  stop on error   suites%9u%9u%9u"
    "\n                  tests %9u%9u%9u"
    "\n                  asserts%8u%9u%9u\n\n",
    14, 14, 3,
    31, 10, 21,
    89, 47, 42,
    4, 4, 1,
    12, 9, 3,
    12, 9, 3);
  }

// some glue functions for the slcan which can have tests
static comm_device_p driver;

#ifdef __linux__
static const char *bsp_hive = "electron";
#else
static const char *bsp_hive = "krypton";
#endif

result_t bsp_can_init(deque_p rx_queue)
  {
  result_t result;
  memid_t key;
  if (failed(result = reg_open_key(0, bsp_hive, &key)))
    return result;

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const canmsg_t *msg)
  {
  if (msg == 0 || driver == 0)
    return e_bad_parameter;

  return slcan_send(driver, msg);
  }

int main(int argc, char* argv[])
  {
  CU_BOOL Run = CU_FALSE;

  setvbuf(stdout, NULL, _IONBF, 0);
  const char *ini_path = "diy-efis.reg";

  if (argc > 1) {
    if (!strcmp("-i", argv[1])) {
      Run = CU_TRUE;
      CU_set_error_action(CUEA_IGNORE);
      }
    else if (!strcmp("-f", argv[1])) {
      Run = CU_TRUE;
      CU_set_error_action(CUEA_FAIL);
      }
    else if (!strcmp("-A", argv[1])) {
      Run = CU_TRUE;
      CU_set_error_action(CUEA_ABORT);
      }
    else if (!strcmp("-e", argv[1])) {
      print_example_results();
      }
    else {
      printf("\nUsage:  AutomatedTest [option]\n\n"
        "        Options: -i  Run, ignoring framework errors [default].\n"
        "                 -f  Run, failing on framework error.\n"
        "                 -A  Run, aborting on framework error.\n"
        "                 -e  Print expected test results and exit.\n"
        "                 -h  Print this message.\n"
        "                 -r <path> Set registry path.\n\n");
      }
    }
  else {
    Run = CU_TRUE;
    CU_set_error_action(CUEA_IGNORE);
    }

  if (CU_TRUE == Run) 
    {
    if (CU_initialize_registry()) 
      {
      printf("\nInitialization of Test Registry failed.");
      }
    else
      {
      AddTests();
      CU_set_output_filename("TestAutomated");

      CU_list_tests_to_file();
      CU_automated_run_tests();
      CU_cleanup_registry();
      }
    }

  return 0;
  }
