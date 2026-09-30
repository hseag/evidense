// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: (c) 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Represents the results of a nucleic acid analysis, including DNA and RNA concentrations and purity ratios.
/// </summary>
/// <remarks>
/// Initializes a new instance of the <see cref="Results"/> class with specified nucleic acid concentrations and purity ratios.
/// </remarks>
/// <param name="dsDNA">Double-stranded DNA concentration.</param>
/// <param name="ssDNA">Single-stranded DNA concentration.</param>
/// <param name="ssRNA">Single-stranded RNA concentration.</param>
/// <param name="purityRatio260_230">260/230 purity ratio.</param>
/// <param name="purityRatio260_280">260/280 purity ratio.</param>
/// <param name="a230">A230</param>
/// <param name="a260">A260</param>
/// <param name="a280">A280</param>
/// <param name="a340">A340</param>
public class Results(double dsDNA, double ssDNA, double ssRNA, double purityRatio260_230, double purityRatio260_280, double? a230 = null, double? a260 = null, double? a280 = null, double? a340 = null)
{
    /// <summary>
    /// Gets or sets the concentration of double-stranded DNA (dsDNA) in ng/ul.
    /// </summary>
    public double DsDNA { get; set; } = dsDNA;

    /// <summary>
    /// Gets or sets the concentration of single-stranded DNA (ssDNA) in ng/ul.
    /// </summary>
    public double SsDNA { get; set; } = ssDNA;

    /// <summary>
    /// Gets or sets the concentration of single-stranded RNA (ssRNA) in ng/ul.
    /// </summary>
    public double SsRNA { get; set; } = ssRNA;

    /// <summary>
    /// Gets or sets the 260/230 purity ratio, which indicates potential contamination by organic compounds or chaotropic salts.
    /// </summary>
    public double PurityRatio260_230 { get; set; } = purityRatio260_230;

    /// <summary>
    /// Gets or sets the 260/280 purity ratio, which indicates protein contamination.
    /// </summary>
    public double PurityRatio260_280 { get; set; } = purityRatio260_280;

    /// <summary>
    /// Gets or sets the absorbance at 230 nm without A340 correction.
    /// </summary>
    public double? A230 { get; set; } = a230;

    /// <summary>
    /// Gets or sets the absorbance at 260 nm without A340 correction.
    /// </summary>
    public double? A260 { get; set; } = a260;

    /// <summary>
    /// Gets or sets the absorbance at 280 nm without A340 correction.
    /// </summary>
    public double? A280 { get; set; } = a280;

    /// <summary>
    /// Gets or sets the absorbance at 340 nm without A340 correction.
    /// </summary>
    public double? A340 { get; set; } = a340;

    /// <summary>
    /// Returns a string representation of the results.
    /// </summary>
    /// <returns>A formatted string with dsDNA, ssDNA, ssRNA, and purity ratios.</returns>
    public override string ToString()
    {
        var ret = $"dsDNA:{DsDNA} ssDNA:{SsDNA} ssRNA:{SsRNA} 260/230:{PurityRatio260_230} 260/280:{PurityRatio260_280}";
        if (A230 is not null)
        {
            ret += $" A230:{A230} A260:{A260} A280:{A280} A340:{A340}";
        }
        return ret;
    }

