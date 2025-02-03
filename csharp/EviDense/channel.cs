// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Represents a measurement channel with sample and reference values, measured in ?V.
/// </summary>
public class Channel
{
    /// <summary>
    /// Gets or sets the sample value in uV.
    /// </summary>
    public double Sample { get; set; }

    /// <summary>
    /// Gets or sets the reference value in uV.
    /// </summary>
    public double Reference { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="Channel"/> class with optional sample and reference values.
    /// </summary>
    /// <param name="sample">The sample value in uV (default: 0.0).</param>
    /// <param name="reference">The reference value in uV (default: 0.0).</param>
    public Channel(double sample = 0.0, double reference = 0.0)
    {
        Sample = sample;
        Reference = reference;
    }

    /// <summary>
    /// Returns a string representation of the channel values.
    /// </summary>
    /// <returns>A string in the format "Sample: [value] Reference: [value]".</returns>
    public override string ToString()
    {
        return $"Sample:{Sample} Reference:{Reference}";
    }

    /// <summary>
    /// Converts the channel values to a JSON representation.
    /// </summary>
    /// <returns>A JsonNode representing the channel values.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();
        obj[Dict.SAMPLE] = JsonValue.Create(Sample);
        obj[Dict.REFERENCE] = JsonValue.Create(Reference);

        return obj;
    }

    /// <summary>
    /// Creates a <see cref="Channel"/> instance from a JSON node.
    /// </summary>
    /// <param name="node">The JSON node containing the channel data.</param>
    /// <returns>A new <see cref="Channel"/> instance with values extracted from the JSON node.</returns>
    /// <exception cref="ArgumentException">Thrown when the JSON node is invalid.</exception>
    public static Channel FromJson(JsonNode node)
    {
        if (node is JsonObject jsonObject &&
            jsonObject.TryGetPropertyValue(Dict.SAMPLE, out JsonNode? sampleNode) &&
            jsonObject.TryGetPropertyValue(Dict.REFERENCE, out JsonNode? referenceNode) &&
            sampleNode is not null &&
            referenceNode is not null)
        {
            return new Channel(sampleNode.GetValue<double>(), referenceNode.GetValue<double>());
        }
        else
        {
            throw new ArgumentException("Invalid JSON node for Channel.");
        }
    }
}
