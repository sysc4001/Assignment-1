/**
 *
 * @file interrupts.cpp
 * @author Jacob Gaumond
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

    int* comp_uptime_ms_ptr;
    *comp_uptime_ms_ptr = 0; // Clock starts counting from 0 on startup

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/



        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}

void increment_uptime(int* comp_uptime_ms_ptr, int increment_val) {
    *comp_uptime_ms_ptr += increment_val;
}

void sim_task_over_time(std::string* execution_output, int* comp_uptime_ms_ptr, int task_time_ms, std::string task_name) {
    // To simulate performing a task (ex: CPU Burst) over a period of time (ms), the program's
    // output (execution_output) is given a new entry with the value in the comp_uptime_ms_ptr
    // pointer (computer uptime), task_time_ms (how long the task takes), and task_name (the name
    // of the task). After execution, the value in comp_uptime_ms_ptr is updated.
    std::string const OUTPUT_DELIMITER = ", ";

    std::string output_entry = std::to_string(*comp_uptime_ms_ptr) + OUTPUT_DELIMITER + std::to_string(task_time_ms) + OUTPUT_DELIMITER + task_name + "\n";
    *execution_output += output_entry;

    increment_uptime(comp_uptime_ms_ptr, task_time_ms);
}
