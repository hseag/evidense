using Hse.EviDense;
using System;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics.Metrics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace Hse.EviDense;

/// <summary>
/// Coordinates a guided measurement run: initial air/sample setup, repeated air+sample pairs,
/// verification, logging, on-the-fly factor calculation, and JSON persistence.
/// </summary>
public class Run
{
    private enum State
    {
        BASELINE,
        AIR,
        SAMPLE
    }

    /// <summary>Number of blank replicates to collect before factor calculation.</summary>
    private int NrOfBlanks_ = 1;

    /// <summary>Destination filename for the JSON log.</summary>
    public string? Filename { get; }
    private Device? Device_ = null;
    private int Count_ = 0;
    private StorageMeasurement Storage_ = new StorageMeasurement();
    private Factors? Factors_ = null;
    private State State_ = State.BASELINE;
    private SingleMeasurement? Baseline_ = null;
    private SingleMeasurement? Air_ = null;
    private SingleMeasurement? Sample_ = null;

    /// <summary>
    /// Initializes a new measurement run, opens the device connection, and prepares the log file path.
    /// </summary>
    /// <param name="nrOfBlanks">Replicates for the blank measurement.</param>
    /// <param name="path">Optional folder for the output JSON file.</param>
    /// <param name="filename">Optional file name; generated if <c>null</c>.</param>
    /// <param name="device">Optional device serial or <c>"SIMULATION"</c> for socket mode.</param>
    public Run(int nrOfBlanks, string? path = null, string? filename = null, string? device = null)
    {
        Filename = null;
        NrOfBlanks_ = nrOfBlanks;
        if (device == null)
        {
            Device_ = new Device();
        }
        else
        {
            Device_ = new Device(device);
        }
        var now = DateTime.UtcNow.ToString("o");

        if (filename != null)
        {
            Filename = filename;
        }
        else
        {
            Filename = string.Format("evidense-{0}-{1}.json", Device_.SerialNumber(), DateTime.Now.ToString("yyyy_MM_dd_HH_mm_ss"));
        }

        if (path != null)
        {
            Filename = System.IO.Path.Combine(path, Filename);
        }
    }

    /// <summary>
    /// Finalizer that disposes the underlying device if still open.
    /// </summary>
    ~Run()
    {
        if (Device_ != null)
        {
            Device_.Dispose();
        }
    }

    /// <summary>
    /// Computes calibration factors once the required blank measurements are collected and applies
    /// the results to stored measurements that do not yet have them.
    /// </summary>
    protected void reCalculate()
    {
        
        if (Factors_ == null && Storage_.Count == NrOfBlanks_)
        {
            var factors = new Factors();
            double count = 0.0;

            foreach (var measurement in Storage_.Measurements().GetRange(0, NrOfBlanks_))
            {
                factors += measurement.Factors();
                count += 1.0;
            }

            Factors_ = factors / count;
        }

        if (Factors_ != null)
        {
            for (int i = 0; i < Storage_.Count; i++)
            {
                if (!Storage_[i].HasResults())
                {
                    Storage_[i].ApplyResults(Factors_);
                }
            }
        }
    }

    /// <summary>
    /// Executes the next step in the run's state machine:
    /// <list type="bullet">
    /// <item>BASELINE: records a new baseline and transitions to AIR.</item>
    /// <item>AIR: measures the air sample and transitions to SAMPLE.</item>
    /// <item>SAMPLE: measures the sample, stores it with metadata, and returns to BASELINE.</item>
    /// </list>
    /// Saves the updated JSON after each step.
    /// </summary>
    /// <param name="comment">Optional annotation stored with the measurement.</param>
    /// <exception cref="InvalidOperationException">When required intermediate values are missing for the current state.</exception>
    public void measure(string comment = "")
    {
        if (Device_ == null)
        {
            throw new Exception("Device cant be null!");
        }

        switch (State_)
        {
            case State.BASELINE:
                {
                    Baseline_ = Device_.Baseline();
                    State_ = State.AIR;
                }
                break;

            case State.AIR:
                {
                    Air_ = Device_.Measure();
                    State_ = State.SAMPLE;
                }
                break;

            case State.SAMPLE:
                {
                    if (Baseline_ == null)
                    {
                        throw new Exception("Baseline cant be null!");
                    }

                    if (Air_ == null)
                    {
                        throw new Exception("Air cant be null!");
                    }

                    Sample_ = Device_.Measure();
                    var measurement = new Measurement(Baseline_, Air_, Sample_);
                    Storage_.Append(measurement, comment, Device_.Logging());
                    State_ = State.BASELINE;
                }
                break;
        }
        reCalculate();
        if (Filename == null)
        {
            throw new Exception("Filename cant be null!");
        }
        Storage_.Save(Filename);
        Count_++;
    }

    /// <summary>
    /// Returns whether the instrument’s cuvette holder is empty, as reported by the device.
    /// </summary>
    public bool checkEmpty()
    {
        if (Device_ == null)
        {
            throw new Exception("Device cant be null!");
        }
        return Device_.IsCuvetteHolderEmpty();
    }
}

