#include "KTRQProcessor.hh"

namespace Katydid {
  static KTDerivedRegistrar<KTProcessor, KTRQProcessor> sRQProcessor("rayleigh-quotient");

  KTRQProcessor::KTRQProcessor() :
    KTProcessor(),
    fRQSignal(),
    fChunkSize(512),
    fNACMDidConverge(false),
    fNoiseACM(NULL),
    fDataMap(NULL)
  {
    fConfigName = "rayleigh-quotient";
    RegisterSignal("rq-calc", 
		   &fRQSignal, 
		   "void (const KTTimeSeriesData*)");
    RegisterSlot("ts-noise", 
		 this, 
		 &KTRQProcessor::ProcessNoiseData, 
		 "void (const KTTimeSeriesData*)");
    RegisterSlot("process-noise-event",
		 this,
		 &KTRQProcessor::ProcessNoiseEvent,
		 "void (boost::shared_ptr<KTEvent>)");
    RegisterSlot("process-candidate-event",
		 this,
		 &KTRQProcessor::ProcessCandidateEvent,
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
	fDataMap = new DataMapType(NULL, fChunkSize);
      }
      else {
	KTWARN(nrq_log, "Config requested for RQProc (chunk size) but already configured...");
      }

      // grab the noise and candidate data names.
      this->SetNoiseDataName(node->GetData<std::string>("noise-data-name", fNoiseName));
      this->SetCandidateDataName(node->GetData<std::string>("candidate-data-name", fCandidateName));
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

  void KTRQProcessor::SetCandidateDataName(std::string newname)
  {
    fCandidateName = newname;
  }

  std::string KTRQProcessor::GetCandidateDataName()
  {
    return fCandidateName;
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
    if( noise != NULL ) {
      if( !(this->fNACMDidConverge) ) {
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
	new (this->fDataMap) DataMapType(rawPtr, this->fChunkSize);
        
	// Update noise estimate with current noise sample.
	KTINFO(nrq_log,"using noise to update NACM estimate...");

	// Calculate first row.  The NACM is a Toeplitz matrix which means we only need to
	// calculate the first row.
	for(unsigned colIdx = 0; colIdx < this->fChunkSize; colIdx++) {
	  (*this->fNoiseACM)(0, colIdx) = KTRQProcessor::LaggedACF(this->fDataMap, colIdx);
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
    }
    else {
      KTWARN(nrq_log,"time series " << fNoiseName << " not found in event!  skipping...");
    }
  }

  double KTRQProcessor::LaggedACF(const DataMapType* data, unsigned lag) 
  {
    double result = 0.0;
    unsigned len = data->size();
    for(unsigned idx = 0; idx < (len - 1 - lag); idx++) {
      result += (*data)(idx) * (*data)(idx + lag);
    }
    return result/len;
  }

  double KTRQProcessor::RayleighQuotient(const DataMapType* tsptr)
  {
    return (tsptr->normalized())*(*(this->fNoiseACM))*(tsptr->adjoint());
  }

  void KTRQProcessor::ProcessNoiseData(const KTTimeSeriesData* noise)
  {
    KTWARN(nrq_log,"unimplemented processing of noise called!");
  }

  void KTRQProcessor::ProcessCandidateEvent(boost::shared_ptr<KTEvent> event) 
  {
    if( this->fNACMDidConverge ) {
      // grab data from the event.
      const KTTimeSeriesData* c = event->GetData<KTTimeSeriesData>(fCandidateName);
      if( c != NULL ) {
	// cast data to time series real data.  
	const KTTimeSeriesReal* cDt = dynamic_cast<const KTTimeSeriesReal*>(c->GetTimeSeries(0));

	// We need to iterate over the chunks in the time series and produce our 
	// own time series.  First things first, get the pointer to the raw data
	// held in the time series.
	unsigned nElem = (cDt->GetData()).data().size();
	const double* rawPtr = (cDt->GetData()).data().begin();

	// Now here is our time series.  The number of elements is equal to the floor of the 
	// number of elements in the incoming time series divided by the chunk size.
	unsigned nOut = nElem/fChunkSize;
	KTBasicTimeSeriesData* nDt = new KTBasicTimeSeriesData(1);
	KTTimeSeriesReal* rqOut = new KTTimeSeriesReal(nOut);

	/*
	 * Iterate over the data in the event, pointing the data map at each chunk consecutively.
	 * first we need to know 
	 */
	for(unsigned offset = 0; offset < nOut; offset++) {
	  new (this->fDataMap) DataMapType(rawPtr + (offset*fChunkSize), this->fChunkSize);
	  
	  // Now we should be able to compute the rayleigh quotient for this chunk.
	  double rq = this->RayleighQuotient(this->fDataMap);

	  // Now set the data in the output time series.
	  rqOut->SetValue(offset, rq);
	}

	/*
	 * Attach the new data to the KTEvent and fire the signal that indicates we have 
	 * compressed this time series.
	 */
	nDt->SetName(fOutputDataName);
	nDt->SetTimeSeries(rqOut);
	event->AddData(nDt);
	fRQSignal(nDt);	
      }
      else {
	KTWARN(nrq_log,"no data named " << fCandidateName << " found in event!  skipping...");
      }
    }
    else {
      KTWARN(nrq_log,"NACM has not converged - no NRQ calculation performed.");
    }
  }

}; // namespace katydid
