/**
 @file KTDBScanTrackClustering.hh
 @brief Contains KTDBScanTrackClustering
 @details [does something]
 @author: [name]
 @date: [date]
 */

#ifndef KTDBSCANTRACKCLUSTERING_HH_
#define KTDBSCANTRACKCLUSTERING_HH_

#include "KTPrimaryProcessor.hh"

#include "KTDBScan.hh"
#include "KTSlot.hh"
#include "KTData.hh"

#include <set>
#include <vector>

namespace Katydid
{
    class KTSliceHeader;
    class KTDiscriminatedPoints1DData;
    class KTParamNode;

    /*!
     @class KTDBScanTrackClustering
     @author N.S. Oblath

     @brief Clustering for finding tracks using the DBSCAN algorithm

     @details

     Configuration name: "dbscan-track-clustering"

     Available configuration values:
     - "epsilon": double --
     - "min-points": unsigned int --
     - "weights": array<double> --

     Slots:
     - "header": void (const KTEggHeader* header) -- [what it does]
     - "points": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTDBScanTrackClustering : public KTPrimaryProcessor
    {
        public:
            typedef KTDBScan::Point DBScanPoint;
            typedef KTDBScan::Points DBScanPoints;
            typedef KTDBScan::Weights DBScanWeights;

        public:
            KTDBScanTrackClustering(const std::string& name = "dbscan-track-clustering");
            virtual ~KTDBScanTrackClustering();

            bool Configure(const KTParamNode* node);

            double GetEpsilon() const;
            void SetEpsilon(double eps);

            unsigned GetMinPoints() const;
            void SetMinPoints(unsigned pts);

            const DBScanWeights& GetWeights() const;
            void SetWeights(const DBScanWeights& weights);
            void SetUniformWeights();
            bool GetWeightsAreUniform() const;

        private:
            KTDBScan fDBScan;

            // dimension weighting
            DBScanWeights fWeights;
            bool fWeightsAreUniform;

            /*
            void UpdateComponents();

            // eps radiuus
            // Two points are neighbors if the distance
            // between them does not exceed threshold value.
            double fEpsilon;

            //minimum number of points
            unsigned fMinPoints;
            */
        public:
            // Store point information locally
            bool TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);

            bool Run();

            bool DoClustering();

        private:
            double fTimeBinWidth;
            double fFreqBinWidth;

            std::vector< DBScanPoints > fCompPoints; // points vectors for each component

            std::set< KTDataPtr > fCandidates;
            unsigned fDataCount;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSliceHeader, KTDiscriminatedPoints1DData > fTakePointSlot;

    };

    inline double KTDBScanTrackClustering::GetEpsilon() const
    {
        return fDBScan.GetEpsilon();//fEpsilon;
    }
    inline void KTDBScanTrackClustering::SetEpsilon(double eps)
    {
        fDBScan.SetEpsilon(eps);
        //fEpsilon = eps;
        //UpdateComponents();
        return;
    }

    inline unsigned KTDBScanTrackClustering::GetMinPoints() const
    {
        return fDBScan.GetMinPoints();
        //return fMinPoints;
    }
    inline void KTDBScanTrackClustering::SetMinPoints(unsigned pts)
    {
        fDBScan.SetMinPoints(pts);
        //fMinPoints = pts;
        //UpdateComponents();
        return;
    }

    inline const KTDBScanTrackClustering::DBScanWeights& KTDBScanTrackClustering::GetWeights() const
    {
        return fWeights;
    }

    inline bool KTDBScanTrackClustering::GetWeightsAreUniform() const
    {
        return fWeightsAreUniform;
    }

}
 /* namespace Katydid */
#endif /* KTDBSCANTRACKCLUSTERING_HH_ */
