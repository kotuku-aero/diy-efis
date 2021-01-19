using System;

namespace CanFly.Proton
{  /*
#	NUMERIC					A number editor
#		Params are:
#			type=<type>		Can aerospace type, usually SHORT or FLOAT
#			value=id		A canerospace value the will be the current value
#			set_value=id		Message to set the value
#			precision=num		Optional precision, if ommitted 0 is assumed and an integer value
#			digits=num		Number of digits, if precision is included will be <digits>.<precision>
#			min_value=num
#			max_value=num
#			circualar=<true/false>	If provided the editor will swap to min on overflow and max on underflow
#			decka_increment=<value>	Normally decka increments the digits, this sets by how much.  defaults to 1
#			deckb_increment=<value>	If a precision is set then this sets how much the minor value changes, otherwise allows for
#						incrementing the digits.
#  	example for a frequency:	type=FLOAT,value=nnn,set_value=nnn,precision=3,digits=2,min_value=108.0,max_value=136.75,circular=true,decka_increment=1,deckb_increment=0.25
   */

  class MenuItemEdit : MenuItem
  {
    private ushort value_id;

    private ushort set_id;


    private float min_value;
    private float max_value;
    private float decka_increment;
    private float deckb_increment;

    public MenuItemEdit(LayoutWidget widget, uint key)
      : base(widget)
    {
      /*
static MenuItem item_edit_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_edit_t *item = (menu_item_edit_t *)neutron_malloc(sizeof(menu_item_edit_t));
  memset(item, 0, sizeof(menu_item_edit_t));

  item.item.item_type = MenuItemType.MiEdit;

  load_item_defaults(wnd, &item.item, key);
  item.item.paint = item_edit_paint;
  item.item.evaluate = item_edit_evaluate;
  item.item.edit = item_edit_edit;
  item.item.event = item_edit_event;

  reg_get_uint16(key, "value-id", &item.value_id);

  // load the set-value message
  reg_get_uint16(key, "set-id", &item.set_id);

  // get the float point flag
  reg_get_bool(key, "send-short", &item.integer_type);

  // get the values
  reg_get_float(key, "min-value", &item.min_value);
  reg_get_float(key, "max-value", &item.max_value);
  reg_get_float(key, "decka-incr", &item.decka_increment);
  reg_get_float(key, "deckb-incr", &item.deckb_increment);

  reg_get_uint16(key, "precision", &item.precision);
  reg_get_uint16(key, "digits", &item.digits);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }
       */
    }

    public override void Paint(Rect area, bool isHighlighted)
    {
      base.Paint(area, isHighlighted);
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
     /*
      // check the enabler
      DefaultEnableHandler(wnd, item, msg);

      menu_item_edit_t* edit = (menu_item_edit_t*)item;
      // see if we update our value
      if (edit.value_id == get_can_id(msg))
        memcpy(&edit.value, msg, sizeof(CanFlyMsg));
      */
      return MenuItemActionResult.MiaNothing;
    }
  }
}
