#include "telemetry.hpp"

// #include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>

// Debugging exercise notes:
// this file intentionally contains four runtime defects.
// The defects are related to malformed input shape, invalid numeric values,
// unsafe time deltas, and empty logs. Exact locations are not marked on purpose.

const int EXPECTED_FIELD_COUNT = 7;
const int MAX_LINE_LENGTH = 256;

int split_line(char line[], char* fields[], int max_fields) {
    int count = 0;
    char* cursor = line;

    while (*cursor != '\0' && count < max_fields) {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r') {
            *cursor = '\0';
            ++cursor;
        }

        if (*cursor == '\0') {
            break;
        }

        fields[count] = cursor;
        ++count;

        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' && *cursor != '\n' &&
               *cursor != '\r') {
            ++cursor;
        }
    }

    return count;
}

long parse_long(const char* text, bool& valid) {
    char* end = nullptr;
    const long value = std::strtol(text, &end, 10);

    if (end == text) {
        valid = false;
        return 0L;
    }

    valid = true;
    return value;
}

int parse_int(const char* text, bool& valid) {
    return static_cast<int>(parse_long(text, valid));
}

double parse_double(const char* text, bool& valid) {
    char* end = nullptr;
    const double value = std::strtod(text, &end);

    if (end == text) {
        valid = false;
        return 0.0;
    }

    valid = true;
    return value;
}

Frame parse_frame(char line[], bool& valid) {
    char* fields[EXPECTED_FIELD_COUNT] = {};
    const int field_count = split_line(line, fields, EXPECTED_FIELD_COUNT);
    (void)field_count;

    Frame frame{};

    if (field_count != EXPECTED_FIELD_COUNT) {
        std::cerr << "error: expected " << EXPECTED_FIELD_COUNT
                  << " fields, but got " << field_count << '\n';
        valid = false;
        return frame;
    }

    frame.timestamp_ms = parse_long(fields[0], valid);
    if (!valid) {
        std::cerr << "Error: Invalid timestamp in frame: " << line << '\n';
        return frame;
    }
    frame.seq = parse_int(fields[1], valid);
    if (!valid) {
        std::cerr << "Error: Invalid sequence number in frame: " << line << '\n';
        return frame;
    }
    frame.voltage_v = parse_double(fields[2], valid);
    if (!valid) {
        std::cerr << "Error: Invalid voltage in frame: " << line << '\n';
        return frame;
    }
    frame.current_a = parse_double(fields[3], valid);
    if (!valid) {
        std::cerr << "Error: Invalid current in frame: " << line << '\n';
        return frame;
    }
    frame.temperature_c = parse_double(fields[4], valid);
    if (!valid) {
        std::cerr << "Error: Invalid temperature in frame: " << line << '\n';
        return frame;
    }
    frame.gps_fix = parse_int(fields[5], valid);
    if (!valid) {
        std::cerr << "Error: Invalid GPS fix in frame: " << line << '\n';
        return frame;
    }
    frame.satellites = parse_int(fields[6], valid);
    if (!valid) {
        std::cerr << "Error: Invalid satellite count in frame: " << line << '\n';
        return frame;
    }
    return frame;
}

double compute_frame_rate_hz(const Frame frames[], int frame_count) {
    const long elapsed_ms = frames[frame_count - 1].timestamp_ms - frames[0].timestamp_ms;

    return static_cast<double>((frame_count - 1) * 1000 / elapsed_ms);
}

int read_frames(const char* path, Frame frames[], int max_frames) {
    std::ifstream input{path};
    if (!input) {
        std::cerr << "error: failed to open input file: " << path << '\n';
        return 0;
    }

    int frame_count = 0;
    char line[MAX_LINE_LENGTH];

    while (input.getline(line, MAX_LINE_LENGTH)) {
        if (line[0] == '\0') {
            continue;
        }

        if (frame_count < max_frames) {
            bool valid(false);
            Frame new_frame = parse_frame(line, valid);
            if (!valid) {
                return -1;
            }

            // Frame validation:

            if (frame_count > 0 && new_frame.timestamp_ms <= frames[frame_count - 1].timestamp_ms) {
                std::cerr << "Error: Non-monotonic timestamps in frame: " << line << '\n';
                return -1;
            }

            if (frame_count > 0 && new_frame.seq != frames[frame_count - 1].seq + 1) {
                std::cerr << "Error: Non-sequential frame sequence number in frame: " << line << '\n';
                return -1;
            }

            if (new_frame.voltage_v < 0.0) {
                std::cerr << "Error: Negative voltage in frame: " << line << '\n';
                return -1;
            }

            if (new_frame.temperature_c < -40.0 || new_frame.temperature_c > 120.0) {
                std::cerr << "Error: Unrealistic temperature in frame: " << line << '\n';
                return -1;
            }

            if (new_frame.gps_fix != 0 && new_frame.gps_fix != 1) {
                std::cerr << "Error: Invalid GPS fix value in frame: " << line << '\n';
                return -1;
            }

            if (new_frame.satellites < 0) {
                std::cerr << "Error: Invalid satellite count in frame: " << line << '\n';
                return -1;
            }

            frames[frame_count] = new_frame;
            ++frame_count;
        }
    }

    return frame_count;
}

Summary summarize(const Frame frames[], int frame_count) {
    Summary summary{};
    summary.frames_total = frame_count;
    summary.frames_valid = frame_count;
    summary.voltage_min = frames[0].voltage_v;
    summary.voltage_max = frames[0].voltage_v;
    summary.low_voltage_frames = 0;

    double temperature_sum = 0.0;

    for (int i = 0; i < frame_count; ++i) {
        if (frames[i].voltage_v < summary.voltage_min) {
            summary.voltage_min = frames[i].voltage_v;
        }

        if (frames[i].voltage_v > summary.voltage_max) {
            summary.voltage_max = frames[i].voltage_v;
        }

        temperature_sum += frames[i].temperature_c;

        if (frames[i].voltage_v < 22.0) {
            ++summary.low_voltage_frames;
        }
    }

    const int temperature_tenths = static_cast<int>(temperature_sum * 10.0) / frame_count;
    summary.temperature_avg = static_cast<double>(temperature_tenths) / 10.0;
    summary.frame_rate_hz = compute_frame_rate_hz(frames, frame_count);
    return summary;
}

void print_summary(const Summary& summary) {
    std::cout << "frames_total " << summary.frames_total << '\n';
    std::cout << "frames_valid " << summary.frames_valid << '\n';
    std::cout << "voltage_min " << summary.voltage_min << '\n';
    std::cout << "voltage_max " << summary.voltage_max << '\n';
    std::cout << "temperature_avg " << summary.temperature_avg << '\n';
    std::cout << "low_voltage_frames " << summary.low_voltage_frames << '\n';
    std::cout << "frame_rate_hz " << summary.frame_rate_hz << '\n';
}
