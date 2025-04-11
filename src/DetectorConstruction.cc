// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4NistManager.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4UnitsTable.hh"
#include "G4Material.hh"
#include "G4Colour.hh"

#include "G4SubtractionSolid.hh"
// Geometries
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"

#include "G4MultiFunctionalDetector.hh"
#include <G4VSensitiveDetector.hh>
#include <G4SDManager.hh>

#include "SensitiveVetoSD.hh"
#include "SensitiveSD.hh"

#include <sstream>

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction()
{
  fDetMess = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{;}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4bool checkOverlaps = true;

  G4NistManager* man = G4NistManager::Instance();
     
  G4Element* Li = man->FindOrBuildElement("Li");
  G4Element* N  = man->FindOrBuildElement("N");
  G4Element* La = man->FindOrBuildElement("La");
  G4Element* Br = man->FindOrBuildElement("Br");
  G4Element* Ce = man->FindOrBuildElement("Ce");

  G4Material* Silicon = new G4Material("Silicon", 14., 28.0855*g/mole, 2.33*g/cm3);
  G4Material* VetoMat = Silicon;

  G4Element* H  = man->FindOrBuildElement("H");
  G4Element* B  = man->FindOrBuildElement("B");
  G4Element* C  = man->FindOrBuildElement("C");
  G4Element* O  = man->FindOrBuildElement("O");
  G4Element* F  = man->FindOrBuildElement("F");
  G4Element* Na = man->FindOrBuildElement("Na");
  G4Element* Mg = man->FindOrBuildElement("Mg");
  G4Element* Al = man->FindOrBuildElement("Al");
  G4Element* Si = man->FindOrBuildElement("Si");
  G4Element* S  = man->FindOrBuildElement("S");
  G4Element* Cl = man->FindOrBuildElement("Cl");
  G4Element* K  = man->FindOrBuildElement("K");
  G4Element* Ca = man->FindOrBuildElement("Ca");
  G4Element* V  = man->FindOrBuildElement("V");
  G4Element* Cr = man->FindOrBuildElement("Cr");
  G4Element* Fe = man->FindOrBuildElement("Fe");
  G4Element* Co = man->FindOrBuildElement("Co");
  G4Element* Ni = man->FindOrBuildElement("Ni");
  G4Element* Cu = man->FindOrBuildElement("Cu");
  G4Element* Zn = man->FindOrBuildElement("Zn");
  G4Element* As = man->FindOrBuildElement("As");
  G4Element* Sr = man->FindOrBuildElement("Sr");
  G4Element* Cd = man->FindOrBuildElement("Cd");
  G4Element* Ba = man->FindOrBuildElement("Ba");
  G4Element* Hg = man->FindOrBuildElement("Hg");
  G4Element* Pb = man->FindOrBuildElement("Pb");
      
// Sand Sediment - 27 elemental composition
  G4Material* SandSediment = new G4Material("SandSediment", 1.535*g/cm3, 27);
  SandSediment->AddElement(H, 3.30144*perCent);
  SandSediment->AddElement(B, 0.00003*perCent);
  SandSediment->AddElement(C, 0.35344*perCent);
  SandSediment->AddElement(O, 61.9999*perCent);
  SandSediment->AddElement(F, 0.00001*perCent);
  SandSediment->AddElement(Na, 1.34864*perCent);
  SandSediment->AddElement(Mg, 0.00734*perCent);
  SandSediment->AddElement(Al, 2.59482*perCent);
  SandSediment->AddElement(Si, 27.5365*perCent);
  SandSediment->AddElement(S, 0.00517*perCent);
  SandSediment->AddElement(Cl, 0.11055*perCent);
  SandSediment->AddElement(K, 0.87891*perCent);
  SandSediment->AddElement(Ca, 0.84806*perCent);
  SandSediment->AddElement(V,  0.00100*perCent);
  SandSediment->AddElement(Cr, 0.00100*perCent);
  SandSediment->AddElement(Fe, 1.00237*perCent);
  SandSediment->AddElement(Co, 0.00050*perCent);
  SandSediment->AddElement(Ni, 0.00100*perCent);
  SandSediment->AddElement(Cu, 0.00100*perCent);
  SandSediment->AddElement(Zn, 0.00200*perCent);
  SandSediment->AddElement(As, 0.00080*perCent);
  SandSediment->AddElement(Br, 0.00038*perCent);
  SandSediment->AddElement(Sr, 0.00104*perCent);
  SandSediment->AddElement(Cd, 0.00010*perCent);
  SandSediment->AddElement(Ba, 0.00250*perCent);
  SandSediment->AddElement(Hg, 0.000002*perCent);
  SandSediment->AddElement(Pb, 0.00150*perCent);
      
  G4Material *vacuum = man->FindOrBuildMaterial("G4_Galactic");

  G4int ncomponents;
  G4double density, temperature, pressure;

  density     = 0.001225*g/cm3;
  pressure    = 98658.96*pascal;
  temperature = 273*kelvin;

  G4Element* Ar = man->FindOrBuildElement("Ar");

  G4Material* Air = new G4Material("Air", density, ncomponents=4, kStateGas, temperature, pressure);
  Air->AddElement(N, 75.53*perCent);
  Air->AddElement(O, 23.18*perCent);
  Air->AddElement(Ar, 1.28*perCent);
  Air->AddElement(C, 0.01*perCent);

  G4NistManager *nist_man = G4NistManager::Instance();
  G4Material *AIR_mat = nist_man->FindOrBuildMaterial("G4_AIR");

  G4Material* H2O = new G4Material("Water", 1.000*g/cm3, 2);
  H2O->AddElement(H, 2);
  H2O->AddElement(O, 1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(78.0*eV);

  G4Material* H3BO3 = new G4Material("Boric_Acid", 1.44*g/cm3, 3);
  H3BO3->AddElement(H, 3);
  H3BO3->AddElement(B, 1);
  H3BO3->AddElement(O, 3);
  //GetIonisation()->SetMeanExcitationEnergy(78.0*eV);

  G4Material* NaCl = new G4Material("Salt", 2.165*g/cm3, 2);
  NaCl->AddElement(Na, 1);
  NaCl->AddElement(Cl, 1);

  G4Material* LiF = new G4Material("Lithium_Fluoride", 2.64*g/cm3, 2);
  LiF->AddElement(Li, 1);
  LiF->AddElement(F, 1);

  G4Material* SeaWater = new G4Material("SeaWater", 1.027*g/cm3, 2);
  SeaWater->AddMaterial(H2O, 98.8*perCent);
  SeaWater->AddMaterial(NaCl, 1.2*perCent);

  G4Material* LaBr3 = new G4Material("LaBr3", 5.08*g/cm3, 2);
  LaBr3->AddElement(La, 1);
  LaBr3->AddElement(Br, 3);

  G4Material* LaBr3_Ce = new G4Material("LaBr3_Ce", 5.29*g/cm3, 2);
  LaBr3_Ce->AddMaterial(LaBr3, 95*perCent);
  LaBr3_Ce->AddElement(Ce, 5*perCent);

  G4Material* C4H8Cl2S = new G4Material("Mustard_Gas", 1.27*g/cm3, 4);
  C4H8Cl2S->AddElement(C, 4);
  C4H8Cl2S->AddElement(H, 8);
  C4H8Cl2S->AddElement(Cl, 2);
  C4H8Cl2S->AddElement(S, 1);

  G4Material* TNT = new G4Material("TNT", 1.654*g/cm3, 4);
  TNT->AddElement(O, 42.26*perCent);
  TNT->AddElement(C, 37.02*perCent);
  TNT->AddElement(N, 18.50*perCent);
  TNT->AddElement(H, 2.22*perCent);

  G4Material* Clark1 = new G4Material("Clark1", 1.422*g/cm3, 4);
  Clark1->AddElement(C, 12);
  Clark1->AddElement(H, 10);
  Clark1->AddElement(As, 1);
  Clark1->AddElement(Cl, 1);

  G4Material* Clark2 = new G4Material("Clark2", 1.45*g/cm3, 4);
  Clark2->AddElement(C, 13);
  Clark2->AddElement(H, 10);
  Clark2->AddElement(As, 1);
  Clark2->AddElement(N, 1);

  G4Material* SiO2 = new G4Material("Silicon_Dioxide", 2.196 *g/cm3, 2);
  SiO2->AddElement(Si, 1);
  SiO2->AddElement(O, 2);

  G4Material* Iron = new G4Material("Iron", 7.850 *g/cm3, 1);
  Iron->AddElement(Fe, 1);

  G4Material* Lead = new G4Material("Lead", 11.34 *g/cm3, 1);
  Lead->AddElement(Pb, 1);

  G4Material* Paraffin = man->FindOrBuildMaterial("G4_PARAFFIN");

  G4Material* Aluminum = new G4Material("Al", 2.7 *g/cm3, 1);
  Aluminum->AddElement(Al, 1);

  G4Material* Paraffin_Boric_Acid = new G4Material("Paraffin_Boric_Acid", 0.972*g/cm3, 2);
  Paraffin_Boric_Acid->AddMaterial(Paraffin, 90*perCent);
  Paraffin_Boric_Acid->AddMaterial(H3BO3, 10*perCent);

  G4Material* Cadmium = new G4Material("Cadmium", 8.65 *g/cm3, 1);
  Cadmium->AddElement(Cd, 1);

  G4Material* Carbon = new G4Material("Carbon", 2.3 *g/cm3, 1);
  Carbon->AddElement(C, 1);

  G4Material* Low_Carbon_Steel = new G4Material("Low_Carbon_Steel", 7.8499*g/cm3, 2);
  Low_Carbon_Steel->AddMaterial(Iron, 99.75*perCent);
  Low_Carbon_Steel->AddMaterial(Carbon, 0.25*perCent);

  G4Material* LCSt = man->FindOrBuildMaterial("Low_Carbon_Steel");

  G4Material* TargetMat = SeaWater;

  switch (targetType) {
    case TargetVariables::fWater:
      TargetMat = SeaWater;
      break;
    case TargetVariables::fMustardGas:
      TargetMat = C4H8Cl2S;
      break;
    case TargetVariables::fTNT:
      TargetMat = TNT;
      break;
    case TargetVariables::fClark1:
      TargetMat = Clark1;
      break;
    case TargetVariables::fClark2:
      TargetMat = Clark2;
      break;
  }

//**************************
  G4RotationMatrix* rotationTarget = new G4RotationMatrix();
  rotationTarget->rotateY(90*deg);

  G4RotationMatrix* rotationNeutronGuide = new G4RotationMatrix();
  rotationNeutronGuide->rotateX(90*deg);

  G4RotationMatrix *detRot = new G4RotationMatrix();
  detRot->rotateX(45*deg);

  G4RotationMatrix* rotationParticleGuide = new G4RotationMatrix();
  rotationParticleGuide->rotateZ(0*deg);
  rotationParticleGuide->rotateX(-90*deg);
  rotationParticleGuide->rotateY(180*deg);

  G4RotationMatrix* rotation_dummy = new G4RotationMatrix();
  rotation_dummy->rotateZ(0*deg);
  rotation_dummy->rotateY(180*deg);

  G4RotationMatrix* rotation3 = new G4RotationMatrix();
  rotation3->rotateX(55*deg);
  rotation3->rotateY(90*deg);

  G4RotationMatrix* rotation4 = new G4RotationMatrix();
  rotation4->rotateX(63*deg);
  rotation4->rotateY(90*deg);

  G4RotationMatrix* rotation5 = new G4RotationMatrix();
  rotation5->rotateY(180.*deg);
  rotation5->rotateX(-180.*deg);

//rotation for gamma guide
  G4RotationMatrix* rotation6 = new G4RotationMatrix();
  rotation6->rotateX(120.*deg);

//-------------------------

  G4double worldSizeDim = 4.2*m;
  G4Box* solidWorld = new G4Box("World", 0.5*worldSizeDim, 0.5*worldSizeDim, 0.5*worldSizeDim);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, SeaWater, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);

  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(1.,1.,0.5));
 // worldVisAtt->SetForceWireframe(true);
  logicWorld->SetVisAttributes(worldVisAtt);

