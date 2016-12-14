/**
 *             This is a port of code available from the US NOAA.
 *
 *             The hard coded coefficients should be valid until 2015.
 *
 *             Updated coeffs from ..
 *             http://www.ngdc.noaa.gov/geomag/WMM/wmm_ddownload.shtml
 *
 *             NASA C source code ..
 *             http://www.ngdc.noaa.gov/geomag/WMM/wmm_wdownload.shtml
 *
 *             Major changes include:
 *                - No geoid model (altitude must be geodetic WGS-84)
 *                - Floating point calculation (not double precision)
 *                - Hard coded coefficients for model
 *                - Elimination of user interface
 *                - Elimination of dynamic memory allocation
 *
 *****************************************************************************/
#include "ahrs.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../dspic33f-lib/microkernel.h"
#include "../can-aerospace/parameter_definitions.h"
#include "gps.h"

// update the magnetic variation every 10 seconds.
#define WMM_UPDATE_RATE 10000

#define MAX_MODEL_DEGREES                   12
#define MAX_SECULAR_VARIATION_MODEL_DEGREES 12
#define NUMTERMS                                91             // ((MAX_MODEL_DEGREES+1)*(MAX_MODEL_DEGREES+2)/2);
#define NUMPCUP                                 92              // NUMTERMS +1
#define NUMPCUPS                                13             // MAX_MODEL_DEGREES +1

// internal structure definitions

typedef struct
  {
  float edition_date;
  float epoch; // Base time of Geomagnetic model epoch (yrs)
  char model_name[20];
  // float Main_Field_Coeff_G[NUMTERMS];	// C - Gauss coefficients of main geomagnetic model (nT)
  // float Main_Field_Coeff_H[NUMTERMS];	// C - Gauss coefficients of main geomagnetic model (nT)
  // float Secular_Var_Coeff_G[NUMTERMS];	// CD - Gauss coefficients of secular geomagnetic model (nT/yr)
  // float Secular_Var_Coeff_H[NUMTERMS];	// CD - Gauss coefficients of secular geomagnetic model (nT/yr)
  uint16_t max_degrees; // Maximum degree of spherical harmonic model
  uint16_t max_secular_degrees; // Maxumum degree of spherical harmonic secular model
  } magnetic_model_t;

typedef struct
  {
  float a; // semi-major axis of the ellipsoid
  float b; // semi-minor axis of the ellipsoid
  float fla; // flattening
  float epssq; // first eccentricity squared
  float eps; // first eccentricity
  float re; // mean radius of  ellipsoid
  } ellipsoid_t;

typedef struct
  {
  float Pcup[NUMPCUP]; // Legendre Function
  float dPcup[NUMPCUP]; // Derivative of Lagendre fn
  } legendre_function_t;

typedef struct
  {
  float relative_radius_power[MAX_MODEL_DEGREES + 1]; // [earth_reference_radius_km / sph. radius ]^n
  float cos_mlambda[MAX_MODEL_DEGREES + 1]; // cp(m)  - cosine of (m*spherical coord. longitude
  float sin_mlambda[MAX_MODEL_DEGREES + 1]; // sp(m)  - sine of (m*spherical coord. longitude)
  } spherical_harmonic_variables_t;

typedef struct
  {
  float declination; /* 1. Angle between the magnetic field vector and true north, positive east */
  float inclination; /*2. Angle between the magnetic field vector and the horizontal plane, positive down */
  float magnetic_field_strength; /*3. Magnetic Field Strength */
  float horizontal_magnetic_field_strength; /*4. Horizontal Magnetic Field Strength */
  float x; /*5. Northern component of the magnetic field vector */
  float y; /*6. Eastern component of the magnetic field vector */
  float z; /*7. Downward component of the magnetic field vector */
  float grid_variation; /*8. The Grid Variation */
  float declination_yearly_rate_of_change; /*9. yearly Rate of change in declination */
  float inclination_yearly_rate_of_change; /*10. yearly Rate of change in inclination */
  float magnetic_field_strength_yearly_rate_of_change; /*11. yearly rate of change in Magnetic field strength */
  float horizontal_magnetic_field_strength_yearly_rate_of_change; /*12. yearly rate of change in horizontal field strength */
  float x_yearly_rate_of_change; /*13. yearly rate of change in the northern component */
  float y_yearly_rate_of_change; /*14. yearly rate of change in the eastern component */
  float z_yearly_rate_of_change; /*15. yearly rate of change in the downward component */
  float grid_variationz_yearly_rate_of_change; /*16. yearly rate of chnage in grid variation */
  } geomagnetic_elements_t;

// Internal Function Prototypes
static int geodetic_to_spherical(lla_t *coord_geodetic,
                                 lla_t *coord_spherical);

// TODO: this is a check only, not needed as we know the date is valid
static int geomag(lla_t *coord_spherical,
                  lla_t *coord_geodetic,
                  geomagnetic_elements_t *geomagnetic_elements);
static int associated_legendre_function(lla_t *coord_spherical,
                                        uint16_t max_degrees,
                                        legendre_function_t *legendre_function);
static int calculate_gomagnetic_elements(xyz_t *MagneticResultsGeo,
                                         geomagnetic_elements_t *geomagnetic_elements);
static int calculate_secular_variation(xyz_t *magnetic_variation,
                                       geomagnetic_elements_t *magnetic_elements);
static int calculate_speherical_harmonic_variables(lla_t *coord_spherical,
                                                   uint16_t max_degrees,
                                                   spherical_harmonic_variables_t *sph_variables);
static int pcup_low(float *Pcup,
                    float *dPcup,
                    float x,
                    uint16_t max_degrees);
static int pcup_hi(float *Pcup,
                   float *dPcup,
                   float x,
                   uint16_t max_degrees);
static int rotate_magnetic_vector(lla_t *,
                                  lla_t *coord_geodetic,
                                  xyz_t *MagneticResultsSph, xyz_t *MagneticResultsGeo);
static int sec_var_summation(legendre_function_t *legendre_function,
                             spherical_harmonic_variables_t *sph_variables,
                             lla_t *coord_spherical,
                             xyz_t *magnetic_results);
static int sec_var_summation_special(spherical_harmonic_variables_t *sph_variables,
                                     lla_t *coord_spherical,
                                     xyz_t *magnetic_results);
static int summation(legendre_function_t *legendre_function,
                     spherical_harmonic_variables_t *sph_variables,
                     lla_t *coord_spherical,
                     xyz_t *magnetic_results);
static int summation_special(spherical_harmonic_variables_t *sph_variables,
                             lla_t *coord_spherical,
                             xyz_t *magnetic_results);
static float get_main_field_coeff_g(uint16_t index);
static float get_main_field_coeff_h(uint16_t index);
static float get_secular_var_coeff_g(uint16_t index);
static float get_secular_var_coeff_h(uint16_t index);
static bool get_mag_vector(const lla_t *pos, xyz_t *B);

