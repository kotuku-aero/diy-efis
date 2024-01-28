#include "spatial.h"

fixed_t add_spatial_lat(fixed_t left, fixed_t right)
  {
  fixed_t result = add_fixed(left, right);

  int deg = fixed_to_int(result);
  // clamp it
  if (deg > 90)
    {
    result = sub_fixed(result, int_to_fixed(deg));
    result = add_fixed(result, int_to_fixed(90));
    }
  else if(fixed_to_int(result) < -90)
    {
    result = sub_fixed(result, int_to_fixed(deg));
    result = add_fixed(result, int_to_fixed(-90));
    }


  return result;
  }

fixed_t sub_spatial_lat(fixed_t left, fixed_t right)
  {
  fixed_t result = sub_fixed(left, right);

  int deg = fixed_to_int(result);
  // clamp it
  if (deg > 90)
    {
    result = sub_fixed(result, int_to_fixed(deg));
    result = add_fixed(result, int_to_fixed(90));
    }
  else if (fixed_to_int(result) < -90)
    {
    result = sub_fixed(result, int_to_fixed(deg));
    result = add_fixed(result, int_to_fixed(-90));
    }

  return result;
  }

fixed_t add_spatial_lng(fixed_t left, fixed_t right)
  {
  fixed_t result = add_fixed(left, right);

  int deg;
  // clamp it
  while ((deg = fixed_to_int(result)) > 179)
    result = sub_fixed(result, int_to_fixed(360));

  while ((deg = fixed_to_int(result)) < -180)
    result = add_fixed(result, int_to_fixed(360));

  return result;
  }

fixed_t sub_spatial_lng(fixed_t left, fixed_t right)
  {
  fixed_t result = sub_fixed(left, right);

  int deg;
  // clamp it
  while ((deg = fixed_to_int(result)) > 179)
    result = sub_fixed(result, int_to_fixed(360));

  while ((deg = fixed_to_int(result)) < -180)
    result = add_fixed(result, int_to_fixed(360));

  return result;
  }


static inline void swap_float(float* left, float* right)
  {
  float tmp = *left;
  *left = *right;
  *right = tmp;
  }

// quite expensive....
uint32_t spatial_distance(const latlng_t* from, const latlng_t* to)
  {
  float from_lat = fixed_to_float(from->lat);
  float from_lng = fixed_to_float(from->lng);

  float to_lat = fixed_to_float(to->lat);
  float to_lng = fixed_to_float(to->lng);

  // make 0..90
  from_lat += 90;
  to_lat += 90;

  // make 0..360
  from_lng += 180;
  to_lng += 180;

  if(from_lat > to_lat)
    swap_float(&from_lat, &to_lat);

  if(from_lng > to_lng)
    swap_float(&from_lng, &to_lng);

  float dist = sqrtf(powf(to_lat - from_lat, 2) + powf(to_lng - from_lng, 2));

  // distnce is degrees == 60nm
  dist *= 60;
  return (uint32_t) nm_to_meters(dist);
  }

static bool rotn_table_init = false;
static fixed_t rotn_table[90][2];

