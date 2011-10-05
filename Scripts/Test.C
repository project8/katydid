// Test.C
// File for convenient unit tests or whatever you want to try out with interactive ROOT,
// without having to retype the same lines over and over again

{
    gROOT->Reset();


    /*
    Katydid::KTHammingWindow* rw = new Katydid::KTHammingWindow();
    rw->SetBinWidthAndWidth(0.1, 10.);
    TH1D* histRW = rw->CreateHistogram();
    TH1D* histFreqRW = rw->CreateFrequencyResponseHistogram();
    delete rw;
    histRW->Draw();
    TCanvas* c2 = new TCanvas("c2","c2");
    histFreqRW->Draw();
    */

    Katydid::KTEgg* egg = new Katydid::KTEgg();

    //egg->SetFileName("../data/tone_8_21_2011_4.egg");
    //egg->SetFileName("/Volumes/Lazlo/flicker.egg");
    egg->SetFileName("/Volumes/Sycorax/magnet_med_LHe_Kr_trap_pos_pol_0");
    if (! egg->BreakEgg()) return;
    if (! egg->ParseEggHeader()) return;

    while (kTRUE)
    {
        // Hatch the event
        Katydid::KTEvent* event = egg->HatchNextEvent();

        Katydid::KTWindowFunction* wfunc = new Katydid::KTHannWindow(event);
        wfunc->SetLength(1.e-6);
        //std::cout << "width as set: " << wfunc->SetWidth(1.e-4) << " s; bin width: " << wfunc->GetBinWidth() << " s; width in bins: " << wfunc->GetWidthInBins() << std::endl;

        Katydid::KTSlidingWindowFFT* fft = new Katydid::KTSlidingWindowFFT();
        fft->SetWindowFunction(wfunc);
        fft->SetOverlap(100);
        fft->SetTransformFlag("ES");
        fft->InitializeFFT();
        fft->TakeData(event);
        fft->Transform();

        TH2D* hist = fft->CreatePowerSpectrumHistogram();
        TCanvas *c1 = new TCanvas("c1", "c1");
        c1->SetLogz(1);
        hist->Draw("colz");

        c1->WaitPrimitive();

        delete c1;
    }

    /*
    Katydid::KTPowerSpectrum* ps = fft->CreatePowerSpectrum();
    delete fft;
    TH1D* hist = ps->CreateMagnitudeHistogram();
    delete ps;

    TCanvas *c1 = new TCanvas("c1", "c1");
    c1->SetLogy(1);
    hist->Draw();
    */


}
