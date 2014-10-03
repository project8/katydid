/*
 * KTHDF5TypeWriterFFT.cc
 *
 *  Created on: 9/12/2014
 *      Author: J.N. Kofron
 */

#include <string>
#include <sstream>

#include "KTHDF5TypeWriterFFT.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTSliceHeader.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTTimeFrequencyDataPolar.hh"
#include "KTTimeFrequencyPolar.hh"


namespace Katydid {
    KTLOGGER(publog, "KTHDF5TypeWriterFFT");

    static KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterFFT> sH5TWFFTReg;

    KTHDF5TypeWriterFFT::KTHDF5TypeWriterFFT() :
        KTHDF5TypeWriter(),
        fPolarFFTBuffer(NULL),
        fPolarFFTDSpace(NULL),
        fCmplxFFTBuffer(NULL),
        fCmplxFFTDSpace(NULL),
        fPolarPwrBuffer(NULL),
        fPolarPwrDSpace(NULL),
        fCmplxPwrBuffer(NULL),
        fCmplxPwrDSpace(NULL),
        fPwrSpecDSpace(NULL),
        fPwrSpecBuffer(NULL),
        fPSDDSpace(NULL),
        fPSDBuffer(NULL),
        fSpectraGroup(NULL),
        fFFTGroup(NULL),
        fPowerGroup(NULL)
    {}

    KTHDF5TypeWriterFFT::~KTHDF5TypeWriterFFT()
    {}

    void KTHDF5TypeWriterFFT::ProcessEggHeader() {   
        KTEggHeader* header = fWriter->GetHeader();
        if(header != NULL) {
            KTDEBUG(publog, "Configuring from Egg header...");
            this->fNComponents = (header->GetNChannels());
            this->fSliceSize = (header->GetSliceSize());

            /*
             * Polar FFT preparation.  
             * Each component gets two rows - one for abs, and one for arg.
             * Each row is fSliceSize/2 + 1 long.  
             * Our array is therefore 2N*(fSliceSize/2 + 1).
             */
            this->fPolarFFTSize = (this->fSliceSize >> 1) + 1;
            this->fPolarFFTBuffer = new fft_buffer(boost::extents[2*fNComponents][this->fPolarFFTSize]);     

            /*
             * Complex FFT preparation.
             * Each component gets two rows, one for real and one for imag.
             * Each row is 2*slice_len + 1 long - positive and negative
             * frequencies.
             * Our array is therefore 2N*(2*fSliceSize + 1).
             */
             this->fCmplxFFTSize = (this->fSliceSize << 1) + 1;
             this->fCmplxFFTBuffer = new fft_buffer(boost::extents[2*fNComponents][this->fCmplxFFTSize]);

             /*
              * Polar power spectrum preparation.
              * Each component gets a single row, as a power spectrum has only
              * magnitude by definition.
              * Power in only positive frequencies and DC is calculated i.e.
              * the shape of the buffer is identical to the fPolarFFTBuffer
              * except there is only one component.
              */
              this->fPolarPwrSize = this->fPolarFFTSize;
              this->fPolarPwrBuffer = new fft_buffer(boost::extents[fNComponents][this->fPolarPwrSize]);

              /*
               * Complex (FFTW) power spectrum preparation.
               * Each component gets a single row as above.
               * Power is calculated in positive and negative frequencies
               * i.e. the buffer has the same shape as the complex fft buffer.
               */
               this->fCmplxPwrSize = this->fCmplxFFTSize;
               this->fCmplxPwrBuffer = new fft_buffer(boost::extents[fNComponents][this->fCmplxPwrSize]);

              /*
               * PS and PSD spectrum preparation.  Each component gets a single 
               * row.
               * Power is only calculated for positive frequencies and DC.
               * The buffer is therefore the same shape as the polar power 
               * spectrum.
               */
               this->fPSDSize = this->fPolarPwrSize;
               this->fPwrSpecSize = this->fPolarPwrSize;
               this->fPSDBuffer = new fft_buffer(boost::extents[fNComponents][this->fPSDSize]);
               this->fPwrSpecBuffer = new fft_buffer(boost::extents[fNComponents][this->fPSDSize]);

        }
        KTDEBUG(publog, "Done.");
        this->fSpectraGroup = fWriter->AddGroup("/spectra");
        this->CreateDataspaces();
    }

