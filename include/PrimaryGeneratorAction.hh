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
  void removeNeutronGen() {fShootNeutron = false;};
  void removeAlphaGen() {fShootAlpha = false;};
  void setNeutronEnergy(G4double energy) {fNeutronEnergy = energy;};
  void setAlphaEnergy(G4double energy) {fAlphaEnergy = energy;};
  void setSourcePosition(G4double yPosition) {fSourcePositionY = yPosition;};

  void GeneratePrimaries(G4Event* anEvent) override;
private:
  PrimaryGeneratorMessenger* fPrimGenMess;

  bool fShootNeutron = true;
  bool fShootAlpha = true;

  G4double fNeutronEnergy = 14.1*MeV;
  G4double fAlphaEnergy = 3.49*MeV;
  G4double fSourcePositionY = -15*cm;

  G4ParticleGun* fGun;
};

#endif
