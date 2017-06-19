#include "../include/Mutations.hh"

#include <stdio.h>   
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "TMath.h"


Mutations::Mutations(unsigned int x, unsigned int y, int N)
  : fDisx(x), fDisy(y), fNtries(N) {
  fNPixels = fDisx * fDisy;
  fTri = sf::VertexArray(sf::Triangles,3);
  
  fFitness = 1.0e15;
  // Index to Change Mutations:
  fTriCountPos = 0;
  fTriCountCol = 0;
  fNMutations = 0;
  fNAttempts = 0;
  fRGBA[0] = "r";
  fRGBA[1] = "g";
  fRGBA[2] = "b";
  fRGBA[3] = "a";
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
//
void Mutations::GenerateTriangles(bool randomcolor, sf::Color col){
  srand( time(NULL) );
  int xhi = fDisx+1;
  int yhi = fDisy+1; 
  for(int m=0; m<fNtries; m++){
    
    unsigned int r = rand()%256; // 0-> 255
    unsigned int g = rand()%256;
    unsigned int b = rand()%256;
    unsigned int a = rand()%256;
    
    if( !randomcolor ){
      r=col.r;
      g=col.g;
      b=col.b;
    }
    
    for(int i=0; i<3; i++){
      int tempx = fRand.Integer(xhi); // 0 -> fDisx 
      int tempy = fRand.Integer(yhi); // 0 -> fDisy
    
      fTri[i].position = sf::Vector2f(tempx,tempy);
      fTri[i].color = sf::Color(r,g,b,a);  // all get the same color for this algorithm
      fTriPositions[3*m+i] = sf::Vector2f(tempx, tempy);
      fVertexMap[3*m+i] = m;
    }
    
    fTriColors[m] = sf::Color(r,g,b,a);
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
    fFitness = CalculateFitness( true, false);
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

  // Generate the Triangles:
  GenerateTriangles(false,fAverage);
}

////////////////////////////////////////////////////////////////////////////////
// Grid the input image, get average RGB value of each grid
//
void Mutations::GridInput(){
  for(int w=0; w<fDisx; w++){
    for(int h=0; h<fDisy; h++){
      sf::Color color_temp = fImage.getPixel(w,h);
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
  double norm = fNPixels * index * pow(255,2.0);
  fitness /= norm;
  return 1 - fitness;
}
////////////////////////////////////////////////////////////////////////////////
// Mutate:
//
void Mutations::Mutate(int R, int col){
  if( fNMutations < 1e5 || fFitness < 0.97 ) {
    R = fDisx/5;
    col = 35;
  }

  int what2mutate = fRand.Integer(2); // 0 or 1
  if( what2mutate == 0 ){
    if( R<=1 ) R=2;    // backup case R: 1 -> 2
  
    int rad   = fRand.Integer(R) + 1; // 1 -> R
    int theta = fRand.Integer(361);   // 0 -> 360
    double theta_rad = theta * (TMath::Pi()/180.0);
    double x = rad*TMath::Cos( theta_rad );
    double y = rad*TMath::Sin( theta_rad );
    
    int vertex = fRand.Integer(3);
    int index = 3*fTriCountPos + vertex;

    // Prevent the triangle from going to far away
    sf::Vector2f newpos = fTriPositions[index] + sf::Vector2f(x,y);
    if( int(newpos.x) < -10     ) x *= -1.0;
    if( int(newpos.x) > fDisx+10 ) x *= -1.0;
    if( int(newpos.y) < -10     ) y *= -1.0;
    if( int(newpos.y) > fDisy+10 ) y *= -1.0;
    
    fTrianglesNext[fTriCountPos][vertex].position = fTriPositions[index] + sf::Vector2f(x,y);
     
    //std::cout << "\nMutation: " <<  fTriPositions[index].x << ", " <<  fTriPositions[index].y
    //<< " ----- " << x << ", " << y << std::endl;

    // Increment for the next, or reset if it is too large:
    fTriCountPos++;
    if( fTriCountPos == fTrianglesNext.size() ) fTriCountPos = 0;  
  } else {
    // we want this to be an odd number
    if( col%2==0 ) col++; 
    int randcol = fRand.Integer(col);
    randcol -= col / 2;
    
    sf::Color oldcolor = fTriColors[fTriCountCol]; 
    unsigned int rgba[4] = {oldcolor.r, oldcolor.g, oldcolor.b, oldcolor.a};
    int random_rgba = fRand.Integer(4);
    rgba[random_rgba] += randcol;
    //std::cout << fRGBA[random_rgba] << "\t" << rgba[random_rgba] << "\t"
    //	      << randcol << "\t" << rgba[random_rgba] - randcol << std::endl;
      
    for(int i=0; i<4; i++){
      int test_rgba = rgba[i];
      if( test_rgba >= 256 ) rgba[i] = 255;
      if( test_rgba < 0 )    rgba[i] = 0;      
    }
    
    sf::Color newcolor = sf::Color(rgba[0],rgba[1],rgba[2],rgba[3]);

    for(int i=0; i<3; i++ )
      fTrianglesNext[fTriCountCol][i].color = newcolor;
    
    fTriCountCol++;
    if( fTriCountCol == fTriColors.size() ) fTriCountCol = 0;
  }
}
////////////////////////////////////////////////////////////////////////////////
// Did we successfully mutate, or we keep original set?
//
void Mutations::CheckMutation(){
  double fit_mutation = CalculateFitness( false, false);
  if( fNAttempts%50==0 ) {
    std::cout << fNAttempts << "\t" << fNMutations
	      << "\t" << fit_mutation << "\t" << fFitness << std::endl;
  }
  if( fit_mutation > fFitness ){
    fFitness = fit_mutation;
    fNow.clear();
    fNow = fNext;
    fNext.clear();
    fTrianglesNow.clear();
    fTrianglesNow = fTrianglesNext;
    // Increment the mutation counter, or successes:
    fNMutations++;
  } else {
    fNext.clear();
    fTrianglesNext.clear();
    fTrianglesNext = fTrianglesNow;
  }
  // Total number of attempts:
  fNAttempts++;
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