typedef struct {
  int32_t main_field_g;
  int32_t main_field_h;
  int32_t secular_field_g;
  int32_t secular_field_h;
  } wmm_coefficient_t;
  
typedef struct {
  can_parameter_definition_t defn;
  magnetic_model_t magnetic_model;
  ellipsoid_t ellipsoid;
  wmm_coefficient_t coefficients[NUMTERMS];
  } wmm_parameters_t;

  // TODO: this should be init parameters...
static const wmm_parameters_t parameters = {
  {
  .data_type = 128,         // custom data type
  .length = sizeof(wmm_parameters_t)
  },
// model init
  {
    .edition_date         = 0.0f,       // edition date
    .epoch                = 2010.0f,    // epoch
    .model_name           = "WMM-2010", // name of the model
    .max_degrees          = MAX_MODEL_DEGREES, // max degrees of model
    .max_secular_degrees  = MAX_SECULAR_VARIATION_MODEL_DEGREES
  },
  // ellipsoid init
  {
  // Sets WGS-84 parameters
  .a = 6378.137f, // semi-major axis of the ellipsoid in km
  .b = 6356.7523142f, // semi-minor axis of the ellipsoid in km
    // 1.0f / 298.257223563f
  .fla = 0.003352811, // flattening
    // sqrt(1 - (parameters.ellipsoid.b * parameters.ellipsoid.b) / (parameters.ellipsoid.a * parameters.ellipsoid.a))
  .eps = 0.081819191, // first eccentricity
    // (parameters.ellipsoid.eps * parameters.ellipsoid.eps)
  .epssq = 0.00669438, // first eccentricity squared
  .re = 6371.2f // Earth's radius in km
  },
  // first column not used but it will be optimized out by compiler
  // This is read from the i2c EEPROM at startup.
  // Floating point values are stored as integers * 10
  {
  { 0, 0, 0, 0},
  { -294966,    0,  116,   0},
  { -15863, 49444,  165, -259},
  { -23966,    0, -121,   0},
  { 30261, 27077,  -44, -225},
  { 16686, -5761,   19, -118},
  { 13401,    0,   4,   0},
  { -23262,  -1602, -41,   73},
  { 12319,   2519, -29,  -39},
  { 6340,  -5366, -77,  -26},
  {    9126,     0, -18,   0},
  {    8089,   2864,  23,   11},
  { 1667, -2112, -87, 27},
  { -3571, 1643, 46, 39},
  { 894, -3091, -21, -8},
  { -2309, 0, -10, 0},
  { 3572, 446, 6, 4},
  { 2003, 1889, -18, 18},
  { -1411, -1182, -10, 12},
  { -1630, 0, 9, 40},
  { -78, 1009, 10, -06},
  { 728, 0, -2, 0},
  { 686, -208, -2, -2},
  { 760, 441, -1, -21},
  { -1414, 615, 20, -4},
  { -228, -663, -17, -6},
  { 132, 31, -3, 5},
  { -779, 550, 17, 9},
  { 805, 0, 1, 0},
  { -751, -579, -1, 7},
  { -47, -211, -6, 3},
  { 453, 65, 13, -1},
  { 139, 249, 4, -1},
  { 104, 70, 3, -8},
  { 17, -277, -7, -3},
  { 49, -33, 6, 3},
  { 244, 0, -1, 0},
  { 81, 110, 1, -1},
  { -145, -200, -6, 2},
  { -56, 119, 2, 4},
  { -193, -174, -2, 4},
  { 115, 167, 3, 1},
  { 109, 70, 3, -1},
  { -141, -108, -6, 4},
  { -37, 17, 2, 3},
  { 54, 0, 0, 0},
  { 94, -205, -1, 0},
  { 34, 115, 0, -2},
  { -52, 128, 3, 0},
  { 31, -72, -4, -1},
  { -124, -74, -3, 1},
  {  -7, 80, 1, 0},
  {  84, 21, -1, -2},
  {  -85, -61, -4, 3},
  {  -101, 70, -2, 2},
  { -20, 0, 0, 0},
  { -63, 28, 0, 1},
  { 9, -1, -1, -1},
  { -11, 47, 2, 0},
  { -2, 44, 0, -1},
  { 25, -72, -1, -1},
  { -3, -10, -2, 0},
  { 22, -39, 0, -1},
  { 31, -20, -1, -2},
  { -10, -20, -2, 0},
  { -28, -83, -2, -1},
  { 30, 0, 0, 0},
  { -15, 2, 0, 0},
  { -21, 17, 0, 1},
  { 17, -6, 1, 0},
  { -5, -18, 0, 1},
  { 5, 9, 0, 0},
  { -8, -4, 0, 1},
  { 4, -25, 0, 0},
  { 18, -13, 0, -1},
  { 01, -21, 0, -1},
  { 07, -19, -1, 0},
  { 38, -18, 0, -1},
  { -22, 0, 0, 0},
  { -2, -9, 0, 0},
  { 3, 3, 1, 0},
  { 10, 21, 1, 0},
  { -6, -25, -1, 0},
  { 9, 5, 0, 0},
  { -1, 6, 0, 1},
  { 5, 0, 0, 0},
  { -4, 1, 0, 0},
  { -4, 3, 0, 0},
  { 2, -9, 0, 0},
  { -8, -2, -1, 0},
  { 0, 9, 1, 0}
  }
  };

static float decimal_date;
static lla_t coord_spherical;
static lla_t coord_geodetic;
static geomagnetic_elements_t geomagnetic_elements;
static legendre_function_t legendre_function;
static spherical_harmonic_variables_t sph_variables;

static xyz_t magnetic_results_spherical;
static xyz_t magnetic_results_geodetic;
static xyz_t magnetic_results_spherical_var;
static xyz_t magnetic_results_geodetic_var;

extern event_t gps_position_updated;

static lla_t last_pos;

static void wmm_worker(void *parg)
  {
  bool need_wmm_update = true;
  while(true)
    {
    wait_for_event(&gps_position_updated, INDEFINITE_WAIT);
    if(!need_wmm_update &&
       !(need_wmm_update = (int16_t) last_pos.lat != (int16_t) ahrs_state.gps_position.lat))
      need_wmm_update = (int16_t) last_pos.lng != (int16_t) ahrs_state.gps_position.lng;
    
    if(need_wmm_update)
      {
      if(get_mag_vector(&ahrs_state.gps_position,
                        &ahrs_state.mag_flux))
        {
        // mag vector is updated.
        memcpy(&last_pos, &ahrs_state.gps_position, sizeof(lla_t));
        need_wmm_update = false;
        set_mask(SENSORUPDATES_mv);     // flag to filter that the magnetic vector is updated.
        }
      }
    }
  }

