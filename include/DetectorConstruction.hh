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
enum TargetVersion {
  fAmmu, fShip
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
  void ConstructTarget(G4LogicalVolume* logicWorld, G4ThreeVector targetCoverShift);
  void ConstructSDandField() override;
  void SetPrimGen(PrimaryGeneratorAction* primGen) {fPrimGen = primGen;};
  void SetGeometryVersion(GeometryVersion version) {geometryVersion = version;};
  void SetTarget(TargetVariables target) {targetType = target;};
  void SetTargetVersion(TargetVersion target) {targetVersion = target;};
  void SetTargetDetectorDistance(G4double distance) {targetDetectorDistance = distance;};
  void SetTargetWallThickness(G4double thickness) {targetWallSize = thickness;};

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
  TargetVersion targetVersion = TargetVersion::fShip;
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

  G4bool checkOverlaps = true;
  G4double targetWallSize = 30 * mm; // 3 mm for fAmmu
  G4double targetDetectorDistance = 0 * cm;
  G4double targetShiftY = 25 * cm; // 25*cm for fAmmu, 75*cm for fShip
};

#endif