static void init_rotn_table()
  {
  if(rotn_table_init == true)
    return;

  fixed_t *ptr = (fixed_t *) rotn_table;

  *ptr++ = float_to_fixed(0.000000000f); *ptr++ = float_to_fixed(1.000000000f);
  *ptr++ = float_to_fixed(0.017452406f); *ptr++ = float_to_fixed(0.999847695f);
  *ptr++ = float_to_fixed(0.034899497f); *ptr++ = float_to_fixed(0.999390827f);
  *ptr++ = float_to_fixed(0.052335956f); *ptr++ = float_to_fixed(0.998629535f);
  *ptr++ = float_to_fixed(0.069756474f); *ptr++ = float_to_fixed(0.997564050f);
  *ptr++ = float_to_fixed(0.087155743f); *ptr++ = float_to_fixed(0.996194698f);
  *ptr++ = float_to_fixed(0.104528463f); *ptr++ = float_to_fixed(0.994521895f);
  *ptr++ = float_to_fixed(0.121869343f); *ptr++ = float_to_fixed(0.992546152f);
  *ptr++ = float_to_fixed(0.139173101f); *ptr++ = float_to_fixed(0.990268069f);
  *ptr++ = float_to_fixed(0.156434465f); *ptr++ = float_to_fixed(0.987688341f);
  *ptr++ = float_to_fixed(0.173648178f); *ptr++ = float_to_fixed(0.984807753f);
  *ptr++ = float_to_fixed(0.190808995f); *ptr++ = float_to_fixed(0.981627183f);
  *ptr++ = float_to_fixed(0.207911691f); *ptr++ = float_to_fixed(0.978147601f);
  *ptr++ = float_to_fixed(0.224951054f); *ptr++ = float_to_fixed(0.974370065f);
  *ptr++ = float_to_fixed(0.241921896f); *ptr++ = float_to_fixed(0.970295726f);
  *ptr++ = float_to_fixed(0.258819045f); *ptr++ = float_to_fixed(0.965925826f);
  *ptr++ = float_to_fixed(0.275637356f); *ptr++ = float_to_fixed(0.961261696f);
  *ptr++ = float_to_fixed(0.292371705f); *ptr++ = float_to_fixed(0.956304756f);
  *ptr++ = float_to_fixed(0.309016994f); *ptr++ = float_to_fixed(0.951056516f);
  *ptr++ = float_to_fixed(0.325568154f); *ptr++ = float_to_fixed(0.945518576f);
  *ptr++ = float_to_fixed(0.342020143f); *ptr++ = float_to_fixed(0.939692621f);
  *ptr++ = float_to_fixed(0.358367950f); *ptr++ = float_to_fixed(0.933580426f);
  *ptr++ = float_to_fixed(0.374606593f); *ptr++ = float_to_fixed(0.927183855f);
  *ptr++ = float_to_fixed(0.390731128f); *ptr++ = float_to_fixed(0.920504853f);
  *ptr++ = float_to_fixed(0.406736643f); *ptr++ = float_to_fixed(0.913545458f);
  *ptr++ = float_to_fixed(0.422618262f); *ptr++ = float_to_fixed(0.906307787f);
  *ptr++ = float_to_fixed(0.438371147f); *ptr++ = float_to_fixed(0.898794046f);
  *ptr++ = float_to_fixed(0.453990500f); *ptr++ = float_to_fixed(0.891006524f);
  *ptr++ = float_to_fixed(0.469471563f); *ptr++ = float_to_fixed(0.882947593f);
  *ptr++ = float_to_fixed(0.484809620f); *ptr++ = float_to_fixed(0.874619707f);
  *ptr++ = float_to_fixed(0.500000000f); *ptr++ = float_to_fixed(0.866025404f);
  *ptr++ = float_to_fixed(0.515038075f); *ptr++ = float_to_fixed(0.857167301f);
  *ptr++ = float_to_fixed(0.529919264f); *ptr++ = float_to_fixed(0.848048096f);
  *ptr++ = float_to_fixed(0.544639035f); *ptr++ = float_to_fixed(0.838670568f);
  *ptr++ = float_to_fixed(0.559192903f); *ptr++ = float_to_fixed(0.829037573f);
  *ptr++ = float_to_fixed(0.573576436f); *ptr++ = float_to_fixed(0.819152044f);
  *ptr++ = float_to_fixed(0.587785252f); *ptr++ = float_to_fixed(0.809016994f);
  *ptr++ = float_to_fixed(0.601815023f); *ptr++ = float_to_fixed(0.798635510f);
  *ptr++ = float_to_fixed(0.615661475f); *ptr++ = float_to_fixed(0.788010754f);
  *ptr++ = float_to_fixed(0.629320391f); *ptr++ = float_to_fixed(0.777145961f);
  *ptr++ = float_to_fixed(0.642787610f); *ptr++ = float_to_fixed(0.766044443f);
  *ptr++ = float_to_fixed(0.656059029f); *ptr++ = float_to_fixed(0.754709580f);
  *ptr++ = float_to_fixed(0.669130606f); *ptr++ = float_to_fixed(0.743144825f);
  *ptr++ = float_to_fixed(0.681998360f); *ptr++ = float_to_fixed(0.731353702f);
  *ptr++ = float_to_fixed(0.694658370f); *ptr++ = float_to_fixed(0.719339800f);
  *ptr++ = float_to_fixed(0.707106781f); *ptr++ = float_to_fixed(0.707106781f);
  *ptr++ = float_to_fixed(0.719339800f); *ptr++ = float_to_fixed(0.694658370f);
  *ptr++ = float_to_fixed(0.731353702f); *ptr++ = float_to_fixed(0.681998360f);
  *ptr++ = float_to_fixed(0.743144825f); *ptr++ = float_to_fixed(0.669130606f);
  *ptr++ = float_to_fixed(0.754709580f); *ptr++ = float_to_fixed(0.656059029f);
  *ptr++ = float_to_fixed(0.766044443f); *ptr++ = float_to_fixed(0.642787610f);
  *ptr++ = float_to_fixed(0.777145961f); *ptr++ = float_to_fixed(0.629320391f);
  *ptr++ = float_to_fixed(0.788010754f); *ptr++ = float_to_fixed(0.615661475f);
  *ptr++ = float_to_fixed(0.798635510f); *ptr++ = float_to_fixed(0.601815023f);
  *ptr++ = float_to_fixed(0.809016994f); *ptr++ = float_to_fixed(0.587785252f);
  *ptr++ = float_to_fixed(0.819152044f); *ptr++ = float_to_fixed(0.573576436f);
  *ptr++ = float_to_fixed(0.829037573f); *ptr++ = float_to_fixed(0.559192903f);
  *ptr++ = float_to_fixed(0.838670568f); *ptr++ = float_to_fixed(0.544639035f);
  *ptr++ = float_to_fixed(0.848048096f); *ptr++ = float_to_fixed(0.529919264f);
  *ptr++ = float_to_fixed(0.857167301f); *ptr++ = float_to_fixed(0.515038075f);
  *ptr++ = float_to_fixed(0.866025404f); *ptr++ = float_to_fixed(0.500000000f);
  *ptr++ = float_to_fixed(0.874619707f); *ptr++ = float_to_fixed(0.484809620f);
  *ptr++ = float_to_fixed(0.882947593f); *ptr++ = float_to_fixed(0.469471563f);
  *ptr++ = float_to_fixed(0.891006524f); *ptr++ = float_to_fixed(0.453990500f);
  *ptr++ = float_to_fixed(0.898794046f); *ptr++ = float_to_fixed(0.438371147f);
  *ptr++ = float_to_fixed(0.906307787f); *ptr++ = float_to_fixed(0.422618262f);
  *ptr++ = float_to_fixed(0.913545458f); *ptr++ = float_to_fixed(0.406736643f);
  *ptr++ = float_to_fixed(0.920504853f); *ptr++ = float_to_fixed(0.390731128f);
  *ptr++ = float_to_fixed(0.927183855f); *ptr++ = float_to_fixed(0.374606593f);
  *ptr++ = float_to_fixed(0.933580426f); *ptr++ = float_to_fixed(0.358367950f);
  *ptr++ = float_to_fixed(0.939692621f); *ptr++ = float_to_fixed(0.342020143f);
  *ptr++ = float_to_fixed(0.945518576f); *ptr++ = float_to_fixed(0.325568154f);
  *ptr++ = float_to_fixed(0.951056516f); *ptr++ = float_to_fixed(0.309016994f);
  *ptr++ = float_to_fixed(0.956304756f); *ptr++ = float_to_fixed(0.292371705f);
  *ptr++ = float_to_fixed(0.961261696f); *ptr++ = float_to_fixed(0.275637356f);
  *ptr++ = float_to_fixed(0.965925826f); *ptr++ = float_to_fixed(0.258819045f);
  *ptr++ = float_to_fixed(0.970295726f); *ptr++ = float_to_fixed(0.241921896f);
  *ptr++ = float_to_fixed(0.974370065f); *ptr++ = float_to_fixed(0.224951054f);
  *ptr++ = float_to_fixed(0.978147601f); *ptr++ = float_to_fixed(0.207911691f);
  *ptr++ = float_to_fixed(0.981627183f); *ptr++ = float_to_fixed(0.190808995f);
  *ptr++ = float_to_fixed(0.984807753f); *ptr++ = float_to_fixed(0.173648178f);
  *ptr++ = float_to_fixed(0.987688341f); *ptr++ = float_to_fixed(0.156434465f);
  *ptr++ = float_to_fixed(0.990268069f); *ptr++ = float_to_fixed(0.139173101f);
  *ptr++ = float_to_fixed(0.992546152f); *ptr++ = float_to_fixed(0.121869343f);
  *ptr++ = float_to_fixed(0.994521895f); *ptr++ = float_to_fixed(0.104528463f);
  *ptr++ = float_to_fixed(0.996194698f); *ptr++ = float_to_fixed(0.087155743f);
  *ptr++ = float_to_fixed(0.997564050f); *ptr++ = float_to_fixed(0.069756474f);
  *ptr++ = float_to_fixed(0.998629535f); *ptr++ = float_to_fixed(0.052335956f);
  *ptr++ = float_to_fixed(0.999390827f); *ptr++ = float_to_fixed(0.034899497f);
  *ptr++ = float_to_fixed(0.999847695f); *ptr++ = float_to_fixed(0.017452406f);

  rotn_table_init = true;
  };

