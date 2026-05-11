#include <iostream>
#include <fstream>
#include <cmath>

struct Parameters {
    static constexpr double ticks_per_revolution = 1024.0;
    static constexpr double wheel_radius_m = 0.3;
    static constexpr double wheelbase_m = 1.0;
};

struct PositionData {
    double x_m;
    double y_m;
    double theta_rad;
};

struct TickData {
    long timestamp_ms;
    long fl_ticks;
    long fr_ticks;
    long bl_ticks;
    long br_ticks;

    TickData operator-(const TickData& other) const {
        TickData result;
        result.timestamp_ms = timestamp_ms - other.timestamp_ms;
        result.fl_ticks = fl_ticks - other.fl_ticks;
        result.fr_ticks = fr_ticks - other.fr_ticks;
        result.bl_ticks = bl_ticks - other.bl_ticks;
        result.br_ticks = br_ticks - other.br_ticks;
        return result;
    }
};

PositionData calcOdometryStep(Parameters params, PositionData pos, TickData step, TickData prev_step) {
    double distance_per_tick = 2 * M_PI * params.wheel_radius_m / params.ticks_per_revolution;
    TickData delta = step - prev_step;
    double d_left  = (delta.fl_ticks + delta.bl_ticks) / 2.;
    double d_right = (delta.fr_ticks + delta.br_ticks) / 2.;
    double dL = d_left  * distance_per_tick;
    double dR = d_right * distance_per_tick;
    double d      = (dL + dR) / 2.;
    double dtheta = (dR - dL) / params.wheelbase_m;
    double x = pos.x_m + d * cos(pos.theta_rad + dtheta / 2);
    double y = pos.y_m + d * sin(pos.theta_rad + dtheta / 2);

    return PositionData{x, y, pos.theta_rad + dtheta};
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    std::ifstream fin(argv[1]);

    if (!fin.is_open()) {
        std::cerr << "error: could not open input file\n";
        return 1;
    }

    Parameters params; // with default values

    TickData step;
    TickData prev_step;
    fin 
        >> prev_step.timestamp_ms
        >> prev_step.fl_ticks
        >> prev_step.fr_ticks
        >> prev_step.bl_ticks
        >> prev_step.br_ticks;

    PositionData pos{0.0, 0.0, 0.0};

    while (fin >> step.timestamp_ms >> step.fl_ticks >> step.fr_ticks >> step.bl_ticks >> step.br_ticks) {
        pos = calcOdometryStep(params, pos, step, prev_step);
        std::cout
            << step.timestamp_ms << " "
            << pos.x_m << " "
            << pos.y_m << " "
            << pos.theta_rad
            << std::endl;

        prev_step = step;
    }

    return 0;
}
