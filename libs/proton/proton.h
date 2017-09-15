#ifndef __proton_h__
#define	__proton_h__

#ifdef	__cplusplus
extern "C"
  {
#endif

/**
 * @file proton.h
 * Proton widget library definitions
 */

  /**
   * @function run_proton(void *parg)
   * Run the main window application loop
   * @param parg  Argument to pass
   */
  extern void run_proton(void *parg);

/**
 * @function load_layout(handle_t canvas, memid_t hive)
 * Load a registry hive that describes a series of windows
 * @param canvas    Parent window to create child windows within
 * @param hive      list of keys that describe each window
 * @return s_ok if loaded ok
 */
extern result_t load_layout(handle_t canvas, memid_t hive);
  extern handle_t main_window;

#ifdef	__cplusplus
  }
#endif

#endif

