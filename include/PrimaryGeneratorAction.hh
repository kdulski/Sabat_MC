/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction();

  G4ParticleGun* GetParticleGun() {return fGun;}
  void RemoveNeutronGen() {fShootNeutron = false;};
  void RemoveAlphaGen() {fShootAlpha = false;};
  void SetNeutronEnergy(G4double energy) {fNeutronEnergy = energy;};
  void SetAlphaEnergy(G4double energy) {fAlphaEnergy = energy;};
  void SetSourcePosition(G4ThreeVector position) {fSourcePosition = position;};
  void SetAlwaysAlphaInVetoTrue() {fAlwaysVeto = true;};
  void SetVetoShiftAndDimensions(double shift, G4ThreeVector dimensions) {
    fVetoShiftFromSource = shift;
    fVetoDimensions = dimensions;
  };

  std::pair<G4double, G4double> CalcAnglesForVeto();
  void GeneratePrimaries(G4Event* anEvent) override;
private:
  PrimaryGeneratorMessenger* fPrimGenMess;

  bool fShootNeutron = true;
  bool fShootAlpha = true;
  bool fAlwaysVeto = false;

  G4double fNeutronEnergy = 14.1*MeV;
  G4double fAlphaEnergy = 3.49*MeV;
  G4double fSourcePositionY = 0*cm;
  G4ThreeVector fSourcePosition; // setting it from the level of DetectorConstruction after constructing the source
  double fVetoShiftFromSource;
  G4ThreeVector fVetoDimensions;

  G4ParticleGun* fGun;
};

#endif
