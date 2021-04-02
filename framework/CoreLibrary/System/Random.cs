//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Represents a random number generator, a device that produces a
  /// sequence of numbers that meet certain statistical requirements for
  /// randomness.
  /// </summary>
  /// <remarks>
  /// Depending on the platform it can be a software dependent pseudo-random generator algorithm 
  /// or a hardware random generator providing true random numbers.
  /// </remarks>
  public class Random
  {
    /// <summary>
    /// Initializes a new instance of the Random class. 
    /// If the provider is algorithm based the initialisation is performed using a time-
    /// dependent default seed value.
    /// </summary>
    public Random()
    {
      CanFly.Runtime.RandomCtor(this);
    }

    /// <summary>
    /// Initializes a new instance of the Random class, using the specified
    /// seed value.
    /// </summary>
    /// <param name="seed">A number used to calculate a starting value for
    /// the pseudo-random number sequence.</param>
    public Random(int seed)
    {
      CanFly.Runtime.RandomCtor(this, seed);
    }

    /// <summary>
    /// Returns a nonnegative random number.
    /// </summary>
    /// <returns>A 32-bit signed integer greater than or equal to zero and
    /// less than MaxValue.</returns>
    public virtual int Next() { return CanFly.Runtime.RandomNext(this); }

    /// <summary>
    /// Returns a nonnegative random number less than the specified maximum.
    /// </summary>
    /// <param name="maxValue">The exclusive upper bound of the random number
    /// to be generated. maxValue must be greater than or equal to zero.</param>
    /// <returns>A 32-bit signed integer greater than or equal to zero, and
    /// less than maxValue.</returns>
    public virtual int Next(int maxValue) { return CanFly.Runtime.RandomNext(this, maxValue); }

    /// <summary>
    /// Returns a random number between 0.0 and 1.0.
    /// </summary>
    /// <returns>A double-precision floating point number greater than or equal
    /// to 0.0, and less than 1.0.</returns>
    public virtual double NextDouble() { return CanFly.Runtime.RandomNextDouble(this); }

    /// <summary>
    /// Fills the elements of a specified array of bytes with random numbers.
    /// </summary>
    /// <param name="buffer">An array of bytes to contain random numbers.</param>
    public virtual void NextBytes(byte[] buffer) { CanFly.Runtime.RandomNextBytes(this, buffer); }
  }
}