//Submarine construction
  G4double submarineDimX = 3*m;
  G4double submarineDimY = 40*cm;
  G4double submarineDimZ = 3*m;
  G4Box* solidSubmarineVolumeOuter = new G4Box("SubmarineVolumeOuter", 0.5*submarineDimX, 0.5*submarineDimY, 0.5*submarineDimZ);
  G4LogicalVolume* logicSubmarineVolume = new G4LogicalVolume(solidSubmarineVolumeOuter, LCSt, "SubmarineVolumeOuter");
  new G4PVPlacement(0, G4ThreeVector(0.,0.,0), logicSubmarineVolume, "SubmarineVolumeOuter", logicWorld, false, 1, checkOverlaps);

  G4VisAttributes* subMarVisAttOuter = new G4VisAttributes(G4Colour(0.5,0.5,1.));
  subMarVisAttOuter->SetForceWireframe(true);
  logicSubmarineVolume->SetVisAttributes(subMarVisAttOuter);

  G4double submarineWallSize = 3*mm;
  G4Box* solidSubVolume = new G4Box("SubVolume",0.5*submarineDimX - submarineWallSize,
                                    0.5*submarineDimY - submarineWallSize, 0.5*submarineDimZ - submarineWallSize);

  G4LogicalVolume* logicSubVolume = new G4LogicalVolume(solidSubVolume, Air,"SubVolume");


  new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicSubVolume, "SubVolume", logicSubmarineVolume, false, 2, checkOverlaps);

  G4VisAttributes* subbMarVisAtt2 = new G4VisAttributes(G4Color(1.0,0.,0.5));
  subbMarVisAtt2->SetForceWireframe(true);
  logicSubVolume->SetVisAttributes(subbMarVisAtt2);
