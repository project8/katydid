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


namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTNormalizedFSDataFFTW;
    class KTParamNode;
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

            bool Configure(const KTParamNode* node);

            bool GetUseNegFreqs() const;
            void SetUseNegFreqs(bool flag);

        private:
            bool fUseNegFreqs;

        public:
            bool SwitchToPolar(KTFrequencySpectrumDataFFTW& fsData);
            bool SwitchToPolar(KTNormalizedFSDataFFTW& fsData);
            bool SwitchToPolar(KTWignerVilleData& fsData);

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

    inline bool KTSwitchFFTWPolar::GetUseNegFreqs() const
    {
        return fUseNegFreqs;
    }

    inline void KTSwitchFFTWPolar::SetUseNegFreqs(bool flag)
    {
        fUseNegFreqs = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTSWITCHFFTWPOLAR_HH_ */