fixed_t fixed_cos(int angle)
  {
  init_rotn_table();

  if(angle < 0)
    angle = 0-angle;

  if(angle >= 90)
    return 0;

  fixed_t result = rotn_table[angle][1];
  return result;
  }

typedef struct _quadrants_t
  {
  fixed_t s0, s1;
  fixed_t c0, c1;
  } quadrants_t;

// first 2 are sin conversions
// second are cos conversion
static const quadrants_t quadrants[4] =
  {
    { SPATIAL_ONE_DEGREE, 0, 0, SPATIAL_ONE_DEGREE },     // 0..89
    { 0, SPATIAL_ONE_DEGREE, SPATIAL_MINUS_ONE_DEGREE, 0 },     // 90..179
    { SPATIAL_MINUS_ONE_DEGREE, 0, 0, SPATIAL_MINUS_ONE_DEGREE },     // 180..269
    { 0, SPATIAL_MINUS_ONE_DEGREE, SPATIAL_ONE_DEGREE, 0 }
  };

// angle is in degrees, not radians.
result_t rotate_spatial_point(const spatial_point_t* center, int16_t angle, spatial_point_t* pt)
  {
  init_rotn_table();

  if (center == 0 || pt == 0)
    return e_bad_parameter;

  while (angle > 360)
    angle -= 360;

  while (angle < 0)
    angle += 360;

  if (angle == 0)
    return s_ok;

  int la = angle % 90;
  int qd = angle / 90;
  // convert the angle to a usefull form
  fixed_t cos_theta = add_fixed(
    mul_fixed(rotn_table[la][0], quadrants[qd].c0),
    mul_fixed(rotn_table[la][1], quadrants[qd].c1));

  fixed_t sin_theta = add_fixed(
    mul_fixed(rotn_table[la][0], quadrants[qd].s0),
    mul_fixed(rotn_table[la][1], quadrants[qd].s1));

  // calc the transformatio
  //   float x2f = (pt->x - center->x) * cos_theta - (pt->y - center->y) * sin_theta;

  fixed_t x2 = sub_fixed(
      mul_fixed(sub_fixed(pt->latlng.lat, center->latlng.lat), cos_theta),
      mul_fixed(sub_fixed(pt->latlng.lng, center->latlng.lng), sin_theta));

  // float y2f = (pt->x - center->x) * sin_theta + (pt->y - center->y) * cos_theta;
  fixed_t y2 = add_fixed(
      mul_fixed(sub_fixed(pt->latlng.lat, center->latlng.lat), sin_theta),
      mul_fixed(sub_fixed(pt->latlng.lng, center->latlng.lng), cos_theta));

  pt->latlng.lat = add_fixed(x2, center->latlng.lat);
  pt->latlng.lng = add_fixed(y2, center->latlng.lng);

  return s_ok;
  }

fixed_t round_fixed(fixed_t value)
  {
#ifdef FIXED_POINT
  if((value & 0x0000FFFF) >= 32768)
    value += 0x00010000;

  return value & 0xFFFF0000;
#else
  return roundf(value);
#endif
  }