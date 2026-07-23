// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Â© 2025 HSE AG, <opensource@hseag.com>

using System;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Device-specific wavelength adjustments used for purity correction.
/// </summary>
public class DeviceSettings(double centerWavelength260 = 260.0, double centerWavelength280 = 280.0)
{
    public double CenterWavelength260 { get; set; } = centerWavelength260;

    public double CenterWavelength280 { get; set; } = centerWavelength280;

    public override string ToString()
    {
        return $"Center wavelength 260:{CenterWavelength260} Center wavelength 280:{CenterWavelength280}";
    }

    public JsonNode ToJson()
    {
        return new JsonObject
        {
            [Dict.CENTER_WAVELENGTHS] = new JsonObject
            {
                [Dict.CH_260] = CenterWavelength260,
                [Dict.CH_280] = CenterWavelength280,
            }
        };
    }

    public static DeviceSettings FromDevice(Device device)
    {
        DeviceSettings settings = new();
        try
        {
            settings.CenterWavelength260 = device.Get<int>(Index.LED260NM_CENTER_WAVE_LENGTH) / 1000.0;
        }
        catch (Exception)
        {
        }

        try
        {
            settings.CenterWavelength280 = device.Get<int>(Index.LED280NM_CENTER_WAVE_LENGTH) / 1000.0;
        }
        catch (Exception)
        {
        }

        return settings;
    }

    public static DeviceSettings FromJson(JsonNode? node)
    {
        if (node == null || node[Dict.CENTER_WAVELENGTHS] == null)
        {
            return new DeviceSettings();
        }

        JsonNode wavelengths = node[Dict.CENTER_WAVELENGTHS]!;
        double centerWavelength260 = wavelengths[Dict.CH_260] == null ? 260.0 : JsonHelpers.ReadDouble(wavelengths[Dict.CH_260], Dict.CH_260);
        double centerWavelength280 = wavelengths[Dict.CH_280] == null ? 280.0 : JsonHelpers.ReadDouble(wavelengths[Dict.CH_280], Dict.CH_280);
        return new DeviceSettings(centerWavelength260, centerWavelength280);
    }
}

