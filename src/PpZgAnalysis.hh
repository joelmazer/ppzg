/** @file PpZgAnalysis.hh
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform z<SUB>g</SUB> analysis.
    @date Mar 02, 2015
*/

#ifndef __PPZGANALYSIS_HH
#define __PPZGANALYSIS_HH

#include "PpZgParameters.hh"
#include "JetAnalyzer.hh"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TString.h"
#include "TRandom.h"
#include "TChain.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TFile.h"
#include "TSystem.h"
#include "TParameter.h"
#include "TClonesArray.h"

#include "fastjet/contrib/Recluster.hh"
#include "fastjet/contrib/SoftDrop.hh"

// Not needed for analysis per se
#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"

#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

#include <assert.h>
#include <iostream>
#include <cmath>
#include <climits>
#include <sstream>

using namespace std;
using namespace fastjet;
using namespace contrib;


/** 
    A helper for geant data
 */
double LookupXsec( TString filename );

/** 
    A helper for Run12 embedding data
 */
double LookupRun12Xsec( TString filename );


/**
   For sorting with a different key
*/
typedef pair<PseudoJet,double> PseudoJetPt;
struct PseudoJetPtGreater {
  bool operator()( PseudoJetPt const & a, PseudoJetPt const & b) { 
    return a.second > b.second ;
  }
};

/**
  To keep original and groomed jets connected
 */
class GroomingResultStruct{
public:
  PseudoJet orig;
  PseudoJet groomed;
  double zg;
  GroomingResultStruct ( PseudoJet orig, PseudoJet groomed, double zg ) : orig(orig), groomed(groomed),zg(zg){};
  
  static bool origptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
    return a.orig.pt()>b.orig.pt();
  };
  
  static bool groomedptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
    return a.groomed.pt()>b.groomed.pt();
  };
  
  // PseudoJet* orig;
  // PseudoJet* groomed;
  // double zg;
  // GroomingResultStruct ( PseudoJet* orig, PseudoJet* groomed, double zg ) : orig(orig), groomed(groomed),zg(zg){};
  
  // static bool origptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
  //   return a.orig->pt()>b.orig->pt();
  // };
  
  // static bool groomedptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
  //   return a.groomed->pt()>b.groomed->pt();
  // };

};


/** 
    convenient output
*/
ostream & operator<<(ostream & ostr, const PseudoJet& jet);

/** 
    Helper for chains
 */
void InitializeReader(  std::shared_ptr<TStarJetPicoReader> pReader, const TString InputName, const Long64_t NEvents, const int PicoDebugLevel );

  

/**
   The main class
 */
class PpZgAnalysis {

private :

  // These need to be initialized
  // ----------------------------
  PpZgParameters pars;   ///< container to have all analysis parameters in one place
  
  // Internal
  // --------  
  float EtaJetCut;       ///< jet eta
  float EtaGhostCut;     ///< ghost eta
  
  fastjet::JetDefinition JetDef;       ///< jet definition

  // Relevant jet candidates
  fastjet::Selector select_jet_eta;        ///< jet rapidity selector
  fastjet::Selector select_jet_pt;         ///< jet p<SUB>T</SUB> selector
  fastjet::Selector select_jet;            ///< compound jet selector

  fastjet::GhostedAreaSpec AreaSpec;      ///< ghosted area specification
  fastjet::AreaDefinition AreaDef;        ///< jet area definition

  // Data
  // ----
  Long64_t NEvents=-1;
  TChain* Events = 0;
  TClonesArray* pFullEvent = 0;             ///< Constituents
  TClonesArray* pHardPartons = 0;           ///< For pythia data, the original hard scatter
  TClonesArray* pHardPartonNames = 0;       ///< For pythia data, the original hard scatter names (ubar, g, ...)
  TStarJetVector* pHT = 0;                  ///< the trigger (HT) object, if it exists

  vector<PseudoJet> particles;
  vector<PseudoJet> partons;
  double rho=0;                             ///< background density
  
  std::shared_ptr<TStarJetPicoReader> pReader=0;
  
  Long64_t evi=0;
  
  Long64_t NEmbEvents=-1;
  TChain* EmbEvents = 0;
  TClonesArray* pEmbEvent = 0;
  std::shared_ptr<TStarJetPicoReader> pEmbReader=0;

  bool Embedding=false;
  Long64_t Embevi =0; // Starting point will be changed!  

  int PicoDebugLevel=0; /// Control DebugLevel in picoDSTs

  // Note that the following are NOT unique across MC trees
  // My fault, will try to fix with a quick and dirty trick:
  // runid (and eventid) are below 1M (itself not exactly optimal)
  // So we'll just hash the data file name and add it to the runid,
  // making sure that it stays in the right range
  int eventid;
  int runid;
  int Embeventid;
  int Embrunid;

  double refmult=0;
  double Embrefmult=0;

  // ULong64_t runevent=0;
  double weight=0;    

  // ULong64_t Embrunevent=0;
  
  // fastjet::Selector select_track_rap;   ///< constituent rapidity selector
  // fastjet::Selector select_lopt;        ///< constituent p<SUB>T</SUB> selector
  // fastjet::Selector select_hipt;        ///< constituent p<SUB>T</SUB> selector

  // fastjet::Selector slo;                ///< compound selector for low  p<SUB>T</SUB> constituents
  // fastjet::Selector shi;                ///< compound selector for high p<SUB>T</SUB> constituents

  Selector NotGhost = !fastjet::SelectorIsPureGhost ();    
  Selector OnlyCharged = NotGhost && ( SelectorChargeRange( -3, -1) || SelectorChargeRange( 1, 3) );
  Selector OnlyNeutral = NotGhost && SelectorChargeRange( 0, 0);

  JetAnalyzer *pJA=0;
  JetAnalyzer *pEmbJA=0;
  Subtractor* pBackgroundSubtractor =  0;
  Subtractor* pEmbBackgroundSubtractor =  0;
  ConstituentSubtractor* pConstituentBackgroundSubtractor = 0;

  // For matching
  // ------------
  fastjet::Selector SelectClose;

  JetAnalyzer* pJAhi;                      ///< JetAnalyzer object for high pT
  JetAnalyzer* pJAlo;                      ///< JetAnalyzer object for low pT
  
  // --------------------------------------------------------------
  // JetAlgorithm ReclusterJetAlgorithm = fastjet::cambridge_algorithm;
  fastjet::JetAlgorithm ReclusterJetAlgorithm;
  fastjet::JetDefinition ReclusterJetDef;
  Recluster * recluster=0; 

  // Results to be passed around
  TClonesArray *pSavedHardPartons=0;           ///< original hard scatter in PYTHIA
  TClonesArray *pSavedHardPartonNames=0;       ///< original hard scatter PID in PYTHIA
  vector<GroomingResultStruct> GroomingResult; ///< grooming result in a nice structured package
  
  
  // std::vector<fastjet::PseudoJet> pHi;     ///< High pT constituents
  // std::vector<fastjet::PseudoJet> pLo;     ///< Low pT constituents

  // std::vector<fastjet::PseudoJet> JAhiResult;  ///< Unaltered clustering result with high pT constituents
  // std::vector<fastjet::PseudoJet> JAloResult;  ///< Unaltered clustering result with low pT constituents
  
  // std::vector<fastjet::PseudoJet> DiJetsHi;    ///< Dijet result with high pT constituents
  // std::vector<fastjet::PseudoJet> DiJetsLo;    ///< Dijet result with low pT constituents

  
public:  

  /** Standard constructor. Parse vectorized argv.
      \param argc: number of arguments
      \param argv: string array of command line options
   */
  PpZgAnalysis ( const int argc, const char** const  );

  /** Destructor. Clean things up
   */
  virtual ~PpZgAnalysis();

  /** Decoupled chain initialization for readability
   */
  bool InitChains ( );

  /** Main routine for one event.
      \return false if at the end of the chain      
   */
  EVENTRESULT RunEvent ( );
 
  // /** This little helper is true if there's at least one 10 GeV jet
  //  */
  // bool Has10Gev;
  // double OverrideJetMin;        ///< quick and dirty way to override the 10 GeV minimum

  // Quick and dirty QA histos - keep them public
  // --------------------------------------------
  TH2D* QA_TowerEt;
  

  // Getters and Setters
  // -------------------
  inline PpZgParameters& GetPars()         { return pars; };

  /// Get jet radius
  inline double GetR ( )                   { return pars.R; };
  /// Set jet radius
  inline void   SetR ( const double newv ) { pars.R=newv;   };


  /// Handle to pico reader
  inline std::shared_ptr<TStarJetPicoReader> GetpReader()  { return pReader; };

  /// Handle to pico EmbReader
  inline std::shared_ptr<TStarJetPicoReader> GetpEmbReader()  { return pEmbReader; };

  /// Get the weight of the current event (mainly for PYTHIA)
  inline double GetEventWeight()           { return weight; };

  /// Get the refmult of the current event
  inline double GetRefmult()           { return refmult; };

  /// Get the runid of the current event
  inline double GetRunid()           { return runid; };

  /// Get the eventid of the current event
  inline double GetEventid()           { return eventid; };

  /// Get the Trigger (HT) object if it exists, for matching
  inline TStarJetVector* GetTrigger() const        { return pHT; };
  
  /// The main result of the analysis
  inline const vector<GroomingResultStruct>& GetGroomingResult()    { return GroomingResult; }

  inline TClonesArray* GetHardPartons()              { return pSavedHardPartons; }
  inline TClonesArray* GetHardPartonNames()          { return pSavedHardPartonNames; }
  
  inline double GetRho()                             { return rho; }

  inline const vector<PseudoJet>& GetParticles() const     { return particles; }
  // /// Get minimum jet p<SUB>T</SUB>
  // inline double GetJet_ptmin ( )                   { return pars.jet_ptmin; };
  // /// Set minimum jet p<SUB>T</SUB>
  // inline void   SetJet_ptmin ( const double newv ) { pars.jet_ptmin=newv;   };

