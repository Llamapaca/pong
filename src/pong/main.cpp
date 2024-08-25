

#include <pong.hpp>

int main()
{

  Pong::Game pong = Pong::getGame();

  while (pong.alive())
  {
    pong.run();
  }
  std::cout << "Main thread exits" << std::endl;
  return 0;
}
