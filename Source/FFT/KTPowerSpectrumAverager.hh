/*
 * KTKTPowerSpectrumAverager.hh
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUMAVERAGER_HH_
#define KTPOWERSPECTRUMAVERAGER_HH_

#include "KTProcessor.hh"

#include "KTEvent.hh"

#include <boost/shared_ptr.hpp>

#include <string>

class TH1D;

namespace Katydid
{
    class KTEggHeader;
    class KTPStoreNode;

    class KTPowerSpectrumAverager : public KTProcessor
    {

        public:
            KTPowerSpectrumAverager();
            virtual ~KTPowerSpectrumAverager();

            Bool_t Configure(const KTPStoreNode* node);

            void ProcessHeader(const KTEggHeader* header);

            void ProcessEvent(boost::shared_ptr<KTEvent> event);

            void Finish();

            const std::string& GetOutputFilePath() const;
            void SetOutputFilePath(const std::string& path);

            const std::string& GetOutputFilenameBase() const;
            void SetOutputFilenameBase(const std::string& fname);

            const std::string& GetOutputFileType() const;
            void SetOutputFileType(const std::string& type);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

        private:
            std::string fOutputFilePath;
            std::string fOutputFilenameBase;
            std::string fOutputFileType;
            std::string fInputDataName;

            Bool_t fStartNewHistFlag;
            std::vector< TH1D* > fAveragePSHists;
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

} /* namespace Katydid */
#endif /* KTPOWERSPECTRUMAVERAGER_HH_ */
