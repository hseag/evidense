// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2025 HSE AG, <opensource@hseag.com>

using System;
using System.IO;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace Hse.EviDense;

/// <summary>
/// Coordinates a guided measurement run including state persistence.
/// </summary>
public class Run : IDisposable
{
    public enum RunState
    {
        BASELINE = 0,
        AIR = 1,
        SAMPLE = 2,
    }

    private readonly int NrOfBlanks_ = 1;
    private readonly bool NoPurityRatio260280Correction_ = true;
    private readonly DeviceSettings? DeviceSettings_ = null;
    private readonly bool OwnsDevice_;
    private bool disposed_;

    public string? Filename { get; private set; }
    public Device Device { get; private set; }
    public int Count { get; private set; } = 0;
    public StorageMeasurement Storage { get; private set; } = new StorageMeasurement();
    public Factors? Factors { get; private set; } = null;
    public RunState State { get; private set; } = RunState.BASELINE;
    public SingleMeasurement? Baseline { get; private set; } = null;
    public SingleMeasurement? Air { get; private set; } = null;
    public SingleMeasurement? Sample { get; private set; } = null;

    public Run(
        int nrOfBlanks,
        string? path = null,
        string? filename = null,
        object? device = null,
        bool noPurityRatio260280Correction = true,
        bool addDeviceInfo = true)
    {
        NrOfBlanks_ = nrOfBlanks;
        NoPurityRatio260280Correction_ = noPurityRatio260280Correction;

        if (device is Device existingDevice)
        {
            Device = existingDevice;
            OwnsDevice_ = false;
        }
        else
        {
            Device = new Device(device as string);
            OwnsDevice_ = true;
        }

        Filename = filename ?? $"evidense-{Device.SerialNumber()}-{DateTime.Now:yyyy_MM_dd_HH_mm_ss}.json";
        if (path != null)
        {
            Filename = Path.Combine(path, Filename);
        }

        if (addDeviceInfo)
        {
            Storage.AddDeviceInfo(Device);
            Storage.AddDict(Dict.PARAMETERS, Parameters());
        }

        if (!NoPurityRatio260280Correction_)
        {
            DeviceSettings_ = DeviceSettings.FromDevice(Device);
            Storage.AddDict(Dict.ADJUSTMENTS, DeviceSettings_.ToJson());
        }
    }

    ~Run()
    {
        Dispose(disposing: false);
    }

    public void Dispose()
    {
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }

    private void Dispose(bool disposing)
    {
        if (disposed_)
        {
            return;
        }

        if (OwnsDevice_)
        {
            Device.Dispose();
        }

        disposed_ = true;
    }

    private JsonObject Parameters()
    {
        return new JsonObject
        {
            [Dict.NR_OF_BLANKS] = NrOfBlanks_,
            [Dict.FACTORS] = Factors?.ToJson(),
        };
    }

    private static JsonSerializerOptions JsonOptions()
    {
        return new JsonSerializerOptions
        {
            WriteIndented = true,
            NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowReadingFromString |
                             System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
        };
    }

    public static string ResolveStateFilename(object? device = null, string? filename = null)
    {
        if (!string.IsNullOrEmpty(filename))
        {
            return filename;
        }

        try
        {
            return device switch
            {
                Device deviceInstance => $"evidense-{deviceInstance.SerialNumber()}-state.json",
                string serial => ResolveStateFilenameForSerial(serial),
                _ => ResolveStateFilenameForDefaultDevice(),
            };
        }
        catch (Exception)
        {
            return "state.json";
        }
    }

    private static string ResolveStateFilenameForSerial(string serial)
    {
        return $"evidense-{serial}-state.json";
    }

    private static string ResolveStateFilenameForDefaultDevice()
    {
        return "state.json";
    }

