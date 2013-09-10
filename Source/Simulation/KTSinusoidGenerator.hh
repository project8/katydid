/*
 * KTSinusoidGenerator.hh
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#ifndef KTSINUSOIDGENERATOR_HH_
#define KTSINUSOIDGENERATOR_HH_

#include "KTTSGenerator.hh"

namespace Katydid
{

    /*!
     @class KTSinusoidGenerator
     @author N. S. Oblath

     @brief Generates a sinusoidal time series

     @details
     Can create a new time series and drive processing, or can add a sinusoidal signal to an existing time series.

     Basic time series formation is dealt with in KTTSGenerator.

     Available configuration options:
     - Inherited from KTTSGenerator
       - "n-slices": UInt_t -- Number of slices to create (used only if creating new slices)
       - "n-channels": UInt_t -- Number of channels per slice to create (used only if creating new slices)
       - "time-series-size": UInt_t -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": Double_t -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": UInt_t -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)
     - From KTSinusoidGenerator
       - "frequency": Double_t -- Frequency of the sinusoid
       - "phase": Double_t -- Phase of the sinusoid
       - "amplitude": Double_t -- Amplitude of the sinusoid

     Slots: (inherited from KTTSGenerator)
     - "slice": void (boost::shared_ptr<KTData>) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals: (inherited from KTTSGenerator)
     - "header": void (const KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (boost::shared_ptr<KTData>) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
    */
    class KTSinusoidGenerator : public KTTSGenerator
    {
        public:
            KTSinusoidGenerator(const std::string& name = "sinusoid-generator");
            virtual ~KTSinusoidGenerator();

            virtual Bool_t ConfigureDerivedGenerator(const KTPStoreNode* node);

            Double_t GetFrequency() const;
            void SetFrequency(Double_t freq);

            Double_t GetPhase() const;
            void SetPhase(Double_t phase);

            Double_t GetAmplitude() const;
            void SetAmplitude(Double_t amp);

        private:
            Double_t fFrequency;
            Double_t fPhase;
            Double_t fAmplitude;

        public:
            virtual Bool_t GenerateTS(KTTimeSeriesData& data);

    };

    inline Double_t KTSinusoidGenerator::GetFrequency() const
    {
        return fFrequency;
    }

    inline void KTSinusoidGenerator::SetFrequency(Double_t freq)
    {
        fFrequency = freq;
        return;
    }

    inline Double_t KTSinusoidGenerator::GetPhase() const
    {
        return fPhase;
    }

    inline void KTSinusoidGenerator::SetPhase(Double_t phase)
    {
        fPhase = phase;
        return;
    }

    inline Double_t KTSinusoidGenerator::GetAmplitude() const
    {
        return fAmplitude;
    }

    inline void KTSinusoidGenerator::SetAmplitude(Double_t amp)
    {
        fAmplitude = amp;
        return;
    }


} /* namespace Katydid */
#endif /* KTSINUSOIDGENERATOR_HH_ */
