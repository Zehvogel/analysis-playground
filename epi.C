#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>

void epi(const TString ifname = "300k")
{
    std::unique_ptr<TFile> f(TFile::Open("sld_out_" + ifname + ".root"));
    
    //f->ls();

    std::unique_ptr<TH2F> epipt(f->Get<TH2F>("epipt"));
    std::unique_ptr<TH2F> epip(f->Get<TH2F>("epip"));
    std::unique_ptr<TH2F> eept(f->Get<TH2F>("eept"));
    std::unique_ptr<TH2F> eep(f->Get<TH2F>("eep"));
    std::unique_ptr<TH2F> ept_eff(f->Get<TH2F>("ept_eff"));
    std::unique_ptr<TH2F> ep_eff(f->Get<TH2F>("ep_eff"));
    //std::unique_ptr<TH2F> ept_all(f->Get<TH2F>("ept_all"));
    //std::unique_ptr<TH2F> ep_all(f->Get<TH2F>("ep_all"));
    
    auto c1 = new TCanvas("epi");
    c1->Divide(2,3);

    c1->cd(1)->SetLogy();
    epipt->SetStats(0);
    epipt->SetTitle("e^{#pm} detected as #pi^{#pm}; cos(#theta); Pt (GeV)   ");
    epipt->Draw("COLZ");
    epipt->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(2)->SetLogy();
    epip->SetStats(0);
    epip->SetTitle("e^{#pm} detected as #pi^{#pm}; cos(#theta); Momentum (GeV)   ");
    epip->Draw("COLZ");
    epip->GetYaxis()->SetTitleOffset(0.7);

    c1->cd(3)->SetLogy();
    ept_eff->SetStats(0);
    ept_eff->SetTitle("e^{#pm} eff; cos(#theta); Pt (GeV)   ");
    ept_eff->Draw("COLZ");
    ept_eff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(4)->SetLogy();
    ep_eff->SetStats(0);
    ep_eff->SetTitle("e^{#pm} eff; cos(#theta); Momentum (GeV)   ");
    ep_eff->Draw("COLZ");
    ep_eff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(5)->SetLogy();
    auto ept_diff = (TH2F*) eept->Clone();
    *ept_diff = *ept_diff - *epipt;
    ept_diff->SetStats(0);
    ept_diff->SetTitle("e^{#pm} diff; cos(#theta); Pt (GeV)   ");
    ept_diff->Draw("COLZ");
    ept_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(6)->SetLogy();
    auto ep_diff = (TH2F*) eep->Clone();
    *ep_diff = *ep_diff - *epip;
    ep_diff->SetStats(0);
    ep_diff->SetTitle("e^{#pm} diff; cos(#theta); Momentum (GeV)   ");
    ep_diff->Draw("COLZ");
    ep_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->SaveAs("epi" + ifname + ".pdf");
}