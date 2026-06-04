#ifndef MILTECH_INCLUDE_UTIL_HPP
#define MILTECH_INCLUDE_UTIL_HPP

#if ENABLE_LOG
  #define LOG(msg) std::cout << "[LOG] " << msg << std::endl
#else
  #define LOG(msg)
#endif
 
#if ENABLE_DEBUG
  #define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
  #define DEBUG(msg)
#endif

#ifdef ENABLE_DEBUG
    const char* STATE_NAMES[5] = {"STOPPED", "ACCELERATING", "DECELERATING", "TURNING", "MOVING"};
#endif

double addAngles(double angle, double increment);

#endif // MILTECH_INCLUDE_UTIL_HPP
