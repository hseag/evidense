
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

    void DisposeCuvette()
    {
        //Dispose the cuvette when you want to use the sample further.
    }
        
    void DispenseToSample()
    {
        //Dispense the sample back.
    }

    void DisposeTip()
    {
        //Dispose the tip.
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
            var volume = 10.5; //Cuvette volume in ul
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
        var air = device.Measure();
        var sample = device.Measure();

        var measurement = new Measurement(baseline, air, sample);

        if (factors == null)
        {
            throw new Exception("Factors can't be null!");
        }

        var results = measurement.Results(factors);

        storage.AppendWithResults(measurement, results, comment);

        Console.WriteLine($"{comment} : {results}");
    }

    static int Main(string[] args)
    {
        try
        {
            Program program = new Program();

            bool first = true;
            int sampleCount = 2;

            for (int sample = 0; sample < sampleCount; sample++)
            {
                program.PickupTip();
                program.AspirateFromSample();
                program.PickUpCuvette();
                program.MoveOverCuvetteGuide();
                if (first)
                {
                    program.MeasureSampleWithBufferOnly("Buffer only");
                    first = false;
                }
                else
                {
                    program.MeasureSample($"Sample #{sample}");
                }
                program.DisposeCuvette();
                program.DispenseToSample();
                program.DisposeTip();
            }

            program.storage.Save("data-evidense.json");
        }
        catch (Exception ex)
        { 
            Console.WriteLine(ex.ToString()); 
            return 1; 
        }
        return 0;
   }
}