//


//Veto construction
  //Active area of silicon detector -> 5 - 55 mm (circle radius)
  G4double vetoDimx = 8*cm;
  G4double vetoDimY = 1*mm; // 2cm -> max thickness for silicon detectors
  G4double vetoDimZ = 8*cm;
  G4ThreeVector vetoShift(0, -11.0*cm, 0.);
  G4Box* solidVeto = new G4Box("DetectorSi", 0.5*vetoDimx, 0.5*vetoDimY, 0.5*vetoDimZ);
  G4LogicalVolume* logicVeto = new G4LogicalVolume(solidVeto, VetoMat, "DetectorSi");
  new G4PVPlacement(0, vetoShift, logicVeto, "DetectorSi", logicSubVolume, false, 13, checkOverlaps);

  G4VisAttributes* VetoAtt = new G4VisAttributes(G4Color(1.0,1.,0.5));
  VetoAtt->SetForceSolid(true);
  logicVeto->SetVisAttributes(VetoAtt);

  G4double neuSourceGuideInnerRadius = 9.5*cm;
  G4double neuSourceGuideOuterRadius = 10*cm;
  G4double neuSourceGuideDimZ = 5.*cm;
  G4ThreeVector neuSourceShift(0.,-25*cm,0);
  G4VSolid* solidSourceGuide = new G4Tubs("SourceGuide", neuSourceGuideInnerRadius, neuSourceGuideOuterRadius, neuSourceGuideDimZ, 0., 2*M_PI*rad);
  G4LogicalVolume* logicSourceGuide = new G4LogicalVolume(solidSourceGuide, LCSt, "SourceGuide");
  new G4PVPlacement(rotationNeutronGuide, neuSourceShift, logicSourceGuide, "SourceGuide", logicWorld, false, 3, checkOverlaps);
  
  //outer steel cap of neutron guide
  G4double neuSourceGuideCapDimZ = 0.2*cm;
  G4ThreeVector neuSourceCapShift = neuSourceShift + G4ThreeVector(0.,neuSourceGuideCapDimZ - neuSourceGuideDimZ,0); 

  G4VSolid* solidSourceGuideOuterCap = new G4Tubs("SourceGuideOuterCap", 0, neuSourceGuideInnerRadius, neuSourceGuideCapDimZ, 0., 2*M_PI*rad);
  G4LogicalVolume* logicSourceGuideOuterCap = new G4LogicalVolume(solidSourceGuideOuterCap, LCSt, "SourceGuideOuterCap");
  new G4PVPlacement(rotationNeutronGuide,neuSourceCapShift , logicSourceGuideOuterCap, "SourceGuideOuterCap", logicWorld, false, 14, checkOverlaps);
  
  //inside of neutron guide filled with air
  G4double neuSourceGuideInsDimZ = neuSourceGuideDimZ - neuSourceGuideCapDimZ;
  G4ThreeVector neuSourceInsShift = neuSourceShift + G4ThreeVector(0.,neuSourceGuideCapDimZ,0);

  G4VSolid* solidSourceGuideIns = new G4Tubs("SourceGuideIns", 0, neuSourceGuideInnerRadius, neuSourceGuideInsDimZ, 0., 2*M_PI*rad);
  G4LogicalVolume* logicSourceGuideIns = new G4LogicalVolume(solidSourceGuideIns, Air, "SourceGuideIns");
  new G4PVPlacement(rotationNeutronGuide, neuSourceInsShift, logicSourceGuideIns, "SourceGuideIns", logicWorld, false, 15, checkOverlaps);
  

  G4VisAttributes* sourceGVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  sourceGVisAtt->SetForceSolid(true);
  logicSourceGuide->SetVisAttributes(sourceGVisAtt);



