/*
 * KTPowerFitData.hh
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#ifndef KTPOWERFITDATA_HH
#define KTPOWERFITDATA_HH

#include "KTData.hh"

#include <inttypes.h>
#include <utility>
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

        protected:
            struct PerComponentData
            {
                // Vectors to store the fit results and uncertainties
                // Mean and Sigma are in MHz
                std::vector<double> fNorm;
                std::vector<double> fMean;
                std::vector<double> fSigma;
                std::vector<double> fMaximum;

                std::vector<double> fNormErr;
                std::vector<double> fMeanErr;
                std::vector<double> fSigmaErr;
                std::vector<double> fMaximumErr;

                int fMainPeak;  // classifier for testing purposes

                SetOfPoints fPoints; // raw power spectrum after rotate-and-project

                // Classifers

                int fIsValid;   // did the fit converge?
                int fNPeaks;    // number of peaks in the fit

                // First four moments, first two in MHz
                double fAverage; // adjusted for track intercept, i.e. a perfect track should give 0
                double fRMS;
                double fSkewness;
                double fKurtosis;

                // Gaussian fit parameters of the peak closest to 0 (central peak)
                double fNormCentral;
                double fMeanCentral;
                double fSigmaCentral;
                double fMaximumCentral;

                double fRMSAwayFromCentral; // RMS in bins greater than 3 sigma from the central peak
                double fCentralPowerFraction; // Ratio of average power within 3 sigma of the central peak to average power greater than 3 sigma from the central peak

                double fTrackIntercept; // just so that we don't lose this info, since it is subtracted away

                unsigned fTrackID; // TrackID from KTProcessedTrackData
            };

        public:
            KTPowerFitData();
            KTPowerFitData(const KTPowerFitData& orig);
            virtual ~KTPowerFitData();

            KTPowerFitData& operator=(const KTPowerFitData& rhs);

        public:

            unsigned GetNComponents() const;
            KTPowerFitData& SetNComponents(unsigned channels);

            std::vector<double> GetNorm( unsigned component = 0 ) const;
            void SetNorm( std::vector<double> norm, unsigned component = 0 );

            std::vector<double> GetMean( unsigned component = 0 ) const;
            void SetMean( std::vector<double> mean, unsigned component = 0 );

            std::vector<double> GetSigma( unsigned component = 0 ) const;
            void SetSigma( std::vector<double> sigma, unsigned component = 0 );

            std::vector<double> GetMaximum( unsigned component = 0 ) const;
            void SetMaximum( std::vector<double> max, unsigned component = 0 );

            std::vector<double> GetNormErr( unsigned component = 0 ) const;
            void SetNormErr( std::vector<double> normErr, unsigned component = 0 );

            std::vector<double> GetMeanErr( unsigned component = 0 ) const;
            void SetMeanErr( std::vector<double> meanErr, unsigned component = 0 );

            std::vector<double> GetSigmaErr( unsigned component = 0 ) const;
            void SetSigmaErr( std::vector<double> sigmaErr, unsigned component = 0 );

            std::vector<double> GetMaximumErr( unsigned component = 0 ) const;
            void SetMaximumErr( std::vector<double> maxErr, unsigned component = 0 );

            int GetIsValid(unsigned component=0) const;
            void SetIsValid(int valid, unsigned component=0);

            int GetMainPeak(unsigned component=0) const;
            void SetMainPeak(int mainPeak, unsigned component=0);

            int GetNPeaks(unsigned component=0) const;
            void SetNPeaks(int n, unsigned component=0); 

            const SetOfPoints& GetSetOfPoints(unsigned component = 0) const;

            void AddPoint(unsigned bin, const Point& point, unsigned component = 0);

            double GetAverage( unsigned component = 0 ) const;
            void SetAverage( double mu1, unsigned component = 0 );

            double GetRMS( unsigned component = 0 ) const;
            void SetRMS( double mu2, unsigned component = 0 );

            double GetSkewness( unsigned component = 0 ) const;
            void SetSkewness( double mu3, unsigned component = 0 );

            double GetKurtosis( unsigned component = 0 ) const;
            void SetKurtosis( double mu4, unsigned component = 0 );

            double GetNormCentral( unsigned component = 0 ) const;
            void SetNormCentral( double a, unsigned component = 0 );

            double GetMeanCentral( unsigned component = 0 ) const;
            void SetMeanCentral( double mu, unsigned component = 0 );

            double GetSigmaCentral( unsigned component = 0 ) const;
            void SetSigmaCentral( double sigma, unsigned component = 0 );

            double GetMaximumCentral( unsigned component = 0 ) const;
            void SetMaximumCentral( double max, unsigned component = 0 );

            double GetRMSAwayFromCentral( unsigned component = 0 ) const;
            void SetRMSAwayFromCentral( double sigma, unsigned component = 0 );

            double GetCentralPowerFraction( unsigned component = 0 ) const;
            void SetCentralPowerFraction( double r, unsigned component = 0 );

            double GetTrackIntercept( unsigned component = 0 ) const;
            void SetTrackIntercept( double alpha, unsigned component = 0 );

            unsigned GetTrackID( unsigned component = 0 ) const;
            void SetTrackID(unsigned trackID, unsigned component = 0);

        private:
            std::vector< PerComponentData > fComponentData;


        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTPowerFitData& hdr);

    inline unsigned KTPowerFitData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline KTPowerFitData& KTPowerFitData::SetNComponents(unsigned channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline std::vector<double> KTPowerFitData::GetNorm(unsigned component) const
    {
        return fComponentData[component].fNorm;
    }

    inline void KTPowerFitData::SetNorm(std::vector<double> norm, unsigned component)
    {
        fComponentData[component].fNorm = norm;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetMean(unsigned component) const
    {
        return fComponentData[component].fMean;
    }

    inline void KTPowerFitData::SetMean(std::vector<double> mean, unsigned component)
    {
        fComponentData[component].fMean = mean;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetSigma(unsigned component) const
    {
        return fComponentData[component].fSigma;
    }

    inline void KTPowerFitData::SetSigma(std::vector<double> sigma, unsigned component)
    {
        fComponentData[component].fSigma = sigma;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetMaximum(unsigned component) const
    {
        return fComponentData[component].fMaximum;
    }

    inline void KTPowerFitData::SetMaximum(std::vector<double> max, unsigned component)
    {
        fComponentData[component].fMaximum = max;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetNormErr(unsigned component) const
    {
        return fComponentData[component].fNormErr;
    }

    inline void KTPowerFitData::SetNormErr(std::vector<double> normErr, unsigned component)
    {
        fComponentData[component].fNormErr = normErr;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetMeanErr(unsigned component) const
    {
        return fComponentData[component].fMeanErr;
    }

    inline void KTPowerFitData::SetMeanErr(std::vector<double> meanErr, unsigned component)
    {
        fComponentData[component].fMeanErr = meanErr;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetSigmaErr(unsigned component) const
    {
        return fComponentData[component].fSigmaErr;
    }

    inline void KTPowerFitData::SetSigmaErr(std::vector<double> sigmaErr, unsigned component)
    {
        fComponentData[component].fSigmaErr = sigmaErr;
        return;
    }

    inline std::vector<double> KTPowerFitData::GetMaximumErr(unsigned component) const
    {
        return fComponentData[component].fMaximumErr;
    }

    inline void KTPowerFitData::SetMaximumErr(std::vector<double> maxErr, unsigned component)
    {
        fComponentData[component].fMaximumErr = maxErr;
        return;
    }

    inline int KTPowerFitData::GetIsValid(unsigned component) const
    {
        return fComponentData[component].fIsValid;
    }

    inline void KTPowerFitData::SetIsValid(int valid, unsigned component)
    {
        fComponentData[component].fIsValid = valid;
        return;
    }

    inline int KTPowerFitData::GetMainPeak(unsigned component) const
    {
        return fComponentData[component].fMainPeak;
    }

    inline void KTPowerFitData::SetMainPeak(int mainPeak, unsigned component)
    {
        fComponentData[component].fMainPeak = mainPeak;
        return;
    }

    inline int KTPowerFitData::GetNPeaks(unsigned component) const
    {
        return fComponentData[component].fNPeaks;
    }

    inline void KTPowerFitData::SetNPeaks(int n, unsigned component)
    {
        fComponentData[component].fNPeaks = n;
        return;
    }

    inline const KTPowerFitData::SetOfPoints& KTPowerFitData::GetSetOfPoints(unsigned component) const
    {
         return fComponentData[component].fPoints;
    }

    inline void KTPowerFitData::AddPoint(unsigned bin, const Point& point, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(bin, point));
    }

    inline double KTPowerFitData::GetAverage(unsigned component) const
    {
        return fComponentData[component].fAverage;
    }

    inline void KTPowerFitData::SetAverage(double mu1, unsigned component)
    {
        fComponentData[component].fAverage = mu1;
        return;
    }

    inline double KTPowerFitData::GetRMS(unsigned component) const
    {
        return fComponentData[component].fRMS;
    }

    inline void KTPowerFitData::SetRMS(double mu2, unsigned component)
    {
        fComponentData[component].fRMS = mu2;
        return;
    }

    inline double KTPowerFitData::GetSkewness(unsigned component) const
    {
        return fComponentData[component].fSkewness;
    }

    inline void KTPowerFitData::SetSkewness(double mu3, unsigned component)
    {
        fComponentData[component].fSkewness = mu3;
        return;
    }

    inline double KTPowerFitData::GetKurtosis(unsigned component) const
    {
        return fComponentData[component].fKurtosis;
    }

    inline void KTPowerFitData::SetKurtosis(double mu4, unsigned component)
    {
        fComponentData[component].fKurtosis = mu4;
        return;
    }

    inline double KTPowerFitData::GetNormCentral(unsigned component) const
    {
        return fComponentData[component].fNormCentral;
    }

    inline void KTPowerFitData::SetNormCentral(double a, unsigned component)
    {
        fComponentData[component].fNormCentral = a;
        return;
    }

    inline double KTPowerFitData::GetMeanCentral(unsigned component) const
    {
        return fComponentData[component].fMeanCentral;
    }

    inline void KTPowerFitData::SetMeanCentral(double mu, unsigned component)
    {
        fComponentData[component].fMeanCentral = mu;
        return;
    }

    inline double KTPowerFitData::GetSigmaCentral(unsigned component) const
    {
        return fComponentData[component].fSigmaCentral;
    }

    inline void KTPowerFitData::SetSigmaCentral(double sigma, unsigned component)
    {
        fComponentData[component].fSigmaCentral = sigma;
        return;
    }

    inline double KTPowerFitData::GetMaximumCentral(unsigned component) const
    {
        return fComponentData[component].fMaximumCentral;
    }

    inline void KTPowerFitData::SetMaximumCentral(double max, unsigned component)
    {
        fComponentData[component].fMaximumCentral = max;
        return;
    }

    inline double KTPowerFitData::GetRMSAwayFromCentral(unsigned component) const
    {
        return fComponentData[component].fRMSAwayFromCentral;
    }

    inline void KTPowerFitData::SetRMSAwayFromCentral(double sigma, unsigned component)
    {
        fComponentData[component].fRMSAwayFromCentral = sigma;
        return;
    }

    inline double KTPowerFitData::GetCentralPowerFraction(unsigned component) const
    {
        return fComponentData[component].fCentralPowerFraction;
    }

    inline void KTPowerFitData::SetCentralPowerFraction(double r, unsigned component)
    {
        fComponentData[component].fCentralPowerFraction = r;
        return;
    }

    inline double KTPowerFitData::GetTrackIntercept(unsigned component) const
    {
        return fComponentData[component].fTrackIntercept;
    }

    inline void KTPowerFitData::SetTrackIntercept(double alpha, unsigned component)
    {
        fComponentData[component].fTrackIntercept = alpha;
        return;
    }

    inline unsigned KTPowerFitData::GetTrackID(unsigned component) const
    {
        return fComponentData[component].fTrackID;
    }

    inline void KTPowerFitData::SetTrackID(unsigned trackID, unsigned component)
    {
        fComponentData[component].fTrackID = trackID;
        return;
    }


} /* namespace Katydid */
#endif /* KTPOWERFITDATA_HH */
