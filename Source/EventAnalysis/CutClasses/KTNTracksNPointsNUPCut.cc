/*
 * KTNTracksNPointsNUPCut.cc
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#include "KTNTracksNPointsNUPCut.hh"

#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    KTLOGGER(ecnuplog, "KTNTracksNPointsNUPCut");

    const std::string KTNTracksNPointsNUPCut::Result::sName = "ntracks-npoints-nup-cut";

    KT_REGISTER_CUT(KTNTracksNPointsNUPCut);

    KTNTracksNPointsNUPCut::KTNTracksNPointsNUPCut(const std::string& name) :
             KTCutOneArg(name),
             fThresholds(),
             fDefaultThresholds(),
             fWideOrNarrow( WideOrNarrow::wide ),
             fTimeOrBinAverage( TimeOrBinAvg::time )
    {
        fDefaultThresholds.fMinAverageNUP = 0.;
        fDefaultThresholds.fMinTotalNUP = 0.;
        fDefaultThresholds.fMinMaxNUP = 0.;
        fDefaultThresholds.fFilled = true;
    }

    KTNTracksNPointsNUPCut::~KTNTracksNPointsNUPCut()
    {}

    bool KTNTracksNPointsNUPCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (! node->has("parameters") || ! node->at("parameters")->is_array())
        {
            KTERROR(ecnuplog, "No cut parameters were provided, or \"parameters\" was not an array");
            return false;
        }

        const scarab::param_node* defaults = node->node_at("default-parameters");
        if (defaults != nullptr)
        {
            fDefaultThresholds.fMinAverageNUP = defaults->get_value("min-average-nup", fDefaultThresholds.fMinAverageNUP);
            fDefaultThresholds.fMinTotalNUP = defaults->get_value("min-total-nup", fDefaultThresholds.fMinTotalNUP);
            fDefaultThresholds.fMinMaxNUP = defaults->get_value("min-max-nup", fDefaultThresholds.fMinMaxNUP);
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
        unsigned maxFTNPointsConfig = 0;
        unsigned maxNTracksConfig = 0;
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
                tempThresholds[tempThreshPos].fMinMaxNUP = oneSetOfParams.get_value< double >("min-max-nup");
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
        // Dimensions are are maxNTracksConfig + 2 rows by maxFTNPointsConfig + 2 columns so that we fill the thresholds at the edges with the default values
        // Positions in the array for zero n-tracks and zero ft-npoints are kept to make later indexing of fThresholds simpler
        maxNTracksConfig += 2;
        maxFTNPointsConfig += 2;
        KTDEBUG(ecnuplog, "maxNTracksConfig = " << maxNTracksConfig << "  " << "maxFTNPointsConfig = " << maxFTNPointsConfig);
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

#ifndef NDEBUG
        {   // print threshold before filling
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
            KTDEBUG(ecnuplog, "Thresholds prior to filling:\n" << arrayStream.str());
        }
#endif

        // Now fill in any gaps in rows with the default thresholds
        for (auto oneRowPtr = fThresholds.begin(); oneRowPtr != fThresholds.end(); ++oneRowPtr)
        {
            if (oneRowPtr->empty()) continue; // this should only be the case for the first row in the 2D array

            for (auto oneThreshPtr = oneRowPtr->begin(); oneThreshPtr != oneRowPtr->end(); ++oneThreshPtr)
            {
                if (! oneThreshPtr->fFilled)
                {
                    (*oneThreshPtr) = fDefaultThresholds;
                }
            }

        }

#ifndef NDEBUG
        {   // print thresholds before filling
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
            KTDEBUG(ecnuplog, "Thresholds prior to filling:\n" << arrayStream.str());
        }
#endif

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

    bool KTNTracksNPointsNUPCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {        
        bool isCut = false;
        unsigned nTracksIndex = std::min(eventData.GetTotalEventSequences(), (unsigned)fThresholds.size() - 1);
        unsigned ftNPointsIndex = std::min(eventData.GetFirstTrackNTrackBins(), (int)fThresholds[nTracksIndex].size() - 1);

        KTDEBUG(ecnuplog, "Applying n-tracks/n-points/nup cut; (" << eventData.GetTotalEventSequences() << ", " << eventData.GetFirstTrackNTrackBins() << ")");
        KTDEBUG(ecnuplog, "Using indices: (" << nTracksIndex << ", " << ftNPointsIndex << ")");
        KTDEBUG(ecnuplog, "Cut thresholds: " << fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP << ", " << fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP << ", " << fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP)

        double ftTotalNUP = eventData.GetFirstTrackTotalNUP();
        if (fWideOrNarrow == WideOrNarrow::wide)
        {
            ftTotalNUP = eventData.GetFirstTrackTotalWideNUP();
        }

        if( ftTotalNUP < fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP )
        {
            KTDEBUG(ecnuplog, "Event is cut based on total NUP: " << ftTotalNUP << " < " << fThresholds[nTracksIndex][ftNPointsIndex].fMinTotalNUP);
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
                KTDEBUG(ecnuplog, "Event is cut based on average NUP: " << ftTotalNUP / divisor << " < " <<fThresholds[nTracksIndex][ftNPointsIndex].fMinAverageNUP);
                isCut = true;
            }
            else
            {
                if( eventData.GetFirstTrackMaxNUP() < fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP )
                {
                    KTDEBUG(ecnuplog, "Event is cut based on max NUP: " << eventData.GetFirstTrackMaxNUP() << " < " << fThresholds[nTracksIndex][ftNPointsIndex].fMinMaxNUP);
                    isCut = true;
                }
            }
        }

        data.GetCutStatus().AddCutResult< KTNTracksNPointsNUPCut::Result >(isCut);

        return isCut;
    }



} // namespace Katydid
