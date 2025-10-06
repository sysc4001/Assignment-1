/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp"

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    int context_save = 10;
    int switch_time = 1; // Switch from user to kernal mode
    int find_vector_time = 1; // Find ISR address or Vector address
    int isr_body_time = 40; // execute ISR

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        int device_delay;
        if (duration_intr < (int)delays.size()) {
            device_delay = delays[duration_intr];
        } else {
            device_delay = isr_body_time;
        }

        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU bursts\n";
            current_time += duration_intr;
        } else if (activity == "SYSCALL") {
            auto [interrupt_log, updated_time] = intr_boilerplate(current_time, duration_intr, context_save, vectors);
            execution += interrupt_log;
            current_time = updated_time;

            execution += std::to_string(current_time) + ", " + std::to_string(device_delay) + ", SYSCALL: run the ISR (device driver)\n";
            current_time += device_delay;

            execution += std::to_string(current_time) + ", " + std::to_string(isr_body_time) + ", transfer data from device to memory\n";
            current_time += 40;

            execution += std::to_string(current_time) + ", 376, check for errors\n";
            current_time += 376;

            execution += std::to_string(current_time) + ", " + std::to_string(switch_time) + ", IRET\n";
            current_time += switch_time;

            execution += std::to_string(current_time) + ", " + std::to_string(switch_time) + ", switch to user mode\n";
            current_time += switch_time;
        } else if (activity == "END_IO") {
            auto [interrupt_log, updated_time] = intr_boilerplate(current_time, duration_intr, context_save, vectors);
            execution += interrupt_log;
            current_time = updated_time;


            execution += std::to_string(current_time) + ", " + std::to_string(device_delay) + ", END_IO: run the ISR (device driver)\n";
            current_time += device_delay;


            execution += std::to_string(current_time) + ", 416, check device status\n";
            current_time += 416;


            execution += std::to_string(current_time) + ", " + std::to_string(switch_time) + ", IRET\n";
            current_time += switch_time;


            execution += std::to_string(current_time) + ", " + std::to_string(switch_time) + ", switch to user mode\n";
            current_time += switch_time;
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
