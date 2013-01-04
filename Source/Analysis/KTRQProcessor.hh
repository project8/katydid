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
#include "KTData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"

#include "boost/shared_ptr.hpp"

namespace Katydid {

  KTLOGGER(nrq_log, "katydid.nrq");

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
    Bool_t GetNACMConverged();
    void SetNACMConverged(Bool_t newval);

    /*
     * Slots and signals.
     */
  public:
    typedef Eigen::Map<const Eigen::RowVectorXd> DataMapType;
    typedef KTSignal< void (const double*) >::signal RQSignal;

    void ProcessNoiseData(const KTTimeSeriesData* noise);
    void ProcessCandidateEvent(boost::shared_ptr<KTEvent> event);
    void ProcessNoiseEvent(boost::shared_ptr<KTEvent> event);

  private:
    RQSignal fRQSignal;

    /*
     * Internal state related to processing
     */ 
  private:
    /* The chunk size is the number of time samples that are processed
     * in one iteration by the RQ processor.  It is also the rank of the
     * autocorrelation matrix that is stored in the internal state of the
     * object.
     */ 
    unsigned fChunkSize;

    /*
     * The Naive Rayleigh Quotient method utilizes properties of the 
     * estimated autocorrelation matrix for the noise process it is
     * observing.  We also store a flag which indicates if the Noise ACM
     * is ready to be used (has converged).
     */
    bool fNACMDidConverge;
    KTBiasedACM* fNoiseACM;

    /*
     * We abstract away the external data representation and use a Mapped
     * version of the data to work with internally.
     */
    DataMapType* fDataMap;

    /*
     * This is the name of the data that is used to generate the noise ACM
     * estimate.  For now we rely on KTEvent generated noise.
     */
    std::string fNoiseName;

    /*
     * Calculates the lagged autocorrelation of a sequence of doubles stored
     * in a DataMap.  This should eventually get refactored into its own place
     * but for now it's a static member function as it does not rely on state.
     */
  public:
    static double LaggedACF(const DataMapType* data, unsigned lag);

    
  }; // class KTRQProcessor

}; // namespace katydid

#endif // __KTRQ_PROC_HH
