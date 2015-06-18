/*
 * KTROOTSpectrogramTypeWriterTransform.hh
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#ifndef KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_
#define KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_

#include "KTROOTSpectrogramWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTROOTSpectrogramTypeWriterTransform : public KTMEROOTTypeWriterBase//, public KTTypeWriterTransform
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

        private:
            std::vector< KTFrequencySpectrumDataPolar > fFSPData;
            std::vector< KTFrequencySpectrumDataFFTW > fFSFData;

    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_ */