int8_t wmm_init(uint16_t *stack, uint16_t stack_size)
  {
  int8_t id;

  // create a low priority task
  id = create_task("WMM", stack, stack_size, wmm_worker, 0, NORMAL_PRIORITY - 10);
  
  //resume(id);
  return id;
  }

/**
 * Called to initialize the wmm module parameters
 * @param worker_task
 * @return 
 */
bool wmm_config(semaphore_t *worker_task)
  {
  // TODO: fix this
  signal(worker_task);
  return true; // and complete
  }

/**************************************************************************************
 *   Example use - very simple - only two exposed functions
 *
 *	Initialize(); // Set default values and constants
 *
 *	get_mag_vector(float Lat, float Lon, float Alt, uint16_t month, uint16_t day, uint16_t year, float B[3]);
 *	e.g. Iceland in may of 2012 = get_mag_vector(65.0, -20.0, 0.0, 5, 5, 2012, B);
 *	Alt is above the WGS-84 Ellipsoid
 *	B is the NED (XYZ) magnetic vector in nTesla
 **************************************************************************************/

static bool get_mag_vector(const lla_t *pos, xyz_t *B)
  {
  if(pos->lat < -90.0f)
    return false; // error

  if(pos->lat > 90.0f)
    return false; // error

  if(pos->lng < -180.0f)
    return false; // error

  if(pos->lng > 180.0f)
    return false; // error

  coord_geodetic.lng = pos->lng;
  coord_geodetic.lat = pos->lat;
  coord_geodetic.alt = pos->alt / 1000.0f; // convert to km

  // Convert from geodetic to Spherical Equations: 17-18, WMM Technical report
  if(geodetic_to_spherical(&coord_geodetic, &coord_spherical) < 0)
    return false; // error

  // Compute the geoMagnetic field elements and their time change
  if(geomag(&coord_spherical, &coord_geodetic, &geomagnetic_elements) < 0)
    return false; // error

  B->x = geomagnetic_elements.x * 1e-2f;
  B->y = geomagnetic_elements.y * 1e-2f;
  B->z = geomagnetic_elements.z * 1e-2f;
  
  return true;
  }

static int geomag(lla_t *coord_spherical,
                  lla_t *coord_geodetic,
                  geomagnetic_elements_t *geomagnetic_elements)
/*
   The main subroutine that calls a sequence of WMM sub-functions to calculate the magnetic field elements for a single point.
   The function expects the model coefficients and point coordinates as input and returns the magnetic field elements and
   their rate of change. Though, this subroutine can be called successively to calculate a time series, profile or grid
   of magnetic field, these are better achieved by the subroutine Grid.

   INPUT: ellipsoid
   coord_spherical
   coord_geodetic
   TimedMagneticModel

   OUTPUT : geomagnetic_elements

 */
  {
  // Compute Spherical Harmonic variables
  if(calculate_speherical_harmonic_variables(coord_spherical, parameters.magnetic_model.max_degrees, &sph_variables) < 0)
    return -2;

  // Compute ALF
  if(associated_legendre_function(coord_spherical, parameters.magnetic_model.max_degrees, &legendre_function) < 0)
    return -3;

  // Accumulate the spherical harmonic coefficients
  if(summation(&legendre_function, &sph_variables, coord_spherical, &magnetic_results_spherical) < 0)
    return -4;

  // Sum the Secular Variation Coefficients
  if(sec_var_summation(&legendre_function, &sph_variables, coord_spherical, &magnetic_results_spherical_var) < 0)
    return -5;

  // Map the computed Magnetic fields to Geodeitic coordinates
  if(rotate_magnetic_vector(coord_spherical, coord_geodetic, &magnetic_results_spherical, &magnetic_results_geodetic) < 0)
    return -6;

  // Map the secular variation field components to Geodetic coordinates
  if(rotate_magnetic_vector(coord_spherical, coord_geodetic, &magnetic_results_spherical_var, &magnetic_results_geodetic_var) < 0)
    return -7;

  // Calculate the Geomagnetic elements, Equation 18 , WMM Technical report
  if(calculate_gomagnetic_elements(&magnetic_results_geodetic, geomagnetic_elements) < 0)
    return -8;

  // Calculate the secular variation of each of the Geomagnetic elements
  if(calculate_secular_variation(&magnetic_results_geodetic_var, geomagnetic_elements) < 0)
    return -9;

  return 0;
  }

int calculate_speherical_harmonic_variables(lla_t *coord_spherical,
                                            uint16_t max_degrees,
                                            spherical_harmonic_variables_t *sph_variables)
/* Computes Spherical variables
   Variables computed are (a/r)^(n+2), cos_m(lamda) and sin_m(lambda) for spherical harmonic
   summations. (Equations 10-12 in the WMM Technical Report)
   INPUT   ellipsoid  data  structure with the following elements
   float a; semi-major axis of the ellipsoid
   float b; semi-minor axis of the ellipsoid
   float fla;  flattening
   float epssq; first eccentricity squared
   float eps;  first eccentricity
   float re; mean radius of  ellipsoid
   coord_spherical    A data structure with the following elements
   float lambda; ( longitude)
   float phig; ( geocentric latitude )
   float r;            ( distance from the center of the ellipsoid)
   max_degrees   integer     ( Maxumum degree of spherical harmonic secular model)\

   OUTPUT  sph_variables  Pointer to the   data structure with the following elements
   float relative_radius_power[MAX_MODEL_DEGREES+1];   [earth_reference_radius_km  sph. radius ]^n
   float cos_mlambda[MAX_MODEL_DEGREES+1]; cp(m)  - cosine of (mspherical coord. longitude)
   float sin_mlambda[MAX_MODEL_DEGREES+1];  sp(m)  - sine of (mspherical coord. longitude)
   CALLS : none
 */
  {
  float cos_lambda, sin_lambda;
  uint16_t m, n;

  cos_lambda = cos(degrees_to_radians(coord_spherical->lng));
  sin_lambda = sin(degrees_to_radians(coord_spherical->lng));

  /* for n = 0 ... model_order, compute (Radius of Earth / Spherica radius r)^(n+2)
     for n  1..max_degrees-1 (this is much faster than calling pow MAX_N+1 times).      */

  sph_variables->relative_radius_power[0] = (parameters.ellipsoid.re / coord_spherical->alt) * (parameters.ellipsoid.re / coord_spherical->alt);
  for(n = 1; n <= max_degrees; n++)
    {
    sph_variables->relative_radius_power[n] = sph_variables->relative_radius_power[n - 1] * (parameters.ellipsoid.re / coord_spherical->alt);
    }

  /*
     Compute cosf(m*lambda), sin(m*lambda) for m = 0 ... max_degrees
     cosf(a + b) = cosf(a)*cosf(b) - sin(a)*sin(b)
     sin(a + b) = cosf(a)*sin(b) + sin(a)*cosf(b)
   */
  sph_variables->cos_mlambda[0] = 1.0f;
  sph_variables->sin_mlambda[0] = 0.0f;

  sph_variables->cos_mlambda[1] = cos_lambda;
  sph_variables->sin_mlambda[1] = sin_lambda;
  for(m = 2; m <= max_degrees; m++)
    {
    sph_variables->cos_mlambda[m] = sph_variables->cos_mlambda[m - 1] * cos_lambda - sph_variables->sin_mlambda[m - 1] * sin_lambda;
    sph_variables->sin_mlambda[m] = sph_variables->cos_mlambda[m - 1] * sin_lambda + sph_variables->sin_mlambda[m - 1] * cos_lambda;
    }

  return 0; // OK
  }

