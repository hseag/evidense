// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Serialization;

namespace Hse.EviDense;


/// <summary>
/// Handles storage and retrieval of measurements using JSON serialization.
/// </summary>
public class StorageMeasurement
{
    private JsonNode data;


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
            using (var reader = new StreamReader(filename))
            {
                var json = reader.ReadToEnd();
                data = JsonNode.Parse(json) ?? new JsonObject();
            }
        }
    }

    /// <summary>
    /// Appends a measurement to storage.
    /// </summary>
    /// <param name="measurement">The measurement to append.</param>
    /// <param name="comment">An optional comment for the measurement.</param>
    public void Append(Measurement measurement, string comment = "")
    {
        if (measurement == null)
            throw new ArgumentException("No measurement object provided to append!");

        var m = measurement.ToJson();
        m[Dict.COMMENT] = comment;

        data[Dict.MEASUREMENTS]?.AsArray().Add(m);
    }

    /// <summary>
    /// Appends a measurement and the results to storage.
    /// </summary>
    /// <param name="measurement">The measurement to append.</param>
    /// /// <param name="results">The results to append.</param>
    /// <param name="comment">An optional comment for the measurement.</param>
    public void AppendWithResults(Measurement measurement, Results results, string comment = "")
    {
        if (measurement == null)
            throw new ArgumentException("No measurement object provided to append!");

        var m = measurement.ToJson();

        if (results != null)
            m[Dict.RESULTS] = results.ToJson();

        m[Dict.COMMENT] = comment;

        data[Dict.MEASUREMENTS]?.AsArray().Add(m);
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
            WriteIndented = true
        };

        using (var writer = new StreamWriter(filename))
        {
            var json = data.ToJsonString(options);
            writer.Write(json);
        }
    }

    /// <summary>
    /// Retrieves the list of stored measurements.
    /// </summary>
    /// <returns>A list of <see cref="Measurement"/> objects.</returns>
    public List<Measurement> Measurements()
    {
        var ret = new List<Measurement>();

        foreach (var m in data[Dict.MEASUREMENTS]?.AsArray() ?? new JsonArray())
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

        foreach (var m in data[Dict.MEASUREMENTS]?.AsArray() ?? new JsonArray())
        {
            if (m != null && m.AsObject().ContainsKey(Dict.RESULTS) && m[Dict.RESULTS] != null)
                ret.Add(EviDense.Results.FromJson(m[Dict.RESULTS]));
        }

        return ret;
    }

    /// <summary>
    /// Gets the measurement at the specified index.
    /// </summary>
    /// <param name="index">The index of the measurement.</param>
    /// <returns>The <see cref="Measurement"/> at the specified index.</returns>
    public Measurement this[int index]
    {
        get
        {
            var measurementsArray = data[Dict.MEASUREMENTS]?.AsArray();
            if (measurementsArray == null || index < 0 || index >= measurementsArray.Count)
                throw new IndexOutOfRangeException("CustomRange index out of range");

            return Measurement.FromJson(measurementsArray.ElementAt(index));
        }
    }


    /// <summary>
    /// Gets the number of stored measurements.
    /// </summary>
    public int Count => data[Dict.MEASUREMENTS]?.AsArray().Count ?? 0;
}