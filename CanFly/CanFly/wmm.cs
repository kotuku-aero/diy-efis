using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;


namespace CanFly
{


  public class WorldMagneticModel
  {
    // TODO: should be read from wmm.cof
    private const int MAX_MODEL_DEGREES = 12;
    private const int MAX_SECULAR_VARIATION_MODEL_DEGREES = 12;

    private int numTerms;

    private const int NUMPCUP = 92;              // numTerms +1
    private const int NUMPCUPS = 13;             // MAX_MODEL_DEGREES +1

    public class MagneticModel
    {
      public DateTime EditionDate;
      public double Epoch; // Base time of Geomagnetic model Epoch (yrs)
      public string ModelName;
      public double[] MainFieldCoeffG;
      public double[] MainFieldCoeffH;
      public double[] SecularVarCoeffG;
      public double[] SecularVarCoeffH;
      public int MaxDegrees; // Maximum degree of spherical harmonic model
      public int MaxSecularDegrees; // Maxumum degree of spherical harmonic secular model
      public bool SecularVariationUsed; // Whether or not the magnetic secular variation vector will be needed by program

      public MagneticModel(int numTerms)
      {
        MainFieldCoeffG = new double[numTerms];
        MainFieldCoeffH = new double[numTerms];
        SecularVarCoeffG = new double[numTerms];
        SecularVarCoeffH = new double[numTerms];
      }
    };

    public class Ellipsoid
    {
      public double a; // semi-major axis of the ellipsoid
      public double b; // semi-minor axis of the ellipsoid
      public double fla; // flattening
      public double epssq; // first eccentricity squared
      public double eps; // first eccentricity
      public double re; // mean radius of  ellipsoid
    };

    public class LegendreFunction
    {
      public double[] Pcup = new double[NUMPCUP]; // Legendre Function
      public double[] dPcup = new double[NUMPCUP]; // Derivative of Lagendre fn
    } ;

    public class SphericalHarmonicVariables
    {
      public double[] relative_radius_power = new double[MAX_MODEL_DEGREES + 1]; // [earth_reference_radius_km / sph. radius ]^n
      public double[] cos_mlambda = new double[MAX_MODEL_DEGREES + 1]; // cp(m)  - cosine of (m*spherical coord. longitude
      public double[] sin_mlambda = new double[MAX_MODEL_DEGREES + 1]; // sp(m)  - sine of (m*spherical coord. longitude)
    } ;

    public class GeomagneticElements
    {
      public double declination; /* 1. Angle between the magnetic field vector and true north, positive east */
      public double inclination; /*2. Angle between the magnetic field vector and the horizontal plane, positive down */
      public double magnetic_field_strength; /*3. Magnetic Field Strength */
      public double horizontal_magnetic_field_strength; /*4. Horizontal Magnetic Field Strength */
      public double x; /*5. Northern component of the magnetic field vector */
      public double y; /*6. Eastern component of the magnetic field vector */
      public double z; /*7. Downward component of the magnetic field vector */
      public double grid_variation; /*8. The Grid Variation */
      public double declination_yearly_rate_of_change; /*9. yearly Rate of change in declination */
      public double inclination_yearly_rate_of_change; /*10. yearly Rate of change in inclination */
      public double magnetic_field_strength_yearly_rate_of_change; /*11. yearly rate of change in Magnetic field strength */
      public double horizontal_magnetic_field_strength_yearly_rate_of_change; /*12. yearly rate of change in horizontal field strength */
      public double x_yearly_rate_of_change; /*13. yearly rate of change in the northern component */
      public double y_yearly_rate_of_change; /*14. yearly rate of change in the eastern component */
      public double z_yearly_rate_of_change; /*15. yearly rate of change in the downward component */
      public double grid_variationz_yearly_rate_of_change; /*16. yearly rate of chnage in grid variation */
    } ;

    public class WmmCoefficient
    {
      public byte N { get; private set; }
      public byte M { get; private set; }
      public double MainFieldG { get; private set; }
      public double MainFieldH { get; private set; }
      public double SecularFieldG { get; private set; }
      public double SecularFieldH { get; private set; }


      public WmmCoefficient(byte n, byte m, double mainFieldG, double mainFieldH, double secularFieldG, double secularFieldH)
      {
        N = n;
        M = m;
        MainFieldG = mainFieldG;
        MainFieldH = mainFieldH;
        SecularFieldG = secularFieldG;
        SecularFieldH = secularFieldH;
      }
    } ;

    private List<WmmCoefficient> coefficients = new List<WmmCoefficient>();
    public class LLA
    {
      public double Lat;
      public double Lng;
      public double Alt;
    };

    public class XYZ
    {
      public double X;
      public double Y;
      public double Z;
    };

    private Ellipsoid ellipsoid = new Ellipsoid();
    private MagneticModel magnetic_model;
    private double decimal_date;
    private LLA coord_spherical = new LLA();
    private LLA coord_geodetic = new LLA();
    private GeomagneticElements geomagnetic_elements = new GeomagneticElements();
    private LegendreFunction legendre_function = new LegendreFunction();
    private SphericalHarmonicVariables sph_variables = new SphericalHarmonicVariables();

    private XYZ magnetic_results_spherical = new XYZ();
    private XYZ magnetic_results_geodetic = new XYZ();
    private XYZ magnetic_results_spherical_var = new XYZ();
    private XYZ magnetic_results_geodetic_var = new XYZ();

