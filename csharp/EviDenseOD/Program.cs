
using Hse.EviDense;

internal class Program
{
    Device device { get; set; }
    Factors ? factors { get; set; }

    StorageMeasurement storage{ get; set; }

    public Program()
    {
        device = new Device();
        storage = new StorageMeasurement();
        var sn = device.SerialNumber();
        Console.WriteLine($"Found eviDense module SN{sn}");
    }

    void PickupTip()
    {
        //The liquid handler picks up a tip.

    }
    void AspirateFromSample()
    {
        //Aspirate at least 11ul sample.
    }

    void PickUpCuvette()
    {
        //Pick up with the tip a cuvette.
    }

    void MoveOverCuvetteGuide()
    {
        //Move the tip with the cuvette over the eviDense cuvette guide.
    }
       
    void DispenseToSample()
    {
        //Dispense the sample back.
    }

    void DisposeTipWithAttachedCuvette()
    {
        //Dispose the tip with attached cuvette.
    }

    void MoveCuvetteInCuvetteGuide()
    {
        //Move the tip/sample/cuvette combination over the cuvette guide.
    }

    void Dispense(double volume)
    {
        //Dispense the given volume into the cuvette
    }

    void Aspirate(double volume)
    {
        //Transfer the given sample volume from the cuvette back into the tip.
    }

    void MoveCuvetteOutOfCuvetteGuide()
    {
        //Move the cuvette out of the cuvette guide.
    }

    void MeasureSampleWithBufferOnly(string comment)
    {
        var baseline = device.Baseline();

        if (device.IsCuvetteHolderEmpty() == true)
        {
            var volume = 10.0; //Cuvette volume in ul
            MoveCuvetteInCuvetteGuide();
            var air = device.Measure();

            Dispense(volume);
            var sample = device.Measure();

            Aspirate(volume);
            MoveCuvetteOutOfCuvetteGuide();

            if(device.IsCuvetteHolderEmpty() == false)
            {
                throw new Exception("Cuvette is stuck in cuvette guide!");
            }

            var firstMeasurement = new Measurement(baseline, air, sample);
            factors = firstMeasurement.Factors();

            storage.Append(firstMeasurement, comment);

            Console.WriteLine($"Calculated factors are: {factors}");
        }
        else
        {
            throw new Exception("Cuvette guide must be empty!");
        }
    }

    void MeasureSample(string comment)
    {
        var baseline = device.Baseline();

        if (device.IsCuvetteHolderEmpty() == true)
        {
            var volume = 10.0; //Cuvette volume in ul
            MoveCuvetteInCuvetteGuide();
            var air = device.Measure();

            Dispense(volume);
            var sample = device.Measure();

            Aspirate(volume);
            MoveCuvetteOutOfCuvetteGuide();

            if (device.IsCuvetteHolderEmpty() == false)
            {
                throw new Exception("Cuvette is stuck in cuvette guide!");
            }

            var measurement = new Measurement(baseline, air, sample);

            if (factors == null)
            {
                throw new Exception("Factors can't be null!");
            }

            var results = measurement.Results(factors);

            storage.AppendWithResults(measurement, results, comment);

            Console.WriteLine($"{comment} : {results}");
        }
        else
        {
            throw new Exception("Cuvette guide must be empty!");
        }
    }

    static int Main(string[] args)
    {
        var device = new Device();
        var sn = device.SerialNumber();
        Console.WriteLine($"Found eviDense module SN{sn}");

        Console.WriteLine("Remove cuvette and press any key:");
        Console.ReadLine();
        var baseline = device.Baseline();

        Console.WriteLine("Insert the cuvette and press any key:");
        Console.ReadLine();
        var sample = device.Measure();

        var od = Measurement.CalculateAbsorbance(baseline, sample);
        var transparency = (new Quadruple(Math.Pow(10.0, -od.Value230), Math.Pow(10.0, -od.Value260), Math.Pow(10.0, -od.Value280), Math.Pow(10.0, -od.Value340))) * 100.0;

        Console.WriteLine($" Optical density (OD):{od}");
        Console.WriteLine($" Transparency [%]{transparency}");

        return 0;
   }
}