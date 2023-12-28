/* 
 * KTHDF5TypeWriterSpectrumAnalysis.cc
 *
 *  Created on: 9/18/2014
 *      Author: J.N. Kofron
 */

#include "KTHDF5TypeWriterSpectrumAnalysis.hh"
#include "KTAmplitudeDistribution.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTKDTreeData.hh"
#include "KTHoughData.hh"
#include "logger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"


#include <sstream>
#include <string>

namespace Katydid {

	LOGGER(publog, "KTHDF5TypeWriterSpectrumAnalysis");
	static Nymph::KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterSpectrumAnalysis> sH5TWALYSrg;

	KTHDF5TypeWriterSpectrumAnalysis::KTHDF5TypeWriterSpectrumAnalysis() :
            KTHDF5TypeWriter(),
            fDiscPointBuffer(),
            fFlushIdx(0),
            fDiscPointType(NULL)
	{
        /*
         * We need to build the DiscPoint type for HDF5.
         */
        fDiscPointType = new H5::CompType(DiscPointSize);

        // Now we just insert fields.
        for (int f = 0; f < 7; ++f)
        {
            fDiscPointType->insertMember(DiscPointFieldName[f], DiscPointFieldOffset[f], DiscPointFieldType[f]);
        }
		}

	KTHDF5TypeWriterSpectrumAnalysis::~KTHDF5TypeWriterSpectrumAnalysis()
	{
		if(fDiscPointType) delete fDiscPointType;
	}

	void KTHDF5TypeWriterSpectrumAnalysis::RegisterSlots()
	{
		fWriter->RegisterSlot("disc-1d", this, &KTHDF5TypeWriterSpectrumAnalysis::WriteDiscriminatedPoints);
		fWriter->RegisterSlot("final-write-points", this, &KTHDF5TypeWriterSpectrumAnalysis::FlushDiscPointBuffer);
	}

	void KTHDF5TypeWriterSpectrumAnalysis::WriteDiscriminatedPoints(Nymph::KTDataPtr data)
	{
		KTDiscriminatedPoints1DData& fcData = data->Of<KTDiscriminatedPoints1DData>();
		KTSliceHeader& header = data->Of<KTSliceHeader>();

		if ( fWriter->OpenAndVerifyFile() == false ) return;

		DiscPoint point;
		point.fSlice = header.GetSliceNumber();
		point.fTimeInRunC = header.GetTimeInRun() + 0.5*header.GetSliceLength();
		for (point.fComponent = 0; point.fComponent < fcData.GetNComponents(); point.fComponent++)
		{
			const KTDiscriminatedPoints1DData::SetOfPoints& points = fcData.GetSetOfPoints(point.fComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                point.fBin = it->first;
                point.fAbscissa = it->second.fAbscissa;
                point.fOrdinate = it->second.fOrdinate;
                point.fThreshold = it->second.fThreshold;
                
                fDiscPointBuffer.push_back(point);
           }
		}
	}

	void KTHDF5TypeWriterSpectrumAnalysis::FlushDiscPointBuffer()
	{
	    if (fDiscPointBuffer.empty())
	    {
	        LDEBUG("DiscPoints buffer is empty; nothing written to the file");
	        return;
	    }

		LDEBUG("Writing DiscPoints buffer");

		//Create the necessary dataspace
		hsize_t* dims = new hsize_t(fDiscPointBuffer.size());
		H5::DataSpace dspace(1, dims);

		if ( !fWriter->OpenAndVerifyFile() ) return;

		// Make a group for the disciminated points
		H5::Group* swfGroup = fWriter->AddGroup("sparsewf");

		// Ok, create the dataset and write it down.
		std::stringstream namestream;
        std::string dsetname;
        namestream << "sparsewf_" << fFlushIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(swfGroup->createDataSet(dsetname.c_str(), *fDiscPointType, dspace));

        dset->write(fDiscPointBuffer.data(), *fDiscPointType);
        fDiscPointBuffer.clear();

        fFlushIdx++;
        delete dset;
		delete dims;
	}

}  // namespace
