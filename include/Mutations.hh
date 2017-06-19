#ifndef MUTATIONS_HH
#define MUTATIONS_HH

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "TString.h"
#include "TRandom.h"
#include "../include/HandleInput.hh"

class Mutations : public sf::Drawable, public sf::Transformable{

private:
  unsigned int fDisx, fDisy;               // True display dimensions in pixels
  unsigned int fNPixels;                   // N Pixels, same for all windows and images

  int fNtries;                             // # of polygons to be drawn

  // Handle the Polygons:
  sf::VertexArray fTri;                        // 1 polygon
  std::vector<sf::VertexArray> fTrianglesNow;  // The set of polygons prior to mutation
  std::vector<sf::VertexArray> fTrianglesNext; // The set of polygons after mutation
  std::map<int,sf::Vector2f> fTriPositions;     // The set of colors
  std::map<int,int> fVertexMap;                 // Mapping between vertex index and triangle
  std::map<int,sf::Color> fTriColors;           // Mapping between triangle and its color

  // The Input Image:
  sf::Image fImage;
  std::vector<sf::Color> fInput;
  sf::Color fAverage;
  
  // The Mutated Image:
  std::vector<sf::Color> fNow, fNext;
  int fTriCountPos, fTriCountCol;
  std::map<int,TString> fRGBA;

  // Diagnostics:
  long long int fNMutations, fNAttempts;
  
  double fFitness;
  TRandom fRand;
  
public:
  Mutations(unsigned int, unsigned int, int);
  ~Mutations() {};
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void GenerateTriangles(bool randcolor,sf::Color);   // Generate intial polygons
  void GetAttempt(sf::Image, bool first_try);         // Get Initial Mutation Attempt:

  void Mutate(int,int);
  
  double CalculateFitness(bool,bool);
  void CheckMutation();
  
  // Retrieve the sample image:
  void SetImage(sf::Image win) {fImage=win;}
  void GetAverageRGB();
  void GridInput();
  void setImageColorVec(std::vector<sf::Color> a) { fInput = a; }

  // Retrieve Diagnostics:
  double GetBestChi2() { return fFitness; }
  long long int GetMutationNumber() { return fNMutations; }
  long long int GetNAttempts() { return fNAttempts; }
  // Testing output of window capture and some functionality:
  void Test(const char* output);
};
#endif
