// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: © 2024 HSE AG, <opensource@hseag.com>

#include "common/evibase.h"
#include "dict.h"
#include "measurement.h"
#include <string.h>
#include <math.h>

#define DEFAULT_CUVETTE_PATH_LENGTH 1.04 //mm
#define DEFAULT_CENTER_WAVELENGTH_280 280.0

typedef struct
{
    double wavelength;
    double absorption;
} WavelengthAbsorption_t;

static const WavelengthAbsorption_t theoreticalDnaAbsorption[] =
{
    {200.0, 2.0},
    {200.3950617283951, 1.428571428571429},
    {200.7901234567901, 1.408637873754153},
    {201.1851851851852, 1.401993355481728},
    {201.5802469135803, 1.382059800664452},
    {201.9753086419753, 1.375415282392027},
    {202.3703703703704, 1.355481727574751},
    {202.7654320987654, 1.335548172757475},
    {203.1604938271605, 1.315614617940199},
    {203.5555555555555, 1.302325581395349},
    {203.9506172839506, 1.282392026578073},
    {204.3456790123457, 1.269102990033223},
    {204.7407407407407, 1.255813953488372},
    {205.1358024691358, 1.235880398671096},
    {205.5308641975309, 1.215946843853821},
    {205.9259259259259, 1.20265780730897},
    {206.320987654321, 1.182724252491694},
    {206.7160493827161, 1.162790697674419},
    {207.1111111111111, 1.149501661129568},
    {207.5061728395062, 1.129568106312292},
    {207.9012345679012, 1.109634551495017},
    {208.2962962962963, 1.096345514950166},
    {208.6913580246914, 1.083056478405316},
    {209.0864197530864, 1.06312292358804},
    {209.4814814814815, 1.049833887043189},
    {209.8765432098765, 1.023255813953488},
    {210.2716049382716, 1.009966777408638},
    {210.6666666666667, 0.9966777408637874},
    {211.0617283950617, 0.9833887043189369},
    {211.4567901234568, 0.9634551495016611},
    {211.8518518518518, 0.9501661129568106},
    {212.2469135802469, 0.9302325581395349},
    {212.641975308642, 0.9169435215946844},
    {213.0370370370371, 0.8970099667774086},
    {213.4320987654321, 0.8837209302325582},
    {213.8271604938271, 0.8704318936877077},
    {214.2222222222222, 0.8504983388704319},
    {214.6172839506173, 0.8372093023255814},
    {215.0123456790124, 0.823920265780731},
    {215.4074074074074, 0.8046511627906977},
    {215.8024691358025, 0.7913621262458472},
    {216.1975308641975, 0.7780730897009967},
    {216.5925925925926, 0.7647840531561462},
    {216.9876543209876, 0.7514950166112956},
    {217.3827160493827, 0.7448504983388704},
    {217.7777777777778, 0.7315614617940199},
    {218.1728395061729, 0.7182724252491694},
    {218.5679012345679, 0.7116279069767442},
    {218.9629629629629, 0.6983388704318937},
    {219.358024691358, 0.6916943521594684},
    {219.7530864197531, 0.6784053156146179},
    {220.1481481481482, 0.6717607973421927},
    {220.5432098765432, 0.6651162790697674},
    {220.9382716049383, 0.6518272425249169},
    {221.3333333333333, 0.6451827242524917},
    {221.7283950617284, 0.6385382059800664},
    {222.1234567901234, 0.6318936877076411},
    {222.5185185185185, 0.6318936877076411},
    {222.9135802469136, 0.6252491694352159},
    {223.3086419753087, 0.6186046511627907},
    {223.7037037037037, 0.6119601328903654},
    {224.0987654320988, 0.6119601328903654},
    {224.4938271604938, 0.6053156146179402},
    {224.8888888888889, 0.6053156146179402},
    {225.2839506172839, 0.5986710963455149},
    {225.679012345679, 0.5986710963455149},
    {226.0740740740741, 0.5920265780730897},
    {226.4691358024691, 0.5920265780730897},
    {226.8641975308642, 0.5920265780730897},
    {227.2592592592592, 0.5853820598006644},
    {227.6543209876543, 0.5853820598006644},
    {228.0493827160494, 0.5853820598006644},
    {228.4444444444445, 0.5853820598006644},
    {228.8395061728395, 0.5787375415282392},
    {229.2345679012346, 0.5787375415282392},
    {229.6296296296296, 0.5787375415282392},
    {230.0246913580247, 0.5787375415282392},
    {230.4197530864197, 0.5787375415282392},
    {230.8148148148148, 0.5720930232558139},
    {231.2098765432099, 0.5720930232558139},
    {231.604938271605, 0.5720930232558139},
    {232.0, 0.5654485049833887},
    {232.395061728395, 0.5654485049833887},
    {232.7901234567901, 0.5588039867109634},
    {233.1851851851852, 0.5588039867109634},
    {233.5802469135803, 0.5521594684385382},
    {233.9753086419753, 0.5521594684385382},
    {234.3703703703704, 0.5455149501661129},
    {234.7654320987654, 0.5455149501661129},
    {235.1604938271605, 0.5388704318936877},
    {235.5555555555555, 0.5388704318936877},
    {235.9506172839506, 0.5322259136212625},
    {236.3456790123457, 0.5255813953488372},
    {236.7407407407408, 0.5255813953488372},
    {237.1358024691358, 0.5189368770764119},
    {237.5308641975309, 0.5122923588039867},
    {237.9259259259259, 0.5122923588039867},
    {238.320987654321, 0.5056478405315615},
    {238.7160493827161, 0.4990033222591362},
    {239.1111111111111, 0.49235880398671095},
    {239.5061728395062, 0.4857142857142857},
    {239.9012345679012, 0.4857142857142857},
    {240.2962962962963, 0.4790697674418605},
    {240.6913580246914, 0.4724252491694352},
    {241.0864197530864, 0.4724252491694352},
    {241.4814814814815, 0.46578073089700995},
    {241.8765432098765, 0.46578073089700995},
    {242.2716049382716, 0.4591362126245847},
    {242.6666666666667, 0.4524916943521595},
    {243.0617283950617, 0.4458471760797342},
    {243.4567901234568, 0.43920265780730895},
    {243.8518518518518, 0.4325581395348837},
    {244.2469135802469, 0.4325581395348837},
    {244.641975308642, 0.42591362126245845},
    {245.0370370370371, 0.4192691029900332},
    {245.4320987654321, 0.41262458471760795},
    {245.8271604938271, 0.41262458471760795},
    {246.2222222222222, 0.4059800664451827},
    {246.6172839506173, 0.39933554817275745},
    {247.0123456790124, 0.3926910299003322},
    {247.4074074074074, 0.3926910299003322},
    {247.8024691358025, 0.38604651162790695},
    {248.1975308641975, 0.3794019933554817},
    {248.5925925925926, 0.37275747508305645},
    {248.9876543209876, 0.3661129568106312},
    {249.3827160493827, 0.35946843853820595},
    {249.7777777777778, 0.3528239202657807},
    {250.1728395061729, 0.34617940199335545},
    {250.5679012345679, 0.3395348837209302},
    {250.9629629629629, 0.33289036544850495},
    {251.358024691358, 0.3262458471760797},
    {251.7530864197531, 0.31960132890365445},
    {252.1481481481482, 0.3129568106312292},
    {252.5432098765432, 0.3129568106312292},
    {252.9382716049383, 0.30631229235880395},
    {253.3333333333333, 0.30631229235880395},
    {253.7283950617284, 0.2996677740863787},
    {254.1234567901234, 0.30631229235880395},
    {254.5185185185185, 0.30631229235880395},
    {254.9135802469136, 0.30631229235880395},
    {255.3086419753087, 0.30631229235880395},
    {255.7037037037037, 0.30631229235880395},
    {256.0987654320988, 0.3129568106312292},
    {256.4938271604938, 0.8305647840531561},
    {256.8888888888889, 0.8372093023255814},
    {257.2839506172839, 0.8372093023255814},
    {257.679012345679, 0.8372093023255814},
    {258.0740740740741, 0.8438538205980066},
    {258.4691358024691, 0.8438538205980066},
    {258.8641975308642, 0.8438538205980066},
    {259.2592592592592, 0.8372093023255814},
    {259.6543209876543, 0.8372093023255814},
    {260.0493827160494, 0.8372093023255814},
    {260.4444444444445, 0.8372093023255814},
    {260.8395061728395, 0.8372093023255814},
    {261.2345679012346, 0.8305647840531561},
    {261.6296296296296, 0.8305647840531561},
    {262.0246913580247, 0.823920265780731},
    {262.4197530864197, 0.8172757475083057},
    {262.8148148148148, 0.8106312292358804},
    {263.2098765432099, 0.8106312292358804},
    {263.604938271605, 0.7973421926910299},
    {264.0, 0.7973421926910299},
    {264.395061728395, 0.7906976744186047},
    {264.7901234567901, 0.7840531561461794},
    {265.1851851851852, 0.7774086378737541},
    {265.5802469135803, 0.770764119601329},
    {265.9753086419753, 0.7641196013289037},
    {266.3703703703704, 0.7574750830564784},
    {266.7654320987654, 0.7508305647840532},
    {267.1604938271605, 0.7441860465116279},
    {267.5555555555555, 0.7375415282392027},
    {267.9506172839506, 0.7308970099667774},
    {268.3456790123457, 0.7242524916943521},
    {268.7407407407408, 0.717607973421927},
    {269.1358024691358, 0.7109634551495017},
    {269.5308641975308, 0.6976744186046512},
    {269.9259259259259, 0.6910299003322259},
    {270.320987654321, 0.6843853820598007},
    {270.7160493827161, 0.6777408637873754},
    {271.1111111111111, 0.6710963455149502},
    {271.5061728395062, 0.6644518272425249},
    {271.9012345679012, 0.6578073089700996},
    {272.2962962962963, 0.6445182724252492},
    {272.6913580246913, 0.6378737541528239},
    {273.0864197530864, 0.6312292358803987},
    {273.4814814814815, 0.6179401993355482},
    {273.8765432098766, 0.6112956810631229},
    {274.2716049382716, 0.5980066445182725},
    {274.6666666666666, 0.5913621262458472},
    {275.0617283950617, 0.5780730897009967},
    {275.4567901234568, 0.5714285714285714},
    {275.8518518518518, 0.5647840531561462},
    {276.2469135802469, 0.5514950166112957},
    {276.641975308642, 0.5448504983388704},
    {277.0370370370371, 0.53156146179402},
    {277.4320987654321, 0.5182724252491695},
    {277.8271604938271, 0.5116279069767442},
    {278.2222222222222, 0.5049833887043189},
    {278.6172839506173, 0.4983388704318937},
    {279.0123456790124, 0.4850498338870432},
    {279.4074074074074, 0.478405315614618},
    {279.8024691358025, 0.4717607973421927},
    {280.1975308641975, 0.4584717607973422},
    {280.5925925925926, 0.4451827242524917},
    {280.9876543209876, 0.4385382059800664},
    {281.3827160493827, 0.4318936877076412},
    {281.7777777777778, 0.425249169435216},
    {282.1728395061729, 0.4119601328903655},
    {282.5679012345679, 0.4053156146179402},
    {282.9629629629629, 0.3920265780730897},
    {283.358024691358, 0.3853820598006645},
    {283.7530864197531, 0.3720930232558139},
    {284.1481481481482, 0.3588039867109635},
    {284.5432098765432, 0.3521594684385382},
    {284.9382716049383, 0.3388704318936877},
    {285.3333333333333, 0.3255813953488372},
    {285.7283950617284, 0.3122923588039867},
    {286.1234567901234, 0.3056478405315615},
    {286.5185185185185, 0.292358803986711},
    {286.9135802469136, 0.2857142857142857},
    {287.3086419753087, 0.2724252491694352},
    {287.7037037037037, 0.2591362126245847},
    {288.0987654320987, 0.2458471760797342},
    {288.4938271604938, 0.239202657807309},
    {288.8888888888889, 0.2259136212624585},
    {289.2839506172839, 0.2192691029900332},
    {289.679012345679, 0.212624584717608},
    {290.0740740740741, 0.2059800664451827},
    {290.4691358024691, 0.1926910299003322},
    {290.8641975308642, 0.186046511627907},
    {291.2592592592592, 0.1727574750830565},
    {291.6543209876543, 0.1661129568106312},
    {292.0493827160494, 0.159468438538206},
    {292.4444444444445, 0.1528239202657807},
    {292.8395061728395, 0.1395348837209302},
    {293.2345679012345, 0.132890365448505},
    {293.6296296296296, 0.1262458471760797},
    {294.0246913580247, 0.1196013289036545},
    {294.4197530864197, 0.1129568106312292},
    {294.8148148148148, 0.106312292358804},
    {295.2098765432099, 0.09966777408637874},
    {295.604938271605, 0.09302325581395349},
    {296.0, 0.08637873754152824},
    {296.395061728395, 0.07973421926910298},
    {296.7901234567901, 0.07308970099667775},
    {297.1851851851852, 0.07308970099667775},
    {297.5802469135803, 0.0664451827242525},
    {297.9753086419753, 0.05980066445182725},
    {298.3703703703704, 0.05980066445182725},
    {298.7654320987654, 0.05315614617940199},
    {299.1604938271605, 0.05315614617940199},
    {299.5555555555555, 0.05315614617940199},
    {299.9506172839506, 0.04651162790697674},
    {300.3456790123457, 0.04651162790697674},
    {300.7407407407408, 0.04651162790697674},
    {301.1358024691358, 0.03986710963455149},
    {301.5308641975308, 0.03986710963455149},
    {301.9259259259259, 0.03986710963455149},
    {302.320987654321, 0.03322259136212625},
    {302.7160493827161, 0.03322259136212625},
    {303.1111111111111, 0.03322259136212625},
    {303.5061728395062, 0.03322259136212625},
    {303.9012345679012, 0.03322259136212625},
    {304.2962962962963, 0.03322259136212625},
    {304.6913580246913, 0.03322259136212625},
    {305.0864197530864, 0.026578073089701},
    {305.4814814814815, 0.026578073089701},
    {305.8765432098766, 0.03322259136212625},
    {306.2716049382716, 0.03322259136212625},
    {306.6666666666666, 0.026578073089701},
    {307.0617283950617, 0.026578073089701},
    {307.4567901234568, 0.026578073089701},
    {307.8518518518518, 0.026578073089701},
    {308.2469135802469, 0.026578073089701},
    {308.641975308642, 0.01993355481727575},
    {309.0370370370371, 0.026578073089701},
    {309.4320987654321, 0.026578073089701},
    {309.8271604938271, 0.01993355481727575},
    {310.2222222222222, 0.01993355481727575},
    {310.6172839506173, 0.01993355481727575},
    {311.0123456790124, 0.01993355481727575},
    {311.4074074074074, 0.01993355481727575},
    {311.8024691358025, 0.0132890365448505},
    {312.1975308641975, 0.0132890365448505},
    {312.5925925925926, 0.01993355481727575},
    {312.9876543209876, 0.01993355481727575},
    {313.3827160493827, 0.0132890365448505},
    {313.7777777777778, 0.0132890365448505},
    {314.1728395061729, 0.0132890365448505}
};

