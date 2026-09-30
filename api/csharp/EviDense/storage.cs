// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;

namespace Hse.EviDense;

/// <summary>
/// Represents an entry in the storage measurement system.
/// Contains a measurement, an optional comment, results, and raw JSON data.
/// </summary>
/// <remarks>
/// Initializes a new instance of the <see cref="StorageMeasurementEntry"/> class.
/// </remarks>
/// <param name="measurement">The measurement data.</param>
/// <param name="comment">An optional comment.</param>
/// <param name="results">The associated results, if available.</param>
/// <param name="node">The raw JSON node, if available.</param>
public class StorageMeasurementEntry(Measurement measurement, String? comment = null, Results? results = null, JsonNode? node = null)
{
    /// <summary>
    /// The stored measurement data.
    /// </summary>
    public Measurement Measurement { get; set; } = measurement;

    /// <summary>
    /// An optional comment associated with the measurement entry.
    /// </summary>
    public String? Comment { get; set; } = comment;

    /// <summary>
    /// The calculated results associated with the measurement, if available.
    /// </summary>
    public Results? Results { get; set; } = results;

    /// <summary>
    /// The raw JSON node representation of the measurement entry.
    /// </summary>
    public JsonNode? Node { get; set; } = node;

    /// <summary>
    /// Returns a string representation of the storage measurement entry.
    /// </summary>
    /// <returns>A formatted string displaying measurement details.</returns>
    public override string ToString()
    {
        return $"Measurement:{Measurement} Comment:{Comment} Results:{Results}";
    }

    /// <summary>
    /// Determines whether the entry contains calculated results.
    /// </summary>
    /// <returns><c>true</c> if results exist, otherwise <c>false</c>.</returns>
    public bool HasResults()
    {
        return Results != null;
    }

    /// <summary>
    /// Applies the calculated results based on given correction factors.
    /// Updates the JSON node representation if available.
    /// </summary>
    /// <param name="factors">The correction factors to apply.</param>
    /// <remarks>
    /// When the entry was loaded from JSON (<see cref="Node"/> != <c>null</c>), the computed results are
    /// written back into that JSON node under <c>"results"</c>.
    /// </remarks> 
    public void ApplyResults(Factors factors, DeviceSettings? deviceSettings = null)
    {
        if (Node != null)
        {
            Node[Dict.RESULTS] = Measurement.Results(factors, deviceSettings: deviceSettings).ToJson();
        }
    }

    /// <summary>
    /// Creates a <see cref="StorageMeasurementEntry"/> instance from a JSON node.
    /// Parses the measurement, comment, and results data.
    /// </summary>
    /// <param name="node">The JSON node containing the measurement entry data.</param>
    /// <returns>A populated <see cref="StorageMeasurementEntry"/> instance.</returns>
    public static StorageMeasurementEntry FromJson(JsonNode node)
    {
        Results? results = null;
        String? comment = null;

        if (node.AsObject().ContainsKey(Dict.RESULTS))
        {
            results = Results.FromJson(node[Dict.RESULTS]);
        }

        if (node.AsObject().ContainsKey(Dict.COMMENT))
        {
            if (node is JsonObject jsonObject &&
            jsonObject.TryGetPropertyValue(Dict.COMMENT, out JsonNode? commentNode) &&
            commentNode is not null)
            {
                comment = commentNode.GetValue<string>();
            }
        }
        return new StorageMeasurementEntry(Measurement.FromJson(node), comment, results, node);
    }
}


/// <summary>
/// Handles storage and retrieval of measurements using JSON serialization.
/// </summary>
public class StorageMeasurement
{
    /// <summary>
    /// Raw JSON structure representing all stored measurement data.
    /// This node may contain measurements, results, metadata, logging, and other associated entries.
    /// </summary>
    public JsonNode data { get; }

    /// <summary>
    /// Initializes a new instance of the <see cref="StorageMeasurement"/> class, optionally loading from a file.
    /// </summary>
    /// <param name="filename">Optional filename to load measurements from.</param>
    public StorageMeasurement(string filename = "")
    {
        if (string.IsNullOrEmpty(filename))
        {
            data = new JsonObject();
            data.AsObject()[Dict.MEASUREMENTS] = new JsonArray();
        }
        else
        {
            using var reader = new StreamReader(filename);
            var json = reader.ReadToEnd();
            data = JsonNode.Parse(json) ?? new JsonObject();
        }
    }

    /// <summary>
    /// Appends a measurement to storage.
    /// </summary>
    /// <param name="measurement">The measurement to append.</param>
    /// <param name="comment">An optional comment for the measurement.</param>
    /// <param name="logging">Optional logging information.</param>
    public void Append(Measurement measurement, string comment = "", List<string>? logging = null)
    {
        if (measurement == null)
            throw new ArgumentException("No measurement object provided to append!");

        var m = measurement.ToJson();
        m[Dict.COMMENT] = comment;

        if (logging != null && logging.Count > 0)
        {
            m[Dict.LOGGING] = new JsonArray();
            foreach (var log in logging)
            {
                m[Dict.LOGGING]?.AsArray().Add(log);
            }
        }

        m[Dict.DATE_TIME] = DateTime.UtcNow.ToString("o");

        data[Dict.MEASUREMENTS]?.AsArray().Add(m);
    }

