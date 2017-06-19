#ifndef MUTATIONS_HH
#define MUTATIONS_HH

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "TRandom.h"
#include "TH1D.h"
#include "TFile.h"
#include "TString.h"
#include "../include/HandleInput.hh"

class Mutations : public sf::Drawable, public sf::Transformable{

private:
  unsigned int fDisx, fDisy;             // True display dimensions in pixels
  unsigned int fNPixels;
  unsigned int fDisxP, fDisyP;           // ~20% larger so polygons can extend, less empy space
  double fNorm;
  int fNtris;                            // # of polygons to be drawn
 
  sf::VertexArray fTri;                  // 1 polygon
  std::vector<sf::VertexArray> fTris;    // The set of polygons

  std::vector<sf::Color> fImageCols;     // Vector of pixel colors of Parent
  std::vector<sf::Color> fCurrentCols, fNextCols; // Colors of current polygons, and wiggled polys
  std::map<int,int> fVertexTriMap;

  int fTriCount,fVCount;
  int fTriCountCol;

  int fWiggleX,fWiggleY,fWiggleC;
  int fIndex, fRandRGBA;
  
  double fFitnessLvl;
  std::map<int,TString> testing;
  
  TRandom fRand;
  
public:
  Mutations(unsigned int, unsigned int, int);
  ~Mutations() {};
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void GenerateTriangless(bool);             // Generate intial polygons, randomly






  
  void GenerateNextCols(sf::Image);    // every iteration
  void GenerateCurrentCols(sf::Image); // once
  void Mutate();
  void MutateColor();
  void Fitness(bool,int,int);
  
  std::map<int,sf::Color> fTriColors;                   // key is # vertices*N, value is vertex color
  std::map<int,sf::Vector2f> fTriPositions;             // key is # vertices*N, val is position
  void setImageColorVec(std::vector<sf::Color> a) { fImageCols = a; }
  // void FinishUp();
  
  // Not exactly related to this algorithm:
  void Test(const char*);
  sf::Color AntiColor(sf::Color);
};
#endif
