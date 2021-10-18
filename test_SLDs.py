from __future__ import print_function
import ROOT
import math as m
from array import array


leptons = [11, 13]
neutrinos = [12, 14]

def events(lcReader):
    while True:
        evt = lcReader.readNextEvent()
        if not evt:
            break
        else:
            yield evt
            

def hadron(particle, quarkID):
    pdg = particle.getPDG()
    return pdg / 100 == quarkID or pdg / 1000 == quarkID


def bhadron(particle):
    return hadron(particle, 5)


def chadron(particle):
    return hadron(particle, 4)


def lepton(particle):
    pdg = particle.getPDG()
    return pdg in leptons


def neutrino(particle):
    pdg = particle.getPDG()
    return pdg in neutrinos

rmap = {
    11: 0,
    -11: 1,
    13: 2,
    -13: 3,
    -211: 4,
    211: 5,
    22: 6
}
n_xbins = len(rmap)
n_ybins = 4
H = ROOT.TH2F("confusion", ";Reco;MC", n_xbins, array('f', range(n_xbins + 1)), n_ybins, array('f', range(n_ybins + 1)))

lcReader = ROOT.IOIMPL.LCFactory.getInstance().createLCReader()
lcReader.open("../ZHDecayMode.slcio")

for evt in events(lcReader):
    particles = evt.getCollection("MCParticlesSkimmed")
    MCTruthRecoLink = evt.getCollection("MCTruthRecoLink")
    nav = ROOT.UTIL.LCRelationNavigator(MCTruthRecoLink)

    for particle in particles:
        if particle.getGeneratorStatus() != 2 and  not (bhadron(particle) or chadron(particle)):
            continue
        daughters = particle.getDaughters()
        d_stable = [x for x in daughters if x.getGeneratorStatus() == 1]
        if len(d_stable) < 2:
            continue
        d_lnu = [x for x in d_stable if abs(x.getPDG()) in leptons or abs(x.getPDG()) in neutrinos]
        if len(d_lnu) < 2:
            continue
        d_pdg = [x.getPDG() for x in d_lnu]
        if abs(sum(d_pdg)) != 1:
            continue
        d_l = [x for x in d_lnu if abs(x.getPDG()) in leptons][0]
        # cut
        p_x = d_l.getMomentum()[0]
        p_y = d_l.getMomentum()[1]
        p_z = d_l.getMomentum()[2]
        p_T = m.sqrt(p_x**2 + p_y**2)
        cos_t = p_z / m.sqrt(p_x**2 + p_y**2 + p_z**2)
        if p_T < 1.0 or cos_t > 0.99:
            continue
        # print("mc:      {}".format(d_pdg))
        recos = [x for x in nav.getRelatedToObjects(d_l)] 
        # print("reco:    {}".format([x.getType() for x in recos]))
        tr_weights = [int(x) % 10000 for x in nav.getRelatedToWeights(d_l)]
        # print("weights: {}".format(tr_weights))
        reco = recos[tr_weights.index(max(tr_weights))]
        print("mc: {0}, reco: {1}".format(d_l.getPDG(), reco.getType()))
        H.Fill(rmap[reco.getType()], rmap[d_l.getPDG()])
        # only sld l/nu left
            
        
lcReader.close()
H.Draw()
raw_input('Press <ret> to end -> ')
