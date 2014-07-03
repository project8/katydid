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
     - "radius": double --
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

            const static unsigned fNDimensions;

        public:
            KTDBScanTrackClustering(const std::string& name = "dbscan-track-clustering");
            virtual ~KTDBScanTrackClustering();

            bool Configure(const KTParamNode* node);

            unsigned GetMinPoints() const;
            void SetMinPoints(unsigned pts);

            const DBScanWeights& GetRadii() const;
            void SetRadii(const DBScanWeights& radii);

        private:
            KTDBScan fDBScan;

            // dimension weighting
            DBScanWeights fRadii;

            /*
            void UpdateComponents();
            */

            // radius
            // Two points are neighbors if the distance
            // between them does not exceed threshold value.
            //double fRadius;

            //minimum number of points
            unsigned fMinPoints;

        public:
            // Store point information locally
            bool TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints);
            bool TakePoint(double time, double frequency /*, double amplitude*/, unsigned component=0);

            void SetNComponents(unsigned nComps);
            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            bool Run();

            bool DoClustering();

            const std::set< KTDataPtr >& GetCandidates() const;
            unsigned GetDataCount() const;

        private:

            double fTimeBinWidth;
            double fFreqBinWidth;

            std::vector< DBScanPoints > fCompPoints; // points vectors for each component
            //std::vector< DBScanPoint > fMaxes;
            //std::vector< DBScanPoint > fMins;

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
    /*
    inline double KTDBScanTrackClustering::GetRadius() const
    {
        //return fDBScan.GetRadius();
        return fRadius;
    }
    inline void KTDBScanTrackClustering::SetRadius(double radius)
    {
        //fDBScan.SetRadius(radius);
        fRadius = radius;
        //UpdateComponents();
        return;
    }
    */
    inline unsigned KTDBScanTrackClustering::GetMinPoints() const
    {
        //return fDBScan.GetMinPoints();
        return fMinPoints;
    }
    inline void KTDBScanTrackClustering::SetMinPoints(unsigned pts)
    {
        //fDBScan.SetMinPoints(pts);
        fMinPoints = pts;
        //UpdateComponents();
        return;
    }

    inline const KTDBScanTrackClustering::DBScanWeights& KTDBScanTrackClustering::GetRadii() const
    {
        return fRadii;
    }

    inline void KTDBScanTrackClustering::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }
    inline void KTDBScanTrackClustering::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline const std::set< KTDataPtr >& KTDBScanTrackClustering::GetCandidates() const
    {
        return fCandidates;
    }
    inline unsigned KTDBScanTrackClustering::GetDataCount() const
    {
        return fDataCount;
    }


}
 /* namespace Katydid */
#endif /* KTDBSCANTRACKCLUSTERING_HH_ */
