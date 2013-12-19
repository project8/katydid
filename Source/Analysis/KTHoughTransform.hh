/**
 @file KTHoughTransform.hh
 @brief Contains KTHoughTransform
 @details 
 @author: N. S. Oblath
 @date: Nov 14, 2012
 */

#ifndef KTHOUGHTRANSFORM_HH_
#define KTHOUGHTRANSFORM_HH_

#include "KTProcessor.hh"

#include "KTData.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTPhysicalArray.hh"
#include "KTSlot.hh"



namespace Katydid
{
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    //class KTSlidingWindowFSDataFFTW;

    /*!
     @class KTHoughTransform
     @author N. S. Oblath

     @brief 

     @details

     Configuration name: "hough-transform"

     Available configuration values:
     - "transform_flag": string -- flag that determines how much planning is done prior to any transforms
     - "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     - "wisdom-filename": string -- filename for loading/saving FFTW wisdom
     - "input-data-name": string -- name of the data to find when processing an event
     - "output-data-name": string -- name to give to the data produced by an FFT

     Slots:
     <!--- "swfs-data": void (KTDataPtr)-->
     - "disc-data": void (KTDataPtr) -- Performs a Hough Transform on discriminated (2D) points; Requires KTDiscriminatedPoints2DData; Adds KTHoughData

     Signals:
     - "hough-transform": void (KTDataPtr) Emitted upon performance of a transform; Guarantees KTHoughData
    */

    class KTHoughTransform : public KTProcessor
    {
        public:
            typedef KTDiscriminatedPoints2DData::SetOfPoints SetOfPoints;

        protected:
            typedef KTSignalConcept< void (KTDataPtr) >::signal HTSignal;

        public:
            KTHoughTransform(const std::string& name = "hough-transform");
            virtual ~KTHoughTransform();

            bool Configure(const KTPStoreNode* node);

            unsigned GetNThetaPoints() const;
            void SetNThetaPoints(unsigned nPoints);

            unsigned GetNRPoints() const;
            void SetNRPoints(unsigned nPoints);

        private:

            unsigned fNThetaPoints;
            unsigned fNRPoints;

        public:
            //bool TransformData(KTSlidingWindowFSDataFFTW& data);
            //KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* powerSpectrum);

            bool TransformData(KTDiscriminatedPoints2DData& data);
            KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* TransformSetOfPoints(const SetOfPoints& points, unsigned nTimeBins, unsigned nFreqBins);


        private:
            //KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* RemoveNegativeFrequencies(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum);

            //***************
             // Signals
             //***************

         private:
             KTSignalData fHTSignal;

             //***************
             // Slots
             //***************

         private:
             KTSlotDataOneType< KTDiscriminatedPoints2DData > fDiscPts2DSlot;
             //KTSlotDataOneType< KTSlidingWindowFSDataFFTW > fSWFSFFTWSlot;

    };

    inline unsigned KTHoughTransform::GetNThetaPoints() const
    {
        return fNThetaPoints;
    }

    inline void KTHoughTransform::SetNThetaPoints(unsigned nPoints)
    {
        fNThetaPoints = nPoints;
        return;
    }

    inline unsigned KTHoughTransform::GetNRPoints() const
    {
        return fNRPoints;
    }

    inline void KTHoughTransform::SetNRPoints(unsigned nPoints)
    {
        fNRPoints = nPoints;
        return;
    }

} /* namespace Katydid */
#endif /* KTHOUGHTRANSFORM_HH_ */
