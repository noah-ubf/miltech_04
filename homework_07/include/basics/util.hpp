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

namespace miltech04 {

double addAngles(double angle, double increment);

} // namespace miltech04

#endif // MILTECH_INCLUDE_UTIL_HPP