    /// <summary>
    /// Appends a measurement and the results to storage.
    /// </summary>
    /// <param name="measurement">The measurement to append.</param>
    /// <param name="results">The results to append.</param>
    /// <param name="comment">An optional comment for the measurement.</param>
    /// <param name="logging">Optional logging information.</param>
    public void AppendWithResults(Measurement measurement, Results results, string comment = "", List<string>? logging = null)
    {
        if (measurement == null)
            throw new ArgumentException("No measurement object provided to append!");

        var m = measurement.ToJson();

        if (results != null)
            m[Dict.RESULTS] = results.ToJson();

        m[Dict.COMMENT] = comment;

        if (logging != null && logging.Count > 0)
        {
            m[Dict.LOGGING] = new JsonArray();
            foreach (var log in logging)
            {
                m[Dict.LOGGING]?.AsArray().Add(log);
            }
        }

        m[Dict.DATE_TIME] = DateTime.UtcNow.ToString("o");

        data[Dict.MEASUREMENTS]?.AsArray().Add(m);
    }

    public void AddDeviceInfo(Device device, string comment = "")
    {
        data[Dict.INFO] = new JsonObject
        {
            ["date"] = DateTime.UtcNow.ToString("o"),
            ["product"] = "eviDense",
            ["production_number"] = device.ProductionNumber(),
            ["serial_number"] = device.SerialNumber(),
            ["firmware_version"] = device.FirmwareVersion(),
            ["comment"] = comment,
            ["api"] = Device.LibraryVersion,
        };
    }

    public void AddDict(string key, JsonNode? value)
    {
        data[key] = value?.DeepClone();
    }

    /// <summary>
    /// Saves the measurement data to a specified file.
    /// </summary>
    /// <param name="filename">The filename to save data to.</param>
    public void Save(string filename)
    {
        var options = new JsonSerializerOptions
        {
            NumberHandling = JsonNumberHandling.AllowReadingFromString | JsonNumberHandling.AllowNamedFloatingPointLiterals,
            WriteIndented = true,
            TypeInfoResolver = new DefaultJsonTypeInfoResolver()
        };

        using var writer = new StreamWriter(filename);
        var json = data.ToJsonString(options);
        writer.Write(json);
    }

    /// <summary>
    /// Retrieves the list of stored measurements.
    /// </summary>
    /// <returns>A list of <see cref="Measurement"/> objects.</returns>
    public List<Measurement> Measurements()
    {
        var ret = new List<Measurement>();

        foreach (var m in data[Dict.MEASUREMENTS]?.AsArray() ?? [])
            if (m != null)
                ret.Add(Measurement.FromJson(m));

        return ret;
    }

    /// <summary>
    /// Retrieves the list of stored results.
    /// </summary>
    /// <returns>A list of <see cref="Measurement"/> objects.</returns>
    public List<Results> Results()
    {
        var ret = new List<Results>();

        foreach (var m in data[Dict.MEASUREMENTS]?.AsArray() ?? [])
        {
            if (m != null && m.AsObject().ContainsKey(Dict.RESULTS) && m[Dict.RESULTS] != null)
                ret.Add(global::Hse.EviDense.Results.FromJson(m[Dict.RESULTS]));
        }

        return ret;
    }

    /// <summary>
    /// Gets the measurement at the specified index.
    /// </summary>
    /// <param name="index">The index of the measurement.</param>
    /// <returns>The <see cref="Measurement"/> at the specified index.</returns>
    public StorageMeasurementEntry this[int index]
    {
        get
        {
            var measurementsArray = data[Dict.MEASUREMENTS]?.AsArray();
            if (measurementsArray == null || index < 0 || index >= measurementsArray.Count)
                throw new IndexOutOfRangeException("CustomRange index out of range");

            var node = measurementsArray.ElementAt(index) ?? throw new IndexOutOfRangeException("CustomRange index out of range");
            return StorageMeasurementEntry.FromJson(node);
        }
    }


    /// <summary>
    /// Gets the number of stored measurements.
    /// </summary>
    public int Count => data[Dict.MEASUREMENTS]?.AsArray().Count ?? 0;

