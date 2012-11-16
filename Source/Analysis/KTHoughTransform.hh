/*
 * KTHoughTransform.hh
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#ifndef KTHOUGHTRANSFORM_HH_
#define KTHOUGHTRANSFORM_HH_

#include "KTProcessor.hh"

#include "KTPhysicalArray.hh"

namespace Katydid
{
    class KTEvent;
    class KTFrequencySpectrumFFTW;
    class KTHoughData;
    class KTSlidingWindowFSDataFFTW;
    class KTWriteableData;

    class KTHoughTransform : public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTWriteableData*) >::signal HTSignal;

        public:
            KTHoughTransform();
            virtual ~KTHoughTransform();

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetNThetaPoints() const;
            void SetNThetaPoints(UInt_t nPoints);

            UInt_t GetNRPoints() const;
            void SetNRPoints(UInt_t nPoints);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:

            UInt_t fNThetaPoints;
            UInt_t fNRPoints;

            std::string fInputDataName;
            std::string fOutputDataName;


        public:
            KTHoughData* TransformData(const KTSlidingWindowFSDataFFTW* data);
            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum);

            //***************
             // Signals
             //***************

         private:
             HTSignal fHTSignal;

             //***************
             // Slots
             //***************

         public:
             //void ProcessHeader(const KTEggHeader* header);
             void ProcessEvent(KTEvent* event);
             void ProcessSWFSData(const KTSlidingWindowFSDataFFTW* data);


    };

    inline UInt_t KTHoughTransform::GetNThetaPoints() const
    {
        return fNThetaPoints;
    }

    inline void KTHoughTransform::SetNThetaPoints(UInt_t nPoints)
    {
        fNThetaPoints = nPoints;
        return;
    }

    inline UInt_t KTHoughTransform::GetNRPoints() const
    {
        return fNRPoints;
    }

    inline void KTHoughTransform::SetNRPoints(UInt_t nPoints)
    {
        fNRPoints = nPoints;
        return;
    }

    inline const std::string& KTHoughTransform::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTHoughTransform::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTHoughTransform::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTHoughTransform::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTHOUGHTRANSFORM_HH_ */
