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
    using namespace Nymph;
    class KTROOTSpectrogramTypeWriterTransform : public KTROOTSpectrogramTypeWriter//, public KTTypeWriterTransform
    {
        public:
            KTROOTSpectrogramTypeWriterTransform();
            virtual ~KTROOTSpectrogramTypeWriterTransform();

            void RegisterSlots();

        public:
            void AddFrequencySpectrumDataPolar(KTDataPtr data);
            void OutputFrequencySpectrumDataPolar();

            void AddFrequencySpectrumDataFFTW(KTDataPtr data);
            void OutputFrequencySpectrumDataFFTW();

            void AddPowerSpectrumData(KTDataPtr data);
            void OutputPowerSpectrumData();

            void AddPSDData(KTDataPtr data);
            void OutputPSDData();

            void TakeLine(KTDataPtr data);

        private:
            std::vector< SpectrogramData > fFSPolarSpectrograms;
            std::vector< SpectrogramData > fFSFFTWSpectrograms;
            std::vector< SpectrogramData > fPowerSpectrograms;
            std::vector< SpectrogramData > fPSDSpectrograms;
            TOrdCollection fLineCollection;
            

        public:
            void OutputSpectrograms();
            void OutputLines();
            void ClearSpectrograms();

        private:
            void OutputASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
            void ClearASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_ */