int associated_legendre_function(lla_t *coord_spherical, uint16_t max_degrees, legendre_function_t *legendre_function)
/* Computes  all of the Schmidt-semi normalized associated Legendre
   functions up to degree max_degrees. If max_degrees <= 16, function pcup_low is used.
   Otherwise pcup_hi is called.
   INPUT  coord_spherical        A data structure with the following elements
   float lambda; ( longitude)
   float phig; ( geocentric latitude )
   float r;       ( distance from the center of the ellipsoid)
   max_degrees         integer          ( Maxumum degree of spherical harmonic secular model)
   legendre_function Pointer to data structure with the following elements
   float *Pcup;  (  pointer to store Legendre Function  )
   float *dPcup; ( pointer to store  Derivative of Lagendre function )

   OUTPUT  legendre_function  Calculated Legendre variables in the data structure

 */
  {
  float sin_phi = sin(degrees_to_radians(coord_spherical->lat)); /* sin  (geocentric latitude) */

  if(max_degrees <= 16 || (1 - fabs(sin_phi)) < 1.0e-10f)
    { /* If max_degrees is less tha 16 or at the poles */
    if(pcup_low(legendre_function->Pcup, legendre_function->dPcup, sin_phi, max_degrees) < 0)
      {
      return -1; // error
      }
    }
  else
    {
    if(pcup_hi(legendre_function->Pcup, legendre_function->dPcup, sin_phi, max_degrees) < 0)
      {
      return -2; // error
      }
    }

  return 0; // OK
  }

int summation(legendre_function_t *legendre_function,
              spherical_harmonic_variables_t *sph_variables,
              lla_t *coord_spherical,
              xyz_t *magnetic_results)
  {
  /* Computes Geomagnetic Field Elements x, y and z in Spherical coordinate system using
     spherical harmonic summation.

     The vector Magnetic field is given by -grad V, where V is Geomagnetic scalar potential
     The gradient in spherical coordinates is given by:

     dV ^     1 dV ^        1     dV ^
     grad V = -- r  +  - -- t  +  -------- -- p
     dr       r dt       r sin(t) dp

     INPUT :  legendre_function
     magnetic_model
     sph_variables
     coord_spherical
     OUTPUT : magnetic_results

     CALLS : summation_special

     Manoj Nair, June, 2009 Manoj.C.Nair@Noaa.Gov
   */

  uint16_t m, n, index;
  float cos_phi;

  magnetic_results->z = 0.0f;
  magnetic_results->y = 0.0f;
  magnetic_results->x = 0.0f;

  for(n = 1; n <= parameters.magnetic_model.max_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      index = (n * (n + 1) / 2 + m);

      /*		    max_degrees        (n+2)     n     m            m           m
              z =   -SUM (a/r)   (n+1) SUM  [g cosf(m p) + h sin(m p)] P (sin(phi))
                              n=1                   m=0   n            n           n  */
      /* Equation 12 in the WMM Technical report.  Derivative with respect to radius.*/
      magnetic_results->z -=
        sph_variables->relative_radius_power[n] *
        (get_main_field_coeff_g(index) *
         sph_variables->cos_mlambda[m] + get_main_field_coeff_h(index) * sph_variables->sin_mlambda[m])
        * (float) (n + 1) * legendre_function->Pcup[index];

      /*		  1 max_degrees  (n+2)    n     m            m           m
              y =    SUM (a/r) (m)  SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                         n=1             m=0   n            n           n  */
      /* Equation 11 in the WMM Technical report. Derivative with respect to longitude, divided by radius. */
      magnetic_results->y +=
        sph_variables->relative_radius_power[n] *
        (get_main_field_coeff_g(index) *
         sph_variables->sin_mlambda[m] - get_main_field_coeff_h(index) * sph_variables->cos_mlambda[m])
        * (float) (m) * legendre_function->Pcup[index];
      /*		   max_degrees  (n+2) n     m            m           m
              x = - SUM (a/r)   SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                         n=1         m=0   n            n           n  */
      /* Equation 10  in the WMM Technical report. Derivative with respect to latitude, divided by radius. */

      magnetic_results->x -=
        sph_variables->relative_radius_power[n] *
        (get_main_field_coeff_g(index) *
         sph_variables->cos_mlambda[m] + get_main_field_coeff_h(index) * sph_variables->sin_mlambda[m])
        * legendre_function->dPcup[index];
      }
    }

  cos_phi = cosf(degrees_to_radians(coord_spherical->lat));
  if(fabs(cos_phi) > 1.0e-10f)
    {
    magnetic_results->y = magnetic_results->y / cos_phi;
    }
  else
    {
    /* Special calculation for component - y - at Geographic poles.
     * If the user wants to avoid using this function,  please make sure that
     * the latitude is not exactly +/-90. An option is to make use the function
     * CheckGeographicPoles.
     */
    if(summation_special(sph_variables, coord_spherical, magnetic_results) < 0)
      {
      return -1; // error
      }
    }

  return 0; // OK
  }

