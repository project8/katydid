/*
 * KTEgg1Reader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGG1READER_HH_
#define KTEGG1READER_HH_

#include "KTEggReader.hh"

#include "KTConstants.hh"

#include <fstream>
#include <sstream>

namespace Katydid
{
    

    class KTEgg1Reader : public KTEggReader
    {
        private:
        public:
            struct HeaderInfo
            {
                int fSliceSize;
                int fFrameIDSize;
                int fRecordSize;
                int fTimeStampSize;
                double fRunLength;
                double fSampleRate;
                double fHertzPerSampleRateUnit;
                double fSecondsPerRunLengthUnit;
                unsigned fDataFormat;

                HeaderInfo() :
                    fSliceSize(0),
                    fFrameIDSize(0),
                    fRecordSize(0),
                    fTimeStampSize(0),
                    fRunLength(0.),
                    fSampleRate(0.),
                    fHertzPerSampleRateUnit(1.),
                    fSecondsPerRunLengthUnit(1.),
                    fDataFormat(sInvalidFormat)
                {}
            };

        public:
            KTEgg1Reader();
            virtual ~KTEgg1Reader();

        public:
            virtual bool Configure(const KTEggProcessor& eggProc);

            virtual Nymph::KTDataPtr BreakEgg(const path_vec& filenames);
            virtual Nymph::KTDataPtr HatchNextSlice();
            virtual bool CloseEgg();

            /// Returns the time since the run started in seconds
            double GetTimeInRun() const;
            virtual double GetIntegratedTime() const;
            /// Returns the time within the current acquisition
            double GetTimeInAcq() const;

            virtual unsigned GetNSlicesProcessed() const ;
            virtual unsigned GetNRecordsProcessed() const;

        private:
            template< typename XReturnType, typename XArrayType >
            XReturnType ConvertFromArray(XArrayType* value);

            std::string fFileName;
            std::ifstream fEggStream;
            std::string fPrelude;
            unsigned fHeaderSize;
            std::string fHeader;

            HeaderInfo fHeaderInfo;

            unsigned fRecordsRead;

            unsigned fLastFrameID;

            static const std::ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

    };

    template< typename XReturnType, typename XArrayType >
    XReturnType KTEgg1Reader::ConvertFromArray(XArrayType* value)
    {
        std::stringstream converter;
        XReturnType converted;
        converter << value;
        converter >> converted;
        return converted;
    }

    inline double KTEgg1Reader::GetTimeInRun() const
    {
        return double(fRecordsRead * fHeaderInfo.fRecordSize) / fHeaderInfo.fSampleRate;
    }
    inline double KTEgg1Reader::GetTimeInAcq() const
    {
        // For the Egg data taken with a free streaming digitizer, the TimeInRun happens to be equal to the TimeInAcq
        return GetTimeInRun();
    }

    inline double KTEgg1Reader::GetIntegratedTime() const
    {
        return GetTimeInRun();
    }

    inline unsigned KTEgg1Reader::GetNSlicesProcessed() const
    {
        return fRecordsRead;
    }

    inline unsigned KTEgg1Reader::GetNRecordsProcessed() const
    {
        return fRecordsRead;
    }


} /* namespace Katydid */
#endif /* KTEGG1READER_HH_ */