//Target
  G4double targetCoverDimX = 194*cm;
  G4double targetCoverDimY = 50*cm;
  G4double targetCoverDimZ = 50*cm;
  G4ThreeVector targetCoverShift(0., -55.*cm, 0.);
  G4Box* solidTGVolumeCover = new G4Box("TargetVolumeCover", 0.5*targetCoverDimX, 0.5*targetCoverDimY, 0.5*targetCoverDimZ);
  G4LogicalVolume* logicTGVolumeCover = new G4LogicalVolume(solidTGVolumeCover, LCSt, "TargetVolumeCover");
  new G4PVPlacement(rotationTarget, targetCoverShift, logicTGVolumeCover, "TargetVolumeCover", logicWorld, false, 4, checkOverlaps);

  G4double targetWallSize = 3*mm;
  G4Box* solidTGVolume = new G4Box("TargetVolume", 0.5*targetCoverDimX - targetWallSize, 0.5*targetCoverDimY - targetWallSize,
                                   0.5*targetCoverDimZ - targetWallSize);
  G4LogicalVolume* logicTGVolume = new G4LogicalVolume(solidTGVolume, TargetMat, "TargetVolume");
  new G4PVPlacement(0, G4ThreeVector(), logicTGVolume, "TargetVolume", logicTGVolumeCover, false, 5, checkOverlaps);

  G4VisAttributes* mustardVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0));
  mustardVisAtt->SetForceSolid(true);
  logicTGVolume->SetVisAttributes(mustardVisAtt);


