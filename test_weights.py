from __future__ import print_function
import ROOT


def events(lcReader):
    while True:
        evt = lcReader.readNextEvent()
        if not evt:
            break
        else:
            yield evt


lcReader = ROOT.IOIMPL.LCFactory.getInstance().createLCReader()
# does not want to work now :(
# lcReader.setReadCollectionNames(["PandoraPFOs", "Durham_2JetsPFOs", "MCParticlesSkimmed", "RecoMCTruthLink"])
lcReader.open("../ZHDecayMode.slcio")

for evt in events(lcReader):
    particles = evt.getCollection("Durham_2JetsPFOs")
    mctruth = evt.getCollection("RecoMCTruthLink")
    nav = ROOT.UTIL.LCRelationNavigator(mctruth)

    for i, particle in enumerate(particles):
        rec = particle.getType()
        true = [x.getPDG() for x in nav.getRelatedToObjects(particle)]
        if len(true) < 2:
            continue
        eweights = [int(x) / 10000 for x in nav.getRelatedToWeights(particle)]
        tweights = [int(x) % 10000 for x in nav.getRelatedToWeights(particle)]
        print("rec: {0}, mc: {1}, eweights: {2}, tweights: {3}".format(rec, true, eweights, tweights))
        
lcReader.close()
