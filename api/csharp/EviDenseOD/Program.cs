
using Hse.EviDense;

internal class Program
{
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

        Console.WriteLine($" Optical density (OD) : {od}");
        Console.WriteLine($" Transparency [%]     : {transparency}");

        return 0;
   }
}