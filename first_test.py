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
            continue
        mcparticles = nav.getRelatedToObjects(particle)
        mcweights = nav.getRelatedToWeights(particle)
        stable = [x.getGeneratorStatus == 1 for x in mcparticles]
        # filter mcparticles to a more sensible subset
        for i, mcp  in enumerate(mcparticles):
            pdg = mcp.getPDG()
            if not stable[i] or pdg not in accepted_particles:
                mcparticles.remove(i)
                mcweights.remove(i)
        
                

# cutoff = 5000
# rec_mask = all_rec_labels < cutoff
# true_mask = all_true_labels < cutoff
# mask = rec_mask & true_mask
# all_rec_labels = all_rec_labels[mask]
# all_true_labels = all_true_labels[mask]


lcReader.close()
ConfusionMatrixDisplay.from_predictions(all_true_labels, all_rec_labels, normalize="true", values_format=".1f")
plt.show()
