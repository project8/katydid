/**
 @file KTConvolution1D.hh
 @brief Contains KTConvolution1D
 @details Performs 1D discrete convolution
 @author: E. Zayas
 @date: Aug 25, 2017
 */

#ifndef KTCONVOLUTION1D_HH_
#define KTCONVOLUTION1D_HH_

#include "KTProcessor.hh"
#include "KTSlot.hh"

#include <vector>
#include <cmath>
#include <fftw3.h>
#include <map>
#include <iostream>

namespace Katydid
{
    
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;

    class KTConvolvedPowerSpectrumData;
    class KTConvolvedFrequencySpectrumDataFFTW;
    class KTConvolvedFrequencySpectrumDataPolar;


    /*!
     @class KTConvolution1D
     @author E. Zayas

     @brief Performs discrete convolution of 1D spectra

     @details
     Uses the overlap-save method to efficiently calculate the convolution. The input is broken up into blocks of size N, where
     N can be specified at runtime or determined automatically. N must be larger than the size of the kernel, and a power of 2 is recommended
     to maximize the FFT efficiency.
  
     Configuration name: "convolution"

     Available configuration values:
     - "kernel": std::string -- Location of a .json file which contains the kernel
     - "block-size": double -- Size of the input blocks. 0 will trigger an automatic determination of the ideal block size
     - "normalize": bool -- Normalize the kernel. If false, the output will be scaled by the norm of the kernel
     - "transform-flag": std:string -- Transform flag for FFTW

     Slots:
     - "ps": void (Nymph::KTDataPtr) -- Convolves a power spectrum; Requires KTPowerSpectrumData; Adds KTConvolvedPowerSpectrumData
     - "fs-fftw": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTConvolvedFrequencySpectrumDataFFTW
     - "fs-polar": void (Nymph::KTDataPtr) -- Convolves a frequency spectrum; Requires KTFrequencySpectrumDataPolar; Adds KTConvolvedFrequencySpectrumDataPolar

     Signals:
     - "conv-ps": void (Nymph::KTDataPtr) -- Emitted upon convolution of a power spectrum; Guarantees KTConvolvedPowerSpectrumData
     - "conv-fs-fftw": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedFrequencySpectrumDataFFTW
     - "conv-fs-polar": void (Nymph::KTDataPtr) -- Emitted upon convolution of a frequency spectrum; Guarantees KTConvolvedFrequencySpectrumDataPolar
    */

    class KTConvolution1D : public Nymph::KTProcessor
    {

        public:

            KTConvolution1D(const std::string& name = "convolution");
            virtual ~KTConvolution1D();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(std::string, Kernel);
            MEMBERVARIABLE(unsigned, BlockSize);
            MEMBERVARIABLE(bool, NormalizeKernel);
            MEMBERVARIABLE_NOSET(std::string, TransformFlag);

            MEMBERVARIABLE_NOSET(double, RegularSize);
            MEMBERVARIABLE_NOSET(double, ShortSize);

            void SetTransformFlag(const std::string& flag);


        private:

            std::vector< double > kernelX;

            typedef std::map< std::string, unsigned > TransformFlagMap;
            TransformFlagMap fTransformFlagMap;
            
            fftw_plan fComplexToRealPlan;
            fftw_plan fRealToComplexPlan;
            fftw_plan fC2CForwardPlan;
            fftw_plan fC2CReversePlan;

            fftw_plan fComplexToRealPlanShort;
            fftw_plan fRealToComplexPlanShort;
            fftw_plan fC2CForwardPlanShort;
            fftw_plan fC2CReversePlanShort;

            double *fInputArrayReal;
            double *fOutputArrayReal;
            fftw_complex *fInputArrayComplex;
            fftw_complex *fOutputArrayComplex;

            fftw_complex *fTransformedInputArray;
            fftw_complex *fTransformedOutputArray;
            fftw_complex *fTransformedInputArrayFromReal;
            fftw_complex *fTransformedOutputArrayFromReal;

            fftw_complex *fTransformedInputArrayShort;
            fftw_complex *fTransformedOutputArrayShort;
            fftw_complex *fTransformedInputArrayFromRealShort;
            fftw_complex *fTransformedOutputArrayFromRealShort;

            double *fInputArrayRealShort;
            double *fOutputArrayRealShort;
            fftw_complex *fInputArrayComplexShort;
            fftw_complex *fOutputArrayComplexShort;

            fftw_complex *fTransformedKernelXAsReal;
            fftw_complex *fTransformedKernelXAsComplex;

            unsigned fTransformFlagUnsigned;
            int fKernelSize;
            bool fInitialized;

        public:
            
            bool ParseKernel();

            bool Convolve1D( KTPowerSpectrumData& data );
            bool Convolve1D( KTFrequencySpectrumDataFFTW& data );
            bool Convolve1D( KTFrequencySpectrumDataPolar& data );

            bool CoreConvolve1D( KTPowerSpectrumDataCore& data, KTConvolvedPowerSpectrumData& newData );
            bool CoreConvolve1D( KTFrequencySpectrumDataFFTWCore& data, KTConvolvedFrequencySpectrumDataFFTW& newData );
            bool CoreConvolve1D( KTFrequencySpectrumDataPolarCore& data, KTConvolvedFrequencySpectrumDataPolar& newData );

            KTPowerSpectrum* DoConvolution( const KTPowerSpectrum* initialSpectrum, const int block, const int step, const int overlap );
            KTFrequencySpectrumFFTW* DoConvolution( const KTFrequencySpectrumFFTW* initialSpectrum, const int block, const int step, const int overlap );
            KTFrequencySpectrumPolar* DoConvolution( const KTFrequencySpectrumPolar* initialSpectrum, const int block, const int step, const int overlap );

            bool DFT_1D_R2C( int size );
            bool RDFT_1D_C2R( int size );
            bool DFT_1D_C2C( int size );
            bool RDFT_1D_C2C( int size );

            void SetupInternalMaps();

            bool FinishSetup();
            void Initialize( int nBinsTotal, int block, int step, int overlap );

            void AllocateArrays( int nSizeRegular, int nSizeShort );
            void FreeArrays();

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fPSSignal;
            Nymph::KTSignalData fFSFFTWSignal;
            Nymph::KTSignalData fFSPolarSignal;

            //***************
            // Slots
            //***************

        private:

            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;            

    };

} /* namespace Katydid */

#endif /* KTCONVOLUTION1D_HH_ */
