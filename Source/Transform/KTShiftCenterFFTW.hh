/**
 @file KTShiftCenterFFTW.hh
 @brief Shifts Center of FFTW Spectrum
 @details Shifts the center of the FFTW frequency spectrum by fCenterFrequency, which is equal by InputCenter from the RSA data.
 @author: L. de Viveiros
 @date: Dec 1, 2014
 */

#ifndef KTSHIFTCENTERFFTW_HH_
#define KTSHIFTCENTERFFTW_HH_


#include "KTLogger.hh"
#include "KTSlot.hh"
#include "KTProcessor.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <cmath>
#include <string>
#include <vector>

namespace Katydid
{
    class KTEggHeader;
    class KTParamNode;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    /*!
     @class KTShiftCenterFFTW
     @author L. de Viveiros

     @brief Shifts Center of FFTW Spectrum

     @details
     Shifts the center of the FFTW frequency spectrum by fCenterFrequency, which is equal by InputCenter from the RSA data.

     Configuration name: "shiftcenter-fftw"

     Slots:
     - "fs-fftw-in": void (KTDataPtr) -- Shifts the center frequency of the FFTW spectrum; Requires KTFrequencySpectrumDataFFTW;

     Signals:
     - "fs-fftw-out": void (KTDataPtr) -- Emitted upon performance of a forward transform; Guarantees KTFrequencySpectrumDataFFTW.
    */

    class KTShiftCenterFFTW : public KTProcessor
    {

        public:
            KTShiftCenterFFTW(const std::string& name = "shiftcenter-fftw");
            virtual ~KTShiftCenterFFTW();

            bool Configure(const KTParamNode* node);

            MEMBERVARIABLE(double, CenterFrequency);


        public:
            bool ShiftCenterFFTW(KTFrequencySpectrumDataFFTW& fsData);

            KTFrequencySpectrumFFTW* ShiftCenterFFTW(const KTFrequencySpectrumFFTW* frequencySpectrum) const;


            //***************
            // Signals
            //***************

        private:
            KTSignalData fFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;

    };

}
 /* namespace Katydid */
#endif /* KTSHIFTCENTERFFTW_HH_ */

