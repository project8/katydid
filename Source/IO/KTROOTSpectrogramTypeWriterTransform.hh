/*
 * KTROOTSpectrogramTypeWriterTransform.hh
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#ifndef KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_
#define KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_

#include "KTROOTSpectrogramWriter.hh"

#include "TOrdCollection.h"

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

            void TakeLine(Nymph::KTDataPtr data);


        private:
            std::vector< SpectrogramData > fFSPolarSpectrograms;
            std::vector< SpectrogramData > fFSFFTWSpectrograms;
            std::vector< SpectrogramData > fPowerSpectrograms;
            std::vector< SpectrogramData > fPSDSpectrograms;
            TOrdCollection *fLineCollection;


        public:
            void OutputSpectrograms();
            void OutputLines();
            void ClearSpectrograms();
            void ClearLines();

        private:
            void OutputASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
            void ClearASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_ */