  // /// Get maximum jet p<SUB>T</SUB>
  // inline double GetJet_ptmax ( )                   { return pars.jet_ptmax; };
  // /// Set maximum jet p<SUB>T</SUB>
  // inline void   SetJet_ptmax ( const double newv ) { pars.jet_ptmax=newv;   };

  // /// Get jet rapidity acceptance
  // inline double GetMax_rap ( )                   { return pars.max_rap; };
  // /// Set jet rapidity acceptance
  // inline void   SetMax_rap ( const double newv ) { pars.max_rap=newv;   };

  // /// Get ghosted area rapidity cut, should be >= max_rap + 2*R
  // inline double GetGhost_maxrap ( )                   { return pars.ghost_maxrap; };
  // /// Set ghosted area rapidity cut, should be >= max_rap + 2*R
  // inline void   SetGhost_maxrap ( const double newv ) { pars.ghost_maxrap=newv;   };


  // // Objects will be handed by _reference_! Obviates need for setter
  // /// Handle to jet definition
  // inline fastjet::JetDefinition& GetJetDef () { return JetDef; }
  // /// Handle to selector for low  p<SUB>T</SUB> constituents
  // inline fastjet::Selector& GetLoConsSelector () { return slo; }
  // /// Handle to selector for high  p<SUB>T</SUB> constituents
  // inline fastjet::Selector& GetHiConsSelector () { return shi; }
  
  /// Handle to selector for jet candidates
  inline fastjet::Selector& GetJetSelector () { return select_jet; }

  /// Handle to ghosted area specification
  inline fastjet::GhostedAreaSpec& GetAreaSpec () { return AreaSpec; }
  /// Handle to jet area definition
  inline fastjet::AreaDefinition& GetAreaDef () { return AreaDef; }

  /// Handle to constituents
  inline std::vector<fastjet::PseudoJet> GetConstituents() {return particles; };

   
  //   TStarJetPicoTowerCuts* towerCuts = pReader->GetTowerCuts();
  //   cout << "DumbTest: towerCuts->MaxEt="<< towerCuts->GetMaxEtCut() << endl;
  //   cout << "DumbTest: towerCuts=" << towerCuts << endl;
    
  //   cout << endl;
  //   TStarJetPicoTrackCuts* trackCuts = pReader->GetTrackCuts();
  //   cout << "DumbTest: trackCuts->MaxPt="<< trackCuts->GetMaxPtCut() << endl;
  //   cout << "DumbTest: trackCuts=" << trackCuts << endl;
    
  //   cout << endl;
  //   cout << "DumbTest: pReader->N=" << pReader->GetNOfEvents() << endl;

  //   pReader->ReadEvent( 0 );
    
  //   // for (int i=0; i<10; ++i){
  //   //   pReader->ReadEvent( 200 +i );
  //   //   cout << 200+i << "  " << pReader->GetEvent()->GetHeader()->GetEventId() << endl;
  //   // }
  //   return;
    
  // };

  // /// Handle to JetAnalyzer for high pT constituents
  // inline JetAnalyzer* GetJAhi() {return pJAhi; };
  // /// Handle to JetAnalyzer for low pT constituents
  // inline JetAnalyzer* GetJAlo() {return pJAlo; };

  // /// Handle to unaltered clustering result with high pT constituents
  // inline std::vector<fastjet::PseudoJet> GetJAhiResult() {return JAhiResult; };
  // /// Handle to unaltered clustering result with low pT constituents
  // inline std::vector<fastjet::PseudoJet> GetJAloResult() {return JAloResult; };

  // /// Handle to high pT constituents
  // inline std::vector<fastjet::PseudoJet> GetLoConstituents() {return pLo; };
  // /// Handle to low pT constituents
  // inline std::vector<fastjet::PseudoJet> GetHiConstituents() {return pHi; };

  
  // /// Handle to Dijet result with high pT constituents
  // inline std::vector<fastjet::PseudoJet> GetDiJetsHi() {return DiJetsHi; };
  // /// Handle to Dijet result with low pT constituents
  // inline std::vector<fastjet::PseudoJet> GetDiJetsLo() {return DiJetsLo; };

};  

/** Helper to perform the TStarJetPicoReader initialization
 */
TStarJetPicoReader GetReader ( TString ChainPattern="~putschke/Data/Pico_ppHT/*.root", 
			       TString TriggerString="ppHT",
			       TString ChainName="JetTree",
			       const double RefMultCut=0
			       );

/** Slightly different, preferred version of GetReader
 */
shared_ptr<TStarJetPicoReader> SetupReader ( TChain* chain, const PpZgParameters& pars );


/** For use with GeantMc data
 */
void TurnOffCuts ( std::shared_ptr<TStarJetPicoReader> pReader );
  
#endif // __PPZGANALYSIS_HH