//Gamma ParticleGuide //trapezoidal
// Trap for gammas -
// Made of low carbon steel - >
// half length z - 50 mm
// theta - 69 deg -> polar angle of the line joining the centres of the faces at +/- half length Z
// phi - 90 deg -> Azimuthal angle of the line joining the center of the face at -half length Z to +half length Z
// half length Y at -half length Z
// half length X of the side at -half length Y of the face at -half length Z
// half length X of the side at +half length Y of the face at -half length Z
// angle with respect to the y axis from the centre of the lower endcap
// half length Y at +half length Z
// half length X of the side at -half length Y of the face at +half length Z
// half length X of the side at +half length Y of the face at +half length Z
// angle with respect to the y axis from the centre of the upper endcap

G4double particleGuideCapDimZ = 0.2*cm;

  G4double particleGuideZHalfLength = 5*cm - particleGuideCapDimZ;
  G4double particleGuideTheta = 69*deg;
  G4double particleGuidePhi = 90*deg;
  G4double particleGuideYHalfLengthAtMinusY = 10*cm;
  G4double particleGuideYHalfLengthAtPlusY = 10*cm;
  G4double particleGuideXHalfLengthAtMinusYMinusZ = 10*cm;
  G4double particleGuideYHalfLengthAtPlusYMinusZ = 10*cm;
  G4double particleGuideXHalfLengthAtMinusYPlusZ = 10*cm;
  G4double particleGuideYHalfLengthAtPlusYPlusZ = 10*cm;
  G4double particleGuideAlphaLowerEndcap = 0*deg;
  G4double particleGuideAlphaUpperEndcap = 0*deg;
  G4double particleGuideWallSize = 3*mm;
  G4ThreeVector particleGuideShift(0.*cm, -25*cm + particleGuideCapDimZ, 33.5*cm); //+ 0.5*cm
  G4Trap* solidParticleGuideOuterCov = new G4Trap("ParticleGuideOuterCov", particleGuideZHalfLength, particleGuideTheta,
                                                  particleGuidePhi, particleGuideYHalfLengthAtMinusY + particleGuideWallSize,
                                                  particleGuideXHalfLengthAtMinusYMinusZ + particleGuideWallSize,
                                                  particleGuideYHalfLengthAtPlusYMinusZ + particleGuideWallSize,
                                                  particleGuideAlphaLowerEndcap, particleGuideYHalfLengthAtPlusY + particleGuideWallSize,
                                                  particleGuideXHalfLengthAtMinusYPlusZ + particleGuideWallSize,
                                                  particleGuideYHalfLengthAtPlusYPlusZ + particleGuideWallSize,
                                                  particleGuideAlphaUpperEndcap);

  G4Trap* solidParticleGuideIns = new G4Trap("ParticleGuideIns", particleGuideZHalfLength,//+0.05*mm,//0.01 is used to better subtraction?  | Changed + to - //Wz
                                             particleGuideTheta, particleGuidePhi,particleGuideYHalfLengthAtMinusY,
                                             particleGuideXHalfLengthAtMinusYMinusZ,  particleGuideYHalfLengthAtPlusYMinusZ,
                                             particleGuideAlphaLowerEndcap, particleGuideYHalfLengthAtPlusY,
                                             particleGuideXHalfLengthAtMinusYPlusZ, particleGuideYHalfLengthAtPlusYPlusZ,
                                             particleGuideAlphaUpperEndcap);

  G4VSolid* solidParticleGuideOuter = new G4SubtractionSolid("ParticleGuideOuterCov-ParticleGuideIns",
                                                             solidParticleGuideOuterCov, solidParticleGuideIns);
  G4LogicalVolume* logicParticleGuideOuter = new G4LogicalVolume(solidParticleGuideOuter, LCSt, "ParticleGuideOuter");
  new G4PVPlacement(rotationParticleGuide, particleGuideShift, logicParticleGuideOuter, "ParticleGuideOuter", logicWorld,
                    false, 6, checkOverlaps);

  G4LogicalVolume* logicParticleGuideIns = new G4LogicalVolume(solidParticleGuideIns, Air, "ParticleGuideIns");
  new G4PVPlacement(rotationParticleGuide, particleGuideShift, logicParticleGuideIns, "ParticleGuideIns", logicWorld,
                    false, 7, checkOverlaps);

  //Particle guide cap
  
  //G4ThreeVector particleGuideCapShift(0., -25*cm + 0.2*cm - particleGuideZHalfLength, 33.5*cm -2.5*cm - 2*particleGuideZHalfLength);
  G4ThreeVector particleGuideCapShift = particleGuideShift + G4ThreeVector(0., -particleGuideZHalfLength - particleGuideCapDimZ, -2.5*particleGuideZHalfLength -5*mm);

  G4VSolid* solidParticleGuideCap = new G4Box("ParticleGuideCap", particleGuideYHalfLengthAtPlusY, particleGuideYHalfLengthAtPlusY, particleGuideCapDimZ);
  G4LogicalVolume* logicParticleGuideCap = new G4LogicalVolume(solidParticleGuideCap, LCSt, "ParticleGuideCap");
  

  new G4PVPlacement(rotationParticleGuide, particleGuideCapShift, logicParticleGuideCap, "ParticleGuideCap", logicWorld,
                    false, 16, checkOverlaps);
  G4VisAttributes* particleGuideVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,1.));
  particleGuideVisAtt->SetForceSolid(true);

  G4VisAttributes* particleVisAttOuter= new G4VisAttributes(G4Colour(0.0,1.0,0.5));
  particleVisAttOuter->SetForceSolid(true);
  logicParticleGuideOuter->SetVisAttributes(particleVisAttOuter);


