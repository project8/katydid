/**
 @file KTOfficialCandidatesWriter.hh
 @brief Contains KTOfficialCandidatesWriter
 @details 
 @author: N. S. Oblath
 @date: Apr 23, 2013
*/

#ifndef KTOFFICIALCANDIDATESWRITER_HH_
#define KTOFFICIALCANDIDATESWRITER_HH_

#include "KTWriter.hh"

#include "KTJSONMaker.hh"
#include "KTSlot.hh"

#include <boost/thread.hpp>

#include <cstdio>


namespace Katydid
{
    using namespace Nymph;
    class KTEggHeader;
    class KTWaterfallCandidateData;
    class KTProcSummary;

    /*!
     @class KTOfficialCandidatesWriter
     @author N. S. Oblath

     @brief JSON file writer

     @details
     This class is thread-safe.
     It can be written to from different threads, and uses a scoped lock to prevent multiple threads from writing to it at the same time.

     If you use the "summary-after-stop" and "stop-close-after-summary" slots, you can have the summary signal come from a different thread than the stop signal.
     These signals can be receive asynchronously.  It's assumed that the "summary-after-stop" signal will come from the slice-producing thread (e.g. from the egg processor),
     and the "stop-close-after-summary" signal will come from the asynchronous thread. The writer will wait for the "stop-..." signal to indicate that no more candidates
     will be received.  Once the "summary-..." signal is received, the summary will be written and the json file closed.  If the "summary-..." signal arrives first,
     the writer will wait for the "stop-..." signal.

     Configuration name: "official-candidate-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "pretty-json": bool -- if true, prints a human-readable file
     - "file-mode": string -- cstdio FILE mode: w, a, r+, w+ (default) or a+

     Slots:
     - "header": void (KTEggHeader*) -- writes the header information to the candidates file; not valid if candidate writing has started
     - "waterfall-candidate": void (KTDataPtr) -- writes candidate information; starts candidate writing mode if it hasn't started yet
     - "summary": void (const KTProcSummary*) -- stops writing candidates, writes the summary information and closes the file
     - "stop": void () -- stops writing candidates and closes the file
     - "summary-after-stop": void (const KTProcSummary*) -- copies the summary information and waits until the "stop-close-after-summary" signal is received to write the summary and close the file.
     - "stop-close-after-summary": void () -- stops writing candidates and waits until the "summary-after-stop" signal is received to write the summary and close the file.
    */

    class KTOfficialCandidatesWriter : public KTWriter
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

            enum Status
            {
                kNotOpenedYet,
                kPriorToCandidates,
                kWritingCandidates,
                kStopped
            };

            typedef boost::recursive_mutex::scoped_lock ScopedLock;

        public:
            KTOfficialCandidatesWriter(const std::string& name = "official-candidate-writer");
            virtual ~KTOfficialCandidatesWriter();

            bool Configure(const KTParamNode* node);

        public:
            bool OpenFile();
            void CloseFile();

            bool OpenAndVerifyFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            bool GetPrettyJSONFlag() const;
            void SetPrettyJSONFlag(bool flag);

            KTJSONMaker< rapidjson::FileStream >* GetJSONMaker() const;

            Status GetStatus() const;

        protected:
            std::string fFilename;
            std::string fFileMode;

            bool fPrettyJSONFlag;

            FILE* fFile;
            rapidjson::FileStream* fFileStream;
            KTJSONMaker< rapidjson::FileStream >* fJSONMaker;

            Status fStatus;

            KTProcSummary* fSummaryCopy;
            bool fWaitingForSummary;

            mutable boost::recursive_mutex fMutex;

        public:
            void WriteHeaderInformation(KTEggHeader* header);

            bool WriteWaterfallCandidate(KTWaterfallCandidateData& wcData);

            void WriteSummaryInformation(const KTProcSummary* summary);
            void WriteSummaryInformationAndCloseFile(const KTProcSummary* summary);

            void CopySummaryInformationAndWaitForStop(const KTProcSummary* summary);
            void StopCandidatesAndWaitForSummary();

        private:
            void EndCandidates();

            //**************
            // Slots
            //**************
        private:
            KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTWaterfallCandidateData > fWaterfallCandidateSlot;
            KTSlotNoArg< void () > fStopWritingSlot;
            KTSlotOneArg< void (const KTProcSummary*) > fSummarySlot;
            KTSlotNoArg< void () > fStopCloseAfterSummarySlot;
            KTSlotOneArg< void (const KTProcSummary*) > fSummaryAfterStopSlot;
    };

    inline const std::string& KTOfficialCandidatesWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTOfficialCandidatesWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTOfficialCandidatesWriter::GetFileMode() const
    {
        return fFileMode;
    }
    inline void KTOfficialCandidatesWriter::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline bool KTOfficialCandidatesWriter::GetPrettyJSONFlag() const
    {
        return fPrettyJSONFlag;
    }
    inline void KTOfficialCandidatesWriter::SetPrettyJSONFlag(bool flag)
    {
        fPrettyJSONFlag = flag;
        return;
    }

    inline KTJSONMaker< rapidjson::FileStream >* KTOfficialCandidatesWriter::GetJSONMaker() const
    {
        return fJSONMaker;
    }

    inline KTOfficialCandidatesWriter::Status KTOfficialCandidatesWriter::GetStatus() const
    {
        return fStatus;
    }

} /* namespace Katydid */
#endif /* KTOFFICIALCANDIDATESWRITER_HH_ */
