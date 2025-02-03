// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Represents the results of a nucleic acid analysis, including DNA and RNA concentrations and purity ratios.
/// </summary>
public class Results
{
    /// <summary>
    /// Gets or sets the concentration of double-stranded DNA (dsDNA) in ng/ul.
    /// </summary>
    public double DsDNA { get; set; }

    /// <summary>
    /// Gets or sets the concentration of single-stranded DNA (ssDNA) in ng/ul.
    /// </summary>
    public double SsDNA { get; set; }

    /// <summary>
    /// Gets or sets the concentration of single-stranded RNA (ssRNA) in ng/ul.
    /// </summary>
    public double SsRNA { get; set; }

    /// <summary>
    /// Gets or sets the 260/230 purity ratio, which indicates potential contamination by organic compounds or chaotropic salts.
    /// </summary>
    public double PurityRatio260_230 { get; set; }

    /// <summary>
    /// Gets or sets the 260/280 purity ratio, which indicates protein contamination.
    /// </summary>
    public double PurityRatio260_280 { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="Results"/> class with specified nucleic acid concentrations and purity ratios.
    /// </summary>
    /// <param name="dsDNA">Double-stranded DNA concentration.</param>
    /// <param name="ssDNA">Single-stranded DNA concentration.</param>
    /// <param name="ssRNA">Single-stranded RNA concentration.</param>
    /// <param name="purityRatio260_230">260/230 purity ratio.</param>
    /// <param name="purityRatio260_280">260/280 purity ratio.</param>
    public Results(double dsDNA, double ssDNA, double ssRNA, double purityRatio260_230, double purityRatio260_280)
    {
        DsDNA = dsDNA;
        SsDNA = ssDNA;
        SsRNA = ssRNA;
        PurityRatio260_230 = purityRatio260_230;
        PurityRatio260_280 = purityRatio260_280;
    }

    /// <summary>
    /// Returns a string representation of the results.
    /// </summary>
    /// <returns>A formatted string with dsDNA, ssDNA, ssRNA, and purity ratios.</returns>
    public override string ToString()
    {
        return $"dsDNA:{DsDNA} ssDNA:{SsDNA} ssRNA:{SsRNA} 260/230:{PurityRatio260_230} 260/280:{PurityRatio260_280}";
    }

    /// <summary>
    /// Converts the current instance to a JSON representation.
    /// </summary>
    /// <returns>A <see cref="JsonNode"/> representing the current object.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.dsDNA] = JsonValue.Create(DsDNA);
        obj[Dict.ssDNA] = JsonValue.Create(SsDNA);
        obj[Dict.ssRNA] = JsonValue.Create(SsRNA);
        obj[Dict.PURITY260_230] = JsonValue.Create(PurityRatio260_230);
        obj[Dict.PURITY260_280] = JsonValue.Create(PurityRatio260_280);

        return obj;
    }

    /// <summary>
    /// Creates a <see cref="Results"/> instance from a JSON representation.
    /// </summary>
    /// <param name="node">The JSON node containing the nucleic acid data.</param>
    /// <returns>A <see cref="Results"/> object populated from the JSON data.</returns>
    /// <exception cref="ArgumentNullException">Thrown if the provided JSON node is null.</exception>
    /// <exception cref="InvalidOperationException">Thrown if required fields are missing or null.</exception>
    public static Results FromJson(JsonNode? node)
    {
        if (node == null) throw new ArgumentNullException(nameof(node));
        return new Results(
            node[Dict.dsDNA]?.GetValue<double>() ?? throw new InvalidOperationException($"{Dict.dsDNA} is missing or null"),
            node[Dict.ssDNA]?.GetValue<double>() ?? throw new InvalidOperationException($"{Dict.ssDNA} is missing or null"),
            node[Dict.ssRNA]?.GetValue<double>() ?? throw new InvalidOperationException($"{Dict.ssRNA} is missing or null"),
            node[Dict.PURITY260_230]?.GetValue<double>() ?? throw new InvalidOperationException($"{Dict.PURITY260_230} is missing or null"),
            node[Dict.PURITY260_280]?.GetValue<double>() ?? throw new InvalidOperationException($"{Dict.PURITY260_280} is missing or null")
        );
    }

