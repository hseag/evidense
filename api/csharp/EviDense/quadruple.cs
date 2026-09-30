// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Represents a quadruple set of measurement values at different wavelengths.
/// This class performs mathematical operations (plus, minus, ...) on all four wavelengths at the same time.
/// </summary>
public class Quadruple
{
    /// <summary>
    /// Gets or sets the value at 230 nm.
    /// </summary>
    public double Value230 { get; set; }

    /// <summary>
    /// Gets or sets the value at 260 nm.
    /// </summary>
    public double Value260 { get; set; }

    /// <summary>
    /// Gets or sets the value at 280 nm.
    /// </summary>
    public double Value280 { get; set; }

    /// <summary>
    /// Gets or sets the value at 340 nm.
    /// </summary>
    public double Value340 { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="Quadruple"/> class.
    /// </summary>
    /// <param name="value230">Value at 230 nm.</param>
    /// <param name="value260">Value at 260 nm (default: same as value230).</param>
    /// <param name="value280">Value at 280 nm (default: same as value230).</param>
    /// <param name="value340">Value at 340 nm (default: same as value230).</param>
    public Quadruple(double value230 = 0, double? value260 = null, double? value280 = null, double? value340 = null)
    {
        if (value260 == null && value280 == null && value340 == null)
        {
            Value230 = value230;
            Value260 = value230;
            Value280 = value230;
            Value340 = value230;
        }
        else
        {
            Value230 = value230;
            Value260 = value260 ?? 0;
            Value280 = value280 ?? 0;
            Value340 = value340 ?? 0;
        }
    }

    /// <summary>
    /// Returns a string representation of the quadruple values.
    /// </summary>
    public override string ToString()
    {
        return $"230={Value230}, 260={Value260}, 280={Value280}, 340={Value340}";
    }

    /// <summary>
    /// Adds a quadruple value to this instance.
    /// </summary>
    public static Quadruple operator +(Quadruple lhs, Quadruple rhs)
    {
        return new Quadruple(
            lhs.Value230 + rhs.Value230,
            lhs.Value260 + rhs.Value260,
            lhs.Value280 + rhs.Value280,
            lhs.Value340 + rhs.Value340);
    }

    /// <summary>
    /// Adds a scalar value to this instance.
    /// </summary>
    public static Quadruple operator +(Quadruple lhs, double rhs)
    {
        return new Quadruple(
            lhs.Value230 + rhs,
            lhs.Value260 + rhs,
            lhs.Value280 + rhs,
            lhs.Value340 + rhs);
    }

    /// <summary>
    /// Subtracts a quadruple value from this instance.
    /// </summary>
    public static Quadruple operator -(Quadruple lhs, Quadruple rhs)
    {
        return new Quadruple(
            lhs.Value230 - rhs.Value230,
            lhs.Value260 - rhs.Value260,
            lhs.Value280 - rhs.Value280,
            lhs.Value340 - rhs.Value340);
    }

    /// <summary>
    /// Subtracts a scalar value from this instance.
    /// </summary>
    public static Quadruple operator -(Quadruple lhs, double rhs)
    {
        return new Quadruple(
            lhs.Value230 - rhs,
            lhs.Value260 - rhs,
            lhs.Value280 - rhs,
            lhs.Value340 - rhs);
    }

    /// <summary>
    /// Divides this instance by a quadruple value.
    /// </summary>
    public static Quadruple operator /(Quadruple lhs, Quadruple rhs)
    {
        return new Quadruple(
            lhs.Value230 / rhs.Value230,
            lhs.Value260 / rhs.Value260,
            lhs.Value280 / rhs.Value280,
            lhs.Value340 / rhs.Value340);
    }

    /// <summary>
    /// Divides this instance by a scalar value.
    /// </summary>
    public static Quadruple operator /(Quadruple lhs, double rhs)
    {
        return new Quadruple(
            lhs.Value230 / rhs,
            lhs.Value260 / rhs,
            lhs.Value280 / rhs,
            lhs.Value340 / rhs);
    }

    /// <summary>
    /// Multiplies this instance by a quadruple value.
    /// </summary>
    public static Quadruple operator *(Quadruple lhs, Quadruple rhs)
    {
        return new Quadruple(
            lhs.Value230 * rhs.Value230,
            lhs.Value260 * rhs.Value260,
            lhs.Value280 * rhs.Value280,
            lhs.Value340 * rhs.Value340);
    }

    /// <summary>
    /// Multiplies this instance by a scalar value.
    /// </summary>
    public static Quadruple operator *(Quadruple lhs, double rhs)
    {
        return new Quadruple(
            lhs.Value230 * rhs,
            lhs.Value260 * rhs,
            lhs.Value280 * rhs,
            lhs.Value340 * rhs);
    }

    /// <summary>
    /// Returns a new instance with absolute values.
    /// </summary>
    public Quadruple Abs()
    {
        return new Quadruple(Math.Abs(Value230), Math.Abs(Value260), Math.Abs(Value280), Math.Abs(Value340));
    }

    /// <summary>
    /// Determines whether this instance is less than or equal to another quadruple.
    /// </summary>
    public static bool operator <=(Quadruple lhs, Quadruple rhs)
    {
        return lhs.Value230 <= rhs.Value230 && lhs.Value260 <= rhs.Value260 && lhs.Value280 <= rhs.Value280 && lhs.Value340 <= rhs.Value340;
    }

    /// <summary>
    /// Determines whether this instance is greater than or equal to another quadruple.
    /// </summary>
    public static bool operator >=(Quadruple lhs, Quadruple rhs)
    {
        return rhs <= lhs;
    }

    /// <summary>
    /// Converts the quadruple to a JSON representation.
    /// </summary>
    /// <returns>A JSON node with the four wavelength values.</returns>
    public JsonNode ToJson()
    {
        return new JsonObject
        {
            [Dict.CH_230] = Value230,
            [Dict.CH_260] = Value260,
            [Dict.CH_280] = Value280,
            [Dict.CH_340] = Value340,
        };
    }

    /// <summary>
    /// Creates a quadruple from a JSON representation.
    /// </summary>
    /// <param name="node">JSON node containing wavelength values.</param>
    /// <returns>A populated quadruple.</returns>
    public static Quadruple FromJson(JsonNode? node)
    {
        ArgumentNullException.ThrowIfNull(node);
        return new Quadruple(
            JsonHelpers.ReadDouble(node[Dict.CH_230], Dict.CH_230),
            JsonHelpers.ReadDouble(node[Dict.CH_260], Dict.CH_260),
            JsonHelpers.ReadDouble(node[Dict.CH_280], Dict.CH_280),
            JsonHelpers.ReadDouble(node[Dict.CH_340], Dict.CH_340));
    }
}

