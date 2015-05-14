/*
 * KTHDF5TypeWriterSpectrumAnalysis.hh
 *
 *      Created on: 9/18/2014
 *          Author: J.N. Kofron
 */

#ifndef __KTHDF5TYPEWRITERSPECTRUMANALYSIS_HH
#define __KTHDF5TYPEWRITERSPECTRUMANALYSIS_HH

#include "KTHDF5Writer.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"

namespace Katydid {
	/*
	 * Definition of the record (struct) we will be writing to disk.
	 */
	typedef struct {
		unsigned fSlice;
		double fTimeInRunCenter;
		unsigned fComponent;
		unsigned fBin;
		double fAbscissa;
		double fOrdinate;
		double fThreshold;
	} DiscPoint;

	size_t DiscPointSize = sizeof(DiscPoint);
	size_t DiscPointFieldOffset[7] = {
		HOFFSET(DiscPoint, fSlice),
		HOFFSET(DiscPoint, fTimeInRunCenter),
		HOFFSET(DiscPoint, fComponent),
		HOFFSET(DiscPoint, fBin),
		HOFFSET(DiscPoint, fAbscissa),
		HOFFSET(DiscPoint, fOrdinate),
		HOFFSET(DiscPoint, fThreshold)
	};

	const char* DiscPointFieldName[7] = {
		"Slice",
		"TimeInRunCenter",
		"Component",
		"Bin",
		"Abscissa",
		"Ordinate",
		"Threshold"
	};

	H5::PredType DiscPointFieldType[7] = {
		H5::PredType::NATIVE_UINT,
		H5::PredType::NATIVE_DOUBLE,
		H5::PredType::NATIVE_UINT,
		H5::PredType::NATIVE_UINT,
		H5::PredType::NATIVE_DOUBLE,
		H5::PredType::NATIVE_DOUBLE,
		H5::PredType::NATIVE_DOUBLE
	};

	class KTHDF5TypeWriterSpectrumAnalysis: public KTHDF5TypeWriter {
		/* constructors/destructors/katydid boilerplate */
	public:
		KTHDF5TypeWriterSpectrumAnalysis();
		virtual ~KTHDF5TypeWriterSpectrumAnalysis();

		void RegisterSlots();

	public:
		void WriteDiscriminatedPoints(KTDataPtr data);
		void FlushDiscPointBuffer();

	private:
		std::vector<DiscPoint> fDiscPointBuffer;
		H5::CompType* fDiscPointType;
		unsigned fFlushIdx;
	};  // class
};

#endif // __KTHDF5TYPEWRITERSPECTRUMANALYSIS_HH