static double measurement_getTheoreticalDnaAbsorption(double wavelength)
{
    size_t i;
    size_t count = sizeof(theoreticalDnaAbsorption) / sizeof(theoreticalDnaAbsorption[0]);

    if(wavelength <= theoreticalDnaAbsorption[0].wavelength)
    {
        return theoreticalDnaAbsorption[0].absorption;
    }
    if(wavelength >= theoreticalDnaAbsorption[count - 1].wavelength)
    {
        return theoreticalDnaAbsorption[count - 1].absorption;
    }

    for(i = 0; i < count - 1; i++)
    {
        double x0 = theoreticalDnaAbsorption[i].wavelength;
        double y0 = theoreticalDnaAbsorption[i].absorption;
        double x1 = theoreticalDnaAbsorption[i + 1].wavelength;
        double y1 = theoreticalDnaAbsorption[i + 1].absorption;

        if((x0 <= wavelength) && (wavelength <= x1))
        {
            return y0 + (y1 - y0) * (wavelength - x0) / (x1 - x0);
        }
    }

    return theoreticalDnaAbsorption[count - 1].absorption;
}

Measurement_t measurement_init(SingleMeasurement_t baseline, SingleMeasurement_t air, SingleMeasurement_t sample, const char * comment)
{
    Measurement_t ret = {.baseline = baseline, .air = air, .sample = sample, .comment = {0}};

    if(comment)
    {
        strncpy_s(ret.comment, MAX_COMMENT_LENGTH, comment, MAX_COMMENT_LENGTH);
    }

    return ret;
}

