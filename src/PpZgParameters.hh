/** @file PpZgParameters.hh
    @author Kolja Kauder
    @brief Common parameters
    @details Used to quickly include the same parameters into different macros.
    @date Mar 23, 2017
   
 */

#ifndef PPZGPARAMETERS_HH
#define PPZGPARAMETERS_HH

#include "JetAnalyzer.hh"

using fastjet::JetAlgorithm;
using fastjet::antikt_algorithm;
using fastjet::kt_algorithm;
using fastjet::cambridge_algorithm;

enum INTYPE{ MCTREE, INTREE, INPICO, MCPICO };

// Subtract background?
enum BGTYPE{ NONE=0, AREA=1, CONSTSUBPRE=2, CONSTSUBPOST=3 };

// Return values for the main routine
enum class EVENTRESULT{
  PROBLEM,
  ENDOFINPUT,
  NOTACCEPTED,
  NOCONSTS,
  NOJETS,
  JETSFOUND
};

class PpZgParameters{

public :
  double R = 0.4;            ///< Resolution parameter ("radius").
  double z_cut = 0.10;       ///< Grooming cut parameter
  double beta  = 0.0;        ///< Grooming angular parameter

  /// Jet algorithm for the original jets
  JetAlgorithm LargeJetAlgorithm=fastjet::antikt_algorithm;
  // JetAlgorithm LargeJetAlgorithm = fastjet::cambridge_algorithm;
  
  /// Alternative reclustering to Cambridge/Aachen (at our own risk)
  bool CustomRecluster=false;
  JetAlgorithm CustomReclusterJetAlgorithm;
  // bool CustomRecluster=true;
  // JetAlgorithm CustomReclusterJetAlgorithm = fastjet::antikt_algorithm;
  bool Recursive=false;  ///< Repeat on subjets?

  /// Repetitions in the background. Anything other than 1 WILL NOT WORK because
  /// a) we're using explicit ghosts (though we don't have to)
  /// b) more importantly, the background subtractor contains fastjet::SelectorNHardest(2)
  ///    which doesn't work jet-by-jet and throws an error
  int GhostRepeat = 1;
  float GhostArea = 0.005;    ///< ghost area

  // int ghost_repeat = 1;
  // double ghost_area = 0.01;    ///< ghost area
  // const double ghost_area = 0.0005;    ///< ghost area

  // const double PtJetMin = 20.0;    ///< Min jet pT
  double PtJetMin = 7.0;      ///< Min jet pT
  double PtJetMax = 1000.0;   ///< Max jet pT
  double LeadPtMin=5.0;                 ///< leading jet minimum p<SUB>T</SUB>
    
  double EtaConsCut = 1.0;    ///< Constituent |&eta;| acceptance
  double PtConsMin=0.2;       ///< Constituent pT minimum
  // const double PtConsMin=2.0;       ///< Constituent pT minimum
  double PtConsMax=1000;        ///< Constituent pT maximum
  
  double RefMultCut=0;        ///< Reference multiplicity. Needs to be rethought to accomodate pp and AuAu
  
  double VzCut=30;            ///< Vertex z 
  // const double VzDiffCut=6;         ///< |Vz(TPC) - Vz(VPD)| <-- NOT WORKING in older data (no VPD)
  double VzDiffCut=1000;      ///< |Vz(TPC) - Vz(VPD)|
  
  double DcaCut=1.0;          ///< track dca
  double NMinFit=20;             ///< minimum number of fit points for tracks
  double FitOverMaxPointsCut=0.52; ///< NFit / NFitPossible
  // double NFitMin=20.0;        ///< Constituent minimum fit points
  // double NFitRatio=0.52;      ///< Constituent NFit / NFitPossible cut

  // ************************************
  // Do NOT cut high tracks and towers!
  // Instead, reject the whole event when
  // of these is found
  // ************************************
  double MaxEtCut=1000;       ///< tower ET cut
  double MaxTrackPt=1000;     ///< track pT cut
  // const double EtTowerMax=1000;       ///< tower ET cut
  // const double PtTrackMax=1000;       ///< tower ET cut

  
  // EVENT rejection cuts
  double MaxEventPtCut=30;       ///< max track pT cut for event
  double MaxEventEtCut=30;       ///< max tower ET cut for event

  double MinEventEtCut=0;        ///< min event ET cut for event

  double ManualHtCut=0.0;        ///< necessary for some embedding picos. Should always equal MinEventEtCut
  
