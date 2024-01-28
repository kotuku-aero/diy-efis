#ifndef __mfd_h__
#define __mfd_h__

#include "../proton/proton.h"

/**
 * @brief Create a layout based on the layout definition
 * @param parent
 * @param key
 * @param aircraft
 * @param layout_fn
 * @return
*/
extern result_t create_layout(handle_t parent, memid_t key, create_widgets_fn layout_fn, handle_t* hndl);

#endif
