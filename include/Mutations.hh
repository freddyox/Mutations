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
  // Setting up the problem:
  unsigned int fDisx, fDisy;               // True display dimensions in pixels
  unsigned int fNPixels;                   // N Pixels, same for all windows and images
  unsigned int fGridX, fGridY;             // Grid input image, sum rgb for each grid
  int fNtries;                             // # of polygons to be drawn

  // Handle the Polygons:
  sf::VertexArray fTri;                         // 1 polygon
  std::vector<sf::VertexArray> fTrianglesNow;   // The set of polygons prior to mutation
  std::vector<sf::VertexArray> fTrianglesNext;  // The set of polygons after mutation
  std::map<int,sf::Vector2f> fTriPositionsNow;  // positions prior to mutation
  std::map<int,sf::Vector2f> fTriPositionsNext; // positions after
  std::map<int,sf::Color> fTriColorsNow;        // colors prior
  std::map<int,sf::Color> fTriColorsNext;       // colors after

  // The Input Image:
  sf::Image fImage;                             // Input image to be replicated
  std::vector<sf::Color> fInput;                // Color vector of input image
  sf::Color fAverage;                           // rgb average of input image
  std::vector<sf::Color> fGridColor;            // grid input image, average pixels in grid
  std::vector<sf::FloatRect> fBoundaryColor;    // boundary representing location of the grid
  
  // The Mutated Image:        
  std::vector<sf::Color> fNow, fNext;           // vector of colors before (after) mutation
  int fTriCountPos, fTriCountCol;               // indices to access vertex array to mutate
  std::map<int,TString> fRGBA;                  // map between random number and random rgba mutated

  // Diagnostics:
  long long int fNMutations, fNAttempts;        // # of mutations, # of attempts     
  double fFitness;                              // 1-chi2-minimzation of rgba difference
  
  TRandom fRand; 
  std::vector<sf::RectangleShape> fRects;       // testing
  
public:
  Mutations(unsigned int, unsigned int, int);
  ~Mutations() {};
  void draw(sf::RenderTarget&, sf::RenderStates) const;              // Generic draw
  
  void GenerateTriangles(bool randcolor,sf::Color color, bool grid); // Generate intial polygons
  void GetAttempt(sf::Image, bool first_try);                        // Get Mutation screen shot

  void Mutate(int,int);  // Apply mutations to position and rgba 
  double CalculateFitness(bool,bool); // difference square of rgb(a) b/t input and mutated
  void CheckMutation();               // check fitness level, keep mutation if new fitness is better
  
  // Retrieve the sample image:
  void SetImage(sf::Image win) {fImage=win;} // Get the Image to process
  void GetAverageRGB();                      // Average the rgb over all pixels
  void GridInput(int);                       // grid input image, then average each grid
  void setImageColorVec(std::vector<sf::Color> a) { fInput = a; } // vector of input colors

  // Retrieve Diagnostics:
  double GetBestChi2() { return fFitness; }     
  long long int GetMutationNumber() { return fNMutations; }
  long long int GetNAttempts() { return fNAttempts; }

  // Testing output of window capture and some functionality:
  void Test(const char* output);
};
#endif
