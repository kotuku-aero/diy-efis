// script to handle the qnh up/down settings that are stored in the ahrs
var id_qnh_up = 1202;
var id_qnh_dn = 1203;
var id_heading_up = 1206;
var id_heading_dn = 1207;
var id_heading = 322;
var id_qnh = 319;

// todo put this in a library
function setting(name) {
  this.name = name;
  var _value = reg_get_uint16(0, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_uint16(0, this.name, v);
    }
  });
}

var qnh = new setting("qnh");
var hdg = new setting("hdg");

function adjust(the_setting, can_id, min_value, max_value, delta, wrap_around)
  {
  the_setting.value += delta;

  if (the_setting.value < min_value)
    the_setting.value = wrap_around ? max_value : min_value;
  else if (the_setting.value > max_value)
    the_setting.value = wrap_around ? min_value : max_value;

  publish(can_id, the_setting.value);
  }

// called when a msg arrives
function ev_msg(msg)
  {
  if(msg == 0)
    return;

  switch(get_message_id(msg))
    {
    case id_qnh_up :
      adjust(qnh, id_qnh, 800, 1050, 1, false);
      break;
    case id_qnh_dn :
      adjust(qnh, id_qnh, 800, 1050, -1, false);
      break;
    case id_heading_up :
      adjust(hdg, id_heading_angle, 1, true);
      break;
    case id_heading_dn :
      adjust(id_heading_angle, 0, 359, -1, true);
      break;
    }
  }
