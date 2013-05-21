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

using std::string;
using std::vector;

using boost::shared_ptr;

namespace Katydid
{

    static KTDerivedNORegistrar< KTProcessor, KTWignerVille > sWVRegistrar("wigner-ville");

    KTWignerVille::KTWignerVille(const std::string& name) :
                KTProcessor(name),
                fFFT(new KTComplexFFTW()),
                fColumns(0),
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
            UIntPair pair = ParsePair(citer->second.get_value< string >());
            KTINFO(wvlog, "Adding WV pair " << pair.first << ", " << pair.second);
            this->AddPair(pair);
        }

        return true;
    }

    void KTWignerVille::InitializeWithHeader(const KTEggHeader* header)
    {
        UInt_t nBins = header->GetSliceSize();
        fFFT->SetSize(nBins);
        fFFT->InitializeFFT();
        delete fInputArray;
        // the min/max range for the input array don't matter, so just use 0 and 1
        fInputArray = new KTTimeSeriesFFTW(nBins, 0., 1.);
        // reserve enough space for all of the delay columns
        fColumns.resize(nBins);
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
        UInt_t size = data1->size();
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

        // Now calculate the lagged ACF at all possible lags.
        register Double_t t1_real;
        register Double_t t1_imag;
        register Double_t t2_real;
        register Double_t t2_imag;
        register UInt_t tau_plus = size - 1;
        register UInt_t tau_minus = 0;
        for(UInt_t freq_bin = 0; freq_bin < size; freq_bin++)
        {
            t1_real = (*data1)(offset + tau_minus)[0];
            t1_imag = (*data1)(offset + tau_minus)[1];
            t2_real = (*data2)(offset + tau_plus)[0];
            t2_imag = (*data2)(offset + tau_plus)[1];

            (*fInputArray)(freq_bin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(freq_bin)[1] = t1_imag * t2_real - t1_real * t2_imag;

            tau_minus++;
            tau_plus--;
        }
    }

} /* namespace Katydid */
