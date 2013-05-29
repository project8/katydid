/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTTimeSeriesData.hh"

#include <algorithm>

using std::string;
using std::vector;

using boost::shared_ptr;

namespace Katydid
{

    static KTDerivedNORegistrar< KTProcessor, KTWignerVille > sWVRegistrar("wigner-ville");

    KTWignerVille::KTWignerVille(const std::string& name) :
                KTProcessor(name),
                fFFT(new KTComplexFFTW()),
                fInputArray(new KTTimeSeriesFFTW(1,0.,1.)),
                fWVSignal("wigner-ville", this),
                fHeaderSlot("header", this, &KTWignerVille::InitializeWithHeader),
                fTimeSeriesSlot("ts", this, &KTWignerVille::TransformData, &fWVSignal),
                fAnalyticAssociateSlot("aa", this, &KTWignerVille::TransformData, &fWVSignal)
    {
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        const KTPStoreNode* childNode = node->GetChild("complex-fftw");
        if (childNode != NULL)
        {
            fFFT->Configure(childNode);
        }

        KTPStoreNode::csi_pair itPair = node->EqualRange("wv-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            UIntPair pair = ParsePairUInt(citer->second.get_value< string >());
            KTINFO(wvlog, "Adding WV pair " << pair.first << ", " << pair.second);
            this->AddPair(pair);
        }

        return true;
    }

    void KTWignerVille::InitializeWithHeader(const KTEggHeader* header)
    {
        //UInt_t nBins = header->GetSliceSize();
        UInt_t nBins = 128;
        fFFT->SetSize(nBins);
        fFFT->InitializeFFT();
        delete fInputArray;
        // the min/max range for the input array don't matter, so just use 0 and 1
        fInputArray = new KTTimeSeriesFFTW(nBins, 0., 1.);
        return;
    }

    Bool_t KTWignerVille::TransformData(KTTimeSeriesData& data)
    {
        return TransformFFTWBasedData(data);
    }

    Bool_t KTWignerVille::TransformData(KTAnalyticAssociateData& data)
    {
        return TransformFFTWBasedData(data);
    }

    void KTWignerVille::CalculateLaggedACF(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t sliceSize = data1->size();
        UInt_t fftSize = fInputArray->size();
        /*
        if (fInputArray->size() != size)
        {
            delete fInputArray;
            fInputArray = new KTTimeSeriesFFTW(size,
                    data1->GetRangeMin(),
                    data1->GetRangeMax() + 0.5 * data1->GetBinWidth());
            KTWARN(wvlog, "Setting the input array size to " << size);
        }
        else
        {
            fInputArray->SetRange(data1->GetRangeMin(), data1->GetRangeMax());
        }
        */
        fInputArray->SetRange(0., (Double_t)fftSize * data1->GetBinWidth());

        //KTERROR(wvlog, "offset = " << offset << "  inArr Size = " << fInputArray->size() << "  data1 Size = " << data1->size() << "  data2 Size = " << data2->size());

        // Now calculate the lagged ACF at all possible lags.
        register Double_t t1_real;
        register Double_t t1_imag;
        register Double_t t2_real;
        register Double_t t2_imag;
        //register UInt_t tau_plus = size - 1;
        //register UInt_t tau_minus = 0;
        ///register UInt_t start = (UInt_t)std::max(0, (Int_t)offset - ((Int_t)size - 1));
        ///register UInt_t end = (UInt_t)std::min((Int_t)offset, (Int_t)size - 1);
        register UInt_t time = offset;
        register Int_t taumax = std::min(std::min((Int_t)time, (Int_t)sliceSize - (Int_t)time -1), (Int_t)fftSize/2-1);

        UInt_t fftBin = 0;
        for (Int_t tau = -taumax; tau <= taumax; tau++)
        {
            t1_real = (*data1)(time + tau)[0];
            t1_imag = (*data1)(time + tau)[1];
            t2_real = (*data1)(time - tau)[0];
            t2_real = (*data2)(time - tau)[0];
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            //KTWARN(wvlog, "  " << time << "  " << taumax << "  " << tau << "  " << fftBin << " -- " << time + tau << "  " << (*data1)(time+tau)[0] << "  " << (*data1)(time+tau)[1] << " -- " << time - tau << "  " << (*data2)(time-tau)[0] << "  " << (*data2)(time-tau)[0]);
            fftBin++;
        }
        for (; fftBin < fftSize; fftBin++)
        {
            (*fInputArray)(fftBin)[0] = 0.;
            (*fInputArray)(fftBin)[1] = 0.;
        }

        ///for (UInt_t inArrBin = 0; inArrBin < start; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
        ///for (UInt_t inArrBin = start; inArrBin <= end; inArrBin++)
        ///{
            //t1_real = (*data1)(offset + tau_minus)[0];
            //t1_imag = (*data1)(offset + tau_minus)[1];
            //t2_real = (*data2)(offset + tau_plus)[0];
            //t2_imag = (*data2)(offset + tau_plus)[1];
            ///t1_real = (*data1)(inArrBin)[0];
            ///t1_imag = (*data1)(inArrBin)[1];
            ///t2_real = (*data2)(offset - inArrBin)[0];
            ///t2_imag = (*data2)(offset - inArrBin)[1];

            ///(*fInputArray)(inArrBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            ///(*fInputArray)(inArrBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            ///KTWARN(wvlog, "  " << inArrBin << " -- " << inArrBin << "  " << (*data1)(inArrBin)[0] << "  " << (*data1)(inArrBin)[1] << " -- " << offset - inArrBin << "  " << (*data2)(inArrBin)[0] << "  " << (*data2)(inArrBin)[1]);
            //KTWARN(wvlog, "  " << inArrBin << " -- " << tau_minus << "  " << offset + tau_minus << " = " << t1_real << "  " << t1_imag << " -- " << tau_plus << "  " << offset + tau_plus << " = " << t2_real << "  " << t2_imag << " -- " << (*fInputArray)(inArrBin)[0] << "  " << (*fInputArray)(inArrBin)[1]);

            //tau_minus++;
            //tau_plus--;
        ///}
        ///for (UInt_t inArrBin = end + 1; inArrBin < size; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
    }

} /* namespace Katydid */
