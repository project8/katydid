/*
 * KTSparseWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTSPARSEWATERFALLCANDIDATEDATA_HH_
#define KTSPARSEWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"

#include <set>

namespace Katydid
{
    class KTTimeFrequency;

    class KTSparseWaterfallCandidateData : public KTExtensibleData< KTSparseWaterfallCandidateData >
    {
        public:
            struct Point
            {
                double fTimeInRun;
                double fFrequency;
                double fAmplitude;
                Point(double tir, double freq, double amp) : fTimeInRun(tir), fFrequency(freq), fAmplitude(amp) {}
            };

            struct PointCompare
            {
                bool operator() (const Point& lhs, const Point& rhs)
                {
                    return lhs.fTimeInRun < rhs.fTimeInRun || (lhs.fTimeInRun == rhs.fTimeInRun && lhs.fFrequency < rhs.fFrequency);
                }
            };

            typedef std::set< Point, PointCompare > Points;

        public:
            KTSparseWaterfallCandidateData();
            virtual ~KTSparseWaterfallCandidateData();

            const Points& GetPoints() const;
            unsigned GetComponent() const;

            //unsigned GetNTimeBins() const;
            double GetTimeBinWidth() const;

            //unsigned GetNFreqBins() const;
            double GetFreqBinWidth() const;

            double GetTimeInRun() const;
            double GetTimeLength() const;
            //uint64_t GetFirstSliceNumber() const;
            //uint64_t GetLastSliceNumber() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            //double GetMeanStartFrequency() const;
            //double GetMeanEndFrequency() const;
            double GetFrequencyWidth() const;

            //unsigned GetStartRecordNumber() const;
            //unsigned GetStartSampleNumber() const;
            //unsigned GetEndRecordNumber() const;
            //unsigned GetEndSampleNumber() const;

            void AddPoint(const Point& point);
            void SetComponent(unsigned component);

            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            void SetTimeInRun(double tir);
            void SetTimeLength(double length);
            //void SetFirstSliceNumber(uint64_t slice);
            //void SetLastSliceNumber(uint64_t slice);
            void SetMinimumFrequency(double freq);
            void SetMaximumFrequency(double freq);
            //void SetMeanStartFrequency(double freq);
            //void SetMeanEndFrequency(double freq);
            void SetFrequencyWidth(double width);

            //void SetStartRecordNumber(unsigned rec);
            //void SetStartSampleNumber(unsigned sample);
            //void SetEndRecordNumber(unsigned rec);
            //void SetEndSampleNumber(unsigned sample);

        protected:
            Points fPoints;
            unsigned fComponent;

            double fTimeBinWidth;
            double fFreqBinWidth;

            double fTimeInRun;
            double fTimeLength;
            //uint64_t fFirstSliceNumber;
            //uint64_t fLastSliceNumber;
            double fMinFrequency;
            double fMaxFrequency;
            //double fMeanStartFrequency;
            //double fMeanEndFrequency;
            double fFrequencyWidth;

            //unsigned fStartRecordNumber;
            //unsigned fStartSampleNumber;
            //unsigned fEndRecordNumber;
            //unsigned fEndSampleNumber;
    };

    inline const KTSparseWaterfallCandidateData::Points& KTSparseWaterfallCandidateData::GetPoints() const
    {
        return fPoints;
    }

    inline unsigned KTSparseWaterfallCandidateData::GetComponent() const
    {
        return fComponent;
    }
/*
    inline unsigned KTSparseWaterfallCandidateData::GetNTimeBins() const
    {
        return fCandidate->GetNTimeBins();
    }

    inline unsigned KTSparseWaterfallCandidateData::GetNFreqBins() const
    {
        return fCandidate->GetNFrequencyBins();
    }
*/
    inline double KTSparseWaterfallCandidateData::GetTimeBinWidth() const
    {
        return fTimeBinWidth;
    }

    inline double KTSparseWaterfallCandidateData::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline double KTSparseWaterfallCandidateData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline double KTSparseWaterfallCandidateData::GetTimeLength() const
    {
        return fTimeLength;
    }
/*
    inline uint64_t KTSparseWaterfallCandidateData::GetFirstSliceNumber() const
    {
        return fFirstSliceNumber;
    }

    inline uint64_t KTSparseWaterfallCandidateData::GetLastSliceNumber() const
    {
        return fLastSliceNumber;
    }
*/
    inline double KTSparseWaterfallCandidateData::GetMinimumFrequency() const
    {
        return fMinFrequency;
    }

    inline double KTSparseWaterfallCandidateData::GetMaximumFrequency() const
    {
        return fMaxFrequency;
    }
/*
    inline double KTSparseWaterfallCandidateData::GetMeanStartFrequency() const
    {
        return fMeanStartFrequency;
    }

    inline double KTSparseWaterfallCandidateData::GetMeanEndFrequency() const
    {
        return fMeanEndFrequency;
    }
*/
    inline double KTSparseWaterfallCandidateData::GetFrequencyWidth() const
    {
        return fFrequencyWidth;
    }
/*
    inline unsigned KTSparseWaterfallCandidateData::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline unsigned KTSparseWaterfallCandidateData::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline unsigned KTSparseWaterfallCandidateData::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline unsigned KTSparseWaterfallCandidateData::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }
*/
    inline void KTSparseWaterfallCandidateData::SetComponent(unsigned component)
    {
        fComponent = component;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetTimeInRun(double tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetTimeLength(double length)
    {
        fTimeLength = length;
        return;
    }
/*
    inline void KTSparseWaterfallCandidateData::SetFirstSliceNumber(uint64_t slice)
    {
        fFirstSliceNumber = slice;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetLastSliceNumber(uint64_t slice)
    {
        fLastSliceNumber = slice;
        return;
    }
*/
    inline void KTSparseWaterfallCandidateData::SetMinimumFrequency(double freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetMaximumFrequency(double freq)
    {
        fMaxFrequency = freq;
        return;
    }
/*
    inline void KTSparseWaterfallCandidateData::SetMeanStartFrequency(double freq)
    {
        fMeanStartFrequency = freq;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetMeanEndFrequency(double freq)
    {
        fMeanEndFrequency = freq;
        return;
    }
*/
    inline void KTSparseWaterfallCandidateData::SetFrequencyWidth(double width)
    {
        fFrequencyWidth = width;
        return;
    }
/*
    inline void KTSparseWaterfallCandidateData::SetStartRecordNumber(unsigned rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetStartSampleNumber(unsigned sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetEndRecordNumber(unsigned rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTSparseWaterfallCandidateData::SetEndSampleNumber(unsigned sample)
    {
        fEndSampleNumber = sample;
        return;
    }
*/
} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
