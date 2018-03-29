/**
 @file KTNormalizedFSData.hh
 @brief Contains KTNormalizedFSDataPolar and KTNormalizedFSDataFFTW
 @details
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTNORMALIZEDFSDATA_HH_
#define KTNORMALIZEDFSDATA_HH_

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPowerSpectrumData.hh"

namespace Katydid
{
    

    class KTNormalizedFSDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTNormalizedFSDataPolar >
    {
        public:
            KTNormalizedFSDataPolar();
            virtual ~KTNormalizedFSDataPolar();

            KTNormalizedFSDataPolar& SetNComponents(unsigned components);

            double GetNormalizedMean(unsigned component = 0) const;
            void SetNormalizedMean(double mean, unsigned component = 0);

            double GetNormalizedVariance(unsigned component = 0) const;
            void SetNormalizedVariance(double var, unsigned component = 0);

        public:
            struct PerComponentData
            {
                double fNormalizedMean;
                double fNormalizedVariance;
            };

            std::vector< PerComponentData > fComponentData;

            static const std::string sName;
    };

    class KTNormalizedFSDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTNormalizedFSDataFFTW >
    {
        public:
            KTNormalizedFSDataFFTW();
            virtual ~KTNormalizedFSDataFFTW();

            KTNormalizedFSDataFFTW& SetNComponents(unsigned components);

            double GetNormalizedMean(unsigned component = 0) const;
            void SetNormalizedMean(double mean, unsigned component = 0);

            double GetNormalizedVariance(unsigned component = 0) const;
            void SetNormalizedVariance(double var, unsigned component = 0);

        public:
            struct PerComponentData
            {
                double fNormalizedMean;
                double fNormalizedVariance;
            };

            std::vector< PerComponentData > fComponentData;

            static const std::string sName;
    };

    class KTNormalizedPSData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTNormalizedPSData >
    {
        public:
            KTNormalizedPSData();
            virtual ~KTNormalizedPSData();

            KTNormalizedPSData& SetNComponents(unsigned components);

            double GetNormalizedMean(unsigned component = 0) const;
            void SetNormalizedMean(double mean, unsigned component = 0);

            double GetNormalizedVariance(unsigned component = 0) const;
            void SetNormalizedVariance(double var, unsigned component = 0);

       public:
            struct PerComponentData
            {
                double fNormalizedMean;
                double fNormalizedVariance;
            };

            std::vector< PerComponentData > fComponentData;

            static const std::string sName;
    };

    inline double KTNormalizedFSDataPolar::GetNormalizedMean(unsigned component) const
    {
        return fComponentData[component].fNormalizedMean;
    }

    inline void KTNormalizedFSDataPolar::SetNormalizedMean(double mean, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedMean = mean;
    }

    inline double KTNormalizedFSDataPolar::GetNormalizedVariance(unsigned component) const
    {
        return fComponentData[component].fNormalizedVariance;
    }

    inline void KTNormalizedFSDataPolar::SetNormalizedVariance(double var, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedVariance = var;
    }



    inline double KTNormalizedFSDataFFTW::GetNormalizedMean(unsigned component) const
    {
        return fComponentData[component].fNormalizedMean;
    }

    inline void KTNormalizedFSDataFFTW::SetNormalizedMean(double mean, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedMean = mean;
    }

    inline double KTNormalizedFSDataFFTW::GetNormalizedVariance(unsigned component) const
    {
        return fComponentData[component].fNormalizedVariance;
    }

    inline void KTNormalizedFSDataFFTW::SetNormalizedVariance(double var, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedVariance = var;
    }



    inline double KTNormalizedPSData::GetNormalizedMean(unsigned component) const
    {
        return fComponentData[component].fNormalizedMean;
    }

    inline void KTNormalizedPSData::SetNormalizedMean(double mean, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedMean = mean;
    }

    inline double KTNormalizedPSData::GetNormalizedVariance(unsigned component) const
    {
        return fComponentData[component].fNormalizedVariance;
    }

    inline void KTNormalizedPSData::SetNormalizedVariance(double var, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component + 1);
        fComponentData[component].fNormalizedVariance = var;
    }


} /* namespace Katydid */
#endif /* KTNORMALIZEDFSDATA_HH_ */