    /// <summary>
    /// Converts the current instance to a JSON representation.
    /// </summary>
    /// <returns>A <see cref="JsonNode"/> representing the current object.</returns>
    /// <summary>
    /// Converts the factors to a JSON representation.
    /// </summary>
    /// <returns>A JSON node representing the factors.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new()
        {
            [Dict.dsDNA] = JsonValue.Create(DsDNA),
            [Dict.ssDNA] = JsonValue.Create(SsDNA),
            [Dict.ssRNA] = JsonValue.Create(SsRNA),
            [Dict.PURITY260_230] = JsonValue.Create(PurityRatio260_230),
            [Dict.PURITY260_280] = JsonValue.Create(PurityRatio260_280)
        };
        if (A230 is not null)
        {
            obj[Dict.A230] = JsonValue.Create(A230);
            obj[Dict.A260] = JsonValue.Create(A260);
            obj[Dict.A280] = JsonValue.Create(A280);
            obj[Dict.A340] = JsonValue.Create(A340);
        }

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
        ArgumentNullException.ThrowIfNull(node);
        return new Results(
            JsonHelpers.ReadDouble(node[Dict.dsDNA], Dict.dsDNA),
            JsonHelpers.ReadDouble(node[Dict.ssDNA], Dict.ssDNA),
            JsonHelpers.ReadDouble(node[Dict.ssRNA], Dict.ssRNA),
            JsonHelpers.ReadDouble(node[Dict.PURITY260_230], Dict.PURITY260_230),
            JsonHelpers.ReadDouble(node[Dict.PURITY260_280], Dict.PURITY260_280),
            JsonHelpers.ReadOptionalDouble(node[Dict.A230]),
            JsonHelpers.ReadOptionalDouble(node[Dict.A260]),
            JsonHelpers.ReadOptionalDouble(node[Dict.A280]),
            JsonHelpers.ReadOptionalDouble(node[Dict.A340])
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
               Math.Abs(PurityRatio260_280 - other.PurityRatio260_280) < delta &&
               CompareOptional(A230, other.A230, delta) &&
               CompareOptional(A260, other.A260, delta) &&
               CompareOptional(A280, other.A280, delta) &&
               CompareOptional(A340, other.A340, delta);
    }

    private static bool CompareOptional(double? left, double? right, double delta)
    {
        if (left is null || right is null)
        {
            return true;
        }

        return Math.Abs(left.Value - right.Value) < delta;
    }

    /// <summary>
    /// Returns the hash code for this instance.
    /// </summary>
    /// <returns>A hash code for the current object.</returns>
    public override int GetHashCode()
    {
        HashCode hash = new();
        hash.Add(DsDNA);
        hash.Add(SsDNA);
        hash.Add(SsRNA);
        hash.Add(PurityRatio260_230);
        hash.Add(PurityRatio260_280);
        hash.Add(A230);
        hash.Add(A260);
        hash.Add(A280);
        hash.Add(A340);
        return hash.ToHashCode();
    }
}

/// <summary>
/// Correction factors due to different light absorption at different wavelengths of the cuvette.
/// </summary>
/// <remarks>
/// Initializes a new instance of the <see cref="Factors"/> class with optional correction factors.
/// </remarks>
/// <param name="fAbsorbanceBufferBlank">Correction absorbance for air to blank measurements.</param>
public class Factors(Quadruple? fAbsorbanceBufferBlank = null)
{
    /// <summary>
    /// Gets or sets the correction absorbance for air to blank measurements.
    /// </summary>
    public Quadruple AbsorbanceBufferBlank { get; set; } = fAbsorbanceBufferBlank ?? new Quadruple(0.0);

    /// <summary>
    /// Returns a string representation of the correction factors.
    /// </summary>
    /// <returns>A formatted string displaying the correction factors.</returns>
    public override string ToString()
    {
        return $"absorbanceBufferBlank:{AbsorbanceBufferBlank}";
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
            AbsorbanceBufferBlank = left.AbsorbanceBufferBlank + right.AbsorbanceBufferBlank,
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
            AbsorbanceBufferBlank = left.AbsorbanceBufferBlank / right.AbsorbanceBufferBlank,
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
            AbsorbanceBufferBlank = left.AbsorbanceBufferBlank / scalar,
        };
    }

    public JsonNode ToJson()
    {
        return new JsonObject
        {
            [Dict.BUFFER_BLANK] = AbsorbanceBufferBlank.ToJson(),
        };
    }

    /// <summary>
    /// Creates factors from a JSON representation.
    /// </summary>
    /// <param name="node">JSON node containing factor values.</param>
    /// <returns>A populated factors object.</returns>
    public static Factors FromJson(JsonNode? node)
    {
        ArgumentNullException.ThrowIfNull(node);

        JsonNode? bufferBlank = node[Dict.BUFFER_BLANK] ?? throw new InvalidOperationException($"{Dict.BUFFER_BLANK} is missing or null");
        return new Factors(Quadruple.FromJson(bufferBlank));
    }
}