    public static Run LoadState(string? filename = null)
    {
        string resolvedFilename = ResolveStateFilename(filename: filename);
        JsonNode? state = JsonNode.Parse(File.ReadAllText(resolvedFilename)) ?? throw new InvalidOperationException("Run state is empty");
        string? measurementFilename = state["filename"]?.GetValue<string>();
        Run run = new(
            state["nr_of_blanks"]?.GetValue<int>() ?? throw new InvalidOperationException("nr_of_blanks missing"),
            filename: measurementFilename,
            device: state["device"]?.GetValue<string>(),
            noPurityRatio260280Correction: state["no_purity_ratio_260_280_correction"]?.GetValue<bool>() ?? true,
            addDeviceInfo: false)
        {
            Count = state["count"]?.GetValue<int>() ?? 0,
            State = (RunState)(state["state"]?.GetValue<int>() ?? 0),
            Baseline = state["baseline"] == null ? null : SingleMeasurement.FromJson(state["baseline"]!),
            Air = state["air"] == null ? null : SingleMeasurement.FromJson(state["air"]!),
            Sample = state["sample"] == null ? null : SingleMeasurement.FromJson(state["sample"]!),
            Factors = state["factors"] == null ? null : global::Hse.EviDense.Factors.FromJson(state["factors"])
        };

        if (!string.IsNullOrEmpty(measurementFilename) && File.Exists(measurementFilename))
        {
            run.Storage = new StorageMeasurement(measurementFilename);
        }
        else
        {
            run.Storage = new StorageMeasurement();
        }

        run.ReCalculate();
        return run;
    }

    public void SaveState(string? filename = null)
    {
        string resolvedFilename = ResolveStateFilename(Device, filename);
        JsonObject state = new()
        {
            ["filename"] = Filename,
            ["nr_of_blanks"] = NrOfBlanks_,
            ["count"] = Count,
            ["state"] = (int)State,
            ["device"] = Device.IsSimulation ? "SIMULATION" : Device.SerialNumber(),
            ["baseline"] = Baseline?.ToJson(),
            ["air"] = Air?.ToJson(),
            ["sample"] = Sample?.ToJson(),
            ["factors"] = Factors?.ToJson(),
            ["no_purity_ratio_260_280_correction"] = NoPurityRatio260280Correction_,
        };

        File.WriteAllText(resolvedFilename, state.ToJsonString(JsonOptions()));
        if (Filename == null)
        {
            throw new InvalidOperationException("Filename cant be null!");
        }
        Storage.Save(Filename);
    }

    protected void ReCalculate()
    {
        if (Factors == null && Storage.Count >= NrOfBlanks_ && NrOfBlanks_ > 0)
        {
            Factors factors = new ();
            double count = 0.0;

            foreach (Measurement measurement in Storage.Measurements().GetRange(0, NrOfBlanks_))
            {
                factors += measurement.Factors();
                count += 1.0;
            }

            Factors = factors / count;
            Storage.AddDict(Dict.PARAMETERS, Parameters());
        }

        if (Factors != null)
        {
            for (int i = 0; i < Storage.Count; i++)
            {
                if (!Storage[i].HasResults())
                {
                    Storage[i].ApplyResults(Factors, DeviceSettings_);
                }
            }
        }
    }

    public void measure(string comment = "")
    {
        switch (State)
        {
            case RunState.BASELINE:
                Baseline = Device.Baseline();
                State = RunState.AIR;
                break;

            case RunState.AIR:
                Air = Device.Measure();
                State = RunState.SAMPLE;
                break;

            case RunState.SAMPLE:
                if (Baseline == null)
                {
                    throw new InvalidOperationException("Baseline cant be null!");
                }
                if (Air == null)
                {
                    throw new InvalidOperationException("Air cant be null!");
                }

                Sample = Device.Measure();
                Storage.Append(new Measurement(Baseline, Air, Sample), comment, Device.Logging());
                State = RunState.BASELINE;
                break;
        }

        ReCalculate();
        if (Filename == null)
        {
            throw new InvalidOperationException("Filename cant be null!");
        }
        Storage.Save(Filename);
        Count++;
    }

    public bool checkEmpty()
    {
        return Device.IsCuvetteHolderEmpty();
    }

    public void ExportAsCsv()
    {
        if (Filename == null)
        {
            throw new InvalidOperationException("Filename cant be null!");
        }
        StorageMeasurement.ExportAsCsv(Filename);
    }

    public void ExportAsKit(string filename, string? comment = null)
    {
        Kit kit = new(Factors ?? throw new InvalidOperationException("Run has no factors"), comment);
        kit.Save(filename);
    }

    public void ImportKit(string filename)
    {
        Kit kit = Kit.Load(filename);
        Factors = kit.Factors;
        Storage.AddDict(Dict.PARAMETERS, Parameters());
        if (Filename == null)
        {
            throw new InvalidOperationException("Filename cant be null!");
        }
        Storage.Save(Filename);
    }
}
