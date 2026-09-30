// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Â© 2025 HSE AG, <opensource@hseag.com>

using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Portable container for run factors.
/// </summary>
public class Kit(Factors factors, string? comment = null)
{
    public Factors Factors { get; set; } = factors;

    public string? Comment { get; set; } = comment;

    public override string ToString()
    {
        return $"factors:{Factors} comment:{Comment}";
    }

    public JsonNode ToJson()
    {
        if (Factors == null)
        {
            throw new InvalidOperationException("Kit has no factors");
        }

        return new JsonObject
        {
            [Dict.COMMENT] = Comment,
            [Dict.FACTORS] = Factors.ToJson(),
        };
    }

    public void Save(string filename)
    {
        using StreamWriter writer = new(filename);
        writer.Write(ToJson().ToJsonString(new JsonSerializerOptions { WriteIndented = true }));
    }

    public static Kit Load(string filename)
    {
        JsonNode? node = JsonNode.Parse(File.ReadAllText(filename));
        return FromJson(node);
    }

    public static Kit FromJson(JsonNode? node)
    {
        ArgumentNullException.ThrowIfNull(node);

        JsonNode? factorsNode = node[Dict.FACTORS] ?? throw new InvalidOperationException("Kit JSON does not contain factors");
        return new Kit(global::Hse.EviDense.Factors.FromJson(factorsNode), node[Dict.COMMENT]?.GetValue<string>());
    }

    public static Kit FromRun(string filename, string? comment = null)
    {
        JsonNode? node = JsonNode.Parse(File.ReadAllText(filename));
        JsonNode? parameters = (node?[Dict.PARAMETERS]) ?? throw new InvalidOperationException("Run JSON does not contain parameters");
        JsonNode? factorsNode = parameters[Dict.FACTORS] ?? throw new InvalidOperationException("Run JSON does not contain factors");
        return new Kit(global::Hse.EviDense.Factors.FromJson(factorsNode), comment ?? node?[Dict.COMMENT]?.GetValue<string>());
    }
}