int sec_var_summation(legendre_function_t *legendre_function,
                      spherical_harmonic_variables_t *
                      sph_variables, lla_t *coord_spherical, xyz_t *magnetic_results)
  {
  /*This Function sums the secular variation coefficients to get the secular variation of the Magnetic vector.
     INPUT :  legendre_function
     magnetic_model
     sph_variables
     coord_spherical
     OUTPUT : magnetic_results

     CALLS : sec_var_summation_special

   */

  uint16_t m, n, index;
  float cos_phi;

  magnetic_results->z = 0.0f;
  magnetic_results->y = 0.0f;
  magnetic_results->x = 0.0f;

  for(n = 1; n <= parameters.magnetic_model.max_secular_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      index = (n * (n + 1) / 2 + m);

      /*		    max_degrees        (n+2)     n     m            m           m
              z =   -SUM (a/r)   (n+1) SUM  [g cosf(m p) + h sin(m p)] P (sin(phi))
                              n=1                   m=0   n            n           n  */
      /*  Derivative with respect to radius.*/
      magnetic_results->z -=
        sph_variables->relative_radius_power[n] *
        (get_secular_var_coeff_g(index) *
         sph_variables->cos_mlambda[m] + get_secular_var_coeff_h(index) * sph_variables->sin_mlambda[m])
        * (float) (n + 1) * legendre_function->Pcup[index];

      /*		  1 max_degrees  (n+2)    n     m            m           m
              y =    SUM (a/r) (m)  SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                         n=1             m=0   n            n           n  */
      /* Derivative with respect to longitude, divided by radius. */
      magnetic_results->y +=
        sph_variables->relative_radius_power[n] *
        (get_secular_var_coeff_g(index) *
         sph_variables->sin_mlambda[m] - get_secular_var_coeff_h(index) * sph_variables->cos_mlambda[m])
        * (float) (m) * legendre_function->Pcup[index];
      /*		   max_degrees  (n+2) n     m            m           m
              x = - SUM (a/r)   SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                         n=1         m=0   n            n           n  */
      /* Derivative with respect to latitude, divided by radius. */

      magnetic_results->x -=
        sph_variables->relative_radius_power[n] *
        (get_secular_var_coeff_g(index) *
         sph_variables->cos_mlambda[m] + get_secular_var_coeff_h(index) * sph_variables->sin_mlambda[m])
        * legendre_function->dPcup[index];
      }
    }
  cos_phi = cosf(degrees_to_radians(coord_spherical->lat));
  if(fabs(cos_phi) > 1.0e-10f)
    {
    magnetic_results->y = magnetic_results->y / cos_phi;
    }
  else
    {
    /* Special calculation for component y at Geographic poles */
    if(sec_var_summation_special(sph_variables, coord_spherical, magnetic_results) < 0)
      {
      return -1; // error
      }
    }

  return 0; // OK
  }

int rotate_magnetic_vector(lla_t *coord_spherical,
                           lla_t *coord_geodetic,
                           xyz_t *MagneticResultsSph, xyz_t *MagneticResultsGeo)
/* Rotate the Magnetic Vectors to Geodetic Coordinates
   Manoj Nair, June, 2009 Manoj.C.Nair@Noaa.Gov
   Equation 16, WMM Technical report

   INPUT : coord_spherical : Data structure lla_t with the following elements
   float lambda; ( longitude)
   float phig; ( geocentric latitude )
   float r;       ( distance from the center of the ellipsoid)

   coord_geodetic : Data structure lla_t with the following elements
   float lambda; (longitude)
   float phi; ( geodetic latitude)
   float alt; (height above the ellipsoid (HaE) )
   float HeightAboveGeoid;(height above the Geoid )

   MagneticResultsSph : Data structure xyz_t with the following elements
   float x;     North
   float y;       East
   float z;    Down

   OUTPUT: MagneticResultsGeo Pointer to the data structure xyz_t, with the following elements
   float x;     North
   float y;       East
   float z;    Down

   CALLS : none

 */
  {
  /* Difference between the spherical and Geodetic latitudes */
  float Psi = degrees_to_radians(coord_spherical->lat - coord_geodetic->lat);

  /* Rotate spherical field components to the Geodeitic system */
  MagneticResultsGeo->z = MagneticResultsSph->x * sin(Psi) + MagneticResultsSph->z * cosf(Psi);
  MagneticResultsGeo->x = MagneticResultsSph->x * cosf(Psi) - MagneticResultsSph->z * sin(Psi);
  MagneticResultsGeo->y = MagneticResultsSph->y;

  return 0;
  }

int calculate_gomagnetic_elements(xyz_t *MagneticResultsGeo, geomagnetic_elements_t *geomagnetic_elements)
/* Calculate all the Geomagnetic elements from x,y and z components
   INPUT     MagneticResultsGeo   Pointer to data structure with the following elements
   float x;    ( North )
   float y;      ( East )
   float z;    ( Down )
   OUTPUT    geomagnetic_elements    Pointer to data structure with the following elements
   float declination; (Angle between the magnetic field vector and true north, positive east)
   float inclination; Angle between the magnetic field vector and the horizontal plane, positive down
   float magnetic_field_strength; Magnetic Field Strength
   float horizontal_magnetic_field_strength; Horizontal Magnetic Field Strength
   float x; Northern component of the magnetic field vector
   float y; Eastern component of the magnetic field vector
   float z; Downward component of the magnetic field vector
   CALLS : none
 */
  {
  geomagnetic_elements->x = MagneticResultsGeo->x;
  geomagnetic_elements->y = MagneticResultsGeo->y;
  geomagnetic_elements->z = MagneticResultsGeo->z;

  geomagnetic_elements->horizontal_magnetic_field_strength = sqrtf(MagneticResultsGeo->x * MagneticResultsGeo->x + MagneticResultsGeo->y * MagneticResultsGeo->y);
  geomagnetic_elements->magnetic_field_strength = sqrtf(geomagnetic_elements->horizontal_magnetic_field_strength * geomagnetic_elements->horizontal_magnetic_field_strength + MagneticResultsGeo->z * MagneticResultsGeo->z);
  geomagnetic_elements->declination = radians_to_degrees(atan2f(geomagnetic_elements->y, geomagnetic_elements->x));
  geomagnetic_elements->inclination = radians_to_degrees(atan2f(geomagnetic_elements->z, geomagnetic_elements->horizontal_magnetic_field_strength));

  return 0; // OK
  }

int calculate_secular_variation(xyz_t *magnetic_variation,
                                geomagnetic_elements_t *magnetic_elements)
