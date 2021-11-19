#include <TH2F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TBox.h>
#include <iostream>

void mupi(const TString ifname = "1000k_100b", std::vector<enum EColorPalette> styles = {kViridis, kAvocado, kDarkBodyRadiator, kInvertedDarkBodyRadiator})
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
    c1->Divide(2,5);

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
    auto mupt_diff = (TH2F*) mumupt->Clone();
    //*mupt_diff = *mupt_diff - *mupipt;
    mupt_diff->SetStats(0);
    mupt_diff->SetTitle("#mu^{#pm} correct; cos(#theta); Pt (GeV)   ");
    mupt_diff->Draw("COLZ");
    mupt_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(4)->SetLogy();
    auto mup_diff = (TH2F*) mumup->Clone();
    //*mup_diff = *mup_diff - *mupip;
    mup_diff->SetStats(0);
    mup_diff->SetTitle("#mu^{#pm} correct; cos(#theta); Momentum (GeV)   ");
    mup_diff->Draw("COLZ");
    mup_diff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(5)->SetLogy();
    auto mupt_sum = (TH2F*) mumupt->Clone();
    *mupt_sum = *mupt_sum + *mupipt;
    mupt_sum->SetStats(0);
    mupt_sum->SetTitle("#mu^{#pm} correct + #pi; cos(#theta); Pt (GeV)   ");
    mupt_sum->Draw("COLZ");
    mupt_sum->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(6)->SetLogy();
    auto mup_sum = (TH2F*) mumup->Clone();
    *mup_sum = *mup_sum + *mupip;
    mup_sum->SetStats(0);
    mup_sum->SetTitle("#mu^{#pm} correct + #pi; cos(#theta); Momentum (GeV)   ");
    mup_sum->Draw("COLZ");
    mup_sum->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(7)->SetLogy();
    mupt_eff->SetStats(0);
    mupt_eff->SetTitle("#mu^{#pm} eff; cos(#theta); Pt (GeV)   ");
    mupt_eff->Draw("COLZ");
    mupt_eff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(8)->SetLogy();
    mup_eff->SetStats(0);
    mup_eff->SetTitle("#mu^{#pm} eff; cos(#theta); Momentum (GeV)   ");
    mup_eff->Draw("COLZ");
    mup_eff->GetYaxis()->SetTitleOffset(0.7);

    Int_t bin_xmin, bin_ymin, bin_xmax, bin_ymax, bin_min, bin_max, foo;
    bin_xmin = 0;
    bin_min = mup_eff->FindBin(-0.6, 3);
    bin_max = mup_eff->FindBin(0.6, 10);
    mup_eff->GetBinXYZ(bin_min, bin_xmin, bin_ymin, foo);
    mup_eff->GetBinXYZ(bin_max, bin_xmax, bin_ymax, foo);
    bin_xmax -= 2;
    bin_ymax -= 1;
    bin_ymin += 1;
    bin_xmin += 1;
    std::cout << "bin_min: " << bin_min << "\t bin_max: " << bin_max << std::endl;
    std::cout << "bin_xmin: " << bin_xmin << "\t bin_xmax: " << bin_xmax << std::endl;
    std::cout << "bin_ymin: " << bin_ymin << "\t bin_ymax: " << bin_ymax << std::endl;
    Double_t x1 = mup_eff->GetXaxis()->GetBinLowEdge(bin_xmin);
    Double_t x2 = mup_eff->GetXaxis()->GetBinUpEdge(bin_xmax);
    Double_t y1 = mup_eff->GetYaxis()->GetBinLowEdge(bin_ymin);
    Double_t y2 = mup_eff->GetYaxis()->GetBinUpEdge(bin_ymax);
    std::cout << "(x1, y1, x2, y2): " << x1 << ", " << y1 << ", " << x2 << ", " << y2 << std::endl; 
    TBox b1(x1, y1, x2, y2);
    b1.SetFillStyle(0);
    b1.SetLineColor(kRed);
    b1.SetLineWidth(1);
    b1.Draw();

    // create 1
    auto one = (TH2F*) mumupt->Clone();
    for (int i = 0; i < one->GetNcells(); i++) {
        one->SetBinContent(i, 1.0);
    }

    c1->cd(9)->SetLogy();
    auto mupt_ieff = (TH2F *) one->Clone();
    *mupt_ieff = *mupt_ieff - *mupt_eff;
    mupt_ieff->SetStats(0);
    mupt_ieff->SetTitle("#mu^{#pm} 1 - eff; cos(#theta); Momentum (GeV)   ");
    mupt_ieff->Draw("COLZ");
    mupt_ieff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->cd(10)->SetLogy();
    auto mup_ieff = (TH2F *) one->Clone();
    *mup_ieff = *mup_ieff - *mup_eff;
    mup_ieff->SetStats(0);
    mup_ieff->SetTitle("#mu^{#pm} 1 - eff; cos(#theta); Momentum (GeV)   ");
    mup_ieff->Draw("COLZ");
    mup_ieff->GetYaxis()->SetTitleOffset(0.7);
    
    c1->SaveAs("mupi" + ifname + ".pdf");
    
    for (auto e: styles) {
        gStyle->SetPalette(e);
        
        c1->SaveAs("mupi" + ifname + std::to_string(e) + ".pdf");
    }
}