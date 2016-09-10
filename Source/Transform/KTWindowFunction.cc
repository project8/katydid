/*
 * KTWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTWindowFunction.hh"

#ifdef FFTW_FOUND
#include "KTForwardFFTW.hh"
#endif

#include "KT2ROOT.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <cmath>

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(fftlog, "KTWindowFunction");

    KTWindowFunction::KTWindowFunction(const string& name) :
            KTConfigurable(name),
            fWindowFunction(1),
            fLength(1.),
            fBinWidth(1.),
            fSize(1),
            fLastSetParameter(kSize)
    {
    }

    KTWindowFunction::~KTWindowFunction()
    {
    }

    bool KTWindowFunction::Configure(const scarab::param_node* node)
    {
        return ConfigureWFSubclass(node);
    }

    double KTWindowFunction::AdaptTo(const KTTimeSeriesData* tsData)
    {
        SetBinWidth(tsData->GetTimeSeries(0)->GetTimeBinWidth());
        return SetSize(tsData->GetTimeSeries(0)->GetNTimeBins());
    }

#ifdef ROOT_FOUND
    TH1D* KTWindowFunction::CreateHistogram(const string& name) const
    {
        TH1D* hist = new TH1D(name.c_str(), "Window Function", fSize, 0, fLength);
        for (unsigned iHistBin=1; iHistBin<=fSize; iHistBin++)
        {
            hist->SetBinContent(iHistBin, this->GetWeight(iHistBin-1));
        }
        hist->SetYTitle("Weight");
        return hist;
    }

#ifdef FFTW_FOUND
    TH1D* KTWindowFunction::CreateFrequencyResponseHistogram(const string& name) const
    {
        KTTimeSeriesReal timeData(fSize, 0., double(fSize) * fBinWidth);
        for (unsigned iBin=0; iBin<fSize+0; iBin++)
        {
            timeData.SetValue(iBin, GetWeight(iBin));
        }
        KTForwardFFTW fft;
        fft.SetTimeSize(fSize);
        fft.SetTransformFlag("ESTIMATE");
        fft.InitializeForRealTDD();
        KTFrequencySpectrumFFTW* freqSpect = fft.Transform(&timeData);
        TH1D* hist = KT2ROOT::CreateMagnitudeHistogram(freqSpect, name);
        hist->SetYTitle("Weight");
        delete freqSpect;
        return hist;
    }
#endif
#endif

    double KTWindowFunction::SetLength(double length)
    {
        fLength = fabs(length);
        if (fLastSetParameter == kBinWidth)
        {
            // Priority is to preserve the bin width, but it might not be an even divisor of the new length
            double prelimSize = fLength / fBinWidth;
            fSize = (unsigned)KTMath::Nint(prelimSize);
            fBinWidth = fLength / (double)fSize;
        }
        else if (fLastSetParameter == kSize || fLastSetParameter == kLength)
        {
            fBinWidth = fLength / fSize;
        }
        RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    double KTWindowFunction::SetBinWidth(double bw)
    {
        fBinWidth = fabs(bw);
        if (fLastSetParameter == kSize || fLastSetParameter == kBinWidth)
        {
            fLength = fSize * fBinWidth;
        }
        else if (fLastSetParameter == kLength)
        {
            // Priority is to preserve the length, but it might not be an even multiple of the new bin width
            double prelimNBins = fLength / fBinWidth;
            fSize = (unsigned)KTMath::Nint(prelimNBins);
            fLength = (double)fSize * fBinWidth;
        }
        RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    double KTWindowFunction::SetBinWidthAndLength(double bw, double length)
    {
        fBinWidth = fabs(bw);
        double prelimNBins = fabs(length) / fBinWidth;
        fSize = (unsigned)KTMath::Nint(prelimNBins);
        fLength = (double)fSize * fBinWidth;
        RebuildWindowFunction();
        fLastSetParameter = kBinWidth;
        return fLength;
    }

    double KTWindowFunction::SetLengthAndBinWidth(double length, double bw)
    {
        fLength = fabs(length);
        double prelimNBins = fLength / fabs(bw);
        fSize = (unsigned)KTMath::Nint(prelimNBins);
        fBinWidth = fLength / (double)fSize;
        RebuildWindowFunction();
        fLastSetParameter = kLength;
        return fBinWidth;
    }

    double KTWindowFunction::SetSize(unsigned wib)
    {
        fSize = wib;
        if (fLastSetParameter == kBinWidth || fLastSetParameter == kSize)
        {
            fLength = (double)fSize * fBinWidth;
        }
        else if (fLastSetParameter == kLength)
        {
            fBinWidth = fLength / (double)fSize;
        }
        RebuildWindowFunction();
        fLastSetParameter = kSize;
        return fLength;
    }


} /* namespace Katydid */
