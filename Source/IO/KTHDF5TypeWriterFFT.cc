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
        polar_fft_buffer(NULL),
        polar_fft_dspace(NULL),
        cmplx_fft_buffer(NULL),
        cmplx_fft_dspace(NULL),
        polar_pwr_buffer(NULL),
        polar_pwr_dspace(NULL),
        cmplx_pwr_buffer(NULL),
        cmplx_pwr_dspace(NULL)
    {}

    KTHDF5TypeWriterFFT::~KTHDF5TypeWriterFFT()
    {}

    void KTHDF5TypeWriterFFT::ProcessEggHeader(KTEggHeader* header)
    {   

        if(header != NULL) {
            KTDEBUG(publog, "Configuring from Egg header...");
            this->n_components = (header->GetNChannels());
            this->slice_size = (header->GetSliceSize());

            /*
             * Polar FFT preparation.  
             * Each component gets two rows - one for abs, and one for arg.
             * Each row is slice_size/2 + 1 long.  
             * Our array is therefore 2N*(slice_size/2 + 1).
             */
            this->polar_fft_size = (this->slice_size >> 1) + 1;
            this->polar_fft_buffer = new fft_buffer(boost::extents[2*n_components][this->polar_fft_size]);     

            /*
             * Complex FFT preparation.
             * Each component gets two rows, one for real and one for imag.
             * Each row is 2*slice_len + 1 long - positive and negative
             * frequencies.
             * Our array is therefore 2N*(2*slice_size + 1).
             */
             this->cmplx_fft_size = (this->slice_size << 1) + 1;
             this->cmplx_fft_buffer = new fft_buffer(boost::extents[2*n_components][this->cmplx_fft_size]);

             /*
              * Polar power spectrum preparation.
              * Each component gets a single row, as a power spectrum has only
              * magnitude by definition.
              * Power in only positive frequencies and DC is calculated i.e.
              * the shape of the buffer is identical to the polar_fft_buffer
              * except there is only one component.
              */
              this->polar_pwr_size = this->polar_fft_size;
              this->polar_pwr_buffer = new fft_buffer(boost::extents[n_components][this->polar_pwr_size]);

              /*
               * Complex (FFTW) power spectrum preparation.
               * Each component gets a single row as above.
               * Power is calculated in positive and negative frequencies
               * i.e. the buffer has the same shape as the complex fft buffer.
               */
               this->cmplx_pwr_size = this->cmplx_fft_size;
               this->cmplx_pwr_buffer = new fft_buffer(boost::extents[n_components][this->cmplx_pwr_size]);

        }
        KTDEBUG(publog, "Done.");
        H5::Group* spectra_group = fWriter->AddGroup("/spectra");
        this->fft_group = new H5::Group(spectra_group->createGroup("/spectra/frequency"));
        this->power_group = new H5::Group(spectra_group->createGroup("/spectra/power"));
        this->CreateDataspaces();
    }

    void KTHDF5TypeWriterFFT::CreateDataspaces() {
        /*
        If the dataspaces have already been created, this is a no-op.  
        Create dataspaces for:
            - Polar FFT data.  This is 2XN where N == slice_len/2 + 1
        */
        if(this->polar_fft_dspace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpaces for Polar FFT");
            hsize_t polar_fft_dims[] = {2*this->n_components, 
                                        this->polar_fft_size};

            this->polar_fft_dspace = new H5::DataSpace(2, polar_fft_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->cmplx_fft_dspace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpaces for Complex FFT");
            hsize_t cmplx_fft_dims[] = {2*this->n_components, 
                                        this->cmplx_fft_size};

            this->cmplx_fft_dspace = new H5::DataSpace(2, cmplx_fft_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->polar_pwr_dspace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for Polar PS");
            hsize_t polar_pwr_dims[] = {this->n_components,
                                        this->polar_pwr_size};
            this->polar_pwr_dspace = new H5::DataSpace(2, polar_pwr_dims);
            KTDEBUG(publog, "Done.");
        }
        if(this->cmplx_pwr_dspace == NULL) {
            KTDEBUG(publog, "Creating H5::DataSpace for Complex PS");
            hsize_t cmplx_pwr_dims[] = {this->n_components,
                                        this->cmplx_pwr_size};
            this->cmplx_pwr_dspace = new H5::DataSpace(2, cmplx_pwr_dims);
            KTDEBUG(publog, "Done.");
        }
    }

     H5::DataSet* KTHDF5TypeWriterFFT::CreatePolarFFTDSet(const std::string& name) {
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fft_group,
                                             *(this->polar_fft_dspace));
        return dset;
    }

     H5::DataSet* KTHDF5TypeWriterFFT::CreatePolarPowerDSet(const std::string& name) {
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->power_group,
                                             *(this->polar_pwr_dspace));
        return dset;
    }    


     H5::DataSet* KTHDF5TypeWriterFFT::CreateComplexFFTDSet(const std::string& name) {
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->fft_group,
                                             *(this->cmplx_fft_dspace));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterFFT::CreateComplexPowerDSet(const std::string& name) {
        H5::DataSet* dset = this->CreateDSet(name, 
                                             this->power_group,
                                             *(this->cmplx_pwr_dspace));
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
        fWriter->RegisterSlot("setup-from-header", this, &KTHDF5TypeWriterFFT::ProcessEggHeader);
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
                    (*this->polar_fft_buffer)[iC][f] = spec[0].GetAbs(f);
                    (*this->polar_fft_buffer)[iC+1][f] = spec[0].GetArg(f);
                }
                
            }
        }
        dset->write(this->polar_fft_buffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
  
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTW(KTDataPtr data) {
        if (!data) return;

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
                    (*this->cmplx_fft_buffer)[iC][f] = spec[0].GetReal(f); 
                    (*this->cmplx_fft_buffer)[iC+1][f] = spec[0].GetImag(f); 
                }
                
            }
        }
        dset->write(this->cmplx_fft_buffer->data(), H5::PredType::NATIVE_DOUBLE);
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
                    (*this->polar_pwr_buffer)[iC][f] = mag; 
                }
                
            }
        }
        dset->write(this->polar_pwr_buffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
    }
    void KTHDF5TypeWriterFFT::WriteFrequencySpectrumDataFFTWPower(KTDataPtr data) {

        if (!data) return;

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
                    (*this->cmplx_pwr_buffer)[iC][f] = mag;
                }
                
            }
        }
        dset->write(this->cmplx_pwr_buffer->data(), H5::PredType::NATIVE_DOUBLE);
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
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterFFT::WritePowerSpectralDensity(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
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


