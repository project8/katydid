/*
 * KTEggReader2011.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER2011_HH_
#define KTEGGREADER2011_HH_

#include "KTEggReader.hh"

#include <fstream>
#include <sstream>

namespace Katydid
{

    class KTEggReader2011 : public KTEggReader
    {
        private:
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
            KTEggReader2011();
            virtual ~KTEggReader2011();

        public:
            virtual KTEggHeader* BreakEgg(const std::string& filename);
            virtual KTEvent* HatchNextEvent(KTEggHeader* header);
            virtual bool CloseEgg();

        private:
            template< typename XReturnType, typename XArrayType >
            XReturnType ConvertFromArray(XArrayType* value);

            std::string fFileName;
            std::ifstream fEggStream;
            std::string fPrelude;
            unsigned fHeaderSize;
            std::string fHeader;

            HeaderInfo fHeaderInfo;

            static const std::ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

    };

    template< typename XReturnType, typename XArrayType >
    XReturnType KTEggReader2011::ConvertFromArray(XArrayType* value)
    {
        std::stringstream converter;
        XReturnType converted;
        converter << value;
        converter >> converted;
        return converted;
    }


} /* namespace Katydid */
#endif /* KTEGGREADER2011_HH_ */
