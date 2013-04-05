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

     Slots:
     \li \c "fs-fftw": void (shared_data< KTData >) -- Switch an fftw FS to polar; Requires KTFrequencySpectrumDataFFTW; Adds KTFrequencySpectrumDataPolar
     \li \c "norm-fs-fftw": void (shared_data< KTData >) -- Switch a normalized fftw FS to polar; Requires KTNormalizedFSDataPolar; Adds KTFrequencySpectrumDataPolar
     \li \c "wv": void (shared_data< KTData >) -- Switch an WV FS to polar; Requires KTWignerVilleData; Adds KTFrequencySpectrumDataPolar

     Signals:
     \li \c "fs-polar": void (shared_data< KTData >) emitted upon performance of a switch from fftw to polar; Guarantees KTFrequencySpectrumDataPolar
    */

    class KTSwitchFFTWPolar : public KTProcessor
    {
        public:
            KTSwitchFFTWPolar(const std::string& name = "gain-normalization");
            virtual ~KTSwitchFFTWPolar();

            Bool_t Configure(const KTPStoreNode* node);

        private:

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

} /* namespace Katydid */
#endif /* KTSWITCHFFTWPOLAR_HH_ */