/*This takes the Magnetic Variation in x, y, and z and uses it to calculate the secular variation of each of the Geomagnetic elements.
        INPUT     magnetic_variation   Data structure with the following elements
                                float x;    ( North )
                                float y;	  ( East )
                                float z;    ( Down )
        OUTPUT   magnetic_elements   Pointer to the data  structure with the following elements updated
                        float declination_yearly_rate_of_change; yearly Rate of change in declination
                        float inclination_yearly_rate_of_change; yearly Rate of change in inclination
                        float magnetic_field_strength_yearly_rate_of_change; yearly rate of change in Magnetic field strength
                        float horizontal_magnetic_field_strength_yearly_rate_of_change; yearly rate of change in horizontal field strength
                        float x_yearly_rate_of_change; yearly rate of change in the northern component
                        float y_yearly_rate_of_change; yearly rate of change in the eastern component
                        float z_yearly_rate_of_change; yearly rate of change in the downward component
                        float grid_variationz_yearly_rate_of_change;yearly rate of chnage in grid variation
        CALLS : none

 */
  {
  magnetic_elements->x_yearly_rate_of_change = magnetic_variation->x;
  magnetic_elements->y_yearly_rate_of_change = magnetic_variation->y;
  magnetic_elements->z_yearly_rate_of_change = magnetic_variation->z;
  magnetic_elements->horizontal_magnetic_field_strength_yearly_rate_of_change = 
    (magnetic_elements->x * magnetic_elements->x_yearly_rate_of_change + magnetic_elements->y * magnetic_elements->y_yearly_rate_of_change) / 
    magnetic_elements->horizontal_magnetic_field_strength; // See equation 19 in the WMM technical report
  magnetic_elements->magnetic_field_strength_yearly_rate_of_change =
    (magnetic_elements->x * magnetic_elements->x_yearly_rate_of_change +
     magnetic_elements->y * magnetic_elements->y_yearly_rate_of_change + magnetic_elements->z * magnetic_elements->z_yearly_rate_of_change) / magnetic_elements->magnetic_field_strength;
  magnetic_elements->declination_yearly_rate_of_change =
    180.0f / M_PI * (magnetic_elements->x * magnetic_elements->y_yearly_rate_of_change -
                       magnetic_elements->y * magnetic_elements->x_yearly_rate_of_change) / (magnetic_elements->horizontal_magnetic_field_strength * magnetic_elements->horizontal_magnetic_field_strength);
  magnetic_elements->inclination_yearly_rate_of_change =
    180.0f / M_PI * (magnetic_elements->horizontal_magnetic_field_strength * magnetic_elements->z_yearly_rate_of_change -
                       magnetic_elements->z * magnetic_elements->horizontal_magnetic_field_strength_yearly_rate_of_change) / (magnetic_elements->magnetic_field_strength * magnetic_elements->magnetic_field_strength);
  magnetic_elements->grid_variationz_yearly_rate_of_change = magnetic_elements->declination_yearly_rate_of_change;

  return 0; // OK
  }

// pcup_hi
static float f1[NUMPCUP];
static float f2[NUMPCUP];
static float PreSqr[NUMPCUP];

// pcup_low
static float schmidtQuasiNorm[NUMPCUP];

// summation_special
static float PcupS[NUMPCUPS];

static int pcup_hi(float *Pcup, float *dPcup, float x, uint16_t max_degrees)
/*	This function evaluates all of the Schmidt-semi normalized associated Legendre
        functions up to degree max_degrees. The functions are initially scaled by
        10^280 sin^m in order to minimize the effects of underflow at large m
        near the poles (see Holmes and Featherstone 2002, J. Geodesy, 76, 279-299).
        Note that this function performs the same operation as pcup_low.
        However this function also can be used for high degree (large max_degrees) models.

        Calling Parameters:
                INPUT
                        max_degrees:	 Maximum spherical harmonic degree to compute.
                        x:		cosf(colatitude) or sin(latitude).

                OUTPUT
                        Pcup:	A vector of all associated Legendgre polynomials evaluated at
                                        x up to max_degrees. The lenght must by greater or equal to (max_degrees+1)*(max_degrees+2)/2.
                  dPcup:   Derivative of Pcup(x) with respect to latitude

                CALLS : none
        Notes:

   Adopted from the FORTRAN code written by Mark Wieczorek September 25, 2005.

   Manoj Nair, Nov, 2009 Manoj.C.Nair@Noaa.Gov

   Change from the previous version
   The prevous version computes the derivatives as
   dP(n,m)(x)/dx, where x = sin(latitude) (or cosf(colatitude) ).
   However, the WMM Geomagnetic routines requires dP(n,m)(x)/dlatitude.
   Hence the derivatives are multiplied by sin(latitude).
   Removed the options for CS phase and normalizations.

   Note: In geomagnetism, the derivatives of ALF are usually found with
   respect to the colatitudes. Here the derivatives are found with respect
   to the latitude. The difference is a sign reversal for the derivative of
   the Associated Legendre Functions.

   The derivates can't be computed for latitude = |90| degrees.
 */
  {
  uint16_t k, kstart, m, n;
  float pm2, pm1, pmm, plm, rescalem, z, scalef;

  /*
   * Note: OP code change to avoid floating point equality test.
   * Was: if (fabs(x) == 1.0)
   */
  if(fabs(x) - 1.0f < 1e-9f)
    {
    // printf("Error in PcupHigh: derivative cannot be calculated at poles\n");
    return -2;
    }

  /* OP Change: 1.0e-280 is too small to store in a float - the compiler truncates
   * it to 0.0f, which is bad as the code below divides by scalef. */
  scalef = 1.0e-20f;

  for(n = 0; n <= 2 * max_degrees + 1; ++n)
    {
    PreSqr[n] = sqrtf((float) (n));
    }

  k = 2;

  for(n = 2; n <= max_degrees; n++)
    {
    k = k + 1;
    f1[k] = (float) (2 * n - 1) / (float) (n);
    f2[k] = (float) (n - 1) / (float) (n);
    for(m = 1; m <= n - 2; m++)
      {
      k = k + 1;
      f1[k] = (float) (2 * n - 1) / PreSqr[n + m] / PreSqr[n - m];
      f2[k] = PreSqr[n - m - 1] * PreSqr[n + m - 1] / PreSqr[n + m] / PreSqr[n - m];
      }
    k = k + 2;
    }

  /*z = sin (geocentric latitude) */
  z = sqrtf((1.0f - x) * (1.0f + x));
  pm2 = 1.0f;
  Pcup[0] = 1.0f;
  dPcup[0] = 0.0f;
  if(max_degrees == 0)
    {
    return -3;
    }
  pm1 = x;
  Pcup[1] = pm1;
  dPcup[1] = z;
  k = 1;

  for(n = 2; n <= max_degrees; n++)
    {
    k = k + n;
    plm = f1[k] * x * pm1 - f2[k] * pm2;
    Pcup[k] = plm;
    dPcup[k] = (float) (n) * (pm1 - x * plm) / z;
    pm2 = pm1;
    pm1 = plm;
    }

  pmm = PreSqr[2] * scalef;
  rescalem = 1.0f / scalef;
  kstart = 0;

  for(m = 1; m <= max_degrees - 1; ++m)
    {
    rescalem = rescalem * z;

    /* Calculate Pcup(m,m) */
    kstart = kstart + m + 1;
    pmm = pmm * PreSqr[2 * m + 1] / PreSqr[2 * m];
    Pcup[kstart] = pmm * rescalem / PreSqr[2 * m + 1];
    dPcup[kstart] = -((float) (m) * x * Pcup[kstart] / z);
    pm2 = pmm / PreSqr[2 * m + 1];
    /* Calculate Pcup(m+1,m) */
    k = kstart + m + 1;
    pm1 = x * PreSqr[2 * m + 1] * pm2;
    Pcup[k] = pm1 * rescalem;
    dPcup[k] = ((pm2 * rescalem) * PreSqr[2 * m + 1] - x * (float) (m + 1) * Pcup[k]) / z;
    /* Calculate Pcup(n,m) */
    for(n = m + 2; n <= max_degrees; ++n)
      {
      k = k + n;
      plm = x * f1[k] * pm1 - f2[k] * pm2;
      Pcup[k] = plm * rescalem;
      dPcup[k] = (PreSqr[n + m] * PreSqr[n - m] * (pm1 * rescalem) - (float) (n) * x * Pcup[k]) / z;
      pm2 = pm1;
      pm1 = plm;
      }
    }

  /* Calculate Pcup(max_degrees,max_degrees) */
  rescalem = rescalem * z;
  kstart = kstart + m + 1;
  pmm = pmm / PreSqr[2 * max_degrees];
  Pcup[kstart] = pmm * rescalem;
  dPcup[kstart] = -(float) (max_degrees) * x * Pcup[kstart] / z;

  return 0; // OK
  }