    /**************************************************************************************
     *   Example use - very simple - only two exposed functions
     *
     *	Initialize(); // Set default values and constants
     *
     *	GetMagVector(double Lat, double Lon, double Alt, int month, int day, int year, double B[3]);
     *	e.g. Iceland in may of 2012 = GetMagVector(65.0, -20.0, 0.0, 5, 5, 2012, B);
     *	Alt is above the WGS-84 Ellipsoid
     *	B is the NED (XYZ) magnetic vector in nTesla
     **************************************************************************************/
    public WorldMagneticModel()
    // Sets default values for WMM subroutines.
    // UPDATES : ellipsoid and magnetic_model
    {
      // Sets WGS-84 parameters
      ellipsoid.a = 6378.137f; // semi-major axis of the ellipsoid in km
      ellipsoid.b = 6356.7523142f; // semi-minor axis of the ellipsoid in km
      ellipsoid.fla = 1.0 / 298.257223563f; // flattening
      ellipsoid.eps = Math.Sqrt(1 - (ellipsoid.b * ellipsoid.b) / (ellipsoid.a * ellipsoid.a)); // first eccentricity
      ellipsoid.epssq = (ellipsoid.eps * ellipsoid.eps); // first eccentricity squared
      ellipsoid.re = 6371.2; // Earth's radius in km

    }

    public double Epoch { get { return magnetic_model.Epoch; } }
    public string ModelName { get { return magnetic_model.ModelName; } }

