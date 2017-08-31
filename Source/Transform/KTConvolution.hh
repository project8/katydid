/**
 @file KTConvolution.hh
 @brief Contains KTConvolution
 @details Performs 1D discrete convolution
 @author: E. Zayas
 @date: Aug 25, 2017
 */

#ifndef KTCONVOLUTION_HH_
#define KTCONVOLUTION_HH_

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
    class KTNormalizedFSDataFFTW;
    class KTNormalizedFSDataPolar;
    class KTPowerSpectrum;
    class KTPowerSpectrumData;


    /*!
     @class KTConvolution
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
     - "transform-flag": std:string -- Transform flag for FFTW

     Slots:
     - "ps": void (Nymph::KTDataPtr) -- Convolves a power spectrum; Requires KTPowerSpectrumData; Adds KTConvolvedPowerSpectrumData

     Signals:
     - "conv-ps": void (Nymph::KTDataPtr) -- Emitted upon convolution of a power spectrum; Guarantees KTConvolvedPowerSpectrumData
    */

    class KTConvolution : public Nymph::KTProcessor
    {

        public:

            KTConvolution(const std::string& name = "convolution");
            virtual ~KTConvolution();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(std::string, Kernel);
            MEMBERVARIABLE(unsigned, BlockSize);
            MEMBERVARIABLE_NOSET(std::string, TransformFlag);
            void SetTransformFlag(const std::string& flag);


        private:

            std::vector< double > kernelX;

            typedef std::map< std::string, unsigned > TransformFlagMap;
            TransformFlagMap fTransformFlagMap;
            
            fftw_plan fComplexToRealPlan;
            fftw_plan fRealToComplexPlan;
            fftw_plan fC2CForwardPlan;
            fftw_plan fC2CReversePlan;

            double *fInputArrayReal;
            double *fOutputArrayReal;
            fftw_complex *fInputArrayComplex;
            fftw_complex *fOutputArrayComplex;

            fftw_complex *fTransformedKernelX;

            unsigned fTransformFlagUnsigned;
            int fKernelSize;

        public:
            
            bool ParseKernel();
            bool Convolve1D_PS( KTPowerSpectrumData& data );
            fftw_complex* DFT_1D_R2C( std::vector< double > in, int n );
            std::vector< double > RDFT_1D_C2R( fftw_complex *input, int n );
            void SetupInternalMaps();

            void AllocateArrays( int nSize );
            void FreeArrays();

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fPSSignal;

            //***************
            // Slots
            //***************

        private:

            Nymph::KTSlotDataOneType< KTPowerSpectrumData > fPSSlot;

    };

} /* namespace Katydid */

#endif /* KTCONVOLUTION_HH_ */
