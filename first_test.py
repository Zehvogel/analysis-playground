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
labels = accepted_particles + [0]

# matrix = np.zeros((len(labels), len(labels)), dtype=np.float64)
all_rec_labels = np.zeros(0)
all_true_labels = np.zeros(0)

# while evt := lcReader.readNextEvent():
for evt in events(lcReader):
    particles = evt.getCollection("Durham_2JetsPFOs")
    mctruth = evt.getCollection("RecoMCTruthLink")
    nav = ROOT.UTIL.LCRelationNavigator(mctruth)

    rec_labels = []
    true_labels = []
    for particle in particles:
        p_x = particle.getMomentum()[0]
        p_y = particle.getMomentum()[1]
        p_z = particle.getMomentum()[2]
        p_T = np.sqrt(p_x**2 + p_y**2)
        cos_t = p_z / np.sqrt(p_x**2 + p_y**2 + p_z**2)
        if p_T < 1.0 or cos_t > 0.99:
            continue
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
        if rec == 22: 
            true = mcPDGs[mc_eweights.index(max(mc_eweights))]
        else:
            true = mcPDGs[mc_tweights.index(max(mc_tweights))]
        if true not in accepted_particles:
            true = 0
        rec_labels.append(rec)
        true_labels.append(true)
    # matrix += confusion_matrix(np.asarray(true_labels), np.asarray(rec_labels), labels=labels)
    all_rec_labels = np.append(all_rec_labels, rec_labels)
    all_true_labels = np.append(all_true_labels, true_labels)

lcReader.close()
# print(matrix)

# fig, ax = plt.subplots()

# ax.matshow(matrix)
ConfusionMatrixDisplay.from_predictions(all_true_labels, all_rec_labels, normalize="true", labels=labels, values_format=".2f")
plt.show()
