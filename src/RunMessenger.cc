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


#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "RunMessenger.hh"
#include "RunAction.hh"


RunMessenger::RunMessenger(RunAction* runAct) : G4UImessenger(), fRun(runAct)
{
  fRunDir = new G4UIdirectory("/sabat/run/");
  fRunDir->SetGuidance("Run list commands");

  fGetNextFilenameAddFrom = new G4UIcmdWithAString("/sabat/run/filenameAddFrom", this);
  fGetNextFilenameAddFrom->SetGuidance("Obtaining the next addition to the file from the file");
}

RunMessenger::~RunMessenger()
{
  delete fRunDir;
  delete fGetNextFilenameAddFrom;
  delete fRemovingAlphaFieldsFromOutput;
}

void RunMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{   
  if (command == fGetNextFilenameAddFrom) {
    fRun->GetFilenameAddFrom(newValue);
  }
}
