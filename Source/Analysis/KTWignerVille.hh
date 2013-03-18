/*
 * KTWignerVille.hh
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */
/**
 @file KTWignerVille.hh
 @brief Contains KTWignerVille
 @details 
 @author: N. S. Oblath
 @date: Oct 19, 2012
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTComplexFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <complex>
#include <utility>


namespace Katydid
{
    KTLOGGER(wvlog, "katydid.analysis");

    class KTAnalyticAssociateData;
    class KTComplexFFTW;
    class KTData;
    class KTEggHeader;
    //class KTFrequencySpectrumPolar;
    //class KTFrequencySpectrumDataFFTW;
    //class KTFrequencySpectrumFFTW;
    //class KTSlidingWindowFFTW;
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    /*!
     @class KTWignerVille
     @author N. S. Oblath

     @brief 

     @details

     Available configuration values:
     \li \c "complex-fftw": string -- 
     \li \c "wv-pair": bool -- channel pair to be used in the Wigner-Ville transform: "[first channel], [second channel]"; e.g. "0, 0" or "0, 1"

     Slots:
     \li \c "header": void (const KTEggHeader*) -- Initializes the transform using an Egg header
     \li \c "ts": void (shared_ptr< KTData >) -- Perform a WV transform on a time series; Requires KTTimeSeriesData; Adds KTWignerVilleData
     \li \c "aa": void (shared_ptr< KTData >) -- Perform a WV transform on an analytic associate: Requires KTAnalyticAssociateData; Adds KTWignerVilleData

     Signals:
     \li \c "wigner-ville": void (shared_ptr< KTData >) -- Emitted upon performance of a WV transform; Guarantees KTWignerVilleData
    */

    class KTWignerVille : public KTProcessor
    {
        private:
            typedef std::vector< KTWVPair > PairVector;

        public:
            KTWignerVille(const std::string& name = "wigner-ville");
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const KTWVPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            KTComplexFFTW* GetFFT();
            const KTComplexFFTW* GetFFT() const;

            void InitializeWithHeader(const KTEggHeader* header);

        private:
            PairVector fPairs;

            KTComplexFFTW* fFFT;
            KTTimeSeriesFFTW* fInputArray;


        public:
            /// Performs the W-V transform on the given time series data.
            Bool_t TransformData(KTTimeSeriesData& data);
            /// Performs the WV transform on the given analytic associate data.
            Bool_t TransformData(KTAnalyticAssociateData& data);

        private:
            template< class XDataType >
            Bool_t TransformFFTWBasedData(XDataType& data);

            void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);

            //***************
             // Signals
             //***************

         private:
             KTSignalData fWVSignal;

             //***************
             // Slots
             //***************

         private:
             KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
             KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
             KTSlotDataOneType< KTAnalyticAssociateData > fAnalyticAssociateSlot;

    };

    inline void KTWignerVille::AddPair(const KTWVPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTWignerVille::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTWignerVille::PairVector& KTWignerVille::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTWignerVille::ClearPairs()
    {
        fPairs.clear();
        return;
    }

    inline KTComplexFFTW* KTWignerVille::GetFFT()
    {
        return fFFT;
    }

    inline const KTComplexFFTW* KTWignerVille::GetFFT() const
    {
        return fFFT;
    }

    template< class XDataType >
    Bool_t KTWignerVille::TransformFFTWBasedData(XDataType& data)
    {
        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return false;
        }

        UInt_t nComponents = data.GetNComponents();
        UInt_t nPairs = fPairs.size();

        // cast all time series into KTTimeSeriesFFTW
        std::vector< const KTTimeSeriesFFTW* > timeSeries(nComponents);
        for (UInt_t iTS=0; iTS < nComponents; iTS++)
        {
            timeSeries[iTS] = dynamic_cast< const KTTimeSeriesFFTW* >(data.GetTimeSeries(iTS));
            if (timeSeries[iTS] == NULL)
            {
                KTERROR(wvlog, "Time series " << iTS << " did not cast to a const KTTimeSeriesFFTW*. No transforms performed.");
                return false;
            }
        }

        KTWignerVilleData& newData = data.template Of< KTWignerVilleData >().SetNComponents(nPairs);

        // Do WV transform for each pair
        UInt_t iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            CrossMultiplyToInputArray(timeSeries[firstChannel], timeSeries[secondChannel], 0);

            KTFrequencySpectrumFFTW* newSpectrum = fFFT->Transform(fInputArray);
            newSpectrum->SetRange(0.5 * newSpectrum->GetRangeMin(), 0.5 * newSpectrum->GetRangeMax());

            newData.SetSpectrum(newSpectrum, iPair);
            newData.SetInputPair(firstChannel, secondChannel, iPair);
            iPair++;
        }
        KTDEBUG(wvlog, "Completed WV transform of " << nComponents << " components");

        return true;
    }


} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