    /// <summary>
    /// Determines whether the specified object is equal to the current instance.
    /// </summary>
    /// <param name="obj">The object to compare with the current instance.</param>
    /// <returns><c>true</c> if the objects are equal; otherwise, <c>false</c>.</returns>
    public override bool Equals(object? obj)
    {
        if (obj is not Results other) return false;

        const double delta = 0.000000001; // Tolerance for floating-point comparisons
        return Math.Abs(DsDNA - other.DsDNA) < delta &&
               Math.Abs(SsDNA - other.SsDNA) < delta &&
               Math.Abs(SsRNA - other.SsRNA) < delta &&
               Math.Abs(PurityRatio260_230 - other.PurityRatio260_230) < delta &&
               Math.Abs(PurityRatio260_280 - other.PurityRatio260_280) < delta;
    }

    public override int GetHashCode()
    {
        return HashCode.Combine(DsDNA, SsDNA, SsRNA, PurityRatio260_230, PurityRatio260_280);
    }
}

/// <summary>
/// Correction factors due to different light absorption at different wavelengths of the cuvette.
/// </summary>
public class Factors
{
    /// <summary>
    /// Gets or sets the correction factor for air to blank measurements.
    /// </summary>
    public Quadruple AirToBlank { get; set; }

    /// <summary>
    /// Gets or sets the correction factor for F340 to NNN measurements.
    /// </summary>
    public Quadruple F340ToNNN { get; set; }

    /// <summary>
    /// Initializes a new instance of the <see cref="Factors"/> class with optional correction factors.
    /// </summary>
    /// <param name="factorAirToBlank">Correction factor for air to blank measurements.</param>
    /// <param name="factor340ToNNN">Correction factor for F340 to NNN measurements.</param>
    public Factors(Quadruple? factorAirToBlank = null, Quadruple? factor340ToNNN = null)
    {
        AirToBlank = factorAirToBlank ?? new Quadruple(0.0);
        F340ToNNN = factor340ToNNN ?? new Quadruple(0.0);
    }

    /// <summary>
    /// Returns a string representation of the correction factors.
    /// </summary>
    /// <returns>A formatted string displaying the correction factors.</returns>
    public override string ToString()
    {
        return $"airToBlank:{AirToBlank} 340ToNNN:{F340ToNNN}";
    }

    /// <summary>
    /// Defines the addition operator for <see cref="Factors"/>.
    /// </summary>
    /// <param name="left">The left operand.</param>
    /// <param name="right">The right operand.</param>
    /// <returns>A new <see cref="Factors"/> instance with the sum of corresponding values.</returns>
    public static Factors operator +(Factors left, Factors right)
    {
        return new Factors
        {
            AirToBlank = left.AirToBlank + right.AirToBlank,
            F340ToNNN = left.F340ToNNN + right.F340ToNNN
        };
    }

    /// <summary>
    /// Defines the division operator for <see cref="Factors"/>.
    /// </summary>
    /// <param name="left">The left operand.</param>
    /// <param name="right">The right operand.</param>
    /// <returns>A new <see cref="Factors"/> instance with the quotient of corresponding values.</returns>
    public static Factors operator /(Factors left, Factors right)
    {
        return new Factors
        {
            AirToBlank = left.AirToBlank / right.AirToBlank,
            F340ToNNN = left.F340ToNNN / right.F340ToNNN
        };
    }

    /// <summary>
    /// Defines the division operator for <see cref="Factors"/> with a scalar value.
    /// </summary>
    /// <param name="left">The left operand.</param>
    /// <param name="scalar">The scalar value.</param>
    /// <returns>A new <see cref="Factors"/> instance with each value divided by the scalar.</returns>
    public static Factors operator /(Factors left, double scalar)
    {
        return new Factors
        {
            AirToBlank = left.AirToBlank / scalar,
            F340ToNNN = left.F340ToNNN / scalar
        };
    }
}

/// <summary>
/// Represents a measurement containing baseline, air, and sample values, along with optional comments.
/// </summary>
public class Measurement
{
    private string comment;
    private SingleMeasurement baseline;
    private SingleMeasurement air;
    private SingleMeasurement sample;

    /// <summary>
    /// Initializes a new instance of the <see cref="Measurement"/> class.
    /// </summary>
    /// <param name="baseline">The baseline measurement.</param>
    /// <param name="air">The air measurement.</param>
    /// <param name="sample">The sample measurement.</param>
    /// <param name="comment">An optional comment for the measurement.</param>
    public Measurement(SingleMeasurement baseline, SingleMeasurement air, SingleMeasurement sample, string comment = "")
    {
        this.baseline = baseline;
        this.air = air;
        this.sample = sample;
        this.comment = comment;
    }

    /// <summary>
    /// Returns a string representation of the measurement.
    /// </summary>
    /// <returns>A formatted string displaying baseline, air, and sample measurements.</returns>
    public override string ToString()
    {
        return $"baseline:{baseline} air:{air} sample:{sample}";
    }

    /// <summary>
    /// Calculates the absorbance based on air and sample measurements.
    /// </summary>
    /// <param name="airToBlank">Optional air-to-blank correction factor.</param>
    /// <returns>The absorbance value as a <see cref="Quadruple"/>.</returns>
    public Quadruple Absorbance(Quadruple? airToBlank = null)
    {
        airToBlank ??= new Quadruple(1.0);
        return CalculateAbsorbance(air, sample, airToBlank);
    }

    /// <summary>
    /// Sets the comment for the measurement.
    /// </summary>
    /// <param name="comment">The comment text.</param>
    public void SetComment(string comment)
    {
        this.comment = comment;
    }

    /// <summary>
    /// Gets the comment associated with the measurement.
    /// </summary>
    /// <returns>The comment text.</returns>
    public string Comment()
    {
        return comment;
    }

    /// <summary>
    /// Computes the correction factors for the measurement. To get valid factors, the sample must not contain DNA or RNA (i.e. buffer only),
    /// </summary>
    /// <returns>A <see cref="Factors"/> object containing computed correction factors.</returns>
    public Factors Factors()
    {
        return new Factors(FactorAirToBlank(), Factor340ToNNN());
    }

    private Quadruple FactorAirToBlank()
    {
        return sample.Sample() / sample.Reference() * air.Reference() / air.Sample();
    }

    private Quadruple Factor340ToNNN()
    {
        Quadruple aNNN = CalculateAbsorbance(air, sample);
        Quadruple a340 = new Quadruple(aNNN.Value340);
        return a340 / aNNN;
    }

    /// <summary>
    /// Computes the final results using given correction factors.
    /// </summary>
    /// <param name="factors">The correction factors.</param>
    /// <returns>A <see cref="Results"/> object containing calculated values.</returns>
    public Results Results(Factors factors)
    {
        return new Results(
            DsDNA(factors.AirToBlank, factors.F340ToNNN),
            SsDNA(factors.AirToBlank, factors.F340ToNNN),
            SsRNA(factors.AirToBlank, factors.F340ToNNN),
            PurityRatio260_230(factors.AirToBlank, factors.F340ToNNN),
            PurityRatio260_280(factors.AirToBlank, factors.F340ToNNN)
        );
    }

    private double DsDNA(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        return AlgorithmV7(airToBlank, f340ToNNN).Value260 * 50 * 10 / 1;
    }

    private double SsDNA(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        return AlgorithmV7(airToBlank, f340ToNNN).Value260 * 33 * 10 / 1;
    }

    private double SsRNA(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        return AlgorithmV7(airToBlank, f340ToNNN).Value260 * 40 * 10 / 1;
    }

    private double PurityRatio260_280(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        Quadruple aNNN = AlgorithmV7(airToBlank, f340ToNNN);
        return aNNN.Value280 == 0 ? double.NaN : aNNN.Value260 / aNNN.Value280;
    }

    private double PurityRatio260_230(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        Quadruple aNNN = AlgorithmV7(airToBlank, f340ToNNN);
        return aNNN.Value230 == 0 ? double.NaN : aNNN.Value260 / aNNN.Value230;
    }

    private Quadruple AlgorithmV7(Quadruple airToBlank, Quadruple f340ToNNN)
    {
        Quadruple aNNN = CalculateAbsorbance(air, sample, airToBlank);
        return aNNN - new Quadruple(aNNN.Value340) * f340ToNNN;
    }

    /// <summary>
    /// Converts the measurement to a JSON representation.
    /// </summary>
    /// <returns>A JSON node representing the measurement.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new JsonObject();

        obj[Dict.BASELINE] = baseline.ToJson();
        obj[Dict.AIR] = air.ToJson();
        obj[Dict.SAMPLE] = sample.ToJson();

        if (!String.IsNullOrEmpty(comment))
        {
            obj[Dict.COMMENT] = JsonValue.Create(comment);
        }

        return obj;
    }

    /// <summary>
    /// Creates a <see cref="Measurement"/> instance from a JSON representation.
    /// </summary>
    /// <param name="node">The JSON node containing the measurement data.</param>
    /// <returns>A <see cref="Measurement"/> object populated from JSON data.</returns>
    /// <exception cref="ArgumentNullException">Thrown if the provided JSON node is null.</exception>
    /// <exception cref="InvalidOperationException">Thrown if required fields are missing.</exception>
    public static Measurement FromJson(JsonNode? node)
    {
        if (node == null) throw new ArgumentNullException(nameof(node));
        return new Measurement(
            SingleMeasurement.FromJson(node[Dict.BASELINE] ?? throw new InvalidOperationException($"{Dict.BASELINE} is missing or null")),
            SingleMeasurement.FromJson(node[Dict.AIR] ?? throw new InvalidOperationException($"{Dict.AIR} is missing or null")),
            SingleMeasurement.FromJson(node[Dict.SAMPLE] ?? throw new InvalidOperationException($"{Dict.SAMPLE} is missing or null")),
            node.AsObject().ContainsKey(Dict.COMMENT) ? node[Dict.COMMENT]?.ToString() ?? string.Empty : string.Empty
        );
    }

    /// <summary>
    /// Calculates the absorbance based on baseline and sample measurements.
    /// </summary>
    /// <param name="baseline">The baseline measurement.</param>
    /// <param name="measurement">The sample measurement.</param>
    /// <param name="correctionFactor">An optional correction factor for absorbance calculation.</param>
    /// <returns>The absorbance values as a <see cref="Quadruple"/>.</returns>
    public static Quadruple CalculateAbsorbance(SingleMeasurement baseline, SingleMeasurement measurement, Quadruple? correctionFactor = null)
    {
        correctionFactor ??= new Quadruple(1.0);
        Quadruple ods = new Quadruple
        {
            Value230 = Math.Log10(baseline.Channel230.Sample / baseline.Channel230.Reference * measurement.Channel230.Reference / measurement.Channel230.Sample * correctionFactor.Value230),
            Value260 = Math.Log10(baseline.Channel260.Sample / baseline.Channel260.Reference * measurement.Channel260.Reference / measurement.Channel260.Sample * correctionFactor.Value260),
            Value280 = Math.Log10(baseline.Channel280.Sample / baseline.Channel280.Reference * measurement.Channel280.Reference / measurement.Channel280.Sample * correctionFactor.Value280),
            Value340 = Math.Log10(baseline.Channel340.Sample / baseline.Channel340.Reference * measurement.Channel340.Reference / measurement.Channel340.Sample * correctionFactor.Value340)
        };
        return ods;
    }
}