void measurement_print(const Measurement_t * self, FILE * stream, bool newLine)
{
    fprintf_s(stream, "baseline: ");
    singleMeasurement_print(&self->baseline, stream, false);
    fprintf_s(stream, " ");
    fprintf_s(stream, "air: ");
    singleMeasurement_print(&self->air, stream, false);
    fprintf_s(stream, " ");
    fprintf_s(stream, "sample: ");
    singleMeasurement_print(&self->sample, stream, false);
    fprintf_s(stream, "%s", newLine ? "\n" : "");
}

Quadruple_t measurement_calculateAbsorbance(const SingleMeasurement_t * baseline, const SingleMeasurement_t * measurement, const Quadruple_t * correctionFactor)
{
    Quadruple_t ods;
    Quadruple_t  correctionFactorOne = quadruple_initAllTheSame(1.0);

    if(!correctionFactor)
    {
        correctionFactor = &correctionFactorOne;
    }

    ods.value230 = log10((double)baseline->channel230.sample / (double)baseline->channel230.reference * (double)measurement->channel230.reference / (double)measurement->channel230.sample * correctionFactor->value230);
    ods.value260 = log10((double)baseline->channel260.sample / (double)baseline->channel260.reference * (double)measurement->channel260.reference / (double)measurement->channel260.sample * correctionFactor->value260);
    ods.value280 = log10((double)baseline->channel280.sample / (double)baseline->channel280.reference * (double)measurement->channel280.reference / (double)measurement->channel280.sample * correctionFactor->value280);
    ods.value340 = log10((double)baseline->channel340.sample / (double)baseline->channel340.reference * (double)measurement->channel340.reference / (double)measurement->channel340.sample * correctionFactor->value340);

    return ods;
}

Quadruple_t measurement_factorAbsorbanceBufferBlank(const Measurement_t * self)
{
    return measurement_calculateAbsorbance(&self->air, &self->sample, NULL);
}

Quadruple_t measurement_absorbance(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    Quadruple_t aSample = measurement_calculateAbsorbance(&self->air, &self->sample, NULL);
    Quadruple_t factor0 = quadruple_initAllTheSame(0.0);
    Quadruple_t absorbance;

    if(!factorAbsorbanceBufferBlank)
    {
        factorAbsorbanceBufferBlank = &factor0;
    }

    absorbance = quadruple_sub(&aSample, factorAbsorbanceBufferBlank);

    if(parameters && (parameters->centerWavelength280 > 0.0))
    {
        double a280Real = measurement_getTheoreticalDnaAbsorption(parameters->centerWavelength280);
        double a280Theoretical = measurement_getTheoreticalDnaAbsorption(DEFAULT_CENTER_WAVELENGTH_280);

        absorbance.value280 = absorbance.value280 * a280Theoretical / a280Real;
    }

    return absorbance;
}

Quadruple_t measurement_algorithmV9(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    Quadruple_t aSampleMinusBlank = measurement_absorbance(self, factorAbsorbanceBufferBlank, parameters);
    Quadruple_t a340              = quadruple_initAllTheSame(aSampleMinusBlank.value340);
    Quadruple_t ret               = quadruple_sub(&aSampleMinusBlank, &a340);

    return ret;
}

