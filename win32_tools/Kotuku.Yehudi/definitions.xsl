<?xml version="1.0" encoding="utf-8" ?>
<stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                xmlns:kt="http://kotuku.aero/schema/kotuku.xsd"
                xmlns="http://www.w3.org/1999/XSL/Transform">
  <output method="text" indent="yes" />
  <param name="app-name"></param>

  <template match="//kt:application">
#ifndef __<value-of select="$app-name"/>__h__
#define __<value-of select="$app-name"/>__h__

#include "../../libs/proton/proton.h"
#include "../../libs/proton/menu_window.h"


#ifdef __cplusplus
extern "C" {
#endif

<for-each select="./kt:id">
  <variable name="id" select="position()"/>
#define <value-of select="."/>  (id_app_action_id_first + <value-of select="$id"/>)
</for-each>

<for-each select="./kt:menu/kt:menu/kt:spin-edit-action">
  <if test="./@get-method">
  extern result_t <value-of select="./@get-method"/>(menu_item_spin_edit_t *edit, int32_t *value);
  </if>
  <if test="./@set-method">
  extern result_t <value-of select="./@set-method"/>(menu_item_spin_edit_t *edit, int32_t value);
  </if>
</for-each>

<for-each select="./kt:menu/kt:menu/kt:checklist-action">
  extern result_t <value-of select="./@get-index"/>(menu_item_checklist_t *checklist, uint16_t *value);
</for-each>

  <for-each select="./kt:alarm-dialog/kt:alarm">
    <if test="./@message-fmt">
  extern result_t <value-of select="./@message-fmt"/>(menu_item_spin_edit_t *edit, int32_t *value);
    </if>
    <if test="./@message-fmt">
  extern result_t <value-of select="./@message-fmt"/>(menu_item_spin_edit_t *edit, int32_t value);
    </if>
  </for-each>

    <for-each select="//@on-message">
  extern result_t <value-of select="."/>(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    </for-each>

    <for-each select="//@on-paint">
  extern void <value-of select="."/>(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    </for-each>

    <for-each select="//@on-create">
  extern result_t <value-of select="."/>(handle_t hwnd, widget_t *widget);
    </for-each>

    extern result_t create_<value-of select="$app-name"/>(handle_t hwnd, aircraft_t *aircraft);

#ifdef __cplusplus
}
#endif

#endif
  </template>
</stylesheet>