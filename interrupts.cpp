#include "interrupts.hpp"
#include <algorithm>

int main(int argc, char** argv) {
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;
    std::string execution = "";

    int context_save_time = std::stoi(argv[4]); // e.g., 10
    int isr_time = std::stoi(argv[5]);          // e.g., 40
    int current_time = 0;
    bool in_interrupt = false;

    while (std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
        activity.erase(std::remove_if(activity.begin(), activity.end(), ::isspace), activity.end());

        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) +
                         ", CPU burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            // system call (start of I/O)
            auto [intr_exec, t] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += intr_exec;
            current_time = t;

            execution += std::to_string(current_time) + ", " + std::to_string(isr_time) +
                         ", execute ISR for device " + std::to_string(duration_intr) + "\n";
            current_time += isr_time;

            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", return to user mode\n";
            current_time++;

            // simulate I/O delay (non-blocking)
            execution += std::to_string(current_time) + ", " + std::to_string(delays[duration_intr]) +
                         ", device " + std::to_string(duration_intr) + " performing I/O (async)\n";
        }
        else if (activity == "END_IO") {
            // I/O completion interrupt
            auto [intr_exec, t] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += intr_exec;
            current_time = t;

            execution += std::to_string(current_time) + ", " + std::to_string(isr_time) +
                         ", handle END_IO for device " + std::to_string(duration_intr) + "\n";
            current_time += isr_time;

            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", return to user mode\n";
            current_time++;
        }
    }

    input_file.close();
    write_output(execution);
    return 0;
}
