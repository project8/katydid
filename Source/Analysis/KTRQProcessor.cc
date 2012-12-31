#include "KTRQProcessor.hh"

namespace Katydid {
  static KTDerivedRegistrar<KTProcessor, KTRQProcessor> sRQProcessor("rayleigh-quotient");

  KTRQProcessor::KTRQProcessor() :
    KTProcessor(),
    fRQSignal(),
    fChunkSize(512),
    fNACMDidConverge(false),
    fNoiseACM(NULL)
  {
    fConfigName = "rayleigh-quotient";
    RegisterSignal("rq-calc", 
		   &fRQSignal, 
		   "void (const double*)");
    RegisterSlot("ts-noise", 
		 this, 
		 &KTRQProcessor::ProcessNoiseData, 
		 "void (const KTTimeSeriesData*)");
    RegisterSlot("process-noise-event",
		 this,
		 &KTRQProcessor::ProcessNoiseEvent,
		 "void (boost::shared_ptr<KTEvent>)");
  }

  KTRQProcessor::~KTRQProcessor() 
  {
    if(fNoiseACM != NULL) delete fNoiseACM;
  }

  Bool_t KTRQProcessor::Configure(const KTPStoreNode* node) 
  {
    Bool_t result = true;
    // Get settings out of config file
    if (node != NULL) {

      // set the chunk size
      this->SetChunkSize(node->GetData<unsigned>("chunk-size", fChunkSize));

      // if the noise ACM is already built, warn but do nothing.  otherwise, 
      // initialize the new matrix.
      if( fNoiseACM == NULL ) {
	fNoiseACM = new KTBiasedACM(fChunkSize, fChunkSize);
      }
      else {
	KTWARN(nrq_log, "Config requested for RQProc (chunk size) but already configured...");
      }

      // grab the noise data name.
      this->SetNoiseDataName(node->GetData<std::string>("noise-data-name", fNoiseName));
    }
    else {
      KTWARN(nrq_log, "NULL config node passed to Configure!");
      result = false;
    }

    return result;
  }

  unsigned KTRQProcessor::GetChunkSize() 
  {
    return this->fChunkSize;
  }

  void KTRQProcessor::SetChunkSize(unsigned newsize) 
  {
    fChunkSize = newsize;
  }

  void KTRQProcessor::SetNoiseDataName(std::string newname)
  {
    fNoiseName = newname;
  }

  std::string KTRQProcessor::GetNoiseDataName()
  {
    return fNoiseName;
  }

  void KTRQProcessor::SetNACMConverged(Bool_t newval)
  {
    this->fNACMDidConverge = newval;
  }
  
  Bool_t KTRQProcessor::GetNACMConverged()
  {
    return this->fNACMDidConverge;
  }
  
  void KTRQProcessor::ProcessNoiseEvent(boost::shared_ptr<KTEvent> event)
  {
    // Grab time series data from event
    const KTTimeSeriesData* noise = event->GetData<KTTimeSeriesData>(fNoiseName);
    if( noise != NULL && !(this->fNACMDidConverge) ) {
      // Grab the first channel of data out to use. 
      const KTTimeSeriesReal* noiseDt = dynamic_cast<const KTTimeSeriesReal*>(noise->GetTimeSeries(0));

      // We need to iterate over the chunks in the time series and produce our 
      // own time series.  First things first, get the pointer to the raw data
      // held in the time series.
      unsigned nElem = (noiseDt->GetData()).data().size();
      const double* rawPtr = (noiseDt->GetData()).data().begin();

      // Now we point the data map at the first fChunkSize piece of data and process it. 
      // For now we only use the first chunk to form the ACM.  Despite the fact that this
      // looks like allocating memory, it isn't.
      new (&(this->fDataMap)) DataMapType(rawPtr, this->fChunkSize);
        
      // Update noise estimate with current noise sample.
      KTINFO(nrq_log,"using noise to update NACM estimate...");

      // Calculate first row.  The NACM is a Toeplitz matrix which means we only need to
      // calculate the first row.
      for(unsigned rowIdx = 0; rowIdx < this->fChunkSize; rowIdx++) {
	(*this->fNoiseACM)(rowIdx, 0) = 1.0;
      }

      for(unsigned row = 1; row < this->fChunkSize; row++) {
	for(unsigned col = 0; col < this->fChunkSize; col++) {
	  // If row = col, we are on the diagonal, which means this element is equal to
	  // row = 0, col = 0.
	  if( row == col ) (*this->fNoiseACM)(row,col) = (*this->fNoiseACM)(0,0);
	  // If row < col, we are below the diagonal, so use the transposed element for
	  // assignment.
	  if( col < row ) (*this->fNoiseACM)(row,col) = (*this->fNoiseACM)(col,row);
	  // If row > col, we are above the diagonal.  Look above and to the left to get this
	  // element.
	  if( col > row ) (*this->fNoiseACM)(row,col) = (*this->fNoiseACM)(row-1, col-1);
	}
      }

      // Check convergence criterion (TEMPORARILY JUST TRUE) and set convergence flag if
      // appropriate.
      if(true) {
	this->SetNACMConverged(true);
	KTINFO(nrq_log,"NACM has converged.");
	KTWARN(nrq_log,"DON'T BELIEVE CONVERGE MSG, TEMPORARY CONVERGENCE CRITERION USED.");
      }

    }
    else {
      KTWARN(nrq_log,"time series " << fNoiseName << " not found in event!  skipping...");
    }
  }

  void KTRQProcessor::ProcessNoiseData(const KTTimeSeriesData* noise)
  {
    KTWARN(nrq_log,"unimplemented processing of noise called!");
  }

  void KTRQProcessor::ProcessCandidateData(const KTTimeSeriesData* signal) 
  {
    KTWARN(nrq_log,"unimplemented processing of signal called!");
  }

}; // namespace katydid