// Detector:
  G4double detectorRadiusMin = 0;
  G4double detectorRadiusMax = 2.54*cm;
  G4double detectorHalfLengthZ = 2.54*cm;
  G4ThreeVector detectorShift(0.*cm, -0.19855*m + 4.8*cm, 51.*cm); // 4.8 is height and 50 cm is distance from source
  G4VSolid* solidAbsor = new G4Tubs("DetectorLaBr", detectorRadiusMin, detectorRadiusMax, detectorHalfLengthZ, 0., 2*M_PI*rad);
  G4LogicalVolume* logicAbsor = new G4LogicalVolume(solidAbsor, LaBr3_Ce, "DetectorLaBr");
  new G4PVPlacement(detRot, detectorShift, logicAbsor, "DetectorLaBr", logicSubVolume, false, 8, checkOverlaps);   //copy number

  G4VisAttributes* solidVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  solidVisAtt->SetForceSolid(true);
  logicAbsor->SetVisAttributes(solidVisAtt);

// SAND
  G4double sandXLength = 4*m;
  G4double sandYLength = 100*cm;
  G4double sandZLength = 4*m;
  G4ThreeVector sandShift(0., -130*cm, 0.);
  G4Box* solidSANDVolume = new G4Box("SAND", 0.5*sandXLength, 0.5*sandYLength, 0.5*sandZLength);
  G4LogicalVolume* logicSANDVolume = new G4LogicalVolume(solidSANDVolume, SandSediment, "SAND");
  new G4PVPlacement(0, sandShift, logicSANDVolume, "SAND", logicWorld, false, 9, checkOverlaps);

  G4VisAttributes* sandVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  sandVisAtt->SetForceSolid(true);
  logicSANDVolume->SetVisAttributes(sandVisAtt);

