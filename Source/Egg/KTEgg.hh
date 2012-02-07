/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

namespace Katydid
{
    class KTEvent;

    class KTEgg
    {
        public:
            struct HeaderInfo
            {
                int fEventSize;
                int fFrameIDSize;
                int fRecordSize;
                int fTimeStampSize;
                double fRunLength;
                double fSampleRate;
                double fHertzPerSampleRateUnit;
                double fSecondsPerRunLengthUnit;

                HeaderInfo() :
                    fTimeStampSize(0),
                    fFrameIDSize(0),
                    fRecordSize(0),
                    fEventSize(0),
                    fRunLength(0.),
                    fSampleRate(0.),
                    fHertzPerSampleRateUnit(1.),
                    fSecondsPerRunLengthUnit(1.)
                {}
            };

        public:
            KTEgg();
            virtual ~KTEgg();

            bool BreakEgg();
            bool ParseEggHeader();
            KTEvent* HatchNextEvent();

            const std::string& GetFileName() const;
            const std::ifstream& GetEggStream() const;
            unsigned GetHeaderSize() const;
            const std::string& GetHeader() const;
            const std::string& GetPrelude() const;

            const HeaderInfo& GetHeaderInfo() const;
            int GetEventSize() const;
            int GetFrameIDSize() const;
            int GetRecordSize() const;
            double GetRunLength() const;
            double GetSampleRate() const;
            int GetTimeStampSize() const;

            double GetHertzPerSampleRateUnit() const;
            double GetSecondsPerRunLengthUnit() const;


            void SetFileName(const std::string& fileName);
            void SetHeaderSize(unsigned size);
            void SetHeader(const std::string& header);
            void SetPrelude(const std::string& prelude);

            void SetEventSize(int size);
            void SetFrameIDSize(int size);
            void SetRecordSize(int size);
            void SetRunLength(double length);
            void SetSampleRate(double rate);
            void SetTimeStampSize(int size);

            void SetHertzPerSampleRateUnit(double hpsru);
            void SetSecondsPerRunLengthUnit(double sprlu);

        private:
            template< typename XReturnType >
            XReturnType ConvertFromCharArray(char* value);

            std::string fFileName;
            std::ifstream fEggStream;
            std::string fPrelude;
            unsigned fHeaderSize;
            std::string fHeader;

            HeaderInfo fHeaderInfo;

            static const std::ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

    };


    inline const std::string& KTEgg::GetFileName() const
    {
        return fFileName;
    }

    inline const std::ifstream& KTEgg::GetEggStream() const
    {
        return fEggStream;
    }

    inline unsigned KTEgg::GetHeaderSize() const
    {
        return fHeaderSize;
    }

    inline const std::string& KTEgg::GetHeader() const
    {
        return fHeader;
    }

    inline const std::string& KTEgg::GetPrelude() const
    {
        return fPrelude;
    }

    inline const KTEgg::HeaderInfo& KTEgg::GetHeaderInfo() const
    {
        return fHeaderInfo;
    }

    inline int KTEgg::GetEventSize() const
    {
        return fHeaderInfo.fEventSize;
    }

    inline int KTEgg::GetFrameIDSize() const
    {
        return fHeaderInfo.fFrameIDSize;
    }

    inline int KTEgg::GetRecordSize() const
    {
        return fHeaderInfo.fRecordSize;
    }

    inline double KTEgg::GetRunLength() const
    {
        return fHeaderInfo.fRunLength;
    }

    inline double KTEgg::GetSampleRate() const
    {
        return fHeaderInfo.fSampleRate;
    }

    inline int KTEgg::GetTimeStampSize() const
    {
        return fHeaderInfo.fTimeStampSize;
    }

    inline double KTEgg::GetHertzPerSampleRateUnit() const
    {
        return fHeaderInfo.fHertzPerSampleRateUnit;
    }

    inline double KTEgg::GetSecondsPerRunLengthUnit() const
    {
        return fHeaderInfo.fSecondsPerRunLengthUnit;
    }



    inline void KTEgg::SetFileName(const std::string& fileName)
    {
        this->fFileName = fileName;
    }

    inline void KTEgg::SetHeaderSize(unsigned size)
    {
        this->fHeaderSize = size;
    }

    inline void KTEgg::SetHeader(const std::string& header)
    {
        this->fHeader = header;
    }

    inline void KTEgg::SetPrelude(const std::string& prelude)
    {
        this->fPrelude = prelude;
    }

    inline void KTEgg::SetEventSize(int size)
    {
        fHeaderInfo.fEventSize = size;
    }

    inline void KTEgg::SetFrameIDSize(int size)
    {
        fHeaderInfo.fFrameIDSize = size;
    }

    inline void KTEgg::SetRecordSize(int size)
    {
        fHeaderInfo.fRecordSize = size;
    }

    inline void KTEgg::SetRunLength(double length)
    {
        fHeaderInfo.fRunLength = length;
    }

    inline void KTEgg::SetSampleRate(double rate)
    {
        fHeaderInfo.fSampleRate = rate;
    }

    inline void KTEgg::SetTimeStampSize(int size)
    {
        fHeaderInfo.fTimeStampSize = size;
    }

    inline void KTEgg::SetHertzPerSampleRateUnit(double hpsru)
    {
        fHeaderInfo.fHertzPerSampleRateUnit = hpsru;
    }

    inline void KTEgg::SetSecondsPerRunLengthUnit(double sprlu)
    {
        fHeaderInfo.fSecondsPerRunLengthUnit = sprlu;
    }


    template< typename XReturnType >
    XReturnType KTEgg::ConvertFromCharArray(char* value)
    {
        std::stringstream converter;
        XReturnType converted;
        converter << value;
        converter >> converted;
        return converted;
    }

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
