#include "../include/Mutations.hh"

#include <iostream>
#include "TMath.h"
#include "TGraph.h"
#include "TAxis.h"

Mutations::Mutations(unsigned int x, unsigned int y, int N, int Nvertices)
  : fDisx(x), fDisy(y), fNtries(N), fNvertices(Nvertices) {
  fNPixels = fDisx * fDisy;

  fTri = sf::VertexArray(sf::Triangles,3);  // SFML vertex array, defined to be triangle
  
  fFitness = 1.0e10;   // Fitness should always be under 1.0 now
 
  fTriCountPos = 0;    // Counter to access triangle position vector
  fTriCountCol = 0;    // Counter to access triangle color vector
  fNMutations = 0;     // # of successful mutations
  fNAttempts = 0;      // # of mutation attempts
  fRGBA[0] = "r";      // testing
  fRGBA[1] = "g";      //   .
  fRGBA[2] = "b";      //   .
  fRGBA[3] = "a";      //   .

  setupOutput("test_root_output");
  hRandPosX_Fitness = new TH2D("hRandPosX_Fitness","",203,-101.0,101.0,100,0.8,1.0);
  hRandPosX_Fitness->SetXTitle("Random Position X Number [pixels]");
  hRandPosX_Fitness->SetYTitle("Fitness [unitless]");
  hRandPosY_Fitness = new TH2D("hRandPosY_Fitness","",203,-101.0,101.0,100,0.8,1.0);
  hRandPosY_Fitness->SetXTitle("Random Position Y Number [pixels]");
  hRandPosY_Fitness->SetYTitle("Fitness [unitless]");
}

////////////////////////////////////////////////////////////////////////////////
// Draw -- The mutated triangles need to be drawn to access its fitness:
//
void Mutations::draw( sf::RenderTarget& target, sf::RenderStates) const {
  std::vector<sf::VertexArray>::const_iterator cit;
  for( cit = fTrianglesNext.begin(); cit != fTrianglesNext.end(); cit++ ){
    target.draw( *cit );
  }
}

