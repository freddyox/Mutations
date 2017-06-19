#include <SFML/Graphics.hpp>

sf::Color AntiColor(sf::Color col){
  unsigned int r = 255 - col.r;
  unsigned int g = 255 - col.g;
  unsigned int b = 255 - col.b;
  unsigned int a = 255 - col.a;
  return sf::Color(r,g,b,a);
}

sf::Color GaussColor(sf::Color col){
  unsigned int colarr[4] = {col.r,col.g,col.b,col.a};
  for(int k=0; k<4; k++){
    int random = fRand.Gaus(colarr[k],20);
    colarr[k] += random;
    if( colarr[k] > 255 ) colarr[k] = 255;
    if( colarr[k] <= 0   ) colarr[k] = 1;
  }
  return sf::Color(colarr[0],colarr[1],colarr[2],colarr[3]);
}

sf::Color HSI(sf::Color col){
  double r = double(col.r);
  double g = double(col.g);
  double b = double(col.b);
 
  double I = (r+g+b)/3.0;
  double S = 255*(1.0 - (3.0/(r+g+b))*smallest(r,g,b));

  double N = 0.5*( (r-g) +(r-b) );
  if( N<0.0 ) N=0.0;
  double D = (r-g)*(r-g) + (r-b)*(g-b);
  double arg = sqrt( N/D );
  double H = acos(arg);
  std::cout << I << "  " << S << "  " << r-g+r-b << std::endl;
  return sf::Color(int(I),int(S),int(H));
}

sf::Color Chroma(sf::Color col){
  double r = double(col.r);
  double g = double(col.g);
  double b = double(col.b);
 
  double alpha = 0.5*(2.0*r - g - b);
  double beta = (1.73205/2.0)*(g-b);
  double H2 = atan2(beta,alpha);
  double C2 = sqrt( pow(alpha,2.0) + pow(beta,2.0) );
  return sf::Color(int(alpha),int(beta),int(H2),int(C2));
}

int smallest(int x, int y, int z){
    return std::min({x, y, z});
}
int largest(int x, int y, int z){
    return std::max({x, y, z});
}
