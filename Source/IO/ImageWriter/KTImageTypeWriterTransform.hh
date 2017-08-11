/*
 * KTImageTypeWriterTransform.hh
 *
 *  Created on: Apr 12, 2017
 *      Author: nsoblath
 */

#ifndef KTIMAGETYPEWRITERTRANSFORM_HH_
#define KTIMAGETYPEWRITERTRANSFORM_HH_

#include "KTImageWriter.hh"

namespace Katydid
{
    
    class KTImageTypeWriterTransform : public KTImageTypeWriter//, public KTTypeWriterTransform
    {
        public:
            KTImageTypeWriterTransform();
            virtual ~KTImageTypeWriterTransform();

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
            std::string fFSPolarIndicator;

            std::vector< SpectrogramData > fFSFFTWSpectrograms;
            std::string fFSFFTWIndicator;

            std::vector< SpectrogramData > fPowerSpectrograms;
            std::string fPowerIndicator;

            std::vector< SpectrogramData > fPSDSpectrograms;
            std::string fPSDIndicator;

        public:
            void OutputSpectrograms();
            void ClearSpectrograms();

        private:
            void OutputASpectrogramSet(std::vector< SpectrogramData >& aSpectSet, const std::string& aDataTypeIndicator);
            void ClearASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
    };


} /* namespace Katydid */
#endif /* KTIMAGETYPEWRITERTRANSFORM_HH_ */
