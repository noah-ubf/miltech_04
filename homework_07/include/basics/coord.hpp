#ifndef MILTECH_INCLUDE_BASICS_COORD_HPP
#define MILTECH_INCLUDE_BASICS_COORD_HPP

namespace miltech04 {

struct Coord {
    double x;
    double y;

    Coord operator+(const Coord& other) const;
    Coord operator-(const Coord& other) const;
    Coord operator*(double s) const;
    Coord operator/(double s) const;
    bool operator==(Coord c) const;
    double length() const;
    Coord normalize() const;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_COORD_HPP