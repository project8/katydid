/**
 @file KTMultiFileROOTTreeReader.hh
 @brief Contains KTMultiFileROOTTreeReader
 @details 
 @author: N. S. Oblath
 @date: May 31, 2013
*/

#ifndef KTMULTIFILEROOTTREEREADER_HH_
#define KTMULTIFILEROOTTREEREADER_HH_

#include "KTReader.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <cstdio>
#include <deque>
#include <string>

class TFile;
class TTree;

namespace Katydid
{
    class KTData;

    /*!
     @class KTMultiFileROOTTreeReader
     @author N. S. Oblath

     @brief ROOT file reader for reading multiple files, extracting data from TTrees, and emitting a signal for each

     @details
     Multiple data-types can be read from each file by specifying multiple run-data-types.

     Configuration name: "mf-root-tree-reader"

     Available configuration values:
     - "input-file": string -- input filename
     - "data-type": nested config -- may be repeated; each type should only be included once
       - "type": string -- the type of file being read. See options below.
       - "tree-name": string -- name of the tree to be extracted

     The run-data-type option determines the function used to read the file.
     The available options are:
     - "amp-dist" -- Emits signal "amp-dist" after file read

     Slots:
     - "amp-dist": void (shared_ptr<KTData>) -- Add amplitude distribution data; Requires KTData; Adds KTAmplitudeDistribution; Emits signal "amp-dist" upon successful file read.

     Signals:
     - "amp-dist": void (shared_ptr<KTData>) -- Emitted after reading an amp-dist file; Guarantees KTAmplitudeDistribution.
    */


    class KTMultiFileROOTTreeReader : public KTReader
    {
        private:
            typedef Bool_t (KTMultiFileROOTTreeReader::*AppendFcn)(TTree*, KTData&);
            struct DataType
            {
                    std::string fName;
                    std::string fTreeName;
                    AppendFcn fAppendFcn;
                    KTSignalData* fSignal;
                    DataType(const std::string& name, const std::string& treeName, AppendFcn fcn, KTSignalData* signal)
                    {
                        fName = name;
                        fTreeName = treeName;
                        fAppendFcn = fcn;
                        fSignal = signal;
                    }
            };

        public:
            KTMultiFileROOTTreeReader(const std::string& name = "mf-root-tree-reader");
            virtual ~KTMultiFileROOTTreeReader();

            Bool_t Configure(const KTPStoreNode* node);

            const std::deque< std::string >& GetFilenames() const;
            void AddFilename(const std::string& filename);

            const std::deque< DataType >& GetDataTypes() const;
            Bool_t AddDataType(const std::string& type, const std::string& treeName);

        private:
            std::deque< std::string > fFilenames;
            std::deque< std::string >::const_iterator fFileIter;

            std::deque< DataType > fDataTypes;

        public:
            virtual Bool_t Run();

            Bool_t Append(KTData& data);

        private:
            TFile* OpenFile(const std::string& filename) const;
            TTree* ExtractTree(TFile* file, const std::string& treeName) const;

        private:
            Bool_t AppendAmpDistData(TTree*, KTData& data);


            //**************
            // Signals
            //**************
        private:
            KTSignalData fAmpDistSignal;
            KTSignalOneArg< void > fDoneSignal;

            //**************
            // Slots
            //**************
        private:
            KTSlotDataOneType< KTData > fAppendAmpDistSlot;
    };

    inline const std::deque< std::string >& KTMultiFileROOTTreeReader::GetFilenames() const
    {
        return fFilenames;
    }

    inline void KTMultiFileROOTTreeReader::AddFilename(const std::string& filename)
    {
        fFilenames.push_back(filename);
        fFileIter = fFilenames.begin();
        return;
    }

    inline const std::deque< KTMultiFileROOTTreeReader::DataType >& KTMultiFileROOTTreeReader::GetDataTypes() const
    {
        return fDataTypes;
    }


} /* namespace Katydid */
#endif /* KTMULTIFILEROOTTREEREADER_HH_ */
