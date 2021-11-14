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


#include <numeric>

using namespace lcio;

void binLogX(TH1 *h)
{
    auto axis = h->GetXaxis();
    int bins = axis->GetNbins();

    auto from = axis->GetXmin();
    auto to = axis->GetXmax();
    auto width = (to - from) / bins;
    auto new_bins = new Axis_t[bins + 1];

    for (int i = 0; i <= bins; i++) {
        new_bins[i] = TMath::Power(10, from + i * width);
    }
    axis->Set(bins, new_bins);
    delete[] new_bins;
}

void plot_SLDs(const char *fileName = "/afs/desy.de/group/flc/pool/reichenl/myMarlinProcessors/SLDLeptonSelector/test_out.slcio", const TString outname = "sld_out")
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
    int n_bins = 100;
    double width = (max_exp - min_exp) / n_bins;

    std::vector<Float_t> logbins(n_bins + 1);

    for (int i = 0; i <= n_bins; i++) {
        logbins[i] = TMath::Power(10, min_exp + i * width);
    }

    auto eptstack = new THStack("eptstack", ";p_{T}");
    auto eept = new TH1F("eept", ";p#_T", logbins.size() - 1, logbins.data());
    //auto eept = new TH1F("eept", ";p_T", 25, -1, 2);
    //binLogX(eept);
    eptstack->Add(eept);
    auto epipt = new TH1F("emupt", ";p#_T", logbins.size() - 1, logbins.data());
    // auto epipt = new TH1F("emupt", ";p_T", 25, -1, 2);
    //binLogX(epipt);
    epipt->SetLineColor(30);
    eptstack->Add(epipt);
    auto eopt = new TH1F("eopt", ";p#_T", logbins.size() - 1, logbins.data());
    // auto eopt = new TH1F("eopt", ";p_T", 25, -1, 2);
    //binLogX(eopt);
    eopt->SetLineColor(9);
    eptstack->Add(eopt);

    auto muptstack = new THStack("muptstack", ";p_{T}");
    auto mumupt = new TH1F("mumupt", ";p#_T", logbins.size() - 1, logbins.data());
    // auto mumupt = new TH1F("mumupt", ";p_T", 25, -1, 2);
    //binLogX(mumupt);
    muptstack->Add(mumupt);
    auto mupipt = new TH1F("mupipt", ";p#_T", logbins.size() - 1, logbins.data());
    //auto mupipt = new TH1F("mupipt", ";p_T", 25, -1, 2);
    //binLogX(mupipt);
    mupipt->SetLineColor(30);
    muptstack->Add(mupipt);
    auto muopt = new TH1F("muopt", ";p#_T", logbins.size() - 1, logbins.data());
    // auto muopt = new TH1F("muopt", ";p_T", 25, -1, 2);
    //binLogX(muopt);
    muopt->SetLineColor(9);
    muptstack->Add(muopt);

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

            switch (mcPDG)
            {
            case 11:
                switch (recPDG)
                {
                case 11:
                    eept->Fill(P.Perp());
                    break;

                case -211:
                    epipt->Fill(P.Perp());
                    break;

                case 0:
                    eopt->Fill(P.Perp());
                    break;

                default:
                    break;
                }
                break;

            case 13:
                switch (recPDG)
                {
                case 13:
                    mumupt->Fill(P.Perp());
                    break;

                case -211:
                    mupipt->Fill(P.Perp());
                    break;

                case 0:
                    muopt->Fill(P.Perp());
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
    c2->Divide(1, 2);

    auto pad1 = c2->cd(1);
    pad1->SetLogx();
    eptstack->Draw();
    auto pad2 = c2->cd(2);
    pad2->SetLogx();
    muptstack->Draw();

    c2->SaveAs(outname + "_emu.pdf");
}