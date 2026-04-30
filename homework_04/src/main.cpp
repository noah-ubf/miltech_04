#include <iostream>
#include <fstream>

struct TickData {
    long timestamp_ms;
    long fl_ticks;
    long fr_ticks;
    long bl_ticks;
    long br_ticks;
};

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

    // TODO: implement wheel odometry for a 4-wheel differential-drive UGV.
    //
    // Parameters:
    //   ticks_per_revolution = 1024
    //   wheel_radius_m       = 0.3
    //   wheelbase_m          = 1.0
    //
    // Input:  text file with 5 whitespace-separated numbers per line:
    //         timestamp_ms fl_ticks fr_ticks bl_ticks br_ticks
    // Output: same tabular format on stdout, starting from the second sample:
    //         timestamp_ms x y theta

    TickData step;
    TickData prev_step;
    fin 
        >> prev_step.timestamp_ms
        >> prev_step.fl_ticks
        >> prev_step.fr_ticks
        >> prev_step.bl_ticks
        >> prev_step.br_ticks;

    while (fin >> step.timestamp_ms >> step.fl_ticks >> step.fr_ticks >> step.bl_ticks >> step.br_ticks) {
        std::cout
            << "Timestamp=" << step.timestamp_ms
            << ", fl_ticks=" << step.fl_ticks
            << ", fr_ticks=" << step.fr_ticks
            << ", bl_ticks=" << step.bl_ticks
            << ", br_ticks=" << step.br_ticks
            << std::endl;
        prev_step = step;
    }

    return 0;
}
