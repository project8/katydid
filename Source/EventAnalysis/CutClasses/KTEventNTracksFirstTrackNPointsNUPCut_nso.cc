/*
 * KTEventNTracksFirstTrackNPointsNUPCut.cc
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#include "KTEventNTracksFirstTrackNPointsNUPCut_nso.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    KTLOGGER(ecnuplog, "KTEventNTracksFirstTrackNPointsNUPCut_nso");

    const std::string KTEventNTracksFirstTrackNPointsNUPCut_nso::Result::sName = "event-ntracks-first-track-npoints-nup-cut-nso";

    KT_REGISTER_CUT(KTEventNTracksFirstTrackNPointsNUPCut_nso);

    KTEventNTracksFirstTrackNPointsNUPCut_nso::KTEventNTracksFirstTrackNPointsNUPCut_nso(const std::string& name) :
                 KTCutOneArg(name),
                 fThresholds(),
                 fWideOrNarrow( WideOrNarrow::wide ),
                 fTimeOrBinAverage( TimeOrBinAvg::time )
    {}

    KTEventNTracksFirstTrackNPointsNUPCut_nso::~KTEventNTracksFirstTrackNPointsNUPCut_nso()
    {}

    bool KTEventNTracksFirstTrackNPointsNUPCut_nso::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (! node->has("parameters") || ! node->at("parameters")->is_array())
        {
            KTERROR(ecnuplog, "No cut parameters were provided, or \"parameters\" was not an array");
            return false;
        }

        const scarab::param_array* parameters = node->array_at("parameters");
        unsigned nParamSets = parameters->size();

        // Create the vector that will temporarily hold the parameter sets after the first loop
        struct extendedThresholds : thresholds
        {
            unsigned fFTNPoints;
            unsigned fNTracks;
        };
        vector< extendedThresholds > tempThresholds(nParamSets);

        // First loop: extract all parameters, and the max dimensions of the thresholds array
        //unsigned maxFTNPointsConfig = 0;
        //unsigned maxNTracksConfig = 0;
        unsigned tempThreshPos = 0;
        for (auto paramIt = parameters->begin(); paramIt != parameters->end(); ++paramIt)
        {
            if (! (*paramIt)->is_node())
            {
                KTERROR(ecnuplog, "Invalid set of parameters");
                return false;
            }
            const scarab::param_node& oneSetOfParams = (*paramIt)->as_node();

            try
            {
                unsigned ftNPoints = oneSetOfParams.get_value< unsigned >("ft-npoints");
                unsigned nTracks = oneSetOfParams.get_value< unsigned >("ntracks");
                if (ftNPoints > maxFTNPointsConfig) maxFTNPointsConfig = ftNPoints;
                if (nTracks > maxNTracksConfig) maxNTracksConfig = nTracks;

                tempThresholds[tempThreshPos].fFTNPoints = ftNPoints;
                tempThresholds[tempThreshPos].fNTracks = nTracks;

                tempThresholds[tempThreshPos].fMinTotalNUP = oneSetOfParams.get_value< double >("min-total-nup");
                tempThresholds[tempThreshPos].fMinAverageNUP = oneSetOfParams.get_value< double >("min-average-nup");
                tempThresholds[tempThreshPos].fMinMaxNUP = oneSetOfParams.get_value< double >("min-max-track-nup");
            }
            catch( scarab::error& e )
            {
                // this will catch scarab::errors from param_node::get_value in the case that a parameter is missing
                KTERROR(ecnuplog, "An incomplete set of parameters was found: " << oneSetOfParams);
                return false;
            }

            ++tempThreshPos;
        }

        // Create the 2D thresholds array
        // Dimensions are are maxNTracksConfig + 1 rows by maxFTNPointsConfig + 1 columns
        // Positions in the array for zero n-tracks and zero ft-npoints are kept to make later indexing of fThresholds simpler
        ++maxNTracksConfig;
        ++maxFTNPointsConfig;
//      KTWARN( "maxNTracksConfig " << maxNTracksConfig <<" " << "maxFTNPointsConfig " << maxFTNPointsConfig);
        fThresholds.clear();
        fThresholds.resize(maxNTracksConfig);
        // skip the first row; there will never be 0 tracks
        for (unsigned iRow = 1; iRow < maxNTracksConfig; ++iRow)
        {
            fThresholds[iRow].resize(maxFTNPointsConfig);
        }

        // Second loop: fill in the 2D array
        for (auto oneParamSet : tempThresholds)
        {
            fThresholds[oneParamSet.fNTracks][oneParamSet.fFTNPoints].fMinTotalNUP = oneParamSet.fMinTotalNUP;
            fThresholds[oneParamSet.fNTracks][oneParamSet.fFTNPoints].fMinAverageNUP = oneParamSet.fMinAverageNUP;
            fThresholds[oneParamSet.fNTracks][oneParamSet.fFTNPoints].fMinMaxNUP = oneParamSet.fMinMaxNUP;
            fThresholds[oneParamSet.fNTracks][oneParamSet.fFTNPoints].fFilled = true;
        }

        // Now fill in any gaps in rows
#ifndef NDEBUG
        unsigned iRow = 0;
#endif
        unsigned iRow = 0;
        for (auto oneRow : fThresholds)
        {
            if (oneRow.empty()) continue;

            unsigned iFilledPos = 0;
            // find the first non-zero position from the left, and fill that value to the left
            for (iFilledPos = 0; ! oneRow[iFilledPos].fFilled && iFilledPos != oneRow.size(); ++iFilledPos) {}
            KTDEBUG(ecnuplog, "Row " << iRow << ": first filled position is " << iFilledPos << " or it ranged out at " << oneRow.size());
//          KTWARN( "Row " << iRow << ": first filled position is " << iFilledPos << " or it ranged out at " << oneRow.size());
            if (iFilledPos == oneRow.size())
            {
                KTWARN(ecnuplog, "Empty threshold row found");
                continue;
            }
            for (unsigned iPos = 1; iPos < iFilledPos; ++iPos)
            {
                oneRow[iPos] = oneRow[iFilledPos];
            }

            // find the first non-zero position from the right, and fill that value to the right
            // there's no risk of finding an unfilled row, since we would have caught that in the previous section
            for (iFilledPos = oneRow.size()-1; ! oneRow[iFilledPos].fFilled; --iFilledPos) {}
            KTDEBUG(ecnuplog, "Row " << iRow << ": last filled position is " << iFilledPos);
//          KTWARN("Row " << iRow << ": last filled position is " << iFilledPos);
            for (unsigned iPos = oneRow.size()-1; iPos > iFilledPos; --iPos)
            {
                oneRow[iPos] = oneRow[iFilledPos];
            }

            // fill in any holes from left to right
            // there are no completely unfilled rows
            // rows will have a minimum size of 2
            // for a given position, if it's unfilled, then fill from the position to the left
            for (unsigned iPos = 1; iPos < oneRow.size(); ++iPos)
            {
                if (! oneRow[iPos].fFilled)
                {
                    oneRow[iPos] = oneRow[iPos-1];
                }
            }

#ifndef NDEBUG
            ++iRow;
#endif
//        	++iRow;
        }

#ifndef NDEBUG
        std::stringstream arrayStream;
        for (auto oneRow : fThresholds)
        {
            arrayStream << "[ ";
            for (auto oneParamSet : oneRow)
            {
                arrayStream << "[" << oneParamSet.fMinTotalNUP << ", " << oneParamSet.fMinAverageNUP << ", " << oneParamSet.fMinMaxNUP << "] ";
            }
            arrayStream << "]\n";
        }
        KTDEBUG(ecnuplog, "Final thresholds array:\n" << arrayStream.str());
#endif
/*        std::stringstream arrayStream;
        for (auto oneRow : fThresholds)
        {
            arrayStream << "[ ";
            for (auto oneParamSet : oneRow)
            {
                arrayStream << "[" << oneParamSet.fMinTotalNUP << ", " << oneParamSet.fMinAverageNUP << ", " << oneParamSet.fMinMaxNUP << "] ";
            }
            arrayStream << "]\n";
        }
        KTWARN("Final thresholds array:\n" << arrayStream.str());*/
        /*
    parameters:
      - ft-npoints: 3
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 13
        min-max-track-nup: 0
      - ft-npoints: 3
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 11
        min-max-track-nup: 0
      - ft-npoints: 3
        ntracks: 3
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 4
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 10
        min-max-track-nup: 0
      - ft-npoints: 4
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 8.5
        min-max-track-nup: 0
      - ft-npoints: 5
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 5
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 6
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 8.5
        min-max-track-nup: 0
      - ft-npoints: 7
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 7.3
        min-max-track-nup: 0

        To fill in the cut parameters:
        1. scan array to get dimensions
        2. create 2D array with those dimensions
        3. store offset and maximum of both dimensions
        3. fill 2D array with -1
        4. fill in values given
        5. for each row:
            1. scan horizontally from left to find first >=0 value, then fill back in to the left side
            2. scan horizontally from the right to find the first >=0 value, then fill back in to the right side
            3. check for holes in between and fill from the left

        Need to work out function to retrieve the cut parameters with appropriate bounding

         */

        if (node->has("wide-or-narrow"))
        {
            if (node->get_value("wide-or-narrow") == "wide")
            {
                SetWideOrNarrow(WideOrNarrow::wide);
            }
            else if (node->get_value("wide-or-narrow") == "narrow")
            {
                SetWideOrNarrow(WideOrNarrow::narrow);
            }
            else
            {
                KTERROR(ecnuplog, "Invalid string for fWideOrNarrow");
                return false;
            }
        }
        if (node->has("time-or-bin-average"))
        {
            if (node->get_value("time-or-bin-average") == "time")
            {
                SetTimeOrBinAverage(TimeOrBinAvg:: time);
            }
            else if (node->get_value("time-or-bin-average") == "bin")
            {
                SetTimeOrBinAverage(TimeOrBinAvg::bin);
            }
            else
            {
                KTERROR(ecnuplog, "Invalid string for fTimeOrBinAverage");
                return false;
            }
        }

        return true;
    }

    bool KTEventNTracksFirstTrackNPointsNUPCut_nso::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {        
/*      bool isCut = false;
        unsigned nTracksIndex = std::min(eventData.GetTotalEventSequences(), (unsigned)fThresholds.size());
        unsigned ftNPointsIndex = std::min(eventData.GetFirstTrackNTrackBins(), (int)fThresholds[nTracksIndex].size());

        double ftTotalNUP = eventData.GetFirstTrackTotalNUP();
        if (fWideOrNarrow == WideOrNarrow::wide)
        {
            ftTotalNUP = eventData.GetFirstTrackTotalWideNUP();
        }

        if( ftTotalNUP < fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP )
        {
            KTWARN("total_nup1"<<" "<<ftNPointsIndex<<" "<<nTracksIndex<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP);
            isCut = true;
        }
        else
        {
            double divisor = eventData.GetFirstTrackTimeLength();
            if (fTimeOrBinAverage == TimeOrBinAvg::bin)
            {
                divisor = (double)eventData.GetFirstTrackNTrackBins();
            }

            if( ftTotalNUP / divisor < fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP )
            {
                KTWARN("average_nup_time1"<<" "<<ftNPointsIndex<<" "<<nTracksIndex<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
                isCut = true;
            }
            else
            {
                if( eventData.GetFirstTrackMaxNUP() < fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP )
                {
                    KTWARN("max_nup_bin1"<<" "<<ftNPointsIndex<<" "<<nTracksIndex<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP);
                    isCut = true;
                }
            }
        }

        data.GetCutStatus().AddCutResult< KTEventNTracksFirstTrackNPointsNUPCut_nso::Result >(isCut);

        return isCut;*/
    	bool isCut = false;
    	//KTWARN( "maxNTracksConfig " << maxNTracksConfig <<" " << "maxFTNPointsConfig " << maxFTNPointsConfig);
    	//When the index is outside of the dimensions of the threshold matrix, there would still be values for thresholds since the size of the 
    	//threshold matrix is not preassigned. These values are usually really big and events got cut accordingly and somehow 
    	//cause a problem in opening root in writing mode for reasons I don't know. 
		if( eventData.GetTotalEventSequences() < maxNTracksConfig and eventData.GetFirstTrackNTrackBins() < maxFTNPointsConfig )
		{	
			//KTWARN( "maxNTracksConfig " << maxNTracksConfig <<" " << "maxFTNPointsConfig " << maxFTNPointsConfig);
//			unsigned nTracksIndex = std::min(eventData.GetTotalEventSequences(), (unsigned)fThresholds.size());
//        	unsigned ftNPointsIndex = std::min(eventData.GetFirstTrackNTrackBins(), (int)fThresholds[nTracksIndex].size());
			unsigned nTracksIndex = eventData.GetTotalEventSequences();
        	unsigned ftNPointsIndex = eventData.GetFirstTrackNTrackBins();
			if ( fWideOrNarrow == WideOrNarrow::narrow )
			{
				if( eventData.GetFirstTrackTotalNUP() < fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP )
				{
//					KTWARN("total_nup1"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP);
					isCut = true;
				}
				if ( fTimeOrBinAverage == TimeOrBinAvg:: time )
				{
					if( eventData.GetFirstTrackTotalNUP() / eventData.GetFirstTrackTimeLength() < fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP )
					{
//						KTWARN("average_nup_time1"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
						isCut = true;
					}
				}
				else
				{
					if( eventData.GetFirstTrackTotalNUP() / eventData.GetFirstTrackNTrackBins() < fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP )
					{
//						KTWARN("average_nup_bin1"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
						isCut = true;
					}
				}
			}
			else
			{
				if( eventData.GetFirstTrackTotalWideNUP() < fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP )
				{
//					KTWARN("total_nup2"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP);
					isCut = true;
				}
				if ( fTimeOrBinAverage == TimeOrBinAvg:: time )
				{
					if( eventData.GetFirstTrackTotalWideNUP() / eventData.GetFirstTrackTimeLength() < fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP )
					{
//						KTWARN("average_nup_time2"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
						isCut = true;
					}
				}
				else
				{
					if( eventData.GetFirstTrackTotalWideNUP() / eventData.GetFirstTrackNTrackBins() < fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP )
					{
//						KTWARN("average_nup_bin2"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
						isCut = true;
					}
				}
			}
			if( eventData.GetFirstTrackMaxNUP() < fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP )
			{
//				KTWARN("max_nup_bin1"<<" "<<eventData.GetFirstTrackNTrackBins()<<" "<<eventData.GetTotalEventSequences()<<" "<<fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP);
				isCut = true;
			}
		}
		data.GetCutStatus().AddCutResult< KTEventNTracksFirstTrackNPointsNUPCut_nso::Result >(isCut);

		return isCut;
    }
    
    

} // namespace Katydid