int pcup_low(float *Pcup, float *dPcup, float x, uint16_t max_degrees)
/*   This function evaluates all of the Schmidt-semi normalized associated Legendre
        functions up to degree max_degrees.

        Calling Parameters:
                INPUT
                        max_degrees:	 Maximum spherical harmonic degree to compute.
                        x:		cosf(colatitude) or sin(latitude).

                OUTPUT
                        Pcup:	A vector of all associated Legendgre polynomials evaluated at
                                        x up to max_degrees.
                   dPcup: Derivative of Pcup(x) with respect to latitude

        Notes: Overflow may occur if max_degrees > 20 , especially for high-latitudes.
        Use pcup_hi for large max_degrees.

   Writted by Manoj Nair, June, 2009 . Manoj.C.Nair@Noaa.Gov.

   Note: In geomagnetism, the derivatives of ALF are usually found with
   respect to the colatitudes. Here the derivatives are found with respect
   to the latitude. The difference is a sign reversal for the derivative of
   the Associated Legendre Functions.
 */
  {
  uint16_t n, m, index, index1, index2;
  float k, z;

  Pcup[0] = 1.0f;
  dPcup[0] = 0.0f;

  /*sin (geocentric latitude) - sin_phi */
  z = sqrtf((1.0f - x) * (1.0f + x));

  /*       First, Compute the Gauss-normalized associated Legendre  functions */
  for(n = 1; n <= max_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      index = (n * (n + 1) / 2 + m);
      if(n == m)
        {
        index1 = (n - 1) * n / 2 + m - 1;
        Pcup[index] = z * Pcup[index1];
        dPcup[index] = z * dPcup[index1] + x * Pcup[index1];
        }
      else if(n == 1 && m == 0)
        {
        index1 = (n - 1) * n / 2 + m;
        Pcup[index] = x * Pcup[index1];
        dPcup[index] = x * dPcup[index1] - z * Pcup[index1];
        }
      else if(n > 1 && n != m)
        {
        index1 = (n - 2) * (n - 1) / 2 + m;
        index2 = (n - 1) * n / 2 + m;
        if(m > n - 2)
          {
          Pcup[index] = x * Pcup[index2];
          dPcup[index] = x * dPcup[index2] - z * Pcup[index2];
          }
        else
          {
          k = (float) (((n - 1) * (n - 1)) - (m * m)) / (float) ((2 * n - 1)
                                                                 * (2 * n - 3));
          Pcup[index] = x * Pcup[index2] - k * Pcup[index1];
          dPcup[index] = x * dPcup[index2] - z * Pcup[index2] - k * dPcup[index1];
          }
        }
      }
    }
  /*Compute the ration between the Gauss-normalized associated Legendre
     functions and the Schmidt quasi-normalized version. This is equivalent to
     sqrt((m==0?1:2)*(n-m)!/(n+m!))*(2n-1)!!/(n-m)!  */

  schmidtQuasiNorm[0] = 1.0f;
  for(n = 1; n <= max_degrees; n++)
    {
    index = (n * (n + 1) / 2);
    index1 = (n - 1) * n / 2;
    /* for m = 0 */
    schmidtQuasiNorm[index] = schmidtQuasiNorm[index1] * (float) (2 * n - 1) / (float) n;

    for(m = 1; m <= n; m++)
      {
      index = (n * (n + 1) / 2 + m);
      index1 = (n * (n + 1) / 2 + m - 1);
      schmidtQuasiNorm[index] = schmidtQuasiNorm[index1] * sqrtf((float) ((n - m + 1) * (m == 1 ? 2 : 1)) / (float) (n + m));
      }
    }

  /* Converts the  Gauss-normalized associated Legendre
            functions to the Schmidt quasi-normalized version using pre-computed
            relation stored in the variable schmidtQuasiNorm */

  for(n = 1; n <= max_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      index = (n * (n + 1) / 2 + m);
      Pcup[index] = Pcup[index] * schmidtQuasiNorm[index];
      dPcup[index] = -dPcup[index] * schmidtQuasiNorm[index];
      /* The sign is changed since the new WMM routines use derivative with respect to latitude
         insted of co-latitude */
      }
    }

  return 0; // OK
  }

int summation_special(spherical_harmonic_variables_t *
                      sph_variables, lla_t *coord_spherical, xyz_t *magnetic_results)
/* Special calculation for the component y at Geographic poles.
   Manoj Nair, June, 2009 manoj.c.nair@noaa.gov
   INPUT: magnetic_model
   sph_variables
   coord_spherical
   OUTPUT: magnetic_results
   CALLS : none
   See Section 1.4, "SINGULARITIES AT THE GEOGRAPHIC POLES", WMM Technical report

 */
  {
  uint16_t n, index;
  float k, sin_phi;
  float schmidtQuasiNorm1;
  float schmidtQuasiNorm2;
  float schmidtQuasiNorm3;

  PcupS[0] = 1;
  schmidtQuasiNorm1 = 1.0f;

  magnetic_results->y = 0.0f;
  sin_phi = sin(degrees_to_radians(coord_spherical->lat));

  for(n = 1; n <= parameters.magnetic_model.max_degrees; n++)
    {
    /*Compute the ration between the Gauss-normalized associated Legendre
       functions and the Schmidt quasi-normalized version. This is equivalent to
       sqrt((m==0?1:2)*(n-m)!/(n+m!))*(2n-1)!!/(n-m)!  */

    index = (n * (n + 1) / 2 + 1);
    schmidtQuasiNorm2 = schmidtQuasiNorm1 * (float) (2 * n - 1) / (float) n;
    schmidtQuasiNorm3 = schmidtQuasiNorm2 * sqrtf((float) (n * 2) / (float) (n + 1));
    schmidtQuasiNorm1 = schmidtQuasiNorm2;
    if(n == 1)
      {
      PcupS[n] = PcupS[n - 1];
      }
    else
      {
      k = (float) (((n - 1) * (n - 1)) - 1) / (float) ((2 * n - 1) * (2 * n - 3));
      PcupS[n] = sin_phi * PcupS[n - 1] - k * PcupS[n - 2];
      }

    /*		  1 max_degrees  (n+2)    n     m            m           m
            y =    SUM (a/r) (m)  SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                       n=1             m=0   n            n           n  */
    /* Equation 11 in the WMM Technical report. Derivative with respect to longitude, divided by radius. */

    magnetic_results->y +=
      sph_variables->relative_radius_power[n] *
      (get_main_field_coeff_g(index) *
       sph_variables->sin_mlambda[1] - get_main_field_coeff_h(index) * sph_variables->cos_mlambda[1])
      * PcupS[n] * schmidtQuasiNorm3;
    }

  return 0; // OK
  }

