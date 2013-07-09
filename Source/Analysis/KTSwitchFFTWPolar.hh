/**
 @file KTSwitchFFTWPolar.hh
 @brief Contains KTSwitchFFTWPolar
 @details Switch fftw-type frequency spectra to polar.
 @author: N. S. Oblath
 @date: Mar 19, 2013
 */

#ifndef KTSWITCHFFTWPOLAR_HH_
#define KTSWITCHFFTWPOLAR_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTData;
    class KTFrequencySpectrumDataFFTW;
    class KTNormalizedFSDataFFTW;
    class KTPStoreNode;
    class KTWignerVilleData;
 

   /*!
     @class KTSwitchFFTWPolar
     @author N. S. Oblath

     @brief Switch fftw-type frequency spectra to polar and vice versa (actually, just the former so far)

     @details
 
     Available configuration values:
     - "use-neg-freqs": bool -- If true [default], corresponding negative and positive frequency bins are summed; if false, the negative frequency bins are dropped.

     Slots:
     - "fs-fftw": void (shared_data< KTData >) -- Switch an fftw FS to polar; Requires KTFrequencySpectrumDataFFTW; Adds KTFrequencySpectrumDataPolar
     - "norm-fs-fftw": void (shared_data< KTData >) -- Switch a normalized fftw FS to polar; Requires KTNormalizedFSDataPolar; Adds KTFrequencySpectrumDataPolar
     - "wv": void (shared_data< KTData >) -- Switch an WV FS to polar; Requires KTWignerVilleData; Adds KTFrequencySpectrumDataPolar

     Signals:
     - "fs-polar": void (shared_data< KTData >) emitted upon performance of a switch from fftw to polar; Guarantees KTFrequencySpectrumDataPolar
    */

    class KTSwitchFFTWPolar : public KTProcessor
    {
        public:
            KTSwitchFFTWPolar(const std::string& name = "switch-fftw-polar");
            virtual ~KTSwitchFFTWPolar();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t GetUseNegFreqs() const;
            void SetUseNegFreqs(Bool_t flag);

        private:
            Bool_t fUseNegFreqs;

        public:
            Bool_t SwitchToPolar(KTFrequencySpectrumDataFFTW& fsData);
            Bool_t SwitchToPolar(KTNormalizedFSDataFFTW& fsData);
            Bool_t SwitchToPolar(KTWignerVilleData& fsData);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFSPolarSignal;
            //KTSignalData fFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            KTSlotDataOneType< KTNormalizedFSDataFFTW > fNormFSFFTWSlot;
            KTSlotDataOneType< KTWignerVilleData > fWignerVilleSlot;

    };

    inline Bool_t KTSwitchFFTWPolar::GetUseNegFreqs() const
    {
        return fUseNegFreqs;
    }

    inline void KTSwitchFFTWPolar::SetUseNegFreqs(Bool_t flag)
    {
        fUseNegFreqs = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTSWITCHFFTWPOLAR_HH_ */
