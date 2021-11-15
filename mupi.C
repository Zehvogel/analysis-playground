#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>

void mupi(const TString ifname = "300k")
{
    std::unique_ptr<TFile> f(TFile::Open("sld_out_" + ifname + ".root"));
    
    //f->ls();

    std::unique_ptr<TH2F> mupipt(f->Get<TH2F>("mupipt"));
    std::unique_ptr<TH2F> mupip(f->Get<TH2F>("mupip"));
    std::unique_ptr<TH2F> mumupt(f->Get<TH2F>("mumupt"));
    std::unique_ptr<TH2F> mumup(f->Get<TH2F>("mumup"));
    std::unique_ptr<TH2F> mupt_eff(f->Get<TH2F>("mupt_eff"));
    std::unique_ptr<TH2F> mup_eff(f->Get<TH2F>("mup_eff"));
    //std::unique_ptr<TH2F> mupt_all(f->Get<TH2F>("mupt_all"));
    //std::unique_ptr<TH2F> mup_all(f->Get<TH2F>("mup_all"));
    
    auto c1 = new TCanvas("mupi");
    c1->Divide(2,3);

    c1->cd(1)->SetLogy();
    mupipt->SetStats(0);
    mupipt->SetTitle("#mu^{#pm} detected as #pi^{#pm}; cos(#theta); Pt (GeV)   ");
    mupipt->Draw("COLZ");
    mupipt->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(2)->SetLogy();
    mupip->SetStats(0);
    mupip->SetTitle("#mu^{#pm} detected as #pi^{#pm}; cos(#theta); Momentum (GeV)   ");
    mupip->Draw("COLZ");
    mupip->GetYaxis()->SetTitleOffset(0.7);

    c1->cd(3)->SetLogy();
    mupt_eff->SetStats(0);
    mupt_eff->SetTitle("#mu^{#pm} eff; cos(#theta); Pt (GeV)   ");
    mupt_eff->Draw("COLZ");
    mupt_eff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(4)->SetLogy();
    mup_eff->SetStats(0);
    mup_eff->SetTitle("#mu^{#pm} eff; cos(#theta); Momentum (GeV)   ");
    mup_eff->Draw("COLZ");
    mup_eff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(5)->SetLogy();
    auto mupt_diff = (TH2F*) mumupt->Clone();
    *mupt_diff = *mupt_diff - *mupipt;
    mupt_diff->SetStats(0);
    mupt_diff->SetTitle("#mu^{#pm} diff; cos(#theta); Pt (GeV)   ");
    mupt_diff->Draw("COLZ");
    mupt_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(6)->SetLogy();
    auto mup_diff = (TH2F*) mumup->Clone();
    *mup_diff = *mup_diff - *mupip;
    mup_diff->SetStats(0);
    mup_diff->SetTitle("#mu^{#pm} diff; cos(#theta); Momentum (GeV)   ");
    mup_diff->Draw("COLZ");
    mup_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->SaveAs("mupi" + ifname + ".pdf");
}