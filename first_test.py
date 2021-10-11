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

all_rec_labels = np.empty(0, dtype=np.int32)
all_true_labels = np.empty(0, dtype=np.int32)

# while evt := lcReader.readNextEvent():
for evt in events(lcReader):
    particles = evt.getCollection("Durham_2JetsPFOs")
    mctruth = evt.getCollection("RecoMCTruthLink")
    nav = ROOT.UTIL.LCRelationNavigator(mctruth)

    rec_labels = np.zeros(len(particles), dtype=np.int32)
    true_labels = np.zeros(len(particles), dtype=np.int32)

    for i, particle in enumerate(particles):
        rec = particle.getType()
        true = nav.getRelatedToObjects(particle)[0].getPDG()
        rec_labels[i] = rec
        true_labels[i] = true
    all_rec_labels = np.append(all_rec_labels, rec_labels)
    all_true_labels = np.append(all_true_labels, true_labels)

# cutoff = 5000
# rec_mask = all_rec_labels < cutoff
# true_mask = all_true_labels < cutoff
# mask = rec_mask & true_mask
# all_rec_labels = all_rec_labels[mask]
# all_true_labels = all_true_labels[mask]

labels = [22, 11, -11, 13, -13, 211, -211, 2112]

lcReader.close()
ConfusionMatrixDisplay.from_predictions(
    all_true_labels, all_rec_labels,
    normalize="true", values_format=".1f", labels=labels)
plt.show()
