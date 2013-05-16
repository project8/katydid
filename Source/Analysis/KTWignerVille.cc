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
//#include "KTSlidingWindowFFTW.hh"
//#include "KTSlidingWindowFSData.hh"
//#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include <set>

#include <iostream>

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
            UIntPair pair = ParsePair(citer->second.get_value< string >());
            KTINFO(wvlog, "Adding WV pair " << pair.first << ", " << pair.second);
            this->AddPair(pair);
        }

        return true;
    }

    void KTWignerVille::InitializeWithHeader(const KTEggHeader* header)
    {
        UInt_t nBins = /*2 */ header->GetSliceSize();
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
    UInt_t size = data1->size();
    if (fInputArray->size() != size) {
      delete fInputArray;
      fInputArray = new KTTimeSeriesFFTW(size, 
					 data1->GetRangeMin(), 
					 data1->GetRangeMax() + 0.5 * data1->GetBinWidth());
      KTWARN(wvlog, "Setting the input array size to " << size);
    }
    else {
      fInputArray->SetRange(data1->GetRangeMin(), data1->GetRangeMax());
    }

    // Now calculate the lagged ACF at all possible lags.
    register double t1_real;
    register double t1_imag;
    register double t2_real;
    register double t2_imag;
    register UInt_t tau_plus = size - 1;
    register UInt_t tau_minus = 0;
    for(UInt_t freq_bin = 0; freq_bin < size; freq_bin++) {
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

    void KTWignerVille::CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t size = data1->size();
        if (fInputArray->size() != /*2*/size)
        {
            delete fInputArray;
            fInputArray = new KTTimeSeriesFFTW(/*2*/size, data1->GetRangeMin(), data1->GetRangeMax() + 0.5 * data1->GetBinWidth());
            KTWARN(wvlog, "Setting the input array size to " << 2 * size);
        }
        else
        {
            fInputArray->SetRange(data1->GetRangeMin(), data1->GetRangeMax());
        }
        /*  // For non-interpolating version
        UInt_t iPoint1 = offset;
        UInt_t iPoint2 = size - 1 + offset;
        */
        /**/ // For the interpolating version
        Double_t real1, real2, imag1, imag2;

        /*
        real1 = (*data1)(offset)[0];
        imag1 = (*data1)(offset)[1];
        real2 = (*data2)(size - 1 + offset)[0] + 0.5 * ((*data2)(size - 1 + offset)[0] - (*data2)(size - 2 + offset)[0]);
        imag2 = (*data2)(size - 1 + offset)[1] + 0.5 * ((*data2)(size - 1 + offset)[1] - (*data2)(size - 2 + offset)[1]);
        (*fInputArray)(0)[0] = real1 * real2 + imag1 * imag2;
        (*fInputArray)(0)[1] = imag1 * real2 - real1 * imag2;
        */
        UInt_t iPoint1 = offset;
        UInt_t iPoint2 = size - 1 + offset;
        /**/
        //for (UInt_t inPoint = 2; inPoint < 2*size; inPoint+=2)
        for (UInt_t inPoint = 0; inPoint < size; inPoint++)
        {
            /**/  // Non-interpolating
            (*fInputArray)(inPoint)[0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            (*fInputArray)(inPoint)[1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            /**/
            /*
            fInputArray[inPoint][0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            fInputArray[inPoint][1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            fInputArray[inPoint-1][0] = 0.5 * (fInputArray[inPoint-2][0] + fInputArray[inPoint][0]);
            fInputArray[inPoint-1][1] = 0.5 * (fInputArray[inPoint-2][1] + fInputArray[inPoint][1]);
            */
            /* // Interpolating
            real1 = (*data1)(iPoint1)[0];
            imag1 = (*data1)(iPoint1)[1];
            real2 = 0.5 * ((*data2)(iPoint2-1)[0] + (*data2)(iPoint2)[0]);
            imag2 = 0.5 * ((*data2)(iPoint2-1)[1] + (*data2)(iPoint2)[1]);
            (*fInputArray)(inPoint)[0] = real1 * real2 + imag1 * imag2;
            (*fInputArray)(inPoint)[1] = imag1 * real2 - real1 * imag2;
            real1 = 0.5 * ((*data1)(iPoint1-1)[0] + (*data1)(iPoint1)[0]);
            imag1 = 0.5 * ((*data1)(iPoint1-1)[1] + (*data1)(iPoint1)[1]);
            real2 = (*data2)(iPoint2)[0];
            imag2 = (*data2)(iPoint2)[1];
            (*fInputArray)(inPoint-1)[0] = real1 * real2 + imag1 * imag2;
            (*fInputArray)(inPoint-1)[1] = imag1 * real2 - real1 * imag2;
            */
            iPoint1++;
            iPoint2--;
        }
        /*
        real1 = (*data1)(size - 1 + offset)[0] + 0.5 * ((*data1)(size - 1 + offset)[0] - (*data1)(size - 2 + offset)[0]);
        imag1 = (*data1)(size - 1 + offset)[1] + 0.5 * ((*data1)(size - 1 + offset)[1] - (*data1)(size - 2 + offset)[1]);
        real2 = (*data2)(offset)[0];
        imag2 = (*data2)(offset)[1];
        (*fInputArray)(2*size - 1)[0] = real1 * real2 + imag1 * imag2;
        (*fInputArray)(2*size - 1)[1] = imag1 * real2 - real1 * imag2;
        */
        return;
    }

} /* namespace Katydid */