    void KTHDF5TypeWriterFFT::CreateDataspaces() {
        /*
        If the dataspaces have already been created, this is a no-op.  
        Create dataspaces for:
            - Polar FFT data.  This is 2XN where N == slice_len/2 + 1
        */
        if(this->fPolarFFTDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpaces for Polar FFT");
            hsize_t polar_fft_dims[] = {2*this->fNComponents, 
                                        this->fPolarFFTSize};

            this->fPolarFFTDSpace = new H5::DataSpace(2, polar_fft_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->fCmplxFFTDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpaces for Complex FFT");
            hsize_t cmplx_fft_dims[] = {2*this->fNComponents, 
                                        this->fCmplxFFTSize};

            this->fCmplxFFTDSpace = new H5::DataSpace(2, cmplx_fft_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->fPolarPwrDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for Polar PS");
            hsize_t polar_pwr_dims[] = {this->fNComponents,
                                        this->fPolarPwrSize};
            this->fPolarPwrDSpace = new H5::DataSpace(2, polar_pwr_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->fCmplxPwrDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for Complex PS");
            hsize_t cmplx_pwr_dims[] = {this->fNComponents,
                                        this->fCmplxPwrSize};
            this->fCmplxPwrDSpace = new H5::DataSpace(2, cmplx_pwr_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->fPwrSpecDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for Power Spectrum");
            hsize_t pwr_spec_dims[] = {this->fNComponents,
                                        this->fPwrSpecSize};
            this->fPwrSpecDSpace = new H5::DataSpace(2, pwr_spec_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->fPSDDSpace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for PSD");
            hsize_t psd_dims[] = {this->fNComponents,
                                        this->fPSDSize};
            this->fPSDDSpace = new H5::DataSpace(2, psd_dims);
            KTDEBUG(publog, "Done.");
        }
    }

     H5::DataSet* KTHDF5TypeWriterFFT::CreatePolarFFTDSet(const std::string& name) {
      if(this->fFFTGroup == NULL) {
        this->fFFTGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/frequency"));
      }
      H5::DataSet* dset = this->CreateDSet(name, 
                                           this->fFFTGroup,
                                           *(this->fPolarFFTDSpace));
        return dset;
    }

     H5::DataSet* KTHDF5TypeWriterFFT::CreatePolarPowerDSet(const std::string& name) {
      if(this->fPowerGroup == NULL) {
        this->fPowerGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/power"));
      }
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fPowerGroup,
                                             *(this->fPolarPwrDSpace));
        return dset;
    }    


     H5::DataSet* KTHDF5TypeWriterFFT::CreateComplexFFTDSet(const std::string& name) {
      if(this->fFFTGroup == NULL) {
        this->fFFTGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/frequency"));
      }
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fFFTGroup,
                                             *(this->fCmplxFFTDSpace));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterFFT::CreateComplexPowerDSet(const std::string& name) {
      if(this->fPowerGroup == NULL) {
        this->fPowerGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/power"));
      }
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fPowerGroup,
                                             *(this->fCmplxPwrDSpace));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterFFT::CreatePowerSpecDSet(const std::string& name) {
      if(this->fPowerGroup == NULL) {
        this->fPowerGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/power"));
      }
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fPowerGroup,
                                             *(this->fPwrSpecDSpace));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterFFT::CreatePSDDSet(const std::string& name) {
      if(this->fPowerGroup == NULL) {
        this->fPowerGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/power"));
      }
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fPowerGroup,
                                             *(this->fPSDDSpace));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterFFT::CreateDSet(const std::string& name,
                                                 const H5::Group* grp, 
                                                 const H5::DataSpace& ds) {
        H5::DSetCreatPropList plist;
        unsigned default_value = 0.0;
        plist.setFillValue(H5::PredType::NATIVE_DOUBLE, &default_value);
        KTDEBUG(publog, "Creating complex FFT dataset.");
        KTDEBUG(publog, grp);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               H5::PredType::NATIVE_DOUBLE,
                                                               ds,
                                                               plist));
        KTDEBUG("Done.");
        return dset;
    }

    void KTHDF5TypeWriterFFT::RegisterSlots() {
        fWriter->RegisterSlot("fs-polar", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("ps", this, &KTHDF5TypeWriterFFT::WritePowerSpectrum);
        fWriter->RegisterSlot("psd", this, &KTHDF5TypeWriterFFT::WritePowerSpectralDensity);
        fWriter->RegisterSlot("ps-dist", this, &KTHDF5TypeWriterFFT::WritePowerSpectrumDistribution);
        fWriter->RegisterSlot("psd-dist", this, &KTHDF5TypeWriterFFT::WritePowerSpectralDensityDistribution);
        fWriter->RegisterSlot("tf-polar", this, &KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTHDF5TypeWriterFFT::WriteMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTHDF5TypeWriterFFT::WriteMultiFSDataFFTW);
    }

    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolar(KTDataPtr data) {
        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "FSpolar_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreatePolarFFTDSet(spectrum_name);
        KTDEBUG(publog, "Done.");

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComp = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Polar FFT data to HDF5 file.");
        for (unsigned iC = 0; iC < nComp; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fPolarFFTBuffer)[iC][f] = spec[0].GetAbs(f);
                    (*this->fPolarFFTBuffer)[iC+1][f] = spec[0].GetArg(f);
                }
                
            }
        }
        dset->write(this->fPolarFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
  
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTW(KTDataPtr data) {
        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "complexFS_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreateComplexFFTDSet(spectrum_name);
        KTDEBUG(publog, "Done.");

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComp = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Complex FFT data to HDF5 file.");
        for (unsigned iC = 0; iC < nComp; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fCmplxFFTBuffer)[iC][f] = spec[0].GetReal(f); 
                    (*this->fCmplxFFTBuffer)[iC+1][f] = spec[0].GetImag(f); 
                }
                
            }
        }
        dset->write(this->fCmplxFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPower(KTDataPtr data) {
        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating polar power spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "polarPS_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreatePolarPowerDSet(spectrum_name);
        KTDEBUG(publog, "Done.");

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nComp = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Polar PS data to HDF5 file.");
        for (unsigned iC = 0; iC < nComp; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    double mag = sqrt(pow(spec[0].GetAbs(f), 2.0) 
                                      + pow(spec[0].GetArg(f),2.0));
                    (*this->fPolarPwrBuffer)[iC][f] = mag; 
                }
                
            }
        }
        dset->write(this->fPolarPwrBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPower(KTDataPtr data) {

        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "complexPS_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreateComplexPowerDSet(spectrum_name);
        KTDEBUG(publog, "Done.");

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nComp = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Complex FFT data to HDF5 file.");
        for (unsigned iC = 0; iC < nComp; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    double mag = sqrt(pow(spec[0].GetReal(f),2.0) + pow(spec[0].GetImag(f),2.0));
                    (*this->fCmplxPwrBuffer)[iC][f] = mag;
                }
                
            }
        }
        dset->write(this->fCmplxPwrBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WritePowerSpectrum(KTDataPtr data) {
        if (! data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "PS_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreatePowerSpecDSet(spectrum_name);

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nComponents; iC++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iC);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                for(int i=0; i < this->fPwrSpecSize; i++) {
                    double val = (*spectrum)(i);
                    (*this->fPwrSpecBuffer)[iC][i] = val;
                }
            }
        }
        dset->write(this->fPwrSpecBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        return;
    }
    void KTHDF5TypeWriterFFT::WritePowerSpectralDensity(KTDataPtr data) {
        if (! data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "PSD_" << sliceN;
        name_builder >> spectrum_name;

        H5::DataSet* dset = this->CreatePSDDSet(spectrum_name);

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nComponents = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nComponents; iC++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iC);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectralDensity();
                for(int i=0; i < this->fPwrSpecSize; i++) {
                    double val = (*spectrum)(i);
                    (*this->fPSDBuffer)[iC][i] = val;
                }               
            }
        }
        dset->write(this->fPSDBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        return;

        
    }
    void KTHDF5TypeWriterFFT::WritePowerSpectrumDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WritePowerSpectralDensityDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolar(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolarPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteTimeFrequencyDataPolarPower(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteMultiFSDataPolar(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WriteMultiFSDataFFTW(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
}