////////////////////////////////////////////////////////////////////////////////
// Generate Initial Triangles:
// 1) randomcolor = true, then 2nd and 3rd argument are irrelevant and tris get
//    a random rgb assigned
// 2) randomcolor=false, grid=false, then tris get colored by col
// 3) randomcolor=false, grid=true, then 2nd argumet is irrelevant. Input image
//    is divided into smaller section where average rgb is then taken. The average
//    x and y position of randomly generated triangles get assigned color of grid average
void Mutations::GenerateTriangles(bool randomcolor, sf::Color col, bool grid){
  srand( time(NULL) );
  int xhi = fDisx+1;
  int yhi = fDisy+1; 
  for(int m=0; m<fNtries; m++){
    
    unsigned int r = fRand.Integer(256); // 0 -> 255
    unsigned int g = fRand.Integer(256);
    unsigned int b = fRand.Integer(256);
    unsigned int a = fRand.Integer(256);
    
    if( !randomcolor ){
      r = col.r;
      g = col.g;
      b = col.b;
      //a = 255;
    }

    double xavg = 0.0;
    double yavg = 0.0;
    for(int i=0; i<3; i++){
      int tempx = fRand.Integer(xhi); // 0 -> fDisx 
      int tempy = fRand.Integer(yhi); // 0 -> fDisy
      // std::cout << tempx << std::endl;
      xavg += tempx;
      yavg += tempy;
      fTri[i].position = sf::Vector2f(tempx,tempy);   
      fTriPositionsNow[3*m+i]  = sf::Vector2f(tempx, tempy);
      fTriPositionsNext[3*m+i] = sf::Vector2f(tempx, tempy);
    }

    xavg /= 3.0;
    yavg /= 3.0;

    int index = -1;
    if( grid ){
      for(int i=0; i<fBoundaryColor.size(); i++){
	if( fBoundaryColor[i].contains( xavg, yavg) ){
	  index = i;
	}
      }
    }
    
    sf::Color boundarycolor;
    if( index < 0 ){
      //std::cout << "The boundary finder has an issue, defaulting to average color." << std::endl;
      for(int i=0; i<3; i++){ 
	fTri[i].color = sf::Color(r,g,b,a);  // all get the same color for this algorithm
      }
    } else {
      boundarycolor = fGridColor[index];
      for(int i=0; i<3; i++){ 
	fTri[i].color = boundarycolor;  // all get the same color for this algorithm
	r = boundarycolor.r;
	g = boundarycolor.g;
	b = boundarycolor.b;
	//a = 255;
      }
    }

    fTriColorsNow[m]  = sf::Color(r,g,b,a);
    fTriColorsNext[m] = sf::Color(r,g,b,a);
    fTrianglesNow.push_back(fTri);
    fTrianglesNext.push_back(fTri);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Generate First Mutation Sample:
//
void Mutations::GetAttempt(sf::Image win,bool first_try){
  for(int w=0; w<fDisx; w++){
    for(int h=0; h<fDisy; h++){
      sf::Color color_temp = win.getPixel(w,h);
      if( first_try ){
	fNow.push_back( color_temp );
      } else {
	fNext.push_back( color_temp );
      }
    }
  }

  // Set fitness between input and initial polygon generation:
  if( first_try ){
    fFitness = CalculateFitness( true, true);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Average RGB of Input
//
void Mutations::GetAverageRGB(){
  int r=0,g=0,b=0;
  for(unsigned int i=0; i<fInput.size(); i++){
    sf::Color color_temp = fInput[i];
    r += color_temp.r;
    g += color_temp.g;
    b += color_temp.b;
  }
  r /= fInput.size();
  g /= fInput.size();
  b /= fInput.size();
  fAverage = sf::Color(r,g,b);

  // Generate the Triangles: ***********need to move this and clean up
  GenerateTriangles(false,fAverage,true);
}

////////////////////////////////////////////////////////////////////////////////
// Grid the input image, get average RGB value of each grid
//
void Mutations::GridInput(int ngrids){
  fGridX = fDisx / ngrids;
  fGridY = fDisy / ngrids;

  for(int col=0; col<ngrids; col++){
    for(int row=0; row<ngrids; row++){
      unsigned int r=0,g=0,b=0,a=0;
      int startcol = col*fGridX;
      int startrow = row*fGridY;
      int endcol = startcol + fGridX;
      int endrow = startrow + fGridY;      
      
      if(col==ngrids-1) endcol=fDisx;
      if(row==ngrids-1) endrow=fDisy;
      
      for(int w=startcol; w<endcol; w++){
	for(int h=startrow; h<endrow; h++){
	  sf::Color temp = fImage.getPixel(w,h);
	  r += temp.r;
	  g += temp.g;
	  b += temp.b;
	  a += temp.a;
	}
      }
      r /= (fGridX*fGridY);
      g /= (fGridX*fGridY);
      b /= (fGridX*fGridY);
      a = 256/2;
      int center_col = startcol + fGridX/2;
      int center_row = startrow + fGridY/2;
      sf::FloatRect boundary(startcol,startrow,fGridX,fGridY);

      fGridColor.push_back( sf::Color(r,g,b) );
      fBoundaryColor.push_back( boundary );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// A chi2 calculation of rgb pixels
//    -first_attempt fills fNow
//    -rgba looks at alpha within rgba, otherwise that is not a factor
double Mutations::CalculateFitness(bool first_attempt, bool rgba=false){
  int index = 3;
  if( rgba ) index = 4;
  
  double fitness = 0.0;
  for(int i=0; i<fNPixels; i++){
    sf::Color image   = fInput[i];
    sf::Color replica;
    if(first_attempt) {
      replica = fNow[i];
    } else {
      replica = fNext[i];
    }
    
    int imagecol[4] = {  image.r,   image.g,   image.b,   image.a};
    int repcol[4]   = {replica.r, replica.g, replica.b, replica.a};
  
    for(int j=0; j<index; j++){
      fitness += pow(imagecol[j]-repcol[j],2.0);
    }
  }
  double norm = fNPixels * index * pow(256,2.0);
  fitness /= norm;
  return 1 - fitness;
}
////////////////////////////////////////////////////////////////////////////////
// Mutate the position or RGBA depending on random #s
//
void Mutations::Mutate(int R, int col){
  // if( fNMutations < 1e5 || fFitness < 0.98 ) {
  //   R = fDisx/6;
  //   col = 50;
  // }
  col = 61;
  
  int what2mutate = fRand.Integer(2); // 0 or 1
  if( what2mutate == 0 ){
    if( R<=1 ) R=2;    // backup case R: 1 -> 2
  
    // int rad   = fRand.Integer(R) + 1; // 1 -> R
    // int theta = fRand.Integer(361);   // 0 -> 360
    // double theta_rad = theta * (TMath::Pi()/180.0);
    // double x = rad*TMath::Cos( theta_rad );
    // double y = rad*TMath::Sin( theta_rad );

    int xn = 301;
    int yn = 301;
    int x = fRand.Integer(xn);
    int y = fRand.Integer(yn);
    if( xn%2==0 ) xn++;
    if( yn%2==0 ) yn++;
    x -= (xn/2);
    y -= (yn/2);
    fRandPosX = double(x);
    fRandPosY = double(y);
    
    int vertex = fRand.Integer(3);
    int index = 3*fTriCountPos + vertex;

    // Prevent the triangle from going too far away
    sf::Vector2f newpos = fTriPositionsNow[index] + sf::Vector2f(x,y);
    if( int(newpos.x) < -10     ) x  = -10;
    if( int(newpos.x) > fDisx+10 ) x = fDisx+10;
    if( int(newpos.y) < -10     ) y  = -10;
    if( int(newpos.y) > fDisy+10 ) y = fDisy+10;
    
    fTrianglesNext[fTriCountPos][vertex].position = fTriPositionsNow[index] + sf::Vector2f(x,y);
    fTriPositionsNext[index] = fTriPositionsNow[index] + sf::Vector2f(x,y);
    
    //std::cout << "\nMutation: " <<  fTriPositions[index].x << ", " <<  fTriPositions[index].y
    //<< " ----- " << x << ", " << y << std::endl;

    // Increment for the next, or reset if it is too large:
    fTriCountPos++;
    if( fTriCountPos == fTrianglesNext.size() ) fTriCountPos = 0;
    
  } else {
    // we want this to be an odd number
    if( col%2==0 ) col++; 
    int randcol = fRand.Integer(col);
    randcol -= (col / 2);
    
    sf::Color oldcolor = fTriColorsNow[fTriCountCol]; 
    unsigned int rgba[4] = {oldcolor.r, oldcolor.g, oldcolor.b, oldcolor.a};
    int random_rgba = fRand.Integer(4);
    rgba[random_rgba] += randcol;
    //std::cout << fRGBA[random_rgba] << "\t" << rgba[random_rgba] << "\t"
    //	      << randcol << "\t" << rgba[random_rgba] - randcol << std::endl;
      
    for(int i=0; i<4; i++){
      int test_rgba = rgba[i];
      if( test_rgba >= 255 ) rgba[i] = 255;
      if( test_rgba < 0 )    rgba[i] = 0;      
    }
    
    sf::Color newcolor = sf::Color(rgba[0],rgba[1],rgba[2],rgba[3]);

    for(int i=0; i<3; i++ ) {
      fTrianglesNext[fTriCountCol][i].color = newcolor;
      fTriColorsNext[fTriCountCol] = newcolor;
    }
    
    fTriCountCol++;
    if( fTriCountCol == fTriColorsNow.size() ) fTriCountCol = 0;
  }
}
////////////////////////////////////////////////////////////////////////////////
// Did we successfully mutate, or we keep original set?
//
void Mutations::CheckMutation(){
  double fit_mutation = CalculateFitness( false, true );
  if( fNAttempts%50==0 ) {
    std::cout << fNAttempts << "\t" << fNMutations
	      << "\t" << fit_mutation << "\t" << fFitness << std::endl;
  }
  if( fit_mutation > fFitness ){
    fFitness = fit_mutation;                  // Update fitness as we have succeeded
    fNow.clear();                             // Clear Now, replace with Next
    fNow = fNext;                             // Replace
    fNext.clear();                            // Clear Next, get ready for new screenshot
    fTrianglesNow.clear();                    // Clear current triangles 
    fTrianglesNow = fTrianglesNext;           // Replace
    fTriPositionsNow.clear();                 // Clear current positions
    fTriPositionsNow = fTriPositionsNext;     // Replace
    fTriColorsNow.clear();                    // Clear current colors
    fTriColorsNow = fTriColorsNext;           // Replace
    if(fNMutations%100==0){
      fNMutateX.push_back( fNMutations );
      fFitnessY.push_back( fFitness );
    }
    fNMutations++;                            // Increment the mutation counter, or successes
  } else {
    fNext.clear();                            // Mutation is bad, discard
    fTrianglesNext.clear();                   // Discard triangles
    fTrianglesNext = fTrianglesNow;           // Reset
    fTriPositionsNext.clear();                // Discard positions
    fTriPositionsNext = fTriPositionsNow;     // Reset
    fTriColorsNext.clear();                   // Discard colors
    fTriColorsNext = fTriColorsNow;           // Reset
  }

  if(fNAttempts%100==0){
    fNAttemptX.push_back( fNAttempts );
    fFitnessYY.push_back( fFitness );
  }

  hRandPosX_Fitness->Fill(fRandPosX,fFitness);
  hRandPosY_Fitness->Fill(fRandPosY,fFitness);
  fNAttempts++;                               // Total number of attempts:
}

////////////////////////////////////////////////////////////////////////////////
// ROOT setup output
//
void Mutations::setupOutput(const char* output){
  fOut = new TFile(Form("%s.root",output),"RECREATE");
}

////////////////////////////////////////////////////////////////////////////////
// ROOT Finish Up
//
void Mutations::FinishUp(){
  TGraph *g1 = new TGraph( fNMutateX.size(), &(fNMutateX[0]), &(fFitnessY[0]) );
  g1->SetName("N_Mutations_vs_Fitness");
  g1->SetTitle("N_Mutations_vs_Fitness");
  g1->SetMarkerStyle(20);
  g1->SetMarkerColor(kBlue+2);
  g1->SetLineColor(kBlue+2);
  g1->GetXaxis()->SetTitle("# of Mutations");
  g1->GetYaxis()->SetTitle("Fitness [unitless]");
  fOut->cd();
  g1->Write();

  TGraph *g2 = new TGraph( fNAttemptX.size(), &(fNAttemptX[0]), &(fFitnessYY[0]) );
  g2->SetName("N_Attempts_vs_Fitness");
  g2->SetTitle("N_Attempts_vs_Fitness");
  g2->SetMarkerStyle(20);
  g2->SetMarkerColor(kBlue+2);
  g2->SetLineColor(kBlue+2);
  g2->GetXaxis()->SetTitle("# of Attempts");
  g2->GetYaxis()->SetTitle("Fitness [unitless]");
  fOut->cd();
  g2->Write();

  hRandPosX_Fitness->Write();
  hRandPosY_Fitness->Write();
  
  fOut->Write();
}


////////////////////////////////////////////////////////////////////////////////
// A Testing Function
//
void Mutations::Test(const char* output){
  sf::Image testing;
  testing.create(fDisx,fDisy,sf::Color(0,0,0));
   
  for(int i=0; i<fDisx; i++){
    for(int j=0; j<fDisy; j++){
      testing.setPixel(i, j, fNow[fDisx*i+j] );
    }
  }
  testing.saveToFile( Form("%s.jpg", output) );
}

