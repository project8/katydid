#ifndef __KTRQ_PROC_HH
#define __KTRQ_PROC_HH

/*
 *  KTRQProcessor.hh
 *   author: kofron
 *   created: 12/30/2012
 *
 *  KTRQProcessor calculates Rayleigh Quotients for time series data.
 *  It requires two time series inputs - one noise to generate the noise
 *  ACM, and another "interesting" input which is the data over which the
 *  rayleigh quotient is calculated.
 */

#include "KTBiasedACM.hh"
#include "KTProcessor.hh"
#include "KTLogger.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTPStoreNode.hh"

#include "boost/shared_ptr.hpp"

namespace Katydid {

  KTLOGGER(nrq_log, "katydid.nrq");

  class KTTimeSeriesData;

  class KTRQProcessor : public KTProcessor {

    /* 
     * Constructors and destructors.
     */
  public:
    KTRQProcessor();
    virtual ~KTRQProcessor();

    /* 
     * Configuration, setters and getters.
     */
  public:
    Bool_t Configure(const KTPStoreNode* node);
    unsigned GetChunkSize();
    void SetChunkSize(unsigned newsize);
    std::string GetNoiseDataName();
    void SetNoiseDataName(std::string noisename);

    /*
     * Slots and signals.
     */
  public:
    typedef KTSignal< void (const double*) >::signal RQSignal;

    void ProcessNoiseData(const KTTimeSeriesData* noise);
    void ProcessCandidateData(const KTTimeSeriesData* signal);
    void ProcessNoiseEvent(boost::shared_ptr<KTEvent> event);

  private:
    RQSignal fRQSignal;

    /*
     * Internal state related to processing
     */ 
  private:
    unsigned fChunkSize;
    KTBiasedACM* fNoiseACM;
    std::string fNoiseName;
    
  }; // class KTRQProcessor

}; // namespace katydid

#endif // __KTRQ_PROC_HH