    public static void ExportAsCsv(string filenameJson)
    {
        if (!File.Exists(filenameJson))
        {
            throw new FileNotFoundException($"File {filenameJson} not found", filenameJson);
        }

        JsonNode? data = JsonNode.Parse(File.ReadAllText(filenameJson));
        string filenameCsv = Path.ChangeExtension(filenameJson, ".csv");
        string[] fieldnames =
        [
            Dict.COMMENT,
            Dict.BASELINE_230_REFERENCE,
            Dict.BASELINE_230_SAMPLE,
            Dict.BASELINE_260_REFERENCE,
            Dict.BASELINE_260_SAMPLE,
            Dict.BASELINE_280_REFERENCE,
            Dict.BASELINE_280_SAMPLE,
            Dict.BASELINE_340_REFERENCE,
            Dict.BASELINE_340_SAMPLE,
            Dict.AIR_230_REFERENCE,
            Dict.AIR_230_SAMPLE,
            Dict.AIR_260_REFERENCE,
            Dict.AIR_260_SAMPLE,
            Dict.AIR_280_REFERENCE,
            Dict.AIR_280_SAMPLE,
            Dict.AIR_340_REFERENCE,
            Dict.AIR_340_SAMPLE,
            Dict.SAMPLE_230_REFERENCE,
            Dict.SAMPLE_230_SAMPLE,
            Dict.SAMPLE_260_REFERENCE,
            Dict.SAMPLE_260_SAMPLE,
            Dict.SAMPLE_280_REFERENCE,
            Dict.SAMPLE_280_SAMPLE,
            Dict.SAMPLE_340_REFERENCE,
            Dict.SAMPLE_340_SAMPLE,
            Dict.dsDNA,
            Dict.ssDNA,
            Dict.ssRNA,
            Dict.A230,
            Dict.A260,
            Dict.A280,
            Dict.A340,
            Dict.PURITY260_230,
            Dict.PURITY260_280,
        ];

        using StreamWriter writer = new(filenameCsv, false, Encoding.UTF8);
        writer.WriteLine(string.Join(";", fieldnames));

        foreach (JsonNode? measurement in data?[Dict.MEASUREMENTS]?.AsArray() ?? [])
        {
            JsonNode? results = measurement?[Dict.RESULTS];
            string[] row =
            [
                GetString(measurement, Dict.COMMENT),
                GetString(measurement, Dict.BASELINE, Dict.CH_230, Dict.REFERENCE),
                GetString(measurement, Dict.BASELINE, Dict.CH_230, Dict.SAMPLE),
                GetString(measurement, Dict.BASELINE, Dict.CH_260, Dict.REFERENCE),
                GetString(measurement, Dict.BASELINE, Dict.CH_260, Dict.SAMPLE),
                GetString(measurement, Dict.BASELINE, Dict.CH_280, Dict.REFERENCE),
                GetString(measurement, Dict.BASELINE, Dict.CH_280, Dict.SAMPLE),
                GetString(measurement, Dict.BASELINE, Dict.CH_340, Dict.REFERENCE),
                GetString(measurement, Dict.BASELINE, Dict.CH_340, Dict.SAMPLE),
                GetString(measurement, Dict.AIR, Dict.CH_230, Dict.REFERENCE),
                GetString(measurement, Dict.AIR, Dict.CH_230, Dict.SAMPLE),
                GetString(measurement, Dict.AIR, Dict.CH_260, Dict.REFERENCE),
                GetString(measurement, Dict.AIR, Dict.CH_260, Dict.SAMPLE),
                GetString(measurement, Dict.AIR, Dict.CH_280, Dict.REFERENCE),
                GetString(measurement, Dict.AIR, Dict.CH_280, Dict.SAMPLE),
                GetString(measurement, Dict.AIR, Dict.CH_340, Dict.REFERENCE),
                GetString(measurement, Dict.AIR, Dict.CH_340, Dict.SAMPLE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_230, Dict.REFERENCE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_230, Dict.SAMPLE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_260, Dict.REFERENCE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_260, Dict.SAMPLE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_280, Dict.REFERENCE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_280, Dict.SAMPLE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_340, Dict.REFERENCE),
                GetString(measurement, Dict.SAMPLE, Dict.CH_340, Dict.SAMPLE),
                GetString(results, Dict.dsDNA),
                GetString(results, Dict.ssDNA),
                GetString(results, Dict.ssRNA),
                GetString(results, Dict.A230),
                GetString(results, Dict.A260),
                GetString(results, Dict.A280),
                GetString(results, Dict.A340),
                GetString(results, Dict.PURITY260_230),
                GetString(results, Dict.PURITY260_280),
            ];

            writer.WriteLine(string.Join(";", row.Select(EscapeCsv)));
        }
    }

    private static string GetString(JsonNode? node, params string[] path)
    {
        JsonNode? current = node;
        foreach (string part in path)
        {
            current = current?[part];
        }

        if (current == null)
        {
            return string.Empty;
        }

        return current.ToJsonString().Trim('"');
    }

    private static string EscapeCsv(string value)
    {
        return value.Contains(';') || value.Contains('"') || value.Contains('\n')
            ? $"\"{value.Replace("\"", "\"\"")}\""
            : value;
    }
}
