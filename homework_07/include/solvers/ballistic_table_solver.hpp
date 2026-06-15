#ifndef MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP
#define MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP

#include <vector>
#include <fstream>

namespace miltech04 {

struct BallisticTable {
    // 5 осей — кожна зі своїм набором вузлів (нерівномірний крок)
    std::vector<float> axisZ0;  // висота
    std::vector<float> axisV0;  // швидкість
    std::vector<float> axisM;   // маса
    std::vector<float> axisD;   // опір
    std::vector<float> axisL;   // підйомна сила
 
    // Результат в кожному вузлі сітки
    struct Result {
        float t;      // час польоту
        float hDist;  // горизонтальна дистанція
    };
 
    // Плоский масив розміром |Z0| * |V0| * |M| * |D| * |L|
    std::vector<Result> data;
 
    // Індекс у плоскому масиві: [iZ0][iV0][iM][iD][iL]
    size_t index(int iz, int iv, int im, int id, int il) const {
        return ((((size_t)iz * axisV0.size() + iv)
                              * axisM.size()  + im)
                              * axisD.size()  + id)
                              * axisL.size()  + il;
    }
 
    const Result& at(int iz, int iv, int im,
                     int id, int il) const {
        return data[index(iz, iv, im, id, il)];
    }
 
    // Завантаження з текстового файлу
    bool load(const char* path) {
        std::ifstream f(path);
        if (!f.is_open()) return false;
 
        int nZ, nV, nM, nD, nL;
        f >> nZ >> nV >> nM >> nD >> nL;
 
        axisZ0.resize(nZ); for (auto& v : axisZ0) f >> v;
        axisV0.resize(nV); for (auto& v : axisV0) f >> v;
        axisM.resize(nM);  for (auto& v : axisM)  f >> v;
        axisD.resize(nD);  for (auto& v : axisD)  f >> v;
        axisL.resize(nL);  for (auto& v : axisL)  f >> v;
 
        size_t total = (size_t)nZ*nV*nM*nD*nL;
        data.resize(total);
 
        // Порядок: Z0 → V0 → m → d → l (зовнішній → внутрішній)
        for (size_t i = 0; i < total; i++)
            f >> data[i].t >> data[i].hDist;
 
        return f.good();
    }
};

class BallisticTableSolver : public IBallisticSolver {
    IConfigLoader* configSource;
    double flightTime;
    double hDist;
    void init();
    double calcFlightTime();
    double calcFireDistance();
    Coord calcPredictedTarget(const Coord& targetPos, const Coord& targetV) const ;
    double getTimeToFirePointFromStopped(const SimStep& drone, const Coord& target) const;
    double getTimeToFirePoint(const SimStep& drone, const Coord target) const;

public:
    BallisticTableSolver(IConfigLoader* configSource);
    virtual bool isValid() override;
    virtual double getFireDistance() override;
    virtual Solution solve(const SimStep& drone, const Target& target) const override;
    virtual ~BallisticTableSolver() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP