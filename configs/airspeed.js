function setting_uint32(name) {
  this.name = name;
  var _value = reg_get_uint32(0, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_uint32(0, this.name, v);
    }
  });
}

var air_density = 1.225;
var speed_of_sound = 340.29471111;
var isa_pressure = 1013.25;
var std_air_temp = 288.15;

var last_tick = ticks();

var id_indicated_airspeed = 315;
var id_true_airspeed = 316;
var id_outside_air_temperature = 324;
var id_differential_pressure = 325;
var id_air_time = 1213;

// how long the aircraft has been flying (hobbs meter)
var airtime = new setting_uint32("airtime");

function calculate_airspeed(value)
{
  var indicated_airpseed;
  var oat = get_published_int16(id_outside_air_temperature);      // pick up the current OAT
  // This is calculated from a reduction of bernoulis theorum
  if (value < 0)
    value = 0;

  // the value is Hpa which is 100pa
  // This is calculated from a reduction of bernoulis theorum
  // pd = 0.5*p*(v*v)
  //
  // where pd is the dynamic pressure
  //  p is the density of air or 1.225 kg/m3
  //  v is the velocity in m/sec
  // replacing for pd yields
  // v = sqrt((2*pd)/1.225)
  // v = sqrt(2*pd*0.8163265306122449)
  // v = sqrt(pd*1.63265306122449)
  // where pd is in pascals which is 100x hpa so
  indicated_airspeed = Math.sqrt(value * 163.265306122449);

  // if speed is < 40kts then we trim it
  if (indicated_airspeed < 20)
  {
    indicated_airspeed = 0;
    true_airspeed = 0;
  }
  else
  {
    // aircraft going > 20m/s
    // get the 1ms tick count
    var now = ticks();

    var runtime = now - last_tick;
    // calculate the time in MS since the last tick

    // the engine runtime is in hours, only update each 1/100 hour
    // which is 36 seconds
    if (runtime > 35999)
    {
      var run_secs = runtime / 1000;
      var run_hours = run_secs / 3600;
      // remove whole hours (normally 0)
      run_secs -= run_hours * 3600;
      // calculate 1/100's of an hour to add
      var run_parts = run_secs / 36;

      airtime.value += run_hours * 100;
      airtime.value += run_parts;

      // send to the can bus
      publish_int32(id_air_time, ToInt32(airtime.value));

      // remove our calculated hours
      run_secs = (run_parts * 36) + (run_hours * 3600);
      // back to MS
      run_secs *= 1000;

      // and remove from accumulator
      runtime -= run_secs;

      // keep the overflow seconds accumulating by adding back the
      // amount accumulates
      last_tick = now - runtime;
    }
  }

  // calculate CAS
  var true_airspeed = speed_of_sound *
    Math.sqrt(((5 * oat) / 15) *
    (Math.pow(value / isa_pressure + 1, 0.28571428571423) - 1));

  publish_float(id_indicated_airspeed, indicated_airspeed);
  publish_float(id_true_airspeed, true_airspeed);
}

// the value is the dynamic pressure in HPA.
function ev_msg(msg)
{
  if (get_message_id(msg) == id_differential_pressure)
    calculate_airspeed(get_float(msg));
}