    public void LoadWMM(System.IO.TextReader rdr)
    {
      // Really, Really needs to be read from a file - out of date in 2015 at latest
      // magnetic_model.EditionDate = 0.0; /* OP change. Originally 5.7863328170559505e-307, truncates to 0.0 */
      // magnetic_model.Epoch = 2010.0;
      // magnetic_model.ModelName = "WMM-2010";

      // read the file from the reader.  First line has the model name
      string header = rdr.ReadLine().Trim();

      string[] args = header.Split(new char[1] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

      if (args.Length != 3)
      {
        Trace.TraceInformation("Input file is not in correct format");
        throw new ArgumentException("Input file is not in correct format");
      }

      coefficients.Clear();

      double epoch = double.Parse(args[0]);
      string modelName = args[1];

      string coefficient = rdr.ReadLine();
      while (!String.IsNullOrEmpty(coefficient) && !coefficient.StartsWith("99999999999999999"))
      {
        args = coefficient.Split(new char[1] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        if (args.Length != 6)
        {
          Trace.TraceInformation("Input file coefficent does not have 6 fields");
          throw new ArgumentException("Input file coefficent does not have 6 fields");
        }

        coefficients.Add(new WmmCoefficient(byte.Parse(args[0]), byte.Parse(args[1]),
          double.Parse(args[2]), double.Parse(args[3]), double.Parse(args[4]), double.Parse(args[5])));

        coefficient = rdr.ReadLine();
      }

      numTerms = coefficients.Count;

      magnetic_model = new MagneticModel(numTerms);

      // Sets Magnetic Model parameters
      magnetic_model.Epoch = epoch;
      magnetic_model.ModelName = modelName;
      magnetic_model.MaxDegrees = MAX_MODEL_DEGREES;
      magnetic_model.MaxSecularDegrees = MAX_SECULAR_VARIATION_MODEL_DEGREES;
      magnetic_model.SecularVariationUsed = false;
    }

    public GeomagneticElements GeomagElements { get { return geomagnetic_elements; } }

    public bool GetMagVector(LLA pos, ref XYZ B)
    {
      if (pos.Lat < -90.0)
        return false; // error

      if (pos.Lat > 90.0)
        return false; // error

      if (pos.Lng < -180.0)
        return false; // error

      if (pos.Lng > 180.0)
        return false; // error

      coord_geodetic.Lng = pos.Lng;
      coord_geodetic.Lat = pos.Lat;
      coord_geodetic.Alt = pos.Alt / 1000.0; // convert to km

      // Convert from geodetic to Spherical Equations: 17-18, WMM Technical report
      if (GeodeticToSpherical(coord_geodetic, ref coord_spherical) < 0)
        return false; // error

      // Compute the geoMagnetic field elements and their time change
      if (Geomag(coord_spherical, coord_geodetic, ref geomagnetic_elements) < 0)
        return false; // error

      B.X = geomagnetic_elements.x * 1e-2;
      B.Y = geomagnetic_elements.y * 1e-2;
      B.Z = geomagnetic_elements.z * 1e-2;

      return true;
    }

    private int Geomag(LLA coord_spherical,
                      LLA coord_geodetic,
                      ref GeomagneticElements geomagnetic_elements)
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
      if (Calculate_SphericalHarmonicVariables(coord_spherical, magnetic_model.MaxDegrees, ref sph_variables) < 0)
        return -2;

      // Compute ALF
      if (AssociatedLegendreFunction(coord_spherical, magnetic_model.MaxDegrees, ref legendre_function) < 0)
        return -3;

      // Accumulate the spherical harmonic coefficients
      if (Summation(legendre_function, sph_variables, coord_spherical, ref magnetic_results_spherical) < 0)
        return -4;

      // Sum the Secular Variation Coefficients
      if (SecVarSummation(legendre_function, sph_variables, coord_spherical, ref magnetic_results_spherical_var) < 0)
        return -5;

      // Map the computed Magnetic fields to Geodeitic coordinates
      if (RotateMagneticVector(coord_spherical, coord_geodetic, magnetic_results_spherical, ref magnetic_results_geodetic) < 0)
        return -6;

      // Map the secular variation field components to Geodetic coordinates
      if (RotateMagneticVector(coord_spherical, coord_geodetic, magnetic_results_spherical_var, ref magnetic_results_geodetic_var) < 0)
        return -7;

      // Calculate the Geomagnetic elements, Equation 18 , WMM Technical report
      if (CalculateGeomagneticElements(magnetic_results_geodetic, ref geomagnetic_elements) < 0)
        return -8;

      // Calculate the secular variation of each of the Geomagnetic elements
      if (CalculateSecularVariation(magnetic_results_geodetic_var, ref geomagnetic_elements) < 0)
        return -9;

      return 0;
    }

    int Calculate_SphericalHarmonicVariables(LLA coord_spherical,
                                                int MaxDegrees,
                                                ref SphericalHarmonicVariables sph_variables)
    /* Computes Spherical variables
       Variables computed are (a/r)^(n+2), cos_m(lamda) and sin_m(lambda) for spherical harmonic
       summations. (Equations 10-12 in the WMM Technical Report)
       INPUT   ellipsoid  data  classure with the following elements
       double a; semi-major axis of the ellipsoid
       double b; semi-minor axis of the ellipsoid
       double fla;  flattening
       double epssq; first eccentricity squared
       double eps;  first eccentricity
       double re; mean radius of  ellipsoid
       coord_spherical    A data classure with the following elements
       double lambda; ( longitude)
       double phig; ( geocentric latitude )
       double r;            ( distance from the center of the ellipsoid)
       MaxDegrees   integer     ( Maxumum degree of spherical harmonic secular model)\

       OUTPUT  sph_variables  Pointer to the   data classure with the following elements
       double relative_radius_power[MAX_MODEL_DEGREES+1];   [earth_reference_radius_km  sph. radius ]^n
       double cos_mlambda[MAX_MODEL_DEGREES+1]; cp(m)  - cosine of (mspherical coord. longitude)
       double sin_mlambda[MAX_MODEL_DEGREES+1];  sp(m)  - sine of (mspherical coord. longitude)
       CALLS : none
     */
    {
      double cos_lambda, sin_lambda;
      int m, n;

      cos_lambda = Math.Cos(DegreesToRadians((short)coord_spherical.Lng));
      sin_lambda = Math.Sin(DegreesToRadians((short)coord_spherical.Lng));

      /* for n = 0 ... model_order, compute (Radius of Earth / Spherica radius r)^(n+2)
         for n  1..MaxDegrees-1 (this is much faster than calling pow MAX_N+1 times).      */

      sph_variables.relative_radius_power[0] = (ellipsoid.re / coord_spherical.Alt) * (ellipsoid.re / coord_spherical.Alt);
      for (n = 1; n <= MaxDegrees; n++)
      {
        sph_variables.relative_radius_power[n] = sph_variables.relative_radius_power[n - 1] * (ellipsoid.re / coord_spherical.Alt);
      }

      /*
         Compute Math.Cos(m*lambda), Math.Sin(m*lambda) for m = 0 ... MaxDegrees
         Math.Cos(a + b) = Math.Cos(a)*Math.Cos(b) - Math.Sin(a)*Math.Sin(b)
         Math.Sin(a + b) = Math.Cos(a)*Math.Sin(b) + Math.Sin(a)*Math.Cos(b)
       */
      sph_variables.cos_mlambda[0] = 1.0;
      sph_variables.sin_mlambda[0] = 0.0;

      sph_variables.cos_mlambda[1] = cos_lambda;
      sph_variables.sin_mlambda[1] = sin_lambda;
      for (m = 2; m <= MaxDegrees; m++)
      {
        sph_variables.cos_mlambda[m] = sph_variables.cos_mlambda[m - 1] * cos_lambda - sph_variables.sin_mlambda[m - 1] * sin_lambda;
        sph_variables.sin_mlambda[m] = sph_variables.cos_mlambda[m - 1] * sin_lambda + sph_variables.sin_mlambda[m - 1] * cos_lambda;
      }

      return 0; // OK
    }

    int AssociatedLegendreFunction(LLA coord_spherical, int MaxDegrees, ref LegendreFunction legendre_function)
    /* Computes  all of the Schmidt-semi normalized associated Legendre
       functions up to degree MaxDegrees. If MaxDegrees <= 16, function PcupLow is used.
       Otherwise PcupHi is called.
       INPUT  coord_spherical        A data classure with the following elements
       double lambda; ( longitude)
       double phig; ( geocentric latitude )
       double r;       ( distance from the center of the ellipsoid)
       MaxDegrees         integer          ( Maxumum degree of spherical harmonic secular model)
       legendre_function Pointer to data classure with the following elements
       double *Pcup;  (  pointer to store Legendre Function  )
       double *dPcup; ( pointer to store  Derivative of Lagendre function )

       OUTPUT  legendre_function  Calculated Legendre variables in the data classure

     */
    {
      double sin_phi = Math.Sin(DegreesToRadians((short)coord_spherical.Lat)); /* Math.Sin  (geocentric latitude) */

      if (MaxDegrees <= 16 || (1 - Math.Abs(sin_phi)) < 1.0e-10f)
      { /* If MaxDegrees is less tha 16 or at the poles */
        if (PcupLow(ref legendre_function.Pcup, ref legendre_function.dPcup, sin_phi, MaxDegrees) < 0)
        {
          return -1; // error
        }
      }
      else
      {
        if (PcupHi(ref legendre_function.Pcup, ref legendre_function.dPcup, sin_phi, MaxDegrees) < 0)
        {
          return -2; // error
        }
      }

      return 0; // OK
    }

    int Summation(LegendreFunction legendre_function,
                  SphericalHarmonicVariables sph_variables,
                  LLA coord_spherical,
                  ref XYZ magnetic_results)
    {
      /* Computes Geomagnetic Field Elements x, y and z in Spherical coordinate system using
         spherical harmonic Summation.

         The vector Magnetic field is given by -grad V, where V is Geomagnetic scalar potential
         The gradient in spherical coordinates is given by:

         dV ^     1 dV ^        1     dV ^
         grad V = -- r  +  - -- t  +  -------- -- p
         dr       r dt       r Math.Sin(t) dp

         INPUT :  legendre_function
         magnetic_model
         sph_variables
         coord_spherical
         OUTPUT : magnetic_results

         CALLS : SummationSpecial

         Manoj Nair, June, 2009 Manoj.C.Nair@Noaa.Gov
       */

      int m, n, index;
      double cos_phi;

      magnetic_results.Z = 0.0;
      magnetic_results.Y = 0.0;
      magnetic_results.X = 0.0;

      for (n = 1; n <= magnetic_model.MaxDegrees; n++)
      {
        for (m = 0; m <= n; m++)
        {
          index = (n * (n + 1) / 2 + m);

          /*		    MaxDegrees        (n+2)     n     m            m           m
                  z =   -SUM (a/r)   (n+1) SUM  [g Math.Cos(m p) + h Math.Sin(m p)] P (Math.Sin(phi))
                                  n=1                   m=0   n            n           n  */
          /* Equation 12 in the WMM Technical report.  Derivative with respect to radius.*/
          magnetic_results.Z -=
            sph_variables.relative_radius_power[n] *
            (GetMainFieldCoeffG(index) *
             sph_variables.cos_mlambda[m] + GetMainFieldCoeffH(index) * sph_variables.sin_mlambda[m])
            * (double)(n + 1) * legendre_function.Pcup[index];

          /*		  1 MaxDegrees  (n+2)    n     m            m           m
                  y =    SUM (a/r) (m)  SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                             n=1             m=0   n            n           n  */
          /* Equation 11 in the WMM Technical report. Derivative with respect to longitude, divided by radius. */
          magnetic_results.Y +=
            sph_variables.relative_radius_power[n] *
            (GetMainFieldCoeffG(index) *
             sph_variables.sin_mlambda[m] - GetMainFieldCoeffH(index) * sph_variables.cos_mlambda[m])
            * (double)(m) * legendre_function.Pcup[index];
          /*		   MaxDegrees  (n+2) n     m            m           m
                  x = - SUM (a/r)   SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                             n=1         m=0   n            n           n  */
          /* Equation 10  in the WMM Technical report. Derivative with respect to latitude, divided by radius. */

          magnetic_results.X -=
            sph_variables.relative_radius_power[n] *
            (GetMainFieldCoeffG(index) *
             sph_variables.cos_mlambda[m] + GetMainFieldCoeffH(index) * sph_variables.sin_mlambda[m])
            * legendre_function.dPcup[index];
        }
      }

      cos_phi = Math.Cos(DegreesToRadians((short) coord_spherical.Lat));
      if (Math.Abs(cos_phi) > 1.0e-10f)
      {
        magnetic_results.Y = magnetic_results.Y / cos_phi;
      }
      else
      {
        /* Special calculation for component - y - at Geographic poles.
         * If the user wants to avoid using this function,  please make sure that
         * the latitude is not exactly +/-90. An option is to make use the function
         * CheckGeographicPoles.
         */
        if (SummationSpecial(sph_variables, coord_spherical, ref magnetic_results) < 0)
        {
          return -1; // error
        }
      }

      return 0; // OK
    }

    int SecVarSummation(LegendreFunction legendre_function,
                          SphericalHarmonicVariables sph_variables,
                        LLA coord_spherical, ref XYZ magnetic_results)
    {
      /*This Function sums the secular variation coefficients to get the secular variation of the Magnetic vector.
         INPUT :  legendre_function
         magnetic_model
         sph_variables
         coord_spherical
         OUTPUT : magnetic_results

         CALLS : SecVarSummationSpecial

       */

      int m, n, index;
      double cos_phi;

      magnetic_model.SecularVariationUsed = true;

      magnetic_results.Z = 0.0;
      magnetic_results.Y = 0.0;
      magnetic_results.X = 0.0;

      for (n = 1; n <= magnetic_model.MaxSecularDegrees; n++)
      {
        for (m = 0; m <= n; m++)
        {
          index = (n * (n + 1) / 2 + m);

          /*		    MaxDegrees        (n+2)     n     m            m           m
                  z =   -SUM (a/r)   (n+1) SUM  [g Math.Cos(m p) + h Math.Sin(m p)] P (Math.Sin(phi))
                                  n=1                   m=0   n            n           n  */
          /*  Derivative with respect to radius.*/
          magnetic_results.Z -=
            sph_variables.relative_radius_power[n] *
            (GetSecularVarCoeffG(index) *
             sph_variables.cos_mlambda[m] + GetSecularVarCoeffH(index) * sph_variables.sin_mlambda[m])
            * (double)(n + 1) * legendre_function.Pcup[index];

          /*		  1 MaxDegrees  (n+2)    n     m            m           m
                  y =    SUM (a/r) (m)  SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                             n=1             m=0   n            n           n  */
          /* Derivative with respect to longitude, divided by radius. */
          magnetic_results.Y +=
            sph_variables.relative_radius_power[n] *
            (GetSecularVarCoeffG(index) *
             sph_variables.sin_mlambda[m] - GetSecularVarCoeffH(index) * sph_variables.cos_mlambda[m])
            * (double)(m) * legendre_function.Pcup[index];
          /*		   MaxDegrees  (n+2) n     m            m           m
                  x = - SUM (a/r)   SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                             n=1         m=0   n            n           n  */
          /* Derivative with respect to latitude, divided by radius. */

          magnetic_results.X -=
            sph_variables.relative_radius_power[n] *
            (GetSecularVarCoeffG(index) *
             sph_variables.cos_mlambda[m] + GetSecularVarCoeffH(index) * sph_variables.sin_mlambda[m])
            * legendre_function.dPcup[index];
        }
      }
      cos_phi = Math.Cos(DegreesToRadians((short) coord_spherical.Lat));
      if (Math.Abs(cos_phi) > 1.0e-10f)
      {
        magnetic_results.Y = magnetic_results.Y / cos_phi;
      }
      else
      {
        /* Special calculation for component y at Geographic poles */
        if (SecVarSummationSpecial(sph_variables, coord_spherical, ref magnetic_results) < 0)
        {
          return -1; // error
        }
      }

      return 0; // OK
    }

    int RotateMagneticVector(LLA coord_spherical,
                              LLA coord_geodetic,
                              XYZ MagneticResultsSph,
                              ref XYZ MagneticResultsGeo)
    /* Rotate the Magnetic Vectors to Geodetic Coordinates
       Manoj Nair, June, 2009 Manoj.C.Nair@Noaa.Gov
       Equation 16, WMM Technical report

       INPUT : coord_spherical : Data classure LLA with the following elements
       double lambda; ( longitude)
       double phig; ( geocentric latitude )
       double r;       ( distance from the center of the ellipsoid)

       coord_geodetic : Data classure LLA with the following elements
       double lambda; (longitude)
       double phi; ( geodetic latitude)
       double alt; (height above the ellipsoid (HaE) )
       double HeightAboveGeoid;(height above the Geoid )

       MagneticResultsSph : Data classure XYZ with the following elements
       double x;     North
       double y;       East
       double z;    Down

       OUTPUT: MagneticResultsGeo Pointer to the data classure XYZ, with the following elements
       double x;     North
       double y;       East
       double z;    Down

       CALLS : none

     */
    {
      /* Difference between the spherical and Geodetic latitudes */
      double Psi = DegreesToRadians((short)(coord_spherical.Lat - coord_geodetic.Lat));

      /* Rotate spherical field components to the Geodeitic system */
      MagneticResultsGeo.Z = MagneticResultsSph.X * Math.Sin(Psi) + MagneticResultsSph.Z * Math.Cos(Psi);
      MagneticResultsGeo.X = MagneticResultsSph.X * Math.Cos(Psi) - MagneticResultsSph.Z * Math.Sin(Psi);
      MagneticResultsGeo.Y = MagneticResultsSph.Y;

      return 0;
    }

    int CalculateGeomagneticElements(XYZ MagneticResultsGeo, 
                                     ref GeomagneticElements geomagnetic_elements)
    /* Calculate all the Geomagnetic elements from x,y and z components
       INPUT     MagneticResultsGeo   Pointer to data classure with the following elements
       double x;    ( North )
       double y;      ( East )
       double z;    ( Down )
       OUTPUT    geomagnetic_elements    Pointer to data classure with the following elements
       double declination; (Angle between the magnetic field vector and true north, positive east)
       double inclination; Angle between the magnetic field vector and the horizontal plane, positive down
       double magnetic_field_strength; Magnetic Field Strength
       double horizontal_magnetic_field_strength; Horizontal Magnetic Field Strength
       double x; Northern component of the magnetic field vector
       double y; Eastern component of the magnetic field vector
       double z; Downward component of the magnetic field vector
       CALLS : none
     */
      {
      geomagnetic_elements.x = MagneticResultsGeo.X;
      geomagnetic_elements.y = MagneticResultsGeo.Y;
      geomagnetic_elements.z = MagneticResultsGeo.Z;

      geomagnetic_elements.horizontal_magnetic_field_strength = Math.Sqrt(MagneticResultsGeo.X * MagneticResultsGeo.X + MagneticResultsGeo.Y * MagneticResultsGeo.Y);
      geomagnetic_elements.magnetic_field_strength = Math.Sqrt(geomagnetic_elements.horizontal_magnetic_field_strength * geomagnetic_elements.horizontal_magnetic_field_strength + MagneticResultsGeo.Z * MagneticResultsGeo.Z);
      geomagnetic_elements.declination = RadiansToDegrees(Math.Atan2(geomagnetic_elements.y, geomagnetic_elements.x));
      geomagnetic_elements.inclination = RadiansToDegrees(Math.Atan2(geomagnetic_elements.z, geomagnetic_elements.horizontal_magnetic_field_strength));

      return 0; // OK
      }

    int CalculateSecularVariation(XYZ magnetic_variation,
                                  ref GeomagneticElements magnetic_elements)
    /*This takes the Magnetic Variation in x, y, and z and uses it to calculate the secular variation of each of the Geomagnetic elements.
            INPUT     magnetic_variation   Data classure with the following elements
                                    double x;    ( North )
                                    double y;	  ( East )
                                    double z;    ( Down )
            OUTPUT   magnetic_elements   Pointer to the data  classure with the following elements updated
                            double declination_yearly_rate_of_change; yearly Rate of change in declination
                            double inclination_yearly_rate_of_change; yearly Rate of change in inclination
                            double magnetic_field_strength_yearly_rate_of_change; yearly rate of change in Magnetic field strength
                            double horizontal_magnetic_field_strength_yearly_rate_of_change; yearly rate of change in horizontal field strength
                            double x_yearly_rate_of_change; yearly rate of change in the northern component
                            double y_yearly_rate_of_change; yearly rate of change in the eastern component
                            double z_yearly_rate_of_change; yearly rate of change in the downward component
                            double grid_variationz_yearly_rate_of_change;yearly rate of chnage in grid variation
            CALLS : none

     */
      {
      magnetic_elements.x_yearly_rate_of_change = magnetic_variation.X;
      magnetic_elements.y_yearly_rate_of_change = magnetic_variation.Y;
      magnetic_elements.z_yearly_rate_of_change = magnetic_variation.Z;
      magnetic_elements.horizontal_magnetic_field_strength_yearly_rate_of_change = 
        (magnetic_elements.x * magnetic_elements.x_yearly_rate_of_change + magnetic_elements.y * magnetic_elements.y_yearly_rate_of_change) / 
        magnetic_elements.horizontal_magnetic_field_strength; // See equation 19 in the WMM technical report
      magnetic_elements.magnetic_field_strength_yearly_rate_of_change =
        (magnetic_elements.x * magnetic_elements.x_yearly_rate_of_change +
         magnetic_elements.y * magnetic_elements.y_yearly_rate_of_change + magnetic_elements.z * magnetic_elements.z_yearly_rate_of_change) / magnetic_elements.magnetic_field_strength;
      magnetic_elements.declination_yearly_rate_of_change =
        180.0 / PI * (magnetic_elements.x * magnetic_elements.y_yearly_rate_of_change -
                           magnetic_elements.y * magnetic_elements.x_yearly_rate_of_change) / (magnetic_elements.horizontal_magnetic_field_strength * magnetic_elements.horizontal_magnetic_field_strength);
      magnetic_elements.inclination_yearly_rate_of_change =
        180.0 / PI * (magnetic_elements.horizontal_magnetic_field_strength * magnetic_elements.z_yearly_rate_of_change -
                           magnetic_elements.z * magnetic_elements.horizontal_magnetic_field_strength_yearly_rate_of_change) / (magnetic_elements.magnetic_field_strength * magnetic_elements.magnetic_field_strength);
      magnetic_elements.grid_variationz_yearly_rate_of_change = magnetic_elements.declination_yearly_rate_of_change;

      return 0; // OK
      }

    // PcupHi
    double[] f1 = new double[NUMPCUP];
    double[] f2 = new double[NUMPCUP];
    double[] PreSqr = new double[NUMPCUP];

    // PcupLow
    double[] schmidtQuasiNorm = new double[NUMPCUP];

    // SummationSpecial
    double[] PcupS = new double[NUMPCUPS];

    int PcupHi(ref double[] Pcup, ref double[] dPcup, double x, int MaxDegrees)
    /*	This function evaluates all of the Schmidt-semi normalized associated Legendre
            functions up to degree MaxDegrees. The functions are initially scaled by
            10^280 Math.Sin^m in order to minimize the effects of underflow at large m
            near the poles (see Holmes and Featherstone 2002, J. Geodesy, 76, 279-299).
            Note that this function performs the same operation as PcupLow.
            However this function also can be used for high degree (large MaxDegrees) models.

            Calling Parameters:
                    INPUT
                            MaxDegrees:	 Maximum spherical harmonic degree to compute.
                            x:		Math.Cos(colatitude) or Math.Sin(latitude).

                    OUTPUT
                            Pcup:	A vector of all associated Legendgre polynomials evaluated at
                                            x up to MaxDegrees. The lenght must by greater or equal to (MaxDegrees+1)*(MaxDegrees+2)/2.
                      dPcup:   Derivative of Pcup(x) with respect to latitude

                    CALLS : none
            Notes:

       Adopted from the FORTRAN code written by Mark Wieczorek September 25, 2005.

       Manoj Nair, Nov, 2009 Manoj.C.Nair@Noaa.Gov

       Change from the previous version
       The prevous version computes the derivatives as
       dP(n,m)(x)/dx, where x = Math.Sin(latitude) (or Math.Cos(colatitude) ).
       However, the WMM Geomagnetic routines requires dP(n,m)(x)/dlatitude.
       Hence the derivatives are multiplied by Math.Sin(latitude).
       Removed the options for CS phase and normalizations.

       Note: In geomagnetism, the derivatives of ALF are usually found with
       respect to the colatitudes. Here the derivatives are found with respect
       to the latitude. The difference is a sign reversal for the derivative of
       the Associated Legendre Functions.

       The derivates can't be computed for latitude = |90| degrees.
     */
      {
      int k, kstart, m, n;
      double pm2, pm1, pmm, plm, rescalem, z, scalef;

      /*
       * Note: OP code change to avoid floating point equality test.
       * Was: if (Math.Abs(x) == 1.0)
       */
      if(Math.Abs(x) - 1.0 < 1e-9)
        {
        // printf("Error in PcupHigh: derivative cannot be calculated at poles\n");
        return -2;
        }

      /* OP Change: 1.0e-280 is too small to store in a double - the compiler truncates
       * it to 0.0, which is bad as the code below divides by scalef. */
      scalef = 1.0e-20f;

      for(n = 0; n <= 2 * MaxDegrees + 1; ++n)
        {
        PreSqr[n] = Math.Sqrt((double) (n));
        }

      k = 2;

      for(n = 2; n <= MaxDegrees; n++)
        {
        k = k + 1;
        f1[k] = (double) (2 * n - 1) / (double) (n);
        f2[k] = (double) (n - 1) / (double) (n);
        for(m = 1; m <= n - 2; m++)
          {
          k = k + 1;
          f1[k] = (double) (2 * n - 1) / PreSqr[n + m] / PreSqr[n - m];
          f2[k] = PreSqr[n - m - 1] * PreSqr[n + m - 1] / PreSqr[n + m] / PreSqr[n - m];
          }
        k = k + 2;
        }

      /*z = Math.Sin (geocentric latitude) */
      z = Math.Sqrt((1.0 - x) * (1.0 + x));
      pm2 = 1.0;
      Pcup[0] = 1.0;
      dPcup[0] = 0.0;
      if(MaxDegrees == 0)
        {
        return -3;
        }
      pm1 = x;
      Pcup[1] = pm1;
      dPcup[1] = z;
      k = 1;

      for(n = 2; n <= MaxDegrees; n++)
        {
        k = k + n;
        plm = f1[k] * x * pm1 - f2[k] * pm2;
        Pcup[k] = plm;
        dPcup[k] = (double) (n) * (pm1 - x * plm) / z;
        pm2 = pm1;
        pm1 = plm;
        }

      pmm = PreSqr[2] * scalef;
      rescalem = 1.0 / scalef;
      kstart = 0;

      for(m = 1; m <= MaxDegrees - 1; ++m)
        {
        rescalem = rescalem * z;

        /* Calculate Pcup(m,m) */
        kstart = kstart + m + 1;
        pmm = pmm * PreSqr[2 * m + 1] / PreSqr[2 * m];
        Pcup[kstart] = pmm * rescalem / PreSqr[2 * m + 1];
        dPcup[kstart] = -((double) (m) * x * Pcup[kstart] / z);
        pm2 = pmm / PreSqr[2 * m + 1];
        /* Calculate Pcup(m+1,m) */
        k = kstart + m + 1;
        pm1 = x * PreSqr[2 * m + 1] * pm2;
        Pcup[k] = pm1 * rescalem;
        dPcup[k] = ((pm2 * rescalem) * PreSqr[2 * m + 1] - x * (double) (m + 1) * Pcup[k]) / z;
        /* Calculate Pcup(n,m) */
        for(n = m + 2; n <= MaxDegrees; ++n)
          {
          k = k + n;
          plm = x * f1[k] * pm1 - f2[k] * pm2;
          Pcup[k] = plm * rescalem;
          dPcup[k] = (PreSqr[n + m] * PreSqr[n - m] * (pm1 * rescalem) - (double) (n) * x * Pcup[k]) / z;
          pm2 = pm1;
          pm1 = plm;
          }
        }

      /* Calculate Pcup(MaxDegrees,MaxDegrees) */
      rescalem = rescalem * z;
      kstart = kstart + m + 1;
      pmm = pmm / PreSqr[2 * MaxDegrees];
      Pcup[kstart] = pmm * rescalem;
      dPcup[kstart] = -(double) (MaxDegrees) * x * Pcup[kstart] / z;

      return 0; // OK
      }

    int PcupLow(ref double[] Pcup, ref double[] dPcup, double x, int MaxDegrees)
    /*   This function evaluates all of the Schmidt-semi normalized associated Legendre
            functions up to degree MaxDegrees.

            Calling Parameters:
                    INPUT
                            MaxDegrees:	 Maximum spherical harmonic degree to compute.
                            x:		Math.Cos(colatitude) or Math.Sin(latitude).

                    OUTPUT
                            Pcup:	A vector of all associated Legendgre polynomials evaluated at
                                            x up to MaxDegrees.
                       dPcup: Derivative of Pcup(x) with respect to latitude

            Notes: Overflow may occur if MaxDegrees > 20 , especially for high-latitudes.
            Use PcupHi for large MaxDegrees.

       Writted by Manoj Nair, June, 2009 . Manoj.C.Nair@Noaa.Gov.

       Note: In geomagnetism, the derivatives of ALF are usually found with
       respect to the colatitudes. Here the derivatives are found with respect
       to the latitude. The difference is a sign reversal for the derivative of
       the Associated Legendre Functions.
     */
      {
      int n, m, index, index1, index2;
      double k, z;

      Pcup[0] = 1.0;
      dPcup[0] = 0.0;

      /*Math.Sin (geocentric latitude) - sin_phi */
      z = Math.Sqrt((1.0 - x) * (1.0 + x));

      /*       First, Compute the Gauss-normalized associated Legendre  functions */
      for(n = 1; n <= MaxDegrees; n++)
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
              k = (double) (((n - 1) * (n - 1)) - (m * m)) / (double) ((2 * n - 1)
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

      schmidtQuasiNorm[0] = 1.0;
      for(n = 1; n <= MaxDegrees; n++)
        {
        index = (n * (n + 1) / 2);
        index1 = (n - 1) * n / 2;
        /* for m = 0 */
        schmidtQuasiNorm[index] = schmidtQuasiNorm[index1] * (double) (2 * n - 1) / (double) n;

        for(m = 1; m <= n; m++)
          {
          index = (n * (n + 1) / 2 + m);
          index1 = (n * (n + 1) / 2 + m - 1);
          schmidtQuasiNorm[index] = schmidtQuasiNorm[index1] * Math.Sqrt((double) ((n - m + 1) * (m == 1 ? 2 : 1)) / (double) (n + m));
          }
        }

      /* Converts the  Gauss-normalized associated Legendre
                functions to the Schmidt quasi-normalized version using pre-computed
                relation stored in the variable schmidtQuasiNorm */

      for(n = 1; n <= MaxDegrees; n++)
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

    int SummationSpecial(SphericalHarmonicVariables sph_variables, 
                         LLA coord_spherical, 
                         ref XYZ magnetic_results)
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
      int n, index;
      double k, sin_phi;
      double schmidtQuasiNorm1;
      double schmidtQuasiNorm2;
      double schmidtQuasiNorm3;

      PcupS[0] = 1;
      schmidtQuasiNorm1 = 1.0;

      magnetic_results.Y = 0.0;
      sin_phi = Math.Sin(DegreesToRadians((short)coord_spherical.Lat));

      for(n = 1; n <= magnetic_model.MaxDegrees; n++)
        {
        /*Compute the ration between the Gauss-normalized associated Legendre
           functions and the Schmidt quasi-normalized version. This is equivalent to
           sqrt((m==0?1:2)*(n-m)!/(n+m!))*(2n-1)!!/(n-m)!  */

        index = (n * (n + 1) / 2 + 1);
        schmidtQuasiNorm2 = schmidtQuasiNorm1 * (double) (2 * n - 1) / (double) n;
        schmidtQuasiNorm3 = schmidtQuasiNorm2 * Math.Sqrt((double) (n * 2) / (double) (n + 1));
        schmidtQuasiNorm1 = schmidtQuasiNorm2;
        if(n == 1)
          {
          PcupS[n] = PcupS[n - 1];
          }
        else
          {
          k = (double) (((n - 1) * (n - 1)) - 1) / (double) ((2 * n - 1) * (2 * n - 3));
          PcupS[n] = sin_phi * PcupS[n - 1] - k * PcupS[n - 2];
          }

        /*		  1 MaxDegrees  (n+2)    n     m            m           m
                y =    SUM (a/r) (m)  SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                           n=1             m=0   n            n           n  */
        /* Equation 11 in the WMM Technical report. Derivative with respect to longitude, divided by radius. */

        magnetic_results.Y +=
          sph_variables.relative_radius_power[n] *
          (GetMainFieldCoeffG(index) *
           sph_variables.sin_mlambda[1] - GetMainFieldCoeffH(index) * sph_variables.cos_mlambda[1])
          * PcupS[n] * schmidtQuasiNorm3;
        }

      return 0; // OK
      }

    int SecVarSummationSpecial(SphericalHarmonicVariables sph_variables, LLA coord_spherical, ref XYZ magnetic_results)
      {
      /*Special calculation for the secular variation Summation at the poles.

         INPUT: magnetic_model
         sph_variables
         coord_spherical
         OUTPUT: magnetic_results
         CALLS : none

       */
      int n, index;
      double k, sin_phi;
      double schmidtQuasiNorm1;
      double schmidtQuasiNorm2;
      double schmidtQuasiNorm3;

      PcupS[0] = 1;
      schmidtQuasiNorm1 = 1.0;

      magnetic_results.Y = 0.0;
      sin_phi = Math.Sin(DegreesToRadians((short)coord_spherical.Lat));

      for(n = 1; n <= magnetic_model.MaxSecularDegrees; n++)
        {
        index = (n * (n + 1) / 2 + 1);
        schmidtQuasiNorm2 = schmidtQuasiNorm1 * (double) (2 * n - 1) / (double) n;
        schmidtQuasiNorm3 = schmidtQuasiNorm2 * Math.Sqrt((double) (n * 2) / (double) (n + 1));
        schmidtQuasiNorm1 = schmidtQuasiNorm2;
        if(n == 1)
          {
          PcupS[n] = PcupS[n - 1];
          }
        else
          {
          k = (double) (((n - 1) * (n - 1)) - 1) / (double) ((2 * n - 1) * (2 * n - 3));
          PcupS[n] = sin_phi * PcupS[n - 1] - k * PcupS[n - 2];
          }

        /*		  1 MaxDegrees  (n+2)    n     m            m           m
                y =    SUM (a/r) (m)  SUM  [g Math.Cos(m p) + h Math.Sin(m p)] dP (Math.Sin(phi))
                           n=1             m=0   n            n           n  */
        /* Derivative with respect to longitude, divided by radius. */

        magnetic_results.Y +=
          sph_variables.relative_radius_power[n] *
          (GetSecularVarCoeffG(index) *
           sph_variables.sin_mlambda[1] - GetSecularVarCoeffH(index) * sph_variables.cos_mlambda[1])
          * PcupS[n] * schmidtQuasiNorm3;
        }

      return 0; // OK
      }

    /**
     * @brief Comput the MainFieldCoeffH accounting for the date
     */
    double GetMainFieldCoeffG(int index)
      {
      if(index >= numTerms)
        {
        return 0;
        }

      int n, m, sum_index, a, b;

      double coeff = coefficients[index].MainFieldG;

      a = magnetic_model.MaxSecularDegrees;
      b = (a * (a + 1) / 2 + a);
      for(n = 1; n <= magnetic_model.MaxDegrees; n++)
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
            coeff += (decimal_date - magnetic_model.Epoch) * GetSecularVarCoeffG(sum_index);
            }
          }
        }

