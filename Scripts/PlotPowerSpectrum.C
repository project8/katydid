// Test.C
// File for convenient unit tests or whatever you want to try out with interactive ROOT,
// without having to retype the same lines over and over again

{
    gROOT->Reset();

    Katydid::KTEgg* egg = new Katydid::KTEgg();

    egg->SetFileName("../data/tone_8_21_2011_4.egg");
    if (! egg->BreakEgg())
    {
        std::cout << "ERROR: Egg did not break" << std::endl;
        return;
    }
    if (! egg->ParseEggHeader())
    {
        std::cout << "ERROR: Header did not parse" << std::endl;
        return;
    }

    // Hatch the event
    Katydid::KTEvent* event = egg->HatchNextEvent();
    if (event == NULL)
    {
        std::cout << "ERROR: Event did not hatch" << std::endl;
        return;
    }

    TCanvas* c1 = new TCanvas("c1", "c1");
    c1->cd();
    TH1I* histAmpDist = event->CreateAmplitudeDistributionHistogram();
    histAmpDist->Draw();

    Katydid::KTSimpleFFT* fft = new Katydid::KTSimpleFFT((Int_t)event->GetRecord()->GetSize());
    fft->SetTransformFlag("ES");
    fft->InitializeFFT();
    fft->TakeData(event);
    fft->Transform();

    Katydid::KTPowerSpectrum* ps = fft->CreatePowerSpectrum();
    delete fft;
    TH1D* histPowerSpect = ps->CreateMagnitudeHistogram();
    TH1D* histPowerDist = ps->CreatePowerDistributionHistogram();
    delete ps;

    TCanvas* c2 = new TCanvas("c2", "c2");
    c2->cd();
    c2->SetLogy(1);
    histPowerSpect->Draw();

    TCanvas* c3 = new TCanvas("c3", "c3");
    c3->cd();
    c3->SetLogy(1);
    histPowerDist->Draw();

}
