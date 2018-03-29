#include "KTAxisProperties.hh"

//ClassImp(Katydid::KTNBinsFunctor< 1 >);
//ClassImp(Katydid::KTNBinsFunctor< 2 >);
//ClassImp(Katydid::KTAxisProperties< 1 >);
//ClassImp(Katydid::KTAxisProperties< 2 >);

namespace Katydid
{
    KTAxisProperties< 1 >::KTAxisProperties() :
            fGetNBinsFunc(new KTDefaultNBins< 1 >()),
            fBinWidth(1.),
            fRangeMin(0.),
            fRangeMax(1.),
            fLabel()
    {
    }

    KTAxisProperties< 1 >::KTAxisProperties(double rangeMin, double rangeMax, KTNBinsFunctor< 1 >* getNBinsFunc) :
            fGetNBinsFunc(getNBinsFunc),
            fBinWidth(1.),
            fRangeMin(rangeMin),
            fRangeMax(rangeMax),
            fLabel()
    {
        if (fGetNBinsFunc == NULL) fGetNBinsFunc = new KTDefaultNBins< 1 >();
        fBinWidth = (rangeMax - rangeMin) / (double)(*fGetNBinsFunc)();
        //std::cout << "created KTAxisProperties<1>: nbins: " << (*fGetNBinsFunc)() << "  binwidth: " << fBinWidth << "  rangemin: " << fRangeMin << "  rangemax: " << fRangeMax << std::endl;
    }

    KTAxisProperties< 1 >::KTAxisProperties(const KTAxisProperties< 1 >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetAxisLabel();
    }

    KTAxisProperties< 1 >::~KTAxisProperties()
    {
        delete fGetNBinsFunc;
    }

    size_t KTAxisProperties< 1 >::GetNDimensions() const
    {
        return 1;
    }

    KTAxisProperties< 1 >& KTAxisProperties< 1 >::operator=(const KTAxisProperties< 1 >& orig)
    {
        fGetNBinsFunc = orig.fGetNBinsFunc->Clone();
        fBinWidth = orig.GetBinWidth();
        fRangeMin = orig.GetRangeMin();
        fRangeMax = orig.GetRangeMax();
        fLabel = orig.GetAxisLabel();
        return *this;
    }

    bool KTAxisProperties< 1 >::empty() const
    {
        return size() == 0;
    }

    size_t KTAxisProperties< 1 >::size() const
    {
        return (*fGetNBinsFunc)();
    }

    size_t KTAxisProperties< 1 >::GetNBins() const
    {
        return (*fGetNBinsFunc)();
    }

    void KTAxisProperties< 1 >::SetNBinsFunc(KTNBinsFunctor< 1 >* getNBinsFunc)
    {
        delete fGetNBinsFunc;
        fGetNBinsFunc = getNBinsFunc;
        fBinWidth = (fRangeMax - fRangeMin) / (double)(*fGetNBinsFunc)();
        return;
    }

    double KTAxisProperties< 1 >::GetBinWidth() const
    {
        return fBinWidth;
    }

    double KTAxisProperties< 1 >::GetRangeMin() const
    {
        return fRangeMin;
    }

    double KTAxisProperties< 1 >::GetRangeMax() const
    {
        return fRangeMax;
    }

    void KTAxisProperties< 1 >::GetRange(double& min, double& max) const
    {
        min = GetRangeMin();
        max = GetRangeMax();
        return;
    }

    void KTAxisProperties< 1 >::SetRangeMin(double min)
    {
        fRangeMin = min;
        fBinWidth = (fRangeMax - fRangeMin) / (double)(*fGetNBinsFunc)();
        return;
    }

    void KTAxisProperties< 1 >::SetRangeMax(double max)
    {
        fRangeMax = max;
        fBinWidth = (fRangeMax - fRangeMin) / (double)(*fGetNBinsFunc)();
        return;
    }

    void KTAxisProperties< 1 >::SetRange(double min, double max)
    {
        SetRangeMin(min);
        SetRangeMax(max);
        fBinWidth = (fRangeMax - fRangeMin) / (double)(*fGetNBinsFunc)();
        return;
    }

    double KTAxisProperties< 1 >::GetBinLowEdge(size_t bin) const
    {
        return fRangeMin + fBinWidth * (double)bin;
    }

    double KTAxisProperties< 1 >::GetBinCenter(size_t bin) const
    {
        return fRangeMin + fBinWidth * ((double)bin + 0.5);
    }

    ssize_t KTAxisProperties< 1 >::FindBin(double pos) const
    {
        return pos < fRangeMin ? 0 :
                pos >= fRangeMax ? size() - 1 :
                        (ssize_t)(floor((pos - fRangeMin) / fBinWidth));
    }

    const std::string& KTAxisProperties< 1 >::GetAxisLabel() const
    {
        return fLabel;
    }

    void KTAxisProperties< 1 >::SetAxisLabel(const std::string& label)
    {
        fLabel = label;
        return;
    }

} /* namespace Katydid */

