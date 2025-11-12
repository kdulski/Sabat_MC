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
/// \file DetectorMessenger.cc

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"


DetectorMessenger::DetectorMessenger(DetectorConstruction* detCons) : G4UImessenger(), fDet(detCons)
{
  fDetDir = new G4UIdirectory("/sabat/det/");
  fDetDir->SetGuidance("Detector list commands");

  fSetTargetMaterial = new G4UIcmdWithAString("/sabat/det/changeTargetMaterial", this);
  fSetTargetMaterial->SetGuidance("Option to change material of the target - Water, MustardGas, TNT, Clark1, Clark2 adn Adamsite");

  fSetGeometryVersion = new G4UIcmdWithAString("/sabat/det/setGeometryVersion", this);
  fSetGeometryVersion->SetGuidance("Option to change geometry version - V1, V2");

  fSetTargetVersion = new G4UIcmdWithAString("/sabat/det/setTargetVersion", this);
  fSetTargetVersion->SetGuidance("Option to change target version - ammo, ship");

  fSetTargetDetectorDistance = new G4UIcmdWithADoubleAndUnit("/sabat/det/setTargetDetectorDistance", this);
  fSetTargetDetectorDistance->SetGuidance("Set the distance between target and the setup");
  fSetTargetDetectorDistance->SetDefaultValue(15*cm);
  fSetTargetDetectorDistance->SetUnitCandidates("cm");

  fSetTargetWallThickness = new G4UIcmdWithADoubleAndUnit("/sabat/det/setTargetWallThickness", this);
  fSetTargetWallThickness->SetGuidance("Set the target wall thickness");
  fSetTargetWallThickness->SetDefaultValue(30*mm);
  fSetTargetWallThickness->SetUnitCandidates("mm");
}

DetectorMessenger::~DetectorMessenger()
{
  delete fDetDir;
  delete fSetTargetMaterial;
  delete fSetGeometryVersion;
  delete fSetTargetVersion;
  delete fSetTargetDetectorDistance;
  delete fSetTargetWallThickness;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fSetTargetMaterial) {
    if (newValue == "Water" || newValue == "water" || newValue == "W" || newValue == "w") {
      fDet->SetTarget(TargetVariables::fWater);
    } else if (newValue == "Mustard" || newValue == "mustard" ||newValue == "MustardGas" || newValue == "mustardgas" ||
               newValue == "M" || newValue == "m" || newValue == "MG" || newValue == "mg") {
      fDet->SetTarget(TargetVariables::fMustardGas);
    } else if (newValue == "TNT" || newValue == "tnt" || newValue == "T" || newValue == "t") {
      fDet->SetTarget(TargetVariables::fTNT);
    } else if (newValue == "Clark1" || newValue == "clark1" || newValue == "C1" || newValue == "c1") {
      fDet->SetTarget(TargetVariables::fClark1);
    } else if (newValue == "Clark2" || newValue == "clark2" || newValue == "C2" || newValue == "c2") {
      fDet->SetTarget(TargetVariables::fClark2);
    } else if (newValue == "Adamsite" || newValue == "adamsite" || newValue == "Adam" || newValue == "adam" || newValue == "A" || newValue == "a") {
      fDet->SetTarget(TargetVariables::fAdamsite);
    }
  } else if (command == fSetGeometryVersion) {
    if (newValue == "V1" || newValue == "v1" || newValue == "1") {
      fDet->SetGeometryVersion(GeometryVersion::fV1);
    } else if (newValue == "V2" || newValue == "v2" || newValue == "2") {
      fDet->SetGeometryVersion(GeometryVersion::fV2);
    } else {
      G4cout << "Unknown geometry version: " << newValue << G4endl;
    }
  } else if (command == fSetTargetVersion) {
    if (newValue == "Ammo" || newValue == "ammo" || newValue == "a") {
      fDet->SetTargetVersion(TargetVersion::fAmmu);
    } else if (newValue == "Ship" || newValue == "ship" || newValue == "s") {
      fDet->SetTargetVersion(TargetVersion::fShip);
    } else {
      G4cout << "Unknown target version: " << newValue << G4endl;
    }
  } else if (command == fSetTargetDetectorDistance) {
    fDet->SetTargetDetectorDistance(fSetTargetDetectorDistance->GetNewDoubleValue(newValue));
  } else if (command == fSetTargetWallThickness) {
    fDet->SetTargetWallThickness(fSetTargetWallThickness->GetNewDoubleValue(newValue));
  }
}
