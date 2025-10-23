/// \file PrimaryGeneratorAction.hh
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4RandomDirection.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "G4Neutron.hh"
// to fill neutron and alpha direction
#include "Analysis.hh"
using namespace CLHEP;


PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction()
{
  fPrimGenMess = new PrimaryGeneratorMessenger(this);
  fGun = new G4ParticleGun();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fGun;
}

std::pair<G4double, G4double> PrimaryGeneratorAction::CalcAnglesForVeto()
{
//Dimensions are given as a 2*half
//Simulation uniform on the plane, if needed simulate on a sphere and project onto plane
//Veto is now in the xz plane, so that is why y define the shift of the Veto from the source
  G4double x = 0.5*fVetoDimensions(0) - fVetoDimensions(0)*G4UniformRand();
  G4double z = 0.5*fVetoDimensions(2) - fVetoDimensions(2)*G4UniformRand();
  G4double y = -fVetoShiftFromSource;
  G4double radius = sqrt(x*x + y*y + z*z);
  G4double cosTheta = z/radius;
  G4double phi = y/fabs(y)*acos(x/sqrt(x*x + y*y));
  return std::make_pair(cosTheta, phi);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4ThreeVector dirAlp;
  G4double cosTheta, Phi;

  if (fAlwaysVeto) {
    std::pair<double, double> ThetaPhi = CalcAnglesForVeto();
    cosTheta = ThetaPhi.first;
    Phi = ThetaPhi.second;
  } else {
    cosTheta = 1. - 2*G4UniformRand();
    Phi = CLHEP::twopi*G4UniformRand();
  }
  G4double sinTheta = sqrt(1. - cosTheta*cosTheta);

  G4ThreeVector dirNeu(sinTheta * cos(Phi), sinTheta * sin(Phi), cosTheta);
  G4ThreeVector dirAlpha(-1*sinTheta * cos(Phi), -1*sinTheta * sin(Phi), -1*cosTheta);

  G4AnalysisManager* analysis = G4AnalysisManager::Instance();

  if (fShootNeutron) {
    fGun->SetParticleDefinition(G4Neutron::Definition());
    fGun->SetParticleEnergy(fNeutronEnergy);
    fGun->SetParticlePosition(fSourcePosition);
    fGun->SetParticleMomentumDirection(dirNeu);

    analysis->FillNtupleDColumn(17, dirNeu.theta()*(180/CLHEP::pi));
    analysis->FillNtupleDColumn(18, dirNeu.phi()*(180/CLHEP::pi));
    fGun->GeneratePrimaryVertex(anEvent);
  }

  if (fShootAlpha) {
    G4ParticleDefinition* myParticle = G4ParticleTable::GetParticleTable()->FindParticle("alpha");
    fGun->SetParticleDefinition(myParticle);
    fGun->SetParticleEnergy(fAlphaEnergy);
    fGun->SetParticlePosition(fSourcePosition);
    fGun->SetParticleMomentumDirection(dirAlpha);

    analysis->FillNtupleDColumn(23, dirAlpha.theta()*180/CLHEP::pi);
    analysis->FillNtupleDColumn(24, dirAlpha.phi()*180/CLHEP::pi);
    fGun->GeneratePrimaryVertex(anEvent);
  }
}
