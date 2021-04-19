/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

namespace CanFly
{
  public struct Position
  {
    /// <summary>
    /// Create a position.  The lat and lng are in relation to the WGS-84 Ellipsoid
    /// </summary>
    /// <param name="lat">Lattitude</param>
    /// <param name="lng">Longitude</param>
    /// <param name="elev">Elevation in meters above WGS-84</param>
    /// <returns>New position</returns>
    public static Position Create(float lat, float lng, float elevation) { return Syscall.CreatePosition(lat, lng, elevation); }
    /// <summary>
    /// Return the lattitude
    /// </summary>
    /// <returns></returns>
    public float Lattitude { get { return Syscall.GetLat(this); } }
    /// <summary>
    /// Returns the longitude of the position
    /// </summary>
    /// <returns></returns>
    public float Longitude { get { return Syscall.GetLng(this); } }
    /// <summary>
    /// Returns the Elevation of the position above the WGS-84 ellipsoid
    /// </summary>
    /// <returns></returns>
    public float Elevation { get { return Syscall.GetElev(this); } }
    /// <summary>
    /// Returns the estimated distance to the position.  This is calculated using
    /// <see href="https://en.wikipedia.org/wiki/Vincenty%27s_formulae">Vincenty's formulae</see> 
    /// to estimate the distance on the WGS-84 Ellipsoid.  The elevation component is not included in
    /// the calculation.  The estimation is accurate to +/1 100m over the half
    /// great circle of the equator
    /// </summary>
    /// <param name="pos">Position to estimate distance to</param>
    /// <returns>Estimated distance in meters</returns>
    public float DistanceTo(Position pos) { return Syscall.Distance(this, pos); }
    /// <summary>
    /// Calculate the estimated brearing (true) between 2 points on the WGS-84
    /// ellipsoid.  This is calculated using
    /// <see href="https://en.wikipedia.org/wiki/Vincenty%27s_formulae">Vincenty's formulae</see> 
    /// to estimate the bearing on the WGS-84 Ellipsoid.
    /// </summary>
    /// <param name="pos">Position to estimate the distance to</param>
    /// <returns>Shortest estimated distance</returns>
    public float Bearing(Position pos) { return Syscall.Bearing(this, pos); }
    /// <summary>
    /// Return the best calculated magnetic variation for the position.
    /// This requires that the CanFly geospatial services are available to
    /// return the calculated variation.  If not available then a
    /// NotSupported exception will be raised.
    /// </summary>
    /// <returns></returns>
    public float MagneticVariation() { return Syscall.MagneticVariation(this); }
  }
}