double measurement_a230(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    return measurement_absorbance(self, factorAbsorbanceBufferBlank, parameters).value230;
}

double measurement_a260(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    return measurement_absorbance(self, factorAbsorbanceBufferBlank, parameters).value260;
}

double measurement_a280(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    return measurement_absorbance(self, factorAbsorbanceBufferBlank, parameters).value280;
}

double measurement_a340(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    return measurement_absorbance(self, factorAbsorbanceBufferBlank, parameters).value340;
}

double measurement_dsDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength, const Parameters_t * parameters)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank, parameters);
    return aNNN.value260 * 50.0 * 10.0 / tempCcuvettePathLength; //50ng/ul * 10mm / 1.04mm
}

double measurement_ssDNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength, const Parameters_t * parameters)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank, parameters);
    return aNNN.value260 * 33.0 * 10.0 / tempCcuvettePathLength; //33ng/ul * 10mm / 1.04mm
}

double measurement_ssRNA(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const double * cuvettePathLength, const Parameters_t * parameters)
{
    double tempCcuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    if(cuvettePathLength)
    {
        tempCcuvettePathLength = *cuvettePathLength;
    }
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank, parameters);
    return aNNN.value260 * 40.0 * 10.0 / tempCcuvettePathLength; //40ng/ul * 10mm / 1.04mm
}

