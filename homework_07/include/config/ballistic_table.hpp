#ifndef MILTECH_INCLUDE_CONFIG_BALLISTIC_TABLE_HPP
#define MILTECH_INCLUDE_CONFIG_BALLISTIC_TABLE_HPP

#include <string>
#include <vector>

namespace miltech04 {

// Результат в кожному вузлі сітки
struct Result {
    double t;      // час польоту
    double hDist;  // горизонтальна дистанція
};

struct BallisticTable {
    // 5 осей — кожна зі своїм набором вузлів (нерівномірний крок)
    std::vector<double> axisZ0;  // висота
    std::vector<double> axisV0;  // швидкість
    std::vector<double> axisM;   // маса
    std::vector<double> axisD;   // опір
    std::vector<double> axisL;   // підйомна сила
  
    // Плоский масив розміром |Z0| * |V0| * |M| * |D| * |L|
    std::vector<Result> data;
 
    // Індекс у плоскому масиві: [iZ0][iV0][iM][iD][iL]
    size_t index(int iz, int iv, int im, int id, int il) const;
    const Result& at(int iz, int iv, int im,
                     int id, int il) const;
    // Завантаження з текстового файлу
    bool load(std::string path);
    Result lookup(double Z0, double V0, double m, double d,  double l) const;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CONFIG_BALLISTIC_TABLE_HPP