// Source boarder for presen.
  G4double pRmin = 0.*cm;
  G4double pRmax = 2.5*cm;
  G4double pSPhi = 0*deg;
  G4double pDPhi = 2*M_PI;
  G4double pSTheta = 0*deg;
  G4double pDTheta = M_PI;

  G4Sphere* solidShape1 = new G4Sphere("SOURCE", pRmin, pRmax, pSPhi, pDPhi, pSTheta, pDTheta);
  G4LogicalVolume* logicShape1 = new G4LogicalVolume(solidShape1, vacuum, "SOURCE");
  new G4PVPlacement(0, G4ThreeVector(0, -15*cm,0), logicShape1, "SOURCE", logicSubVolume, false, 10);
      
  G4VisAttributes * sourceVisAtt = new G4VisAttributes(G4Colour(1.,1.,0.));
  sourceVisAtt->SetForceWireframe(true);
  logicShape1->SetVisAttributes(sourceVisAtt);

// Iron shielding
  G4double shieldXLength = 10*cm;
  G4double shieldYLength = 5*cm;
  G4double shieldZLength = 20*cm;
  G4ThreeVector shieldShift(0, -15.0*cm, 24.*cm);
  G4Box* ironShield = new G4Box("Shield", 0.5*shieldXLength, 0.5*shieldYLength, 0.5*shieldZLength);
  G4LogicalVolume* logicIron = new G4LogicalVolume(ironShield, Iron, "Shield");
  new G4PVPlacement(0, shieldShift, logicIron, "Shield", logicSubVolume, false, 11);

  G4VisAttributes* ironVisAtt= new G4VisAttributes(G4Colour(1.,1.,0.5));
  ironVisAtt->SetForceWireframe(true);
  ironVisAtt->SetForceSolid(true);
  logicIron->SetVisAttributes(ironVisAtt);

// Lead Cap
  G4double capXLength = 10*cm;
  G4double capYLength = 5*cm;
  G4double capZLength = 4*cm;
  G4ThreeVector capShift(0, -15.0*cm, 36.*cm);
  G4Box* LeadCap = new G4Box("PbShield", 0.5*capXLength, 0.5*capYLength, 0.5*capZLength);
  G4LogicalVolume* logicLead = new G4LogicalVolume(LeadCap, Lead, "PbShield");
  new G4PVPlacement(0, capShift, logicLead, "Shield", logicSubVolume, false, 12);

  G4VisAttributes* LeadVisAtt= new G4VisAttributes(G4Colour(0.2,1.,0.5));
  LeadVisAtt->SetForceWireframe(true);
  LeadVisAtt->SetForceSolid(true);
  logicLead->SetVisAttributes(LeadVisAtt);


  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    sdManager->SetVerboseLevel(2);

    SensitiveSD* myDetector = new SensitiveSD("Detector");   
    SetSensitiveDetector("DetectorLaBr", myDetector); 
    sdManager->AddNewDetector(myDetector);

    SensitiveVetoSD* vetoDetector = new SensitiveVetoSD("Veto");
    SetSensitiveDetector("DetectorSi", vetoDetector);
    sdManager->AddNewDetector(vetoDetector);
}