      return coeff;
      }

    double GetMainFieldCoeffH(int index)
      {
      if(index >= numTerms)
        {
        return 0;
        }

      int n, m, sum_index, a, b;
      double coeff = coefficients[index].MainFieldH;

      a = magnetic_model.MaxSecularDegrees;
      b = (a * (a + 1) / 2 + a);
      for(n = 1; n <= magnetic_model.MaxDegrees; n++)
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
            coeff += (decimal_date - magnetic_model.Epoch) * GetSecularVarCoeffH(sum_index);
            }
          }
        }

      return coeff;
      }

    double GetSecularVarCoeffG(int index)
      {
      if(index >= numTerms)
        {
        return 0;
        }

      return coefficients[index].SecularFieldG;
      }

    double GetSecularVarCoeffH(int index)
      {
      if(index >= numTerms)
        {
        return 0;
        }

      return coefficients[index].SecularFieldH;
      }

    int GeodeticToSpherical(LLA coord_geodetic, ref LLA coord_spherical)
    // Converts Geodetic coordinates to Spherical coordinates
    // Convert geodetic coordinates, (defined by the WGS-84
    // reference ellipsoid), to Earth Centered Earth Fixed Cartesian
    // coordinates, and then to spherical coordinates.
      {
      double CosLat, SinLat, rc, xp, zp; // all local variables

      CosLat = Math.Cos(DegreesToRadians((short)coord_geodetic.Lat));
      SinLat = Math.Sin(DegreesToRadians((short)coord_geodetic.Lat));

      // compute the local radius of curvature on the WGS-84 reference ellipsoid
      rc = ellipsoid.a / Math.Sqrt(1.0 - ellipsoid.epssq * SinLat * SinLat);

      // compute ECEF Cartesian coordinates of specified point (for longitude=0)

      xp = (rc + coord_geodetic.Alt) * CosLat;
      zp = (rc * (1.0 - ellipsoid.epssq) + coord_geodetic.Alt) * SinLat;

      // compute spherical radius and angle lambda and phi of specified point

      coord_spherical.Alt = Math.Sqrt(xp * xp + zp * zp);
      coord_spherical.Lat = RadiansToDegrees(Math.Sin(zp / coord_spherical.Alt)); // geocentric latitude
      coord_spherical.Lng = coord_geodetic.Lng; // longitude

      return 0; // OK
      }
    public const double PI = 3.1415926535897932384626433832795;
    public const double pi_div_180 = PI / 180.0;

    public double DegreesToRadians(short value)
    {
      return ((double)value) * pi_div_180;
    }

    public short RadiansToDegrees(double value)
    {
      return (short)(value / pi_div_180);
    }

  }
}





