//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file PrimaryGeneratorMessenger.cc

#include "PrimaryGeneratorMessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"


PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* primGenAct) : G4UImessenger(), fPrimGen(primGenAct)
{
  fPrimGenDir = new G4UIdirectory("/sabat/primGen/");
  fPrimGenDir->SetGuidance("Primary Generator list commands");

  fRemoveNeutronFromGen = new G4UIcmdWithoutParameter("/sabat/primGen/removeNeutron", this);
  fRemoveNeutronFromGen->SetGuidance("Option not to simulate neutron");

  fRemoveAlphaFromGen = new G4UIcmdWithoutParameter("/sabat/primGen/removeAlpha", this);
  fRemoveAlphaFromGen->SetGuidance("Option not to simulate alpha");

  fSetNeutronEnergy = new G4UIcmdWithADoubleAndUnit("/sabat/primGen/setNeutronEnergy", this);
  fSetNeutronEnergy->SetGuidance("Set the energy of the generated neutron");
  fSetNeutronEnergy->SetDefaultValue(14.1*MeV);
  fSetNeutronEnergy->SetUnitCandidates("MeV");

  fSetAlphaEnergy = new G4UIcmdWithADoubleAndUnit("/sabat/primGen/setAlphaEnergy", this);
  fSetAlphaEnergy->SetGuidance("Set the energy of the generated alpha");
  fSetAlphaEnergy->SetDefaultValue(3.49*MeV);
  fSetAlphaEnergy->SetUnitCandidates("MeV");

  fSetSourcePositionY = new G4UIcmdWithADoubleAndUnit("/sabat/primGen/setSourcePositionY", this);
  fSetSourcePositionY->SetGuidance("Set the Y position of the source");
  fSetSourcePositionY->SetDefaultValue(-15*cm);
  fSetSourcePositionY->SetUnitCandidates("cm");

  fRequireHitOfAlphaInVeto = new G4UIcmdWithoutParameter("/sabat/primGen/requireAlphaInVeto", this);
  fRequireHitOfAlphaInVeto->SetGuidance("Option not to simulate events when alpha always hit the Veto");
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fPrimGenDir;
  delete fRemoveNeutronFromGen;
  delete fRemoveAlphaFromGen;
  delete fSetNeutronEnergy;
  delete fSetAlphaEnergy;
  delete fSetSourcePositionY;
  delete fRequireHitOfAlphaInVeto;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fRemoveNeutronFromGen) {
    fPrimGen->RemoveNeutronGen();
  } else if (command == fRemoveAlphaFromGen) {
    fPrimGen->RemoveAlphaGen();
  } else if (command == fSetNeutronEnergy) {
    fPrimGen->SetNeutronEnergy(fSetNeutronEnergy->GetNewDoubleValue(newValue));
  } else if (command == fSetAlphaEnergy) {
    fPrimGen->SetAlphaEnergy(fSetAlphaEnergy->GetNewDoubleValue(newValue));
  } else if (command == fSetSourcePositionY) {
    fPrimGen->SetSourcePosition(G4ThreeVector(0., fSetSourcePositionY->GetNewDoubleValue(newValue), 20. * cm));
  } else if (command == fRequireHitOfAlphaInVeto) {
    fPrimGen->SetAlwaysAlphaInVetoTrue();
  }
}
