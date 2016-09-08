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
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

#include <vector>

namespace Katydid
{
    
    /*!
     @class KTHoughTransform
     @author N. S. Oblath

     @brief 

     @details
     See http://en.wikipedia.org/wiki/Hough_transform for details on the Hough Transform.

     Given the (r, theta) parameterization, the slope-intercept equation can be written as:
     y = (-cos(theta)/sin(theta)) * x + r / sin(theta)

     Configuration name: "hough-transform"

     Available configuration values:
     - "n-theta-points": unsigned int -- number of points used to divide up the theta axis
     - "n-r-points: unsigned int -- number of points used to divide up the radius axis

     Slots:
     - "swf-cand": void (Nymph::KTDataPtr) -- Performs a Hough Transform on sparse waterfall candidate data; Requires KTSparseWaterfallCandidateData; Adds KTHoughData
     - "wf-cand": void (Nymph::KTDataPtr) -- Performs a Hough Transform on waterfall candidate data; Requires KTWaterfallCandidateData; Adds KTHoughData
     - "disc": void (Nymph::KTDataPtr) -- Performs a Hough Transform on discriminated (2D) points; Requires KTDiscriminatedPoints2DData; Adds KTHoughData

     Signals:
     - "hough": void (Nymph::KTDataPtr) Emitted upon performance of a transform; Guarantees KTHoughData
    */

    class KTHoughTransform : public Nymph::KTProcessor
    {
        public:
            typedef KTDiscriminatedPoints2DData::SetOfPoints SetOfPoints;
            typedef KTSparseWaterfallCandidateData::Points SWFPoints;

        public:
            KTHoughTransform(const std::string& name = "hough-transform");
            virtual ~KTHoughTransform();

            bool Configure(const scarab::param_node* node);

            unsigned GetNThetaPoints() const;
            void SetNThetaPoints(unsigned nPoints);

            unsigned GetNRPoints() const;
            void SetNRPoints(unsigned nPoints);

        private:

            unsigned fNThetaPoints;
            unsigned fNRPoints;

        public:
            bool TransformData(KTSparseWaterfallCandidateData& data);
            KTPhysicalArray< 2, double >* TransformPoints(const SWFPoints& points, double minTime, double timeLength, double minFreq, double freqWidth);

            bool TransformData(KTWaterfallCandidateData& data);
            KTPhysicalArray< 2, double >* TransformSpectrum(const KTTimeFrequency* powerSpectrum);

            bool TransformData(KTDiscriminatedPoints2DData& data);
            KTPhysicalArray< 2, double >* TransformSetOfPoints(const SetOfPoints& points, unsigned nTimeBins, unsigned nFreqBins);


        private:
            //KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* RemoveNegativeFrequencies(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum);

            std::vector< double > fCosTheta;
            std::vector< double > fSinTheta;

            //***************
             // Signals
             //***************

         private:
             Nymph::KTSignalData fHTSignal;

             //***************
             // Slots
             //***************

         private:
             Nymph::KTSlotDataOneType< KTSparseWaterfallCandidateData > fSWFCandSlot;
             Nymph::KTSlotDataOneType< KTWaterfallCandidateData > fWFCandSlot;
             Nymph::KTSlotDataOneType< KTDiscriminatedPoints2DData > fDiscPts2DSlot;

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