/// <summary>
/// Represents a measurement containing baseline, air, and sample values, along with optional comments.
/// </summary>
/// <remarks>
/// Initializes a new instance of the <see cref="Measurement"/> class.
/// </remarks>
/// <param name="baseline">The baseline measurement.</param>
/// <param name="air">The air measurement.</param>
/// <param name="sample">The sample measurement.</param>
/// <param name="comment">An optional comment for the measurement.</param>
public class Measurement(SingleMeasurement baseline, SingleMeasurement air, SingleMeasurement sample, string comment = "")
{
    private static readonly (double Wavelength, double Absorption)[] TheoreticalDnaAbsorption =
    [
        (258.4691358024691, 0.8438538205980066),
        (258.8641975308642, 0.8438538205980066),
        (259.2592592592592, 0.8372093023255814),
        (259.6543209876543, 0.8372093023255814),
        (260.0493827160494, 0.8372093023255814),
        (260.4444444444445, 0.8372093023255814),
        (260.8395061728395, 0.8372093023255814),
        (261.2345679012346, 0.8305647840531561),
        (261.6296296296296, 0.8305647840531561),
        (262.0246913580247, 0.823920265780731),
        (262.4197530864197, 0.8172757475083057),
        (262.8148148148148, 0.8106312292358804),
        (263.2098765432099, 0.8106312292358804),
        (263.604938271605, 0.7973421926910299),
        (264, 0.7973421926910299),
        (264.395061728395, 0.7906976744186047),
        (264.7901234567901, 0.7840531561461794),
        (265.1851851851852, 0.7774086378737541),
        (265.5802469135803, 0.770764119601329),
        (265.9753086419753, 0.7641196013289037),
        (266.3703703703704, 0.7574750830564784),
        (266.7654320987654, 0.7508305647840532),
        (267.1604938271605, 0.7441860465116279),
        (267.5555555555555, 0.7375415282392027),
        (267.9506172839506, 0.7308970099667774),
        (268.3456790123457, 0.7242524916943521),
        (268.7407407407408, 0.717607973421927),
        (269.1358024691358, 0.7109634551495017),
        (269.5308641975308, 0.6976744186046512),
        (269.9259259259259, 0.6910299003322259),
        (270.320987654321, 0.6843853820598007),
        (270.7160493827161, 0.6777408637873754),
        (271.1111111111111, 0.6710963455149502),
        (271.5061728395062, 0.6644518272425249),
        (271.9012345679012, 0.6578073089700996),
        (272.2962962962963, 0.6445182724252492),
        (272.6913580246913, 0.6378737541528239),
        (273.0864197530864, 0.6312292358803987),
        (273.4814814814815, 0.6179401993355482),
        (273.8765432098766, 0.6112956810631229),
        (274.2716049382716, 0.5980066445182725),
        (274.6666666666666, 0.5913621262458472),
        (275.0617283950617, 0.5780730897009967),
        (275.4567901234568, 0.5714285714285714),
        (275.8518518518518, 0.5647840531561462),
        (276.2469135802469, 0.5514950166112957),
        (276.641975308642, 0.5448504983388704),
        (277.0370370370371, 0.53156146179402),
        (277.4320987654321, 0.5182724252491695),
        (277.8271604938271, 0.5116279069767442),
        (278.2222222222222, 0.5049833887043189),
        (278.6172839506173, 0.4983388704318937),
        (279.0123456790124, 0.4850498338870432),
        (279.4074074074074, 0.478405315614618),
        (279.8024691358025, 0.4717607973421927),
        (280.1975308641975, 0.4584717607973422),
        (280.5925925925926, 0.4451827242524917),
        (280.9876543209876, 0.4385382059800664),
        (281.3827160493827, 0.4318936877076412),
        (281.7777777777778, 0.425249169435216),
        (282.1728395061729, 0.4119601328903655),
        (282.5679012345679, 0.4053156146179402),
        (282.9629629629629, 0.3920265780730897),
    ];

