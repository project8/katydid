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

class TH2D;

namespace Katydid
{

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

            void OutputSpectrograms();

            void ClearSpectrograms();

        private:
            struct SpectrogramData {
                TH2D* fSpectrogram;
                unsigned fFirstFreqBin; // frequency-axis bin 0 is this bin in the incoming data
                unsigned fLastFreqBin; // frequency-axis last-bin is this bin in the incoming data
                unsigned fNextTimeBinToFill; // keep track of the progress filling the spectrogram from slice to slice
            };

            void OutputASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);
            void ClearASpectrogramSet(std::vector< SpectrogramData >& aSpectSet);

            std::vector< SpectrogramData > fFSPolarSpectrograms;
            std::vector< SpectrogramData > fFSFFTWSpectrograms;

    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITERTRANSFORM_HH_ */
