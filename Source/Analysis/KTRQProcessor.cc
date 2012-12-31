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
      this->SetChunkSize(node->GetData<unsigned>("chunk-size", fChunkSize));
      // if the noise ACM is already built, warn but do nothing.  otherwise, 
      // initialize the new matrix.
      if( fNoiseACM == NULL ) {
	fNoiseACM = new KTBiasedACM(fChunkSize, fChunkSize);
      }
      else KTWARN(nrq_log, "Config requested for RQProc but already configured...");
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
    KTWARN(nrq_log,"unimplemented processing of noise event called!");
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
