#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

#include "../include/HandleInput.hh"
#include "../include/Mutations.hh"

int main(int argc, char* argv[]) {
  //////////////////////////////////////////////////////
  //                   Initialize                     //
  //////////////////////////////////////////////////////

  // 1) Initalize the image to be mutated, grab display x/y
  std::string fImName = "/home/obrecht/Documents/Projects_New/GeneticEvolution/pics/mona-lisa.jpg";
  HandleInput fImage(fImName);
  
  sf::Vector2u fSize = fImage.getSize();
  if( fSize.x==0 || fSize.y==0 ){ std::cerr << "Error: Window sizes are zero!" << std::endl; }
  
  // This is clunky, and totally poor coding practices.
  std::vector<sf::Color> fcols = fImage.getColorVec();

  // 2) Initialize the class to handle mutations:
  Mutations fMutate(fSize.x, fSize.y, 50);
  fMutate.setImageColorVec(fcols);
  fMutate.GetAverageRGB();
  bool fFirstTry = true;
  
  //////////////////////////////////////////////////////
  //                   Windows                        //
  //////////////////////////////////////////////////////
  sf::RenderWindow window(sf::VideoMode(fSize.x,fSize.y), "Initial Image");
  window.setFramerateLimit(60);
  
  sf::RenderWindow window_mutate(sf::VideoMode(fSize.x,fSize.y), "Mutated Image");
  window_mutate.setFramerateLimit(60);

  //Set their positions relative to any desktop:
  sf::VideoMode desktop_dimensions = sf::VideoMode::getDesktopMode();
  unsigned int desktop_center_x = desktop_dimensions.width  / 2;
  unsigned int desktop_center_y = desktop_dimensions.height / 2;
  
  unsigned int window_width = window.getSize().x;
  unsigned int window_height = window.getSize().y;

  sf::Vector2i pos( desktop_center_x - window_width,desktop_center_y - window_height/2) ;
  sf::Vector2i pos_mutate( desktop_center_x, desktop_center_y - window_height/2) ;
  
  window.setPosition( pos );
  window_mutate.setPosition( pos_mutate );

  // sf::RenderWindow window_dia(sf::VideoMode(fSize.x,fSize.y), "Diagnostics");
  // window_dia.setFramerateLimit(60);
  // sf::Font font;
  // if( !font.loadFromFile("fonts/arial.ttf") ){
  //   std::cerr << "Error loading the fonts..." << std::endl;
  // }
  // sf::Text text("0.0%", font);
  // text.setCharacterSize(30);
  // text.setStyle(sf::Text::Bold);
  // text.setColor(sf::Color::Red);

  // sf::Text text1("0", font);
  // text1.setCharacterSize(30);
  // text1.setStyle(sf::Text::Bold);
  // text1.setColor(sf::Color::Red);
  long int counter=0;
 
  //////////////////////////////////////////////////////
  //                   Main Loop                      //
  //////////////////////////////////////////////////////
  while( window.isOpen() && window_mutate.isOpen() ) {

    sf::Event event;
    while( window.pollEvent(event) ) {
      if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
  	window.close();
      }
    }
    while( window_mutate.pollEvent(event) ) {
      if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
  	window_mutate.close();
      }
    }
    window.clear();
    window_mutate.clear();
    //window_dia.clear(sf::Color::White);

    // Needs to happen after first try:
    if( !fFirstTry ) {
      fMutate.Mutate(8,5);
    }
    
    window.draw( fImage );
    window_mutate.draw( fMutate );

    // if(counter%100==0) {
    //   text.setString( Form("%f",100.0*fMutate.GetBestChi2() ));
    //   text.setPosition(10,100);
    //   text1.setString( Form("%d",fMutate.GetMutationNumber() ) );
    //   text1.setPosition(10,10);
    //   }
    //window_dia.draw(text);
    //window_dia.draw(text1);

    // This can be cleaned up:
    if( fFirstTry ){
      fMutate.GetAttempt( window_mutate.capture(),true);
      //fMutate.Test("a_test");
      fMutate.SetImage( window_mutate.capture() );
      fFirstTry = false;
    }else{
      fMutate.GetAttempt( window_mutate.capture(),false);
      fMutate.CheckMutation();
    }
    
    window.display();
    window_mutate.display();
    //window_dia.display();

    counter++;
  } 
  return 0;
}
