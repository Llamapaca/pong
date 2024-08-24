#include <iostream>
#include <iomanip>

int delay = 0;
float average = 0.0f;

void log_delta_time(float deltaTime) {
  delay++;
  if (average == 0.0) {
    std::cout << std::endl;
    average = deltaTime;
    return;
  }
  average = (average + deltaTime) / 2;
  if (delay >= 100) {
    std::cout << "\033[A" << "Frametime: " << std::setprecision(4)
              << average * 1000 << "  ms" << std::endl;
    delay = 0;
  }
};