  // double VzCut=30;            ///< Vertex z 
  // double VzDiffCut=6;         ///< |Vz(TPC) - Vz(VPD)|

  // // Tracks: Some standard high quality cuts
  // const double DcaCut=1.0;          ///< Constituent dca cut

  BGTYPE SubtractBg=AREA;
  BGTYPE EmbSubtractBg=AREA;

  // BGTYPE SubtractBg=NONE;
  // BGTYPE EmbSubtractBg=NONE;

  // Geant files have messed up runid and event id.
  // Switch for fixing. Should be turned on by default for Geant files, off otherwise.
  bool UseGeantNumbering=false;

  // // MC example
  // // ----------
  // TString InputName = "Data/RhicPythia/RhicPythiaOnly_10_ptHat=20_23.root"; ///< Input name
  // INTYPE intype = MCTREE;             ///< Input type (can be a pico dst, a result tree, an MC tree)
  // TString ChainName = "tree";         ///< Name of the input chain
  // TString TriggerName = "All";        ///< Trigger type (All, MB, HT, pp, ppHT, ppJP)
  
  // // GEANTMC example
  // // ---------------
  // TString InputName = "Data/AddedGeantPythia/picoDst_25_35_0.root"; ///< Input name
  // INTYPE intype = MCPICO;             ///< Input type (can be a pico dst, a result tree, an MC tree)
  // TString ChainName = "JetTreeMc";    ///< Name of the input chain
  // TString TriggerName = "All";        ///< Trigger type (All, MB, HT, pp, ppHT, ppJP)
  
  // // GEANT example
  // // ---------------
  // TString InputName = "Data/AddedGeantPythia/picoDst_25_35_0.root"; ///< Input name
  // INTYPE intype = INPICO;             ///< Input type (can be a pico dst, a result tree, an MC tree)
  // TString ChainName = "JetTree";      ///< Name of the input chain
  // TString TriggerName = "All";        ///< Trigger type (All, MB, HT, pp, ppHT, ppJP)

  // real data pico example
  // ----------------------
  TString InputName = "Data/ppHT/*.root"; ///< Input name
  INTYPE intype = INPICO;                 ///< Input type (can be a pico dst, a result tree, an MC tree)
  TString ChainName = "JetTree";          ///< Name of the input chain
  TString TriggerName = "ppHT";           ///< Trigger type (All, MB, HT, pp, ppHT, ppJP)
    
  // // real AuAu data pico example
  // // -------------------
  // TString InputName = "Data/SmallAuAu/Small_Clean809.root"; ///< Input name
  // INTYPE intype = INPICO;                 ///< Input type (can be a pico dst, a result tree, an MC tree)
  // TString ChainName = "JetTree";          ///< Name of the input chain
  // TString TriggerName = "HT";           ///< Trigger type (All, MB, HT, pp, ppHT, ppJP)

  // Allow Embedding
  // ---------------
  // NONE
  TString EmbInputName = "";           ///< Embedding input name. Leave blank for no embedding
  INTYPE Embintype = MCTREE;           ///< Embedding input type (can be a pico dst, a result tree, an MC tree)
  TString EmbChainName = "";           ///< Name of the embedding input chain
  TString EmbTriggerName = "none";       ///< Embedding trigger type (All, MB, HT, pp, ppHT, ppJP)

  // // This is equivalent to -embi FAKERHIC
  // TString EmbInputName = "Data/FakeAuAu20_*root";           ///< Embedding input name. Leave blank for no embedding
  // INTYPE Embintype = MCTREE;           ///< Embedding input type (can be a pico dst, a result tree, an MC tree)
  // TString EmbChainName = "tree";       ///< Name of the embedding input chain
  // TString EmbTriggerName = "All";       ///< Embedding trigger type (All, MB, HT, pp, ppHT, ppJP)

  // // real data  
  // TString EmbInputName = "";           ///< Embedding input name. Leave blank for no embedding
  // INTYPE Embintype = MCTREE;           ///< Embedding input type (can be a pico dst, a result tree, an MC tree)
  // TString EmbChainName = "tree";       ///< Name of the embedding input chain
  // TString EmbTriggerName = "MB";       ///< Embedding trigger type (All, MB, HT, pp, ppHT, ppJP)

  
  int nMix=1;                   ///< How many events to mix

  // Only putting it here so that it can be initialized in the analysis class and then used
  TString OutFileName = "Results/TmpResult.root";     ///< Output file


};
#endif // PPZGPARAMETERS_HH
