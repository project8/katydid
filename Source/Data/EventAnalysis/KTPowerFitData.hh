/*
 * KTPowerFitData.hh
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#ifndef KTPOWERFITDATA_HH
#define KTPOWERFITDATA_HH

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include <vector>
#include <map>

namespace Katydid
{
    
    class KTPowerFitData : public Nymph::KTExtensibleData< KTPowerFitData >
    {
        public:
            struct Point
            {
                double fAbscissa;
                double fOrdinate;
                double fThreshold;
                Point(double abscissa, double ordinate, double threshold) : fAbscissa(abscissa), fOrdinate(ordinate), fThreshold(threshold) {}
            };
            typedef std::map< unsigned, Point > SetOfPoints;

        public:
            KTPowerFitData();
            KTPowerFitData(const KTPowerFitData& orig);
            virtual ~KTPowerFitData();

            KTPowerFitData& operator=(const KTPowerFitData& rhs);

        public:
            // Vectors to store the fit results and uncertainties
            // Mean and Sigma are in MHz
            MEMBERVARIABLEREF(std::vector< double >, Norm);
            MEMBERVARIABLEREF(std::vector< double >, Mean);
            MEMBERVARIABLEREF(std::vector< double >, Sigma);
            MEMBERVARIABLEREF(std::vector< double >, Maximum);
            MEMBERVARIABLEREF(std::vector< double >, NormErr);
            MEMBERVARIABLEREF(std::vector< double >, MeanErr);
            MEMBERVARIABLEREF(std::vector< double >, SigmaErr);
            MEMBERVARIABLEREF(std::vector< double >, MaximumErr);

            MEMBERVARIABLE(bool, IsValid);   // did the fit converge?

            MEMBERVARIABLE(int, MainPeak);  // classifier for testing purposes
            MEMBERVARIABLE(unsigned, NPeaks);    // number of peaks in the fit

            MEMBERVARIABLEREF(SetOfPoints, SetOfPoints); // raw power spectrum after rotate-and-project
            void AddPoint(unsigned bin, const Point& point);

            // First four moments, first two in MHz
            MEMBERVARIABLE(double, Average); // adjusted for track intercept, i.e. a perfect track should give 0
            MEMBERVARIABLE(double, RMS);
            MEMBERVARIABLE(double, Skewness);
            MEMBERVARIABLE(double, Kurtosis);

            // Gaussian fit parameters of the peak closest to 0 (central peak)
            MEMBERVARIABLE(double, NormCentral);
            MEMBERVARIABLE(double, MeanCentral);
            MEMBERVARIABLE(double, SigmaCentral);
            MEMBERVARIABLE(double, MaximumCentral);
            MEMBERVARIABLE(double, RMSAwayFromCentral); // RMS in bins greater than 3 sigma from the central peak
            MEMBERVARIABLE(double, CentralPowerFraction); // Ratio of average power within 3 sigma of the central peak to average power greater than 3 sigma from the central peak
            MEMBERVARIABLE(double, TrackIntercept); // just so that we don't lose this info, since it is subtracted away
            MEMBERVARIABLE(unsigned, TrackID); // TrackID from KTProcessedTrackData

        public:
            static const std::string sName;
    };

    inline void KTPowerFitData::AddPoint(unsigned bin, const Point& point)
    {
        fSetOfPoints.insert(std::make_pair(bin, point));
    }


    //std::ostream& operator<<(std::ostream& out, const KTPowerFitData& hdr);

} /* namespace Katydid */
#endif /* KTPOWERFITDATA_HH */
