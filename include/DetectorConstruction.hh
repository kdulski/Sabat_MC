/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class (Mandatory)

#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "PrimaryGeneratorAction.hh"
#include "DetectorMessenger.hh"
#include <G4VUserDetectorConstruction.hh>
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include<string>

class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

enum TargetVariables {
  fWater, fMustardGas, fTNT, fClark1, fClark2, fAdamsite
};
enum GeometryVersion {
  fV1, fV2
};

/// Detector construction class to define materials (with their physical properties) and detector geometry.
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  /// constructor
  DetectorConstruction();
  /// destructor
  virtual ~DetectorConstruction();

  G4VPhysicalVolume* Construct() override;

  //Alternative geometry construction
  void ConstructMaterials();
  G4VPhysicalVolume* ConstructV1();
  G4VPhysicalVolume* ConstructV2();
  
  void ConstructSDandField() override;
  void SetPrimGen(PrimaryGeneratorAction* primGen) {fPrimGen = primGen;};
  void SetTarget(TargetVariables target) {targetType = target;};
  void SetGeometryVersion(GeometryVersion version) {geometryVersion = version;};

  void SetCADFilename(std::string name) {
    filename = name;
  };
  void SetCADFiletype(std::string type) {
    filetype = type;
  };

  G4ThreeVector GetSourcePos() {return sourcePos;};
  
private:
  DetectorMessenger* fDetMess;
  PrimaryGeneratorAction* fPrimGen;
  G4ThreeVector offset;
  std::string filename;
  std::string filetype;

  TargetVariables targetType = TargetVariables::fWater;
  GeometryVersion geometryVersion = GeometryVersion::fV2;
  G4ThreeVector sourcePos;
      
  G4VSolid *cad_solid;
  G4LogicalVolume * cad_logical;
  G4VPhysicalVolume *cad_physical;

  //Materials
  G4Material* fSeaWater;
  G4Material* fSandSediment;
  G4Material* fLCSt;
  G4Material* fTargetMat;
  G4Material* fAir;
  G4Material* fLaBr3_Ce;
  G4Material* fPolypropylene;
  G4Material* fVacuum;
  G4Material* fVetoMat;
  G4Material* fIron;
  G4Material* fLead;
};

#endif
