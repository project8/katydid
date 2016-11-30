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

#include <cstdio>
#include <deque>
#include <string>

class TFile;
class TTree;

namespace Katydid
{
    
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
     - "amp-dist": void (Nymph::KTDataPtr) -- Add amplitude distribution data; Requires KTData; Adds KTAmplitudeDistribution; Emits signal "amp-dist" upon successful file read.

     Signals:
     - "amp-dist": void (Nymph::KTDataPtr) -- Emitted after reading an amp-dist file; Guarantees KTAmplitudeDistribution.
    */


    class KTMultiFileROOTTreeReader : public Nymph::KTReader
    {
        private:
            typedef bool (KTMultiFileROOTTreeReader::*AppendFcn)(TTree*, Nymph::KTData&);
            struct DataType
            {
                    std::string fName;
                    std::string fTreeName;
                    AppendFcn fAppendFcn;
                    Nymph::KTSignalData* fSignal;
                    DataType(const std::string& name, const std::string& treeName, AppendFcn fcn, Nymph::KTSignalData* signal)
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

            bool Configure(const scarab::param_node* node);

            const std::deque< std::string >& GetFilenames() const;
            void AddFilename(const std::string& filename);

            const std::deque< DataType >& GetDataTypes() const;
            bool AddDataType(const std::string& type, const std::string& treeName);

        private:
            std::deque< std::string > fFilenames;
            std::deque< std::string >::const_iterator fFileIter;

            std::deque< DataType > fDataTypes;

        public:
            virtual bool Run();

            bool Append(Nymph::KTData& data);

        private:
            TFile* OpenFile(const std::string& filename) const;
            TTree* ExtractTree(TFile* file, const std::string& treeName) const;

        private:
            bool AppendAmpDistData(TTree*, Nymph::KTData& data);


            //**************
            // Signals
            //**************
        private:
            Nymph::KTSignalData fAmpDistSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //**************
            // Slots
            //**************
        private:
            Nymph::KTSlotDataOneType< Nymph::KTData > fAppendAmpDistSlot;
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