static int sec_var_summation_special(spherical_harmonic_variables_t *
                              sph_variables, lla_t *coord_spherical, xyz_t *magnetic_results)
  {
  /*Special calculation for the secular variation summation at the poles.

     INPUT: magnetic_model
     sph_variables
     coord_spherical
     OUTPUT: magnetic_results
     CALLS : none

   */
  uint16_t n, index;
  float k, sin_phi;
  float schmidtQuasiNorm1;
  float schmidtQuasiNorm2;
  float schmidtQuasiNorm3;

  PcupS[0] = 1;
  schmidtQuasiNorm1 = 1.0f;

  magnetic_results->y = 0.0f;
  sin_phi = sin(degrees_to_radians(coord_spherical->lat));

  for(n = 1; n <= parameters.magnetic_model.max_secular_degrees; n++)
    {
    index = (n * (n + 1) / 2 + 1);
    schmidtQuasiNorm2 = schmidtQuasiNorm1 * (float) (2 * n - 1) / (float) n;
    schmidtQuasiNorm3 = schmidtQuasiNorm2 * sqrtf((float) (n * 2) / (float) (n + 1));
    schmidtQuasiNorm1 = schmidtQuasiNorm2;
    if(n == 1)
      {
      PcupS[n] = PcupS[n - 1];
      }
    else
      {
      k = (float) (((n - 1) * (n - 1)) - 1) / (float) ((2 * n - 1) * (2 * n - 3));
      PcupS[n] = sin_phi * PcupS[n - 1] - k * PcupS[n - 2];
      }

    /*		  1 max_degrees  (n+2)    n     m            m           m
            y =    SUM (a/r) (m)  SUM  [g cosf(m p) + h sin(m p)] dP (sin(phi))
                       n=1             m=0   n            n           n  */
    /* Derivative with respect to longitude, divided by radius. */

    magnetic_results->y +=
      sph_variables->relative_radius_power[n] *
      (get_secular_var_coeff_g(index) *
       sph_variables->sin_mlambda[1] - get_secular_var_coeff_h(index) * sph_variables->cos_mlambda[1])
      * PcupS[n] * schmidtQuasiNorm3;
    }

  return 0; // OK
  }

/**
 * @brief Comput the MainFieldCoeffH accounting for the date
 */
static float get_main_field_coeff_g(uint16_t index)
  {
  if(index >= NUMTERMS)
    {
    return 0;
    }

  uint16_t n, m, sum_index, a, b;

  float coeff = parameters.coefficients[index].main_field_g;
  // adjust to float from stored format
  coeff /= 10.0f;

  a = parameters.magnetic_model.max_secular_degrees;
  b = (a * (a + 1) / 2 + a);
  for(n = 1; n <= parameters.magnetic_model.max_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      sum_index = (n * (n + 1) / 2 + m);

      /* Hacky for now, will solve for which conditions need summing analytically */
      if(sum_index != index)
        {
        continue;
        }

      if(index <= b)
        {
        coeff += (decimal_date - parameters.magnetic_model.epoch) * get_secular_var_coeff_g(sum_index);
        }
      }
    }

  return coeff;
  }

static float get_main_field_coeff_h(uint16_t index)
  {
  if(index >= NUMTERMS)
    {
    return 0;
    }

  uint16_t n, m, sum_index, a, b;
  float coeff = parameters.coefficients[index].main_field_h;
  coeff /= 10;

  a = parameters.magnetic_model.max_secular_degrees;
  b = (a * (a + 1) / 2 + a);
  for(n = 1; n <= parameters.magnetic_model.max_degrees; n++)
    {
    for(m = 0; m <= n; m++)
      {
      sum_index = (n * (n + 1) / 2 + m);

      /* Hacky for now, will solve for which conditions need summing analytically */
      if(sum_index != index)
        {
        continue;
        }

      if(index <= b)
        {
        coeff += (decimal_date - parameters.magnetic_model.epoch) * get_secular_var_coeff_h(sum_index);
        }
      }
    }

  return coeff;
  }

static float get_secular_var_coeff_g(uint16_t index)
  {
  if(index >= NUMTERMS)
    {
    return 0;
    }

  float coeff = parameters.coefficients[index].secular_field_g;
  coeff /= 10.0f;
  return coeff;
  }

static float get_secular_var_coeff_h(uint16_t index)
  {
  if(index >= NUMTERMS)
    {
    return 0;
    }

  float coeff = parameters.coefficients[index].secular_field_h;
  coeff /= 10.0f;
  return coeff;
  }

static int geodetic_to_spherical(lla_t *coord_geodetic, lla_t *coord_spherical)
// Converts Geodetic coordinates to Spherical coordinates
// Convert geodetic coordinates, (defined by the WGS-84
// reference ellipsoid), to Earth Centered Earth Fixed Cartesian
// coordinates, and then to spherical coordinates.
  {
  float CosLat, SinLat, rc, xp, zp; // all local variables

  CosLat = cosf(degrees_to_radians(coord_geodetic->lat));
  SinLat = sin(degrees_to_radians(coord_geodetic->lat));

  // compute the local radius of curvature on the WGS-84 reference ellipsoid
  rc = parameters.ellipsoid.a / sqrtf(1.0f - parameters.ellipsoid.epssq * SinLat * SinLat);

  // compute ECEF Cartesian coordinates of specified point (for longitude=0)

  xp = (rc + coord_geodetic->alt) * CosLat;
  zp = (rc * (1.0f - parameters.ellipsoid.epssq) + coord_geodetic->alt) * SinLat;

  // compute spherical radius and angle lambda and phi of specified point

  coord_spherical->alt = sqrtf(xp * xp + zp * zp);
  coord_spherical->lat = radians_to_degrees(asinf(zp / coord_spherical->alt)); // geocentric latitude
  coord_spherical->lng = coord_geodetic->lng; // longitude

  return 0; // OK
  }
