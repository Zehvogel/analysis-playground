#ifndef __CINT__
//put LCIO includes here
#include <lcio.h>
#include <IO/LCReader.h>
#include <IOIMPL/LCFactory.h>
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>
#include <UTIL/LCIterator.h>
#include <UTIL/LCRelationNavigator.h>
#endif

// put ROOT includes here
#include <TH2F.h>
#include <TCanvas.h>
#include <TVector3.h>
#include <THStack.h>
#include <TStyle.h>
#include <TFile.h>


#include <numeric>

using namespace lcio;

void plot_SLDs(const char *fileName = "/afs/desy.de/group/flc/pool/reichenl/myMarlinProcessors/SLDLeptonSelector/test_out.slcio", const TString outname = "sld_out", Int_t cosbins = 10, Int_t ptbins = 10)
{
    const char *mcColName = "MCSLDLeptons";
    const char *relColname = "SLDMCRecoLink";

    std::map<int, Int_t> rmap = {
        {11, 0},
        {-11, 1},
        {13, 2},
        {-13, 3},
        {-211, 4},
        {211, 5},
        {22, 6},
        {0, 7}
    };

    Int_t n_xbins = rmap.size();
    Int_t n_ybins = 4; // e, eb, mu, mub
    auto xbins = std::vector<Float_t>(n_xbins + 1);
    std::iota(xbins.begin(), xbins.end(), 0.0);
    auto ybins = std::vector<Float_t>(n_ybins + 1);
    std::iota(ybins.begin(), ybins.end(), 0.0);
    auto h = new TH2F("confusion", ";Reco; MC", n_xbins, xbins.data(), n_ybins, ybins.data());
    h->SetStats(0);

    double min_exp = -1;
    double max_exp = TMath::Log10(50);
    double width = (max_exp - min_exp) / ptbins;

    std::vector<Double_t> logbins(ptbins + 1);

    for (int i = 0; i <= ptbins; i++) {
        logbins[i] = TMath::Power(10, min_exp + i * width);
    }

    auto eept = new TH2F("eept", "", cosbins, -1.0, 1.0, logbins.size() - 1, logbins.data());
    auto epipt = new TH2F("epipt", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto eopt = new TH2F("eopt", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());

    auto mumupt = new TH2F("mumupt", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto mupipt = new TH2F("mupipt", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto muopt = new TH2F("muopt", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());

    auto eep = new TH2F("eep", "", cosbins, -1.0, 1.0, logbins.size() - 1, logbins.data());
    auto epip = new TH2F("epip", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto eop = new TH2F("eop", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());

    auto mumup = new TH2F("mumup", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto mupip = new TH2F("mupip", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());
    auto muop = new TH2F("muop", "", cosbins, -1, 1, logbins.size() - 1, logbins.data());

    auto lcReader = LCFactory::getInstance()->createLCReader();
    lcReader->open(fileName);

    while (const auto evt = lcReader->readNextEvent()) {
        auto rel = evt->getCollection(relColname);
        auto nav = std::make_unique<LCRelationNavigator>(rel);

        //iterate over mc sld leptons
        LCIterator<MCParticle> mcp_iter(evt, mcColName);
        while (const auto mcp = mcp_iter.next()) {

            //get related rec pfo
            auto reco = dynamic_cast<ReconstructedParticle *>(nav->getRelatedToObjects(mcp)[0]);

            int recPDG = 0;
            if (reco != NULL) {
                recPDG = reco->getType();
            };
            int mcPDG = mcp->getPDG();

            // std::cout << "[mc, rec]: [" << mcPDG << ", " << recPDG << "]" << std::endl;

            h->Fill(rmap[recPDG], rmap[mcPDG]);

            TVector3 P(mcp->getMomentum());

            switch (abs(mcPDG))
            {
            case 11:
                switch (abs(recPDG))
                {
                case 11:
                    eept->Fill(P.CosTheta(), P.Perp());
                    eep->Fill(P.CosTheta(), P.Mag());
                    break;

                case 211:
                    epipt->Fill(P.CosTheta(), P.Perp());
                    epip->Fill(P.CosTheta(), P.Mag());
                    break;

                case 0:
                    eopt->Fill(P.CosTheta(), P.Perp());
                    eop->Fill(P.CosTheta(), P.Mag());
                    break;

                default:
                    break;
                }
                break;

            case 13:
                switch (abs(recPDG))
                {
                case 13:
                    mumupt->Fill(P.CosTheta(), P.Perp());
                    mumup->Fill(P.CosTheta(), P.Mag());
                    break;

                case 211:
                    mupipt->Fill(P.CosTheta(), P.Perp());
                    mupip->Fill(P.CosTheta(), P.Mag());
                    break;

                case 0:
                    muopt->Fill(P.CosTheta(), P.Perp());
                    muop->Fill(P.CosTheta(), P.Mag());
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }
    lcReader->close();

    gStyle->SetPalette(112);

    auto c1 = new TCanvas("confc", "foo", 800, 400);

    // TODO: set labels
    auto xaxis = h->GetXaxis();
    auto yaxis = h->GetYaxis();
    xaxis->SetBinLabel(1, "11");
    xaxis->SetBinLabel(2, "-11");
    xaxis->SetBinLabel(3, "13");
    xaxis->SetBinLabel(4, "-13");
    xaxis->SetBinLabel(5, "-211");
    xaxis->SetBinLabel(6, "211");
    xaxis->SetBinLabel(7, "22");
    xaxis->SetBinLabel(8, "none");
    yaxis->SetBinLabel(1, "11");
    yaxis->SetBinLabel(2, "-11");
    yaxis->SetBinLabel(3, "13");
    yaxis->SetBinLabel(4, "-13");
    h->SetMarkerSize(4);
    h->SetMarkerColor(0);
    c1->cd();
    h->Draw("COLZ TEXT");

    c1->SaveAs(outname + ".pdf");

    auto c2 = new TCanvas("emuhists", "");
    c2->Divide(2, 2);

    auto pad1 = c2->cd(1);
    pad1->SetLogy();
    auto ept_all = (TH2F*) eopt->Clone();
    ept_all->Add(epipt);
    ept_all->Add(eept);
    auto ept_eff = (TH2F*) eept->Clone();
    ept_eff->Divide(ept_all);
    ept_eff->SetStats(0);
    ept_eff->SetTitle("e; cos(#theta);Pt (GeV)");
    ept_eff->Draw("COLZ");
    ept_eff->GetYaxis()->SetTitleOffset(0.7);

    auto pad2 = c2->cd(2);
    pad2->SetLogy();
    auto mupt_all = (TH2F*) muopt->Clone();
    mupt_all->Add(mupipt);
    mupt_all->Add(mumupt);
    auto mupt_eff = (TH2F*) mumupt->Clone();
    mupt_eff->Divide(mupt_all);
    mupt_eff->SetStats(0);
    mupt_eff->SetTitle("#mu; cos(#theta);Pt (GeV)");
    mupt_eff->Draw("COLZ");
    mupt_eff->GetYaxis()->SetTitleOffset(0.7);

    auto pad3 = c2->cd(3);
    pad3->SetLogy();
    auto ep_all = (TH2F*) eop->Clone();
    ep_all->Add(epip);
    ep_all->Add(eep);
    auto ep_eff = (TH2F*) eep->Clone();
    ep_eff->Divide(ep_all);
    ep_eff->SetStats(0);
    ep_eff->SetTitle("e; cos(#theta);Momentum (GeV)");
    ep_eff->Draw("COLZ");
    ep_eff->GetYaxis()->SetTitleOffset(0.7);

    auto pad4 = c2->cd(4);
    pad4->SetLogy();
    auto mup_all = (TH2F*) muop->Clone();
    mup_all->Add(mupip);
    mup_all->Add(mumup);
    auto mup_eff = (TH2F*) mumup->Clone();
    mup_eff->Divide(mup_all);
    mup_eff->SetStats(0);
    mup_eff->SetTitle("#mu; cos(#theta);Momentum (GeV)");
    mup_eff->Draw("COLZ");
    mup_eff->GetYaxis()->SetTitleOffset(0.7);

    auto outfile = TFile(outname + ".root", "RECREATE");

    h->Write();

    ept_all->SetName("ept_all");
    ept_eff->SetName("ept_eff");
    ep_all->SetName("ep_all");
    ep_eff->SetName("ep_eff");
    mupt_all->SetName("mupt_all");
    mupt_eff->SetName("mupt_eff");
    mup_all->SetName("mup_all");
    mup_eff->SetName("mup_eff");

    eept->Write();
    epipt->Write();
    eopt->Write();
    ept_all->Write();
    ept_eff->Write();

    eep->Write();
    epip->Write();
    eop->Write();
    ep_all->Write();
    ep_eff->Write();

    mumupt->Write();
    mupipt->Write();
    muopt->Write();
    mupt_all->Write();
    mupt_eff->Write();

    mumup->Write();
    mupip->Write();
    muop->Write();
    mup_all->Write();
    mup_eff->Write();

    outfile.Close();
    
    c2->SaveAs(outname + "_emu.pdf");
}