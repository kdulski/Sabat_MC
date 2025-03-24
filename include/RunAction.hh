#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4ParticleDefinition.hh"
#include "G4UserRunAction.hh"
#include "InitConfig.hh"
#include "G4Run.hh"

class RunAction : public G4UserRunAction
{
public:
  RunAction();
  ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void AddSecondary(const G4ParticleDefinition*, G4double energy);
  void AddTrackLength(G4double length);
  void GetFilenameAddFrom(std::string file) {fOutputAddFile = file;};
  std::string GetAddition();
  std::string GetFlagForAlphaDetectorFields() {return fIncludeAlphaDetectorFields;};
private:
  std::string fOutputAddFile = "";
  std::string fIncludeAlphaDetectorFields = "t";
};

#endif
