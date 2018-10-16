#include "../include/HandleInput.hh"
#include <iostream>

HandleInput::HandleInput(std::string s) : fImageName(s) {

  if( !fImage.loadFromFile(fImageName) ) {
    std::cerr << "ERROR: " << fImageName << " failed to open. Check spelling." << std::endl;
  }
  if( !fTx.loadFromImage(fImage) ){
    std::cerr << "ERROR: Texture failed..." << std::endl;
  }

  fSp.setTexture( fTx );
  sf::FloatRect rect = fSp.getLocalBounds();
  fWidth  = rect.width;
  fHeight = rect.height;
  fSize = sf::Vector2u( fWidth, fHeight );
  
  for(int i=0; i<fWidth; i++){
    for(int j=0; j<fHeight; j++){
      sf::Color tempcol = fImage.getPixel(i,j);
      fImageColors.push_back( tempcol );
    }
  }
  
}

void HandleInput::draw( sf::RenderTarget& target, sf::RenderStates) const {
  target.draw(fSp);
}
