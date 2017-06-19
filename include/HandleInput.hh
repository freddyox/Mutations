#ifndef HANDLEINPUT_HH
#define HANDLEINPUT_HH

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class HandleInput : public sf::Drawable, public sf::Transformable {

private:
  double fDisx, fDisy;
  const std::string fImageName;

  sf::Image fImage;
  sf::Texture fTx;
  sf::Sprite fSp;

  int fWidth, fHeight;
  sf::Vector2u fSize;
  
public:
  HandleInput(std::string);
  ~HandleInput() {};
  
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  int getWidth()  { return fWidth; }
  int getHeight() { return fHeight; }
  sf::Vector2u getSize() { return fSize; }
  
  std::vector<sf::Color> fImageColors;
  std::vector<sf::Color> getColorVec() { return fImageColors; }
  
};
#endif
