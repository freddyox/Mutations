#include "../include/Mutations.hh"

#include <stdio.h>   
#include <stdlib.h>
#include <iostream>
#include <cmath>


Mutations::Mutations(unsigned int x, unsigned int y, int N)
  : fDisx(x), fDisy(y), fNtris(N) {
  fDisxP = fDisx*3/2;
  fDisyP = fDisy*3/2;
  fNPixels = fDisx * fDisy;
  fTri = sf::VertexArray(sf::Triangles,3);
  GenerateTriangless(true);



 
}

////////////////////////////////////////////////////////////////////////////////
// Draw
//
void Mutations::draw( sf::RenderTarget& target, sf::RenderStates) const {
  std::vector<sf::VertexArray>::const_iterator cit;
  for( cit = fTris.begin(); cit != fTris.end(); cit++ ){
    target.draw( *cit );
  }
}

////////////////////////////////////////////////////////////////////////////////
// Generate Initial Triangles:
//
void Mutations::GenerateTriangless(bool random){
  srand( time(NULL) );
  for(int m=0; m<fNtris; m++){
    
    unsigned int r = rand()%256; // 0-> 255
    unsigned int g = rand()%256;
    unsigned int b = rand()%256;
    unsigned int a = rand()%256;
    
    if( !random ){ r=0; g=0; b=0; a=0; }
    
    for(int i=0; i<3; i++){
      int tempx = (rand()%(fDisxP+1)) - fDisx/4;
      int tempy = (rand()%(fDisyP+1)) - fDisy/4;
    
      fTri[i].position = sf::Vector2f(tempx,tempy);
      fTri[i].color = sf::Color(r,g,b,a);  // all get the same color for this algorithm
      fTriPositions[3*m+i] = sf::Vector2f(tempx,tempy);
      fVertexTriMap[3*m+i] = m;
    }
    
    fTriColors[m] = sf::Color(r,g,b,a);
    fTris.push_back(fTri);
  }
}

void Mutations::Mutate(){
  // Let's systematically go through the entire vector so we do not miss opportunities:
  if( fTriCount == fTris.size() ) {
    fTriCount = 0;
    fVCount++;
  }
  if( fVCount >= 3 ) fVCount = 0;
  
  int wigglex = fRand.Integer(11) - 5; 
  int wiggley = fRand.Integer(11) - 5;
  fWiggleX = wigglex;
  fWiggleY = wiggley;
  
  int index = 3*fTriCount + fVCount;
  fIndex = index;
  
  fTris[fTriCount][fVCount].position = fTriPositions[index] + sf::Vector2f(wigglex,wiggley);
  fTriPositions[index] += sf::Vector2f(wigglex,wiggley); // update it
}

void Mutations::MutateColor(){

  if( fTriCountCol == fTris.size() ) {
    fTriCountCol = 0;
  }

  int wiggle = fRand.Integer(7) - 3;
  sf::Color tricol = fTriColors[fTriCountCol]; 
  unsigned int rgba[4] = {tricol.r, tricol.g, tricol.b, tricol.a};
  unsigned int rgbat[4] = {tricol.r, tricol.g, tricol.b, tricol.a};
  int random_rgba = fRand.Integer(4); //
  rgba[random_rgba] += wiggle;

  fRandRGBA = random_rgba;
  fWiggleC = wiggle;
  
  sf::Color newcolor = sf::Color(rgba[0],rgba[1],rgba[2],rgba[3]);

  if( fTriCountCol== 0 ){
    std::cout << "wiggling " << testing[random_rgba] << " by " << wiggle <<  std::endl;
    std::cout << Form("b: %d \t %d \t %d \t %d",rgbat[0],rgbat[1],rgbat[2],rgbat[3]) << std::endl;
    std::cout << Form("a: %d \t %d \t %d \t %d",rgba[0],rgba[1],rgba[2],rgba[3]) << std::endl;
  }
  for(int i=0; i<3; i++){
    fTris[fTriCountCol][i].color = newcolor;
  }
  
  fTriColors[fTriCountCol] = newcolor;
}

void Mutations::GenerateCurrentCols(sf::Image win){
  for(int w=0; w<fDisx; w++){
    for(int h=0; h<fDisy; h++){
      sf::Color coltemp = win.getPixel(w,h);
      fCurrentCols.push_back( coltemp );
    }
  }
}

void Mutations::GenerateNextCols(sf::Image win){
  for(int w=0; w<fDisx; w++){
    for(int h=0; h<fDisy; h++){
      sf::Color coltemp = win.getPixel(w,h);
      fNextCols.push_back( coltemp );
    }
  }
}

void Mutations::Fitness(bool first, int which, int C){
  
  double fitness = 0.0;
  for(int i=0; i<fNPixels; i++){
    sf::Color image   = fImageCols[i];
    sf::Color replica;
    if(first) {
      replica = fCurrentCols[i];
    } else {
      replica = fNextCols[i];
    }
    int imagecol[4] = {  image.r,  image.g,  image.b,  image.a};
    int repcol[4]   = {replica.r,replica.g,replica.b,replica.a};
  
    for(int j=0; j<4; j++){
      fitness += pow(imagecol[j]-repcol[j],2.0);
    }
  }
  
  std::cout << C << "  " << fitness << "  " << fFitnessLvl << std::endl;

  if( fitness < fFitnessLvl ) {
    fFitnessLvl = fitness;    // set fitness
    if(C>0){
      fCurrentCols.clear();     
      fCurrentCols = fNextCols; // mutated becomes current
    }
    fNextCols.clear();        // clear mutated
  } else {
    // Keep Current, but clear Next and prepare for a new set:
    fNextCols.clear();
    if(which==1){
      std::cout << "do we move back" << std::endl;
      fTris[fTriCount][fVCount].position = fTriPositions[fIndex] - sf::Vector2f(fWiggleX,fWiggleY);
      fTriPositions[fIndex] -=  sf::Vector2f(fWiggleX,fWiggleY); // update it
    }
    if(which==2){
      sf::Color temp;
      switch(fRandRGBA){
      case 0: temp = sf::Color(fWiggleC,0,0,0);
	break;
      case 1: temp = sf::Color(0,fWiggleC,0,0);
	break;
      case 2: temp = sf::Color(0,0,fWiggleC,0);
	break;
      case 3: temp = sf::Color(0,0,0,fWiggleC);
	break;
      default:
	break;
      }
      
      fTriColors[fTriCountCol] -= temp;
    }
  }
  if(which==1){
    fTriCount++;
  }
  if(which==2){
    fTriCountCol++;
  }
}

// This method works in reproducing Mona-lisa
void Mutations::Test(const char* name){
  sf::Image testing;
  testing.create(fDisx,fDisy,sf::Color(0,0,0));
   
  for(int i=0; i<fDisx; i++){
    for(int j=0; j<fDisy; j++){
      testing.setPixel(i, j, AntiColor(fImageCols[350*i+j]) );
    }
  }
  testing.saveToFile( Form("%s.jpg", name) );
}

sf::Color Mutations::AntiColor(sf::Color col){
  unsigned int r = 255 - col.r;
  unsigned int g = 255 - col.g;
  unsigned int b = 255 - col.b;
  unsigned int a = 255 - col.a;
  return sf::Color(r,g,b,a);
}
