/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERANALYSIS_HH_
#define KTBASICROOTTYPEWRITERANALYSIS_HH_

#include "KTBasicROOTFileWriter.hh"

namespace Katydid
{
    class KTCorrelationData;
    class KTHoughData;
    class KTGainVariationData;

    class KTBasicROOTTypeWriterAnalysis : public KTBasicROOTTypeWriter
    {
        public:
            friend class KTTimeSeriesData;

        public:
            KTBasicROOTTypeWriterAnalysis();
            virtual ~KTBasicROOTTypeWriterAnalysis();

            void RegisterSlots();


            //************************
            // Correlation Data
            //************************
        public:
            void WriteCorrelationData(const KTCorrelationData* data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void WriteHoughData(const KTHoughData* data);

            //************************
            // Gain Variation Data
            //************************
        public:
            void WriteGainVariationData(const KTGainVariationData* data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERANALYSIS_HH_ */
