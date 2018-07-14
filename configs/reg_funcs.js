exports.setting_uint32 = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_uint32(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_uint32(key, this.name, v);
    }
  });
}

exports.setting_uint16 = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_uint16(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_uint16(key, this.name, v);
    }
  });
}

exports.setting_int32 = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_int32(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_int32(key, this.name, v);
    }
  });
}

exports.setting_int16 = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_int16(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_int16(key, this.name, v);
    }
  });
}

exports.setting_float = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_float(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_float(key, this.name, v);
    }
  });
}

exports.setting_string = function (key, name) {
  this.name = name;
	this.key = key;
  var _value = reg_get_string(key, name);
  Object.defineProperty(this, 'value', {
    enumerable: false,
    configurable: false,
    get: function () { return _value; },
    set: function (v) {
      _value = v;
      reg_set_string(key, this.name, v);
    }
  });
}
