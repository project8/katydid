#include "KTRQProcessor.hh"

namespace Katydid {
  static KTDerivedRegistrar<KTProcessor, KTRQProcessor> sRQProcessor("rayleigh-quotient");

  KTRQProcessor::KTRQProcessor() :
    KTProcessor(),
    fRQSignal(),
    fChunkSize(512),
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
  
  void KTRQProcessor::ProcessNoiseEvent(boost::shared_ptr<KTEvent> event)
  {
    // Grab time series data from event
    const KTTimeSeriesData* noise = event->GetData<KTTimeSeriesData>(fNoiseName);
    if( noise != NULL ) {
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
      
      
      KTINFO(nrq_log,"using noise to update NACM estimate...")
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
