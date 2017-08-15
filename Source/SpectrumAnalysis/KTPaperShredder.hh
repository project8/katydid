/**
 @file KTPaperShredder.hh
 @brief Contains KTPaperShredder
 @details Sends pretend tracks to spectrogram collector corresponding to "stripes" of data
 @author: E. Zayas
 @date: Aug 15, 2017
 */

#ifndef KTPAPERSHREDDER_HH_
#define KTPAPERSHREDDER_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

namespace Katydid
{
    
    class KTEggHeader;
    class KTProcessedTrackData;

    /*!
     @class KTPaperShredder
     @author E. Zayas

     @brief 

     @details
  
     Configuration name: "paper-shredder"

     Available configuration values:
     - "min-frequency": double -- minimum frequency to acquire in stripes
     - "max-frequency": double -- maximum frequency to acquire in stripes
     - "stripe-size": double -- size of stripes in seconds
     - "stripe-stride": double -- difference between start times of stripes. Defaults to the same as stripe-size (no overlap)
     - "n-slices-size": int -- number of slices per stripe (stripe size in terms of slices)
     - "n-slices-stride": int -- number of slices per stride

     The above provide two independent ways to specify the stripe size. If both are provided for either size or stride, the value in seconds will be used.

     Slots:
     - "header": Uses acquisition information to construct stripe timestamps; Requires KTEggHeader

     Signals:
     - "stripe": Emitted with timestamps of a stripe; Guarantees KTProcessedTrackData
    */

    class KTPaperShredder : public Nymph::KTProcessor
    {

        public:
            KTPaperShredder(const std::string& name = "paper-shredder");
            virtual ~KTPaperShredder();

            bool Configure(const scarab::param_node* node);

            double GetStripeSize() const;
            void SetStripeSize( double t );

            double GetStripeStride() const;
            void SetStripeStride( double s );

            unsigned GetNSlicesSize() const;
            void SetNSlicesSize( unsigned n );

            unsigned GetNSlicesStride() const;
            void SetNSlicesStride( unsigned n );

            double GetMinFrequency() const;
            void SetMinFrequency( double freq );

            double GetMaxFrequency() const;
            void SetMaxFrequency( double freq );

        private:

            double fStripeSize;
            double fStripeStride;
            unsigned fNSlicesSize;
            unsigned fNSlicesStride;
            double fMinFrequency;
            double fMaxFrequency;

            double fAcquisitionTime;

        public:
            
            bool MakeStripes( KTEggHeader& header );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fStripeSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;

    };

    inline double KTPaperShredder::GetStripeSize() const
    {
        return fStripeSize;
    }

    inline void KTPaperShredder::SetStripeSize( double t )
    {
        fStripeSize = t;
        return;
    }

    inline double KTPaperShredder::GetStripeStride() const
    {
        return fStripeStride;
    }

    inline void KTPaperShredder::SetStripeStride( double s )
    {
        fStripeStride = s;
        return;
    }

    inline unsigned KTPaperShredder::GetNSlicesSize() const
    {
        return fNSlicesSize;
    }

    inline void KTPaperShredder::SetNSlicesSize( unsigned n )
    {
        fNSlicesSize = n;
        return;
    }

    inline unsigned KTPaperShredder::GetNSlicesStride() const
    {
        return fNSlicesStride;
    }

    inline void KTPaperShredder::SetNSlicesStride( unsigned n )
    {
        fNSlicesStride = n;
        return;
    }

    inline double KTPaperShredder::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTPaperShredder::SetMinFrequency( double freq )
    {
        fMinFrequency = freq;
        return;
    }

    inline double KTPaperShredder::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTPaperShredder::SetMaxFrequency( double freq )
    {
        fMaxFrequency = freq;
        return;
    }

    
} /* namespace Katydid */
#endif /* KTPAPERSHREDDER_HH_ */