double measurement_purityRatio260_280(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank, parameters);
    return  aNNN.value260 / aNNN.value280;
}

double measurement_purityRatio260_230(const Measurement_t * self, const Quadruple_t * factorAbsorbanceBufferBlank, const Parameters_t * parameters)
{
    Quadruple_t aNNN = measurement_algorithmV9(self, factorAbsorbanceBufferBlank, parameters);
    return  aNNN.value260 / aNNN.value230;
}

bool measurement_fromJson(cJSON * node, Measurement_t * measurement)
{
    bool ret = false;
    cJSON * oBaseline = cJSON_GetObjectItem(node, DICT_BASELINE);
    cJSON * oAir      = cJSON_GetObjectItem(node, DICT_AIR);
    cJSON * oSample   = cJSON_GetObjectItem(node, DICT_SAMPLE);
    if(oBaseline && oAir && oSample)
    {
        SingleMeasurement_t baseline = {};
        SingleMeasurement_t air = {};
        SingleMeasurement_t sample = {};
        if(singleMeasurement_fromJson(oBaseline, &baseline) && singleMeasurement_fromJson(oAir, &air) && singleMeasurement_fromJson(oSample, &sample))
        {
            *measurement = measurement_init(baseline, air, sample, NULL);
            ret = true;
        }
    }

    return ret;
}

Parameters_t parametersCreate()
{
    Parameters_t ret;

    ret.blanksStart       = 1;
    ret.blanksEnd         = 0;
    ret.cuvettePathLength = DEFAULT_CUVETTE_PATH_LENGTH;
    ret.centerWavelength280 = DEFAULT_CENTER_WAVELENGTH_280;

    return ret;
}

bool parametersApplyAdjustmentsFromJson(cJSON * node, Parameters_t * parameters)
{
    cJSON * oAdjustments = NULL;
    cJSON * oCenterWavelengths = NULL;
    cJSON * o280 = NULL;

    if(!node || !parameters)
    {
        return false;
    }

    oAdjustments = cJSON_GetObjectItem(node, DICT_ADJUSTMENTS);
    if(!oAdjustments)
    {
        return false;
    }

    oCenterWavelengths = cJSON_GetObjectItem(oAdjustments, DICT_CENTER_WAVELENGTHS);
    if(!oCenterWavelengths)
    {
        return false;
    }

    o280 = cJSON_GetObjectItem(oCenterWavelengths, DICT_280);
    if(!cJSON_IsNumber(o280))
    {
        return false;
    }

    parameters->centerWavelength280 = cJSON_GetNumberValue(o280);
    return true;
}

