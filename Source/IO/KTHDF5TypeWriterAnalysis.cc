/* 
 * KTHDF5TypeWriterAnalysis.cc
 *
 *  Created on: 9/18/2014
 *      Author: J.N. Kofron
 */

#include "KTHDF5TypeWriterAnalysis.hh"
#include "KTAmplitudeDistribution.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTKDTreeData.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"


#include <sstream>
#include <string>

namespace Katydid {

	KTLOGGER(publog, "KTHDF5TypeWriterAnalysis");
	static KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterAnalysis> sH5TWALYSrg;

	KTHDF5TypeWriterAnalysis::KTHDF5TypeWriterAnalysis() :
		KTHDF5TypeWriter(),
		fDiscPointBuffer(),
		fFlushIdx(0),
		fDiscPointType(NULL) {
			/*
			 * We need to build the DiscPoint type for HDF5.
			 */
			this->fDiscPointType = new H5::CompType(DiscPointSize);

			// Now we just insert fields.
			for (int f = 0; f < 7; f++) {
				this->fDiscPointType->insertMember(
					DiscPointFieldName[f],
					DiscPointFieldOffset[f],
					DiscPointFieldType[f]);
			}
		}

	KTHDF5TypeWriterAnalysis::~KTHDF5TypeWriterAnalysis() {
		if(fDiscPointType) delete fDiscPointType;
	}

	void KTHDF5TypeWriterAnalysis::RegisterSlots() {
		fWriter->RegisterSlot("disc-1d", this, &KTHDF5TypeWriterAnalysis::WriteDiscriminatedPoints);
		fWriter->RegisterSlot("final-write-points", this, &KTHDF5TypeWriterAnalysis::FlushDiscPointBuffer);
	}

	void KTHDF5TypeWriterAnalysis::WriteDiscriminatedPoints(KTDataPtr data) {
		KTDiscriminatedPoints1DData& fcData = data->Of<KTDiscriminatedPoints1DData>();
		KTSliceHeader& header = data->Of<KTSliceHeader>();

		if ( fWriter->OpenAndVerifyFile() == false ) return;

		DiscPoint point;
		point.fSlice = header.GetSliceNumber();
		point.fTimeInRunCenter = header.GetTimeInRun();
		for (point.fComponent = 0; point.fComponent < fcData.GetNComponents(); point.fComponent++ ) {
			const KTDiscriminatedPoints1DData::SetOfPoints& points = fcData.GetSetOfPoints(point.fComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it) {
                point.fBin = it->first;
                point.fAbscissa = it->second.fAbscissa;
                point.fOrdinate = it->second.fOrdinate;
                point.fThreshold = it->second.fThreshold;
                this->fDiscPointBuffer.push_back(point);
           }
		}
	}

	void KTHDF5TypeWriterAnalysis::FlushDiscPointBuffer() {
		KTDEBUG("Writing DiscPoints buffer");

		//Create the necessary dataspace
		hsize_t* dims = new hsize_t(this->fDiscPointBuffer.size());
		H5::DataSpace dspace(1, dims);

		if (fWriter->OpenAndVerifyFile() == false) return;

		// Make a group
		H5::Group* swfGroup = fWriter->AddGroup("sparsewf");

		// Ok, create the dataset and write it down.
		std::stringstream namestream;
        std::string dsetname;
        namestream << "sparsewf_" << this->fFlushIdx;
        namestream >> dsetname;
        H5::DataSet* dset = new H5::DataSet(swfGroup->createDataSet(dsetname.c_str(),
                                                                    *(this->fDiscPointType),
                                                                    dspace));

        dset->write((this->fDiscPointBuffer).data(), *(this->fDiscPointType));
        this->fDiscPointBuffer.clear();

        this->fFlushIdx++;
        delete dset;
		delete dims;
	}

}  // namespace