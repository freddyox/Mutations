#include "TFile.h"
#include "TF1.h"
#include "TGraph.h"

void analyze_output(const char* rootfile){

  TFile *fIn = new TFile(rootfile);
  TGraph *hFit = (TGraph*)fIn->Get( "N_Mutations_vs_Fitness" ); 
  
  if( !hFit ){
    std::cout << "Error grabbing N_Mutations_vs_Fitness" << std::endl;
  }

  // TF1 *linear = new TF1("linear","pol1", 1500, 5000 );
  //hFit->Fit(linear,"R");
  TF1 *tanh = new TF1("tanh","1.0 - TMath::Exp(-[0]*x)");
  tanh->SetParameter(0,400);
  hFit->Fit(tanh);
  hFit->Draw();
  
  // std::cout << "Mutations:\n";
  // std::cout << "m = " << linear->GetParameter(1) << std::endl;
  // std::cout << "b = " << linear->GetParameter(0) << std::endl;

  TGraph *hFit1 = (TGraph*)fIn->Get( "N_Attempts_vs_Fitness" ); 
  
  if( !hFit1 ){
    std::cout << "Error grabbing N_Attempts_vs_Fitness" << std::endl;
  }

  TF1 *linear_N = new TF1("linear_n","pol1", 6000, 10000 );
  hFit1->Fit(linear_N,"R");

  std::cout << "Attemptd:\n";
  std::cout << "m = " << linear_N->GetParameter(1) << std::endl;
  std::cout << "b = " << linear_N->GetParameter(0) << std::endl;

  return;
}
