// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Represents a single measurement consisting of multiple wavelength channels.
/// </summary>
public class SingleMeasurement
{
    /// <summary>
    /// Gets or sets the channel for 230 nm measurements.
    /// </summary>
    public Channel Channel230 { get; set; }

    /// <summary>
    /// Gets or sets the channel for 260 nm measurements.
    /// </summary>
    public Channel Channel260 { get; set; }

    /// <summary>
    /// Gets or sets the channel for 280 nm measurements.
    /// </summary>
    public Channel Channel280 { get; set; }

    /// <summary>
    /// Gets or sets the channel for 340 nm measurements.
    /// </summary>
    public Channel Channel340 { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="SingleMeasurement"/> class with optional channels.
    /// </summary>
    /// <param name="channel230">The channel for 230 nm (default: new Channel()).</param>
    /// <param name="channel260">The channel for 260 nm (default: new Channel()).</param>
    /// <param name="channel280">The channel for 280 nm (default: new Channel()).</param>
    /// <param name="channel340">The channel for 340 nm (default: new Channel()).</param>
    public SingleMeasurement(
        Channel? channel230 = null,
        Channel? channel260 = null,
        Channel? channel280 = null,
        Channel? channel340 = null)
    {
        Channel230 = channel230 ?? new Channel();
        Channel260 = channel260 ?? new Channel();
        Channel280 = channel280 ?? new Channel();
        Channel340 = channel340 ?? new Channel();
    }

    /// <summary>
    /// Returns a string representation of the measurement values.
    /// </summary>
    /// <returns>A formatted string containing all channel values.</returns>
    public override string ToString()
    {
        return $"230: [{Channel230}] 260: [{Channel260}] 280: [{Channel280}] 340: [{Channel340}]";
    }

    /// <summary>
    /// Converts the measurement data to a JSON representation.
    /// </summary>
    /// <returns>A JsonNode representing the measurement.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.CH_230] = Channel230.ToJson();
        obj[Dict.CH_260] = Channel260.ToJson();
        obj[Dict.CH_280] = Channel280.ToJson();
        obj[Dict.CH_340] = Channel340.ToJson();
        return obj;
    }

    /// <summary>
    /// Returns a quadruple representing the sample values from all channels.
    /// </summary>
    /// <returns>A <see cref="Quadruple"/> containing the sample values.</returns>
    public Quadruple Sample()
    {
        return new Quadruple(
            Channel230.Sample,
            Channel260.Sample,
            Channel280.Sample,
            Channel340.Sample
        );
    }

    /// <summary>
    /// Returns a quadruple representing the reference values from all channels.
    /// </summary>
    /// <returns>A <see cref="Quadruple"/> containing the reference values.</returns>
    public Quadruple Reference()
    {
        return new Quadruple(
            Channel230.Reference,
            Channel260.Reference,
            Channel280.Reference,
            Channel340.Reference
        );
    }

    /// <summary>
    /// Creates a <see cref="SingleMeasurement"/> instance from a JSON node.
    /// </summary>
    /// <param name="node">The JSON node containing measurement data.</param>
    /// <returns>A new <see cref="SingleMeasurement"/> instance.</returns>
    /// <exception cref="ArgumentNullException">Thrown if the JSON node is null or missing required properties.</exception>
    public static SingleMeasurement FromJson(JsonNode node)
    {
        if (node == null) throw new ArgumentNullException(nameof(node));

        return new SingleMeasurement(
            Channel.FromJson(node[Dict.CH_230] ?? throw new ArgumentNullException(nameof(node))),
            Channel.FromJson(node[Dict.CH_260] ?? throw new ArgumentNullException(nameof(node))),
            Channel.FromJson(node[Dict.CH_280] ?? throw new ArgumentNullException(nameof(node))),
            Channel.FromJson(node[Dict.CH_340] ?? throw new ArgumentNullException(nameof(node)))
        );
    }
}