bool measurement_calculateFactors(cJSON *oMeasurments, const Parameters_t * parameters, Factors_t * factors)
{
    bool ret = false;
    cJSON *iterator = NULL;
    uint32_t index = 0;
    uint32_t count = 0;

    uint32_t length = cJSON_GetArraySize(oMeasurments);

    factors->fAbsorbanceBufferBlank = quadruple_initAllTheSame(0.0);

    cJSON_ArrayForEach(iterator, oMeasurments)
    {
        if ((index < parameters->blanksStart) || (index >= (length - parameters->blanksEnd)))
        {
            Measurement_t m = {};
            measurement_fromJson(iterator, &m);
            Quadruple_t f1  = measurement_factorAbsorbanceBufferBlank(&m);
            factors->fAbsorbanceBufferBlank = quadruple_add(&factors->fAbsorbanceBufferBlank, &f1);
            count++;
        }
        index++;
    }

    if (count == 0)
    {
        factors->fAbsorbanceBufferBlank = quadruple_initAllTheSame(1.0);
    }
    else
    {
        Quadruple_t qCount       = quadruple_initAllTheSame(count);

        factors->fAbsorbanceBufferBlank = quadruple_div(&factors->fAbsorbanceBufferBlank, &qCount);
        ret = count == (parameters->blanksStart + parameters->blanksEnd);
    }
    return ret;
}

static cJSON *calculate(cJSON *measurement, const Factors_t * factors, const Parameters_t * parameters)
{
    cJSON *obj = cJSON_CreateObject();

    Measurement_t m = {};

    measurement_fromJson(measurement, &m);

    cJSON_AddNumberToObject(obj, DICT_DS_DNA, measurement_dsDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength, parameters));
    cJSON_AddNumberToObject(obj, DICT_SS_DNA, measurement_ssDNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength, parameters));
    cJSON_AddNumberToObject(obj, DICT_SS_RNA, measurement_ssRNA(&m, &factors->fAbsorbanceBufferBlank, &parameters->cuvettePathLength, parameters));
    cJSON_AddNumberToObject(obj, DICT_A230, measurement_a230(&m, &factors->fAbsorbanceBufferBlank, parameters));
    cJSON_AddNumberToObject(obj, DICT_A260, measurement_a260(&m, &factors->fAbsorbanceBufferBlank, parameters));
    cJSON_AddNumberToObject(obj, DICT_A280, measurement_a280(&m, &factors->fAbsorbanceBufferBlank, parameters));
    cJSON_AddNumberToObject(obj, DICT_A340, measurement_a340(&m, &factors->fAbsorbanceBufferBlank, parameters));

    cJSON_AddNumberToObject(obj, DICT_PURITY_260_230, measurement_purityRatio260_230(&m, &factors->fAbsorbanceBufferBlank, parameters));
    cJSON_AddNumberToObject(obj, DICT_PURITY_260_280, measurement_purityRatio260_280(&m, &factors->fAbsorbanceBufferBlank, parameters));

    return obj;
}

bool measurement_calculate(cJSON * oMeasurements, const Parameters_t * parameters)
{
    bool ret = false;

    if (oMeasurements)
    {
        Factors_t factors = {};

        if(measurement_calculateFactors(oMeasurements, parameters, &factors))
        {
            cJSON *iterator = NULL;
            cJSON_ArrayForEach(iterator, oMeasurements)
            {
                cJSON_DeleteItemFromObject(iterator, DICT_CALCULATED);
                cJSON_AddItemToObject(iterator, DICT_CALCULATED, calculate(iterator, &factors, parameters));
            }
            ret = true;
        }
    }
    return ret;
}
