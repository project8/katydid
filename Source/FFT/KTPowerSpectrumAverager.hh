/*
 * KTKTPowerSpectrumAverager.hh
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUMAVERAGER_HH_
#define KTPOWERSPECTRUMAVERAGER_HH_

#include "KTProcessor.hh"

#include "KTBundle.hh"

#include "TH1.h"

#include <boost/shared_ptr.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace Katydid
{
    class KTEggHeader;
    class KTPStoreNode;
    class KTFrequencySpectrumData;
    class KTFrequencySpectrumDataFFTW;

    class KTPowerSpectrumAverager : public KTProcessor
    {

        public:
            KTPowerSpectrumAverager();
            virtual ~KTPowerSpectrumAverager();

            Bool_t Configure(const KTPStoreNode* node);

            void SetToStartNewHistogram();

            void AddFrequencySpectrumData(const KTFrequencySpectrumData* data);

//            void AddCorrelationData(const KTCorrelationData* data);

            void AddFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* data);

            void CreateHistograms();

            const std::string& GetOutputFilePath() const;
            void SetOutputFilePath(const std::string& path);

            const std::string& GetOutputFilenameBase() const;
            void SetOutputFilenameBase(const std::string& fname);

            const std::string& GetOutputFileType() const;
            void SetOutputFileType(const std::string& type);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

        private:
            template< class FSDataType >
            void AddFSData(const FSDataType* data);

            std::string fOutputFilePath;
            std::string fOutputFilenameBase;
            std::string fOutputFileType;
            std::string fInputDataName;

            Bool_t fStartNewHistFlag;
            std::vector< TH1D* > fAveragePSHists;

        public:
            //************
            // Slots
            //************
            void ProcessHeader(const KTEggHeader* header);

            void ProcessEvent(boost::shared_ptr<KTBundle> bundle);

            void ProcessFrequencySpectrumData(const KTFrequencySpectrumData* data);

            void ProcessFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data);
            
 //           void ProcessCorrelationData(const KTCorrelationData* data);

            void Finish();

    };

    inline const std::string& KTPowerSpectrumAverager::GetOutputFilePath() const
    {
        return fOutputFilePath;
    }

    inline void KTPowerSpectrumAverager::SetOutputFilePath(const std::string& path)
    {
        fOutputFilePath = path;
        return;
    }

    inline const std::string& KTPowerSpectrumAverager::GetOutputFilenameBase() const
    {
        return fOutputFilenameBase;
    }

    inline void KTPowerSpectrumAverager::SetOutputFilenameBase(const std::string& fname)
    {
        fOutputFilenameBase = fname;
        return;
    }

    inline const std::string& KTPowerSpectrumAverager::GetOutputFileType() const
    {
        return fOutputFileType;
    }

    inline void KTPowerSpectrumAverager::SetOutputFileType(const std::string& type)
    {
        fOutputFileType = type;
        return;
    }

    inline const std::string& KTPowerSpectrumAverager::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTPowerSpectrumAverager::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    template< class FSDataType >
    void KTPowerSpectrumAverager::AddFSData(const FSDataType* data)
    {
        if (fStartNewHistFlag)
        {
            fStartNewHistFlag = false;

            for (std::vector<TH1D*>::iterator it=fAveragePSHists.begin(); it != fAveragePSHists.end(); it++)
            {
                delete *it;
            }
            fAveragePSHists.clear();
            if (fAveragePSHists.size() != data->GetNChannels())
                fAveragePSHists.resize(data->GetNChannels());

            std::string histNameBase("PowerSpectrum");
            for (UInt_t iChannel=0; iChannel < data->GetNChannels(); iChannel++)
            {
                std::stringstream conv;
                conv << iChannel;
                std::string histName = histNameBase + conv.str();
                TH1D* newPS = data->GetSpectrum(iChannel)->CreatePowerHistogram(histName);
                fAveragePSHists[iChannel] = newPS;
            }
        }
        else
        {
            for (UInt_t iChannel=0; iChannel < data->GetNChannels(); iChannel++)
            {
                TH1D* newPS = data->GetSpectrum(iChannel)->CreatePowerHistogram();
                fAveragePSHists[iChannel]->Add(newPS);
                delete newPS;
            }
        }
        return;
    }


} /* namespace Katydid */
#endif /* KTPOWERSPECTRUMAVERAGER_HH_ */
