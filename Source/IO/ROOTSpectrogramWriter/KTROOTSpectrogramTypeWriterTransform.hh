/*
 * KTROOTSpectrogramTypeWriterTransform.hh
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#ifndef KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_
#define KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_

#include "KTROOTSpectrogramWriter.hh"

namespace Katydid
{
    
    class KTROOTSpectrogramTypeWriterTransform : public KTROOTSpectrogramTypeWriter//, public KTTypeWriterTransform
    {
        public:
            KTROOTSpectrogramTypeWriterTransform();
            virtual ~KTROOTSpectrogramTypeWriterTransform();

            void RegisterSlots();

        public:
            void AddFrequencySpectrumDataPolar(Nymph::KTDataPtr data);
            void OutputFrequencySpectrumDataPolar();

            void AddFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
            void OutputFrequencySpectrumDataFFTW();

            void AddPowerSpectrumData(Nymph::KTDataPtr data);
            void OutputPowerSpectrumData();

            void AddPSDData(Nymph::KTDataPtr data);
            void OutputPSDData();

        private:
            std::vector< SpectrogramData > fFSPolarSpectrograms;
            std::vector< SpectrogramData > fFSFFTWSpectrograms;
            std::vector< SpectrogramData > fPowerSpectrograms;
            std::vector< SpectrogramData > fPSDSpectrograms;

        public:
            void OutputSpectrograms();
            void ClearSpectrograms();

        private:
            void OutputASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
            void ClearASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_ */
