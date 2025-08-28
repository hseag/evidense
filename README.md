# eviDense UV Photometer

The eviDense UV module is an ultra-compact 4-color UV absorbance-based photometer which enables full Quality Control on liquid handlers. 
For more information see https://www.on-deck-photometer.com. To control the eviDense UV module, HSE AG provides software interfaces in C or C#.

## Measuring procedure in short
1. Pick up a tip with your liquid handler
2. Aspirate at least 10 &#956;l of sample
3. Pick up a cuvette
4. Move the cuvette over the eviDense UV module
5. Prepare the eviDense UV module with a baseline measurement
6. Insert the cuvette into the eviDense UV module
7. Measure the empty cuvette
8. Dispense approximately 10 &#956;l sample into the cuvette
9. Measure the cuvette with sample
10. Calculate the concentration of the sample
11. Move the cuvette out off the eviDense UV module
12. Dispose the tip with the attached cuvette

The C# documentation can be found [here](csharp/doc/api/Hse.EviDense.html).