    private string comment = comment;
    private readonly SingleMeasurement baseline = baseline;
    private readonly SingleMeasurement air = air;
    private readonly SingleMeasurement sample = sample;

    /// <summary>
    /// Defines the default path length of the cuvette.
    /// </summary>
    public const double DefaultCuvettePathLength = 1.04; //mm

    /// <summary>
    /// Returns a string representation of the measurement.
    /// </summary>
    /// <returns>A formatted string displaying baseline, air, and sample measurements.</returns>
    public override string ToString()
    {
        return $"baseline:{baseline} air:{air} sample:{sample}";
    }

    /// <summary>
    /// It calculates the absorbance based on air and sample measurements.
    /// This is calculated in the same way as the concentration and purity ratio, except that A340 is not subtracted from A230, A260 or A280.
    /// </summary>
    /// <param name="fAbsorbanceBufferBlank">Optional buffer absorbance correction factor.</param>
    /// <param name="deviceSettings">Optional device-specific wavelength settings for purity correction.</param>
    /// <returns>The absorbance value as a <see cref="Quadruple"/>.</returns>
    public Quadruple Absorbance(Quadruple? fAbsorbanceBufferBlank = null, DeviceSettings? deviceSettings = null)
    {
        fAbsorbanceBufferBlank ??= new Quadruple(0.0);
        Quadruple absorbance = CalculateAbsorbance(air, sample) - fAbsorbanceBufferBlank;
        if (deviceSettings != null)
        {
            double a280Real = GetTheoreticalDnaAbsorption(deviceSettings.CenterWavelength280);
            double a280Theoretical = GetTheoreticalDnaAbsorption(280.0);
            absorbance.Value280 = absorbance.Value280 * a280Theoretical / a280Real;
        }
        return absorbance;
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
        return new Factors(FactorAbsorbanceBufferBlank());
    }

    private Quadruple FactorAbsorbanceBufferBlank()
    {
        Quadruple a = CalculateAbsorbance(air, sample);
        return a;
    }

    /// <summary>
    /// Computes the final results using given correction factors.
    /// </summary>
    /// <param name="factors">The correction factors.</param>
    /// <param name="cuvettePathLength">The cuvette path length.</param>
    /// <param name="deviceSettings">Optional device-specific wavelength settings for purity correction.</param>
    /// <returns>A <see cref="Results"/> object containing calculated values.</returns>
    public Results Results(Factors factors, double cuvettePathLength = DefaultCuvettePathLength, DeviceSettings? deviceSettings = null)
    {
        var absorbance = Absorbance(factors.AbsorbanceBufferBlank, deviceSettings);
        return new Results(
            DsDNA(factors.AbsorbanceBufferBlank, cuvettePathLength, deviceSettings),
            SsDNA(factors.AbsorbanceBufferBlank, cuvettePathLength, deviceSettings),
            SsRNA(factors.AbsorbanceBufferBlank, cuvettePathLength, deviceSettings),
            PurityRatio260_230(factors.AbsorbanceBufferBlank, deviceSettings),
            PurityRatio260_280(factors.AbsorbanceBufferBlank, deviceSettings),
            absorbance.Value230,
            absorbance.Value260,
            absorbance.Value280,
            absorbance.Value340
        );
    }

    private double DsDNA(Quadruple fAbsorbanceBufferBlank, double cuvettePathLength, DeviceSettings? deviceSettings = null)
    {
        return AlgorithmV9(fAbsorbanceBufferBlank, deviceSettings).Value260 * 50 * 10 / cuvettePathLength;
    }

    private double SsDNA(Quadruple fAbsorbanceBufferBlank, double cuvettePathLength, DeviceSettings? deviceSettings = null)
    {
        return AlgorithmV9(fAbsorbanceBufferBlank, deviceSettings).Value260 * 33 * 10 / cuvettePathLength;
    }

    private double SsRNA(Quadruple fAbsorbanceBufferBlank, double cuvettePathLength, DeviceSettings? deviceSettings = null)
    {
        return AlgorithmV9(fAbsorbanceBufferBlank, deviceSettings).Value260 * 40 * 10 / cuvettePathLength;
    }

    private double PurityRatio260_280(Quadruple fAbsorbanceBufferBlank, DeviceSettings? deviceSettings = null)
    {
        Quadruple aNNN = AlgorithmV9(fAbsorbanceBufferBlank, deviceSettings);
        return aNNN.Value280 == 0 ? double.NaN : aNNN.Value260 / aNNN.Value280;
    }

    private double PurityRatio260_230(Quadruple fAbsorbanceBufferBlank, DeviceSettings? deviceSettings = null)
    {
        Quadruple aNNN = AlgorithmV9(fAbsorbanceBufferBlank, deviceSettings);
        return aNNN.Value230 == 0 ? double.NaN : aNNN.Value260 / aNNN.Value230;
    }

    private Quadruple AlgorithmV9(Quadruple fAbsorbanceBufferBlank, DeviceSettings? deviceSettings = null)
    {
        Quadruple aSampleMinusBlank = Absorbance(fAbsorbanceBufferBlank, deviceSettings);
        return aSampleMinusBlank - new Quadruple(aSampleMinusBlank.Value340);
    }

    private static double GetTheoreticalDnaAbsorption(double wavelength)
    {
        if (wavelength <= TheoreticalDnaAbsorption[0].Wavelength)
        {
            return TheoreticalDnaAbsorption[0].Absorption;
        }

        int last = TheoreticalDnaAbsorption.Length - 1;
        if (wavelength >= TheoreticalDnaAbsorption[last].Wavelength)
        {
            return TheoreticalDnaAbsorption[last].Absorption;
        }

        for (int i = 0; i < last; i++)
        {
            var (Wavelength, Absorption) = TheoreticalDnaAbsorption[i];
            var p1 = TheoreticalDnaAbsorption[i + 1];
            if (Wavelength <= wavelength && wavelength <= p1.Wavelength)
            {
                return Absorption + (p1.Absorption - Absorption) * (wavelength - Wavelength) / (p1.Wavelength - Wavelength);
            }
        }

        return TheoreticalDnaAbsorption[last].Absorption;
    }

    /// <summary>
    /// Creates factors from a JSON representation.
    /// </summary>
    /// <param name="node">JSON node containing factor values.</param>
    /// <returns>A populated factors object.</returns>
    public static global::Hse.EviDense.Factors FactorsFromJson(JsonNode? node)
    {
        return global::Hse.EviDense.Factors.FromJson(node);
    }

    /// <summary>
    /// Converts the measurement to a JSON representation.
    /// </summary>
    /// <returns>A JSON node representing the measurement.</returns>
    public JsonNode ToJson()
    {
        JsonObject obj = new()
        {
            [Dict.BASELINE] = baseline.ToJson(),
            [Dict.AIR] = air.ToJson(),
            [Dict.SAMPLE] = sample.ToJson()
        };

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
        ArgumentNullException.ThrowIfNull(node);
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
        Quadruple ods = new()
        {
            Value230 = Math.Log10(baseline.Channel230.Sample / baseline.Channel230.Reference * measurement.Channel230.Reference / measurement.Channel230.Sample * correctionFactor.Value230),
            Value260 = Math.Log10(baseline.Channel260.Sample / baseline.Channel260.Reference * measurement.Channel260.Reference / measurement.Channel260.Sample * correctionFactor.Value260),
            Value280 = Math.Log10(baseline.Channel280.Sample / baseline.Channel280.Reference * measurement.Channel280.Reference / measurement.Channel280.Sample * correctionFactor.Value280),
            Value340 = Math.Log10(baseline.Channel340.Sample / baseline.Channel340.Reference * measurement.Channel340.Reference / measurement.Channel340.Sample * correctionFactor.Value340)
        };
        return ods;
    }
}

