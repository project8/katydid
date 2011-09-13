// Test.C
// File for convenient unit tests or whatever you want to try out with interactive ROOT,
// without having to retype the same lines over and over again

{
    gROOT->Reset();

    Katydid::KTEgg* egg = new Katydid::KTEgg();

    egg->SetFileName("../data/tone_8_21_2011_4.egg");
    if (! egg->BreakEgg()) return;
    if (! egg->ParseEggHeader()) return;

    // Hatch the event
    if (! egg->HatchNextEvent()) continue;
    const Katydid::KTEvent* event = egg->GetData();

    Katydid::KTSimpleFFT* fft = new Katydid::KTSimpleFFT((Int_t)event->GetRecordSize());
    fft->SetTransformFlag("ES");
    fft->InitializeFFT();
    fft->TakeData(event);
    fft->Transform();

    Katydid::KTPowerSpectrum* ps = fft->CreatePowerSpectrum();
    delete fft;
    TH1D* hist = ps->CreateMagnitudeHistogram();
    delete ps;

    TCanvas *c1 = new TCanvas("c1", "c1");
    c1->SetLogy(1);
    hist->Draw();

}
