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


#include "param.hh"
#include "KTSlot.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

#include <Eigen/Sparse>

#include <ctime>

namespace Katydid {

  KTLOGGER(nrq_log, "KTRQProcessor");

  class KTRQProcessor : public Nymph::KTProcessor {

    /* 
     * Constructors and destructors.
     */
  public:
    KTRQProcessor(const std::string& name = "rayleigh-quotient");
    virtual ~KTRQProcessor();

    /* 
     * Configuration, setters and getters.
     */
  public:
    bool Configure(const scarab::param_node* node);
    unsigned GetChunkSize();
    void SetChunkSize(unsigned newsize);
    std::string GetNoiseDataName();
    void SetNoiseDataName(std::string noisename);
    std::string GetCandidateDataName();
    void SetCandidateDataName(std::string noisename);
    bool GetNACMConverged();
    void SetNACMConverged(bool newval);

    /*
     * Slots and signals.
     */
  public:
    typedef Eigen::Map<const Eigen::RowVectorXd> DataMapType;


  private:
    Nymph::KTSlotDataOneType< KTTimeSeriesData > fNoiseSlot;
    bool ProcessNoiseData(KTTimeSeriesData& noise);
    Nymph::KTSlotDataOneType< KTTimeSeriesData > fCandidateSlot;
    bool ProcessCandidateData(KTTimeSeriesData& candidate);
    Nymph::KTSignalData fRQSignal;

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
     * estimate.  For now we rely on KTBundle generated noise.
     */
    std::string fNoiseName;

    /*
     * This is the name of data that we actually evaluate the RQ for.
     */
    std::string fCandidateName;

    /*
     * This is the output data name.
     */
    std::string fOutputDataName;

    /*
     * Calculates the lagged autocorrelation of a sequence of doubles stored
     * in a DataMap.  This should bundleually get refactored into its own place
     * but for now it's a static member function as it does not rely on state.
     */
  public:
    static double LaggedACF(const DataMapType* data, unsigned lag);

    /*
     * Calculates the Rayleigh Quotient over the stored noise autocorrelation
     * matrix given a data map containing time series data.
     */
    double RayleighQuotient(const DataMapType* tsptr);

    
  }; // class KTRQProcessor

}; // namespace katydid

#endif // __KTRQ_PROC_HH
