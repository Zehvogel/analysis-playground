from __future__ import print_function
import ROOT
import math as m
# from array import array
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix
import numpy as np


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


# rmap = {
#     11: 0,
#     -11: 1,
#     13: 2,
#     -13: 3,
#     -211: 4,
#     211: 5,
#     22: 6
# }
# n_xbins = len(rmap)
# n_ybins = 4
# H = ROOT.TH2F("confusion", ";Reco;MC", n_xbins, array('f', range(n_xbins + 1)), n_ybins, array('f', range(n_ybins + 1)))

lcReader = ROOT.IOIMPL.LCFactory.getInstance().createLCReader()
lcReader.open("../ZHDecayMode.slcio")
r_labels = []
mc_labels = []

for evt in events(lcReader):
    particles = evt.getCollection("MCParticlesSkimmed")
    MCTruthRecoLink = evt.getCollection("MCTruthRecoLink")
    nav = ROOT.UTIL.LCRelationNavigator(MCTruthRecoLink)

    for particle in particles:
        if particle.getGeneratorStatus() != 2 and not (bhadron(particle) or chadron(particle)):
            continue
        daughters = particle.getDaughters()
        d_stable = [x for x in daughters if x.getGeneratorStatus() == 1]
        if len(d_stable) < 2:
            continue
        d_lnu = [x for x in d_stable if abs(
            x.getPDG()) in leptons or abs(x.getPDG()) in neutrinos]
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
        # print("mc: {0}, reco: {1}".format(d_l.getPDG(), reco.getType()))
        r_labels.append(reco.getType())
        mc_labels.append(d_l.getPDG())
        # H.Fill(rmap[reco.getType()], rmap[d_l.getPDG()])
        # only sld l/nu left


lcReader.close()

accepted_particles = [11, -11, 13, -13]
all_particles = accepted_particles + [211, -211, 22]

mat = np.asarray(confusion_matrix(mc_labels, r_labels, labels=all_particles, normalize="true"))
mat = mat[:len(accepted_particles) - len(all_particles), :]
fig, ax = plt.subplots()
im = ax.imshow(mat)

ax.set_xticks(np.arange(len(all_particles)))
ax.set_yticks(np.arange(len(accepted_particles)))
ax.set_xticklabels(all_particles)
ax.set_yticklabels(accepted_particles)

# Loop over data dimensions and create text annotations.
for i in range(len(accepted_particles)):
    for j in range(len(all_particles)):
        text = ax.text(j, i, "{0:.2f}".format(mat[i, j]),
                       ha="center", va="center", color="w" if mat[i, j] < 0.5 else "b")

fig.tight_layout()
plt.show()

print(mat)
# H.Draw()
# input('Press <ret> to end -> ')
