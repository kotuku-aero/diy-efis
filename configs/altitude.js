
// todo put this in a library
function setting(name) {
  this.name = name;
  var _value = reg_get_uint16(0, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value },
    set: function (v) {
      _value = v;
      reg_set_uint16(0, this.name, v);
    }
  });
}

var air_density = 1.225;
var speed_f_sound = 340.29471111;
var isa_pressure = 1013.25;
var std_air_temp = 288.15;
// hpa per 1000ft at 15 deg c = 36.0843
var hpa_per_m = 0.118386811023622;
var last_altitude = 0;
var last_altitude_time = ticks();

var id_static_pressure = 326;

var vsi_bleed = new setting("vsi-rate");


function calculate_altitude(value) {
  // pick up the qnh
  var qnh = get_published_int16(id_qnh);
  // round to value
  value = Math.floor(value + 0.5);

  var now = ticks();

  //pressure_altitude = (1-pow(_value/1013.25, 0.190284)) * 44307.694;
  var pressure_altitude = (1 - Math.pow(_value * 0.000987167, 0.190284)) * 44307.694;
  var baro_corrected_altitude = (1 - Math.pow(_value / qnh, 0.190284)) * 44307.694;

  // we calculate the id_baro_corrected_altitude
  publish_float(id_baro_corrected_altitude, baro_corrected_altitude);
  publish_float(id_pressure_altitude, pressure_altitude);

  var num_vs_ticks = now - last_vs_time;
  // update vertical speed every 500msec
  if (num_vs_ticks > 500) {
    // calculate how may 500msec slots we are calculating for.
    var num_incr = num_vs_ticks / 500;

    vertical_speed = baro_corrected_altitude - last_altitude;

    // vertical speed is m/sec, calculate to num 0.5 secs
    vertical_speed /= num_incr;
    vertical_speed *= 2;       // convert to m/s

    num_incr = num_vs_ticks;
    num_incr %= 500;          // how much time of the last tick to calc

    last_altitude_time = now - num_incr;
    last_altitude = baro_corrected_altitude;

    // send to the publisher which should have an FIR filter applied to it.
    publish_float(id_altitude_rate, vertical_speed);
  }
}

function ev_msg(msg) {
  if (get_message_id(msg) == id_static_pressure)
    calculate_altitude(get_float(msg));
}
