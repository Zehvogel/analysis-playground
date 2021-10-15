import ROOT
import numpy as np
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
import matplotlib.pyplot as plt


def events(lcReader):
    while True:
        evt = lcReader.readNextEvent()
        if not evt:
            break
        else:
            yield evt


lcReader = ROOT.IOIMPL.LCFactory.getInstance().createLCReader()
lcReader.setReadCollectionNames(
    ["PandoraPFOs", "Durham_2JetsPFOs", "MCParticlesSkimmed", "RecoMCTruthLink"])
lcReader.open("../ZHDecayMode.slcio")

accepted_particles = [22, 11, -11, 13, -13, 211, -211]

all_rec_labels = []
all_true_labels = []

# while evt := lcReader.readNextEvent():
for evt in events(lcReader):
    particles = evt.getCollection("Durham_2JetsPFOs")
    mctruth = evt.getCollection("RecoMCTruthLink")
    nav = ROOT.UTIL.LCRelationNavigator(mctruth)

    for particle in particles:
        rec = particle.getType()
        if rec not in accepted_particles:
            rec = 0
        mcparticles = nav.getRelatedToObjects(particle)
        mcweights = nav.getRelatedToWeights(particle)
        stable = [x.getGeneratorStatus() == 1 for x in mcparticles]
        # filter mcparticles to a more sensible subset
        stable_mcparticles = []
        stable_mcweights = []
        for i, p in enumerate(mcparticles):
            if stable[i]:
                stable_mcparticles.append(p)
                stable_mcweights.append(mcweights[i])
        if len(stable_mcparticles) == 0:
            continue
        mcPDGs = [x.getPDG() for x in stable_mcparticles]
        mc_eweights = [int(x) / 10000 for x in stable_mcweights]
        mc_tweights = [int(x) % 10000 for x in stable_mcweights]
        true = mcPDGs[mc_eweights.index(max(mc_eweights))]
        if true not in accepted_particles:
            true = 0
        all_rec_labels.append(rec)
        all_true_labels.append(true)

# cutoff = 5000
# rec_mask = all_rec_labels < cutoff
# true_mask = all_true_labels < cutoff
# mask = rec_mask & true_mask
# all_rec_labels = all_rec_labels[mask]
# all_true_labels = all_true_labels[mask]

labels = accepted_particles + [0]

lcReader.close()
ConfusionMatrixDisplay.from_predictions(
    all_true_labels, all_rec_labels, normalize="true", values_format=".1f", labels=labels)
plt.show()
