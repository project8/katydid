/*
 * KTROOTImageWriter.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTROOTIMAGEWRITER_HH_
#define KTROOTIMAGEWRITER_HH_

#include "KTWriter.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTCorrelationData.hh"

#include "TFile.h"

namespace Katydid
{
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;

    class KTROOTImageWriter : public KTWriter
    {
        public:
            KTROOTImageWriter();
            virtual ~KTROOTImageWriter();

            Bool_t Configure(const KTPStoreNode* node);

            //************************
            // Basic Publish and Write
            //************************
        public:

            void Publish(const KTWriteableData* data);

            void Write(const KTWriteableData* data);

            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void Write(const KTFrequencySpectrumData* data);
            void Write(const KTFrequencySpectrumDataFFTW* data);

            const std::string& GetFilenameBaseFrequencySpectrum() const;
            void SetFilenameBaseFrequencySpectrum(const std::string& fnameBase);

        protected:
            std::string fFilenameBaseFrequencySpectrum;

            //************************
            // Correlation Data
            //************************
        public:
            void Write(const KTCorrelationData* data);

            //************************
            // Sliding Window Data
            //************************
        public:
            void Write(const KTSlidingWindowFSData* data);
            void Write(const KTSlidingWindowFSDataFFTW* data);

    };

    inline const std::string& KTROOTImageWriter::GetFilenameBaseFrequencySpectrum() const
    {
        return fFilenameBaseFrequencySpectrum;
    }
    inline void KTROOTImageWriter::SetFilenameBaseFrequencySpectrum(const std::string& fnameBase)
    {
        fFilenameBaseFrequencySpectrum = fnameBase;
        return;
    }


} /* namespace Katydid */
#endif /* KTROOTIMAGEWRITER_HH_ */
