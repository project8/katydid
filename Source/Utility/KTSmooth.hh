/*
 * KTSmooth.hh
 *
 *  Created on: Jul 23, 2014
 *      Author: nsoblath
 */

#ifndef KTSMOOTH_HH_
#define KTSMOOTH_HH_

#include "KTPhysicalArray.hh"

namespace Katydid
{
    class KTSmooth
    {
        public:
            KTSmooth();
            virtual ~KTSmooth();

            /*!
             * 2D Smoothing
             * Algorithm taken directly from ROOT 5.34, and adapted to KTPhysicalArray.
             *
             * Parameters "kernelOpt":
             *   0 = use the k5a kernel
             *   1 = use the k5b kernel
             *   2 = use the k3a kernel
             *
             * Documentation from TH2.cxx (see http://root.cern.ch/root/html534/src/TH2.cxx.html#lD_u6B):
             * Smooth bin contents of this 2-d histogram using kernel algorithms
             * similar to the ones used in the raster graphics community.
             * Bin contents in the active range are replaced by their smooth values.
             * If Errors are defined via Sumw2, they are also scaled and computed.
             * However, note the resulting errors will be correlated between different-bins, so
             * the errors should not be used blindly to perform any calculation involving several bins,
             * like fitting the histogram.  One would need to compute also the bin by bin correlation matrix.
             *
             * 3 kernels are proposed k5a, k5b and k3a.
             * k5a and k5b act on 5x5 cells (i-2,i-1,i,i+1,i+2, and same for j)
             * k5b is a bit more stronger in smoothing
             * k3a acts only on 3x3 cells (i-1,i,i+1, and same for j).
             * By default the kernel "k5a" is used. You can select the kernels "k5b" or "k3a"
             * via the option argument.
             * If TAxis::SetRange has been called on the x or/and y axis, only the bins
             * in the specified range are smoothed.
             * In the current implementation if the first argument is not used (default value=1).
             *
             * implementation by David McKee (dmckee@bama.ua.edu). Extended by Rene Brun
             */
            template< typename XDataType >
            static bool Smooth(KTPhysicalArray< 2, XDataType >* array, unsigned kernelOpt = 0)
            {
                double k5a[5][5] =  { { 0, 0, 1, 0, 0 },
                                      { 0, 2, 2, 2, 0 },
                                      { 1, 2, 5, 2, 1 },
                                      { 0, 2, 2, 2, 0 },
                                      { 0, 0, 1, 0, 0 } };
                double k5b[5][5] =  { { 0, 1, 2, 1, 0 },
                                      { 1, 2, 4, 2, 1 },
                                      { 2, 4, 8, 4, 2 },
                                      { 1, 2, 4, 2, 1 },
                                      { 0, 1, 2, 1, 0 } };
                double k3a[3][3] =  { { 0, 1, 0 },
                                      { 1, 2, 1 },
                                      { 0, 1, 0 } };

                unsigned ksize_x=5;
                unsigned ksize_y=5;
                double* kernel = &k5a[0][0];
                switch (kernelOpt)
                {
                    case 1:
                        kernel = &k5b[0][0];
                        break;
                    case 2:
                        kernel = &k3a[0][0];
                        ksize_x=3;
                        ksize_y=3;
                        break;
                    default:
                        break;
                }

                // Kernel tail sizes (kernel sizes must be odd for this to work!)
                unsigned x_push = (ksize_x - 1) / 2;
                unsigned y_push = (ksize_y - 1) / 2;

                // find i,j ranges
                unsigned xBinStart = 0;
                unsigned xBinEnd  = array->GetNBins(1);
                unsigned yBinStart = 0;
                unsigned yBinEnd  = array->GetNBins(2);

                // Copy all the data to a temporary buffer
                KTPhysicalArray< 2, XDataType > buffer(*array);

                // main work loop
                double content, error, norm, k;
                unsigned xb, yb;
                for (unsigned xBin = xBinStart; xBin < xBinEnd; ++xBin)
                {
                    for (unsigned yBin = yBinStart; yBin < yBinEnd; ++yBin)
                    {
                        content = 0.0;
                        error = 0.0;
                        norm = 0.0;

                        for (unsigned n = 0; n < ksize_x; ++n)
                        {
                            for (unsigned m=0; m<ksize_y; ++m)
                            {
                                xb = xBin + (n - x_push);
                                yb = yBin + (m - y_push);
                                if ( (xb >= xBinStart) && (xb < xBinEnd) && (yb >= yBinStart) && (yb < yBinEnd) )
                                {
                                    k = kernel[n * ksize_y + m];
                                    //if ( (k != 0.0 ) && (buf[bin] != 0.0) ) { // General version probably does not want the second condition
                                    if ( k != 0.0 )
                                    {
                                        norm    += k;
                                        content += k * buffer(xb, yb);
                                    }
                                }
                            } // loop over y side of the kernel
                        } // loop over x side of the kernel

                        if ( norm != 0.0 )
                        {
                            (*array)(xBin, yBin) = content / norm;
                        }
                    }
                }

                return true;
            }
    };

} /* namespace Katydid */

#endif /* KTSMOOTH_HH_ */
