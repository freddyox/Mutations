#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>

#include "../include/HandleInput.hh"
#include "../include/Mutations.hh"

int main(int argc, char* argv[]) {
  //////////////////////////////////////////////////////
  //                   Initialize                     //
  //////////////////////////////////////////////////////
  std::string tempname = "";
  int nvertices = 3;
  if(argc==1){        // no arguments
    tempname = "/home/obrecht/Documents/Projects_New/Mutations/pics/einstein25.jpg";
    nvertices = 3;
  }
  else if( argc==2 ){ // 1 argument
    tempname = argv[1];
    nvertices = 3;
  }
  else if( argc==3 ){
    tempname = argv[1];
    std::istringstream ss(argv[2]);
    int x;
    if( !(ss >> x) ){
      std::cout << "Invalid vertex argument. Setting it to the default value of 3" << std::endl;
      x=3;
    } else {
      nvertices = std::atoi(argv[2]);
    }
  } else {
    std::cerr << "Wrong number of arguments pass. Default settings will be used." << std::endl;
    tempname = "/home/obrecht/Documents/Projects_New/Mutations/pics/mona-lisa.jpg";
    nvertices = 3;
  }

  // 1) Initalize the image to be mutated, grab display x/y
  std::string fImName = tempname;

  HandleInput fImage(fImName);
  
  sf::Vector2u fSize = fImage.getSize();
  if( fSize.x==0 || fSize.y==0 ){ std::cerr << "Error: Window sizes are zero!" << std::endl; }
  
  // This is clunky, and totally poor coding practices.
  std::vector<sf::Color> fcols = fImage.getColorVec();

  // 2) Initialize the class to handle mutations:
  Mutations fMutate(fSize.x, fSize.y, 500, nvertices);
  fMutate.setImageColorVec(fcols);
  fMutate.SetImage( fImage.getImage() );
  fMutate.GridInput(4);
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
  int nss = 0;
  //////////////////////////////////////////////////////
  //                   Main Loop                      //
  //////////////////////////////////////////////////////
  while( window.isOpen() && window_mutate.isOpen() ) {

    sf::Event event;
    while( window.pollEvent(event) ) {
      if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
  	window.close();
	fMutate.FinishUp();
      }
    }
    while( window_mutate.pollEvent(event) ) {
      if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
  	window_mutate.close();
      }
    }
    window.clear();
    window_mutate.clear(sf::Color::Black);
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
      //fMutate.Test("a_test");1
      fFirstTry = false;
    } else{
      //if(counter>0 && counter%10==0 ) {
	fMutate.GetAttempt( window_mutate.capture(),false);
	fMutate.CheckMutation();
	//}
    }
    
    window.display();
    window_mutate.display();
    //window_dia.display();

    if(counter%2500==0) {
      sf::Image ss = window_mutate.capture();
      int n = int(fMutate.GetNAttempts());
      int m = int(fMutate.GetMutationNumber());
      const char* name = Form("screenshots/%03d_%06d_%05d_%.4f.png",nss,n,m,fMutate.GetBestChi2() );
      ss.saveToFile( name );
      nss++;
    }

    if( fMutate.CloseProgram() ){ return 0; }
    
    counter++;
  }
 
  return 0;
}
