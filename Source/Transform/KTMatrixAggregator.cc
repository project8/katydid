/*
 * KTMultiChannelInnerProduct.cc
 *
 *  Created on: Apr 28, 2021
 *      Author: F. Thomas
 */

#include "KTMatrixAggregator.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "KTDemangle.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(magglog, "KTMatrixAggregator");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTMatrixAggregator, "matrix-aggregator");

    KTMatrixAggregator::KTMatrixAggregator(const std::string& name) :
            KTProcessor{name},
			fMaxCols{1},
			fSignalCount{0},
			fNRows{0},
			fBufferMat{},
            fMatrixSignal{"matrix", this}
			//why does this not work?
            //fTSSlot("ts-fftw", this, &KTMatrixAggregator::SlotFunction, &fMatrixSignal)
    {
		RegisterSlot("ts-fftw", this, &KTMatrixAggregator::SlotFunction);
    }

    bool KTMatrixAggregator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        fMaxCols = node->get_value< unsigned >("max-cols", fMaxCols);

        return true;
    }

    void KTMatrixAggregator::ShrinkMatrix()
    {
    	fBufferMat.conservativeResize(Eigen::NoChange_t{}, fSignalCount);
    }

    bool KTMatrixAggregator::AddCol(KTTimeSeriesData& tsData)
    {
    	const KTTimeSeriesFFTW* ts = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(0));
        //KTTimeSeries* ts = tsData.GetTimeSeries(0);
        
        unsigned nComponents = tsData.GetNComponents();
        unsigned nTimebins = ts->GetNTimeBins();
        unsigned rows = nComponents * nTimebins;
        
        if(fBufferMat.rows()== 0)
        {
            fBufferMat.resize(rows, fMaxCols);
        }
        
        if(fBufferMat.rows() != rows)
        {
            KTERROR(magglog, "The input slice has not the same number of timebins or components as the first.");
            return false;
        }

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            ts = dynamic_cast< const KTTimeSeriesFFTW* >(tsData.GetTimeSeries(iComponent));

            if (ts->GetNTimeBins() != nTimebins)
            {
                KTERROR(magglog, "TS " << iComponent << " has not the same length as TS 0.");
                return false;
            }
            
            fBufferMat.block(nTimebins*iComponent, fSignalCount, nTimebins, 1) = ts->GetData();
            KTDEBUG(magglog, "Added TS " << iComponent << " to the matrix.");
            
        }


        return true;
    }

    void KTMatrixAggregator::SlotFunction(Nymph::KTDataPtr data)
    {

        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(magglog, "Data not found with type <" << DemangledName(typeid(KTTimeSeriesData)) << ">");
            return;
        }

        // Call the function
        if (! AddCol(data->Of< KTTimeSeriesData >()))
        {
            KTERROR(magglog, "Could not add TS to matrix");
            return;
        }
        // Increase signal counter
        fSignalCount++;
        bool emitSignal = false;
		if (fSignalCount == fMaxCols)
		{
			fSignalCount = 0;
			KTINFO(magglog, "Completed the matrix.");
			emitSignal = true;
		}

		if (data->GetLastData())
		{
			KTDEBUG(magglog, "Emitting last-data signal");
			//shrink the matrix to current signal count if end of data is reached

			ShrinkMatrix();
			KTINFO(magglog, "Completed the matrix.");
			emitSignal = true;
		}

		if(emitSignal) {
			KTAggregatedTSMatrixData& aggMatrix = data->Of< KTAggregatedTSMatrixData >();
			 //adjust labels and KTAxis things?
			aggMatrix.GetData() = std::move(fBufferMat);
			fMatrixSignal(data);
			fBufferMat = Eigen::ArrayXcd();

		}

        return;
    }

} /* namespace Katydid */
