// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Globalization;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

internal static class JsonHelpers
{
    public static double ReadDouble(JsonNode? node, string keyName)
    {
        ArgumentNullException.ThrowIfNull(node);

        if (node is JsonValue value)
        {
            if (value.TryGetValue<double>(out double number))
            {
                return number;
            }

            if (value.TryGetValue<string>(out string? text))
            {
                return ParseNamedDouble(text, keyName);
            }
        }

        throw new InvalidOperationException($"{keyName} is missing or null");
    }

    public static double? ReadOptionalDouble(JsonNode? node)
    {
        if (node == null)
        {
            return null;
        }

        if (node is JsonValue value)
        {
            if (value.TryGetValue<double>(out double number))
            {
                return number;
            }

            if (value.TryGetValue<string>(out string? text))
            {
                return ParseNamedDouble(text, "value");
            }
        }

        return null;
    }

    private static double ParseNamedDouble(string? text, string keyName)
    {
        if (string.IsNullOrWhiteSpace(text))
        {
            throw new InvalidOperationException($"{keyName} is missing or null");
        }

        return text switch
        {
            "NaN" => double.NaN,
            "Infinity" => double.PositiveInfinity,
            "-Infinity" => double.NegativeInfinity,
            _ when double.TryParse(text, NumberStyles.Float, CultureInfo.InvariantCulture, out double result) => result,
            _ => throw new InvalidOperationException($"{keyName} cannot be converted to a double"),
        };
    }
}
