/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp" // terminal giving me a weird error, so I flipped <> to ""

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
    const int context_save_time = 10; // time taken to save context
    const int isr_activity_time = 40; // time taken for each chunk of ISR activity
    //int IO_time = 0.1; // time taken for IO operation

 
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
    

        /******************ADD YOUR SIMULATION CODE HERE*************************/
     if (activity == "CPU") {
         execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
         current_time += duration_intr;
        } 
        else if(activity == "SYSCALL") {
            auto [prev_execution, new_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += prev_execution;
            current_time = new_time;
            
        }
        else if( activity == "END_IO") {
            // IO operation handling code here
            auto [prev_execution, new_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            execution += prev_execution;
            current_time = new_time;
           
            //ISR body execution, assuming fixed time for IO operation
            int device_delay = delays.at(duration_intr-1); // get the delay for the device
            while (device_delay> 0){
                int chunk = std::min(isr_activity_time, device_delay);
                execution += std::to_string(current_time) + ", " + std::to_string(chunk) + ", ISR END_IO 'execution' \n";
                current_time += chunk;
                device_delay -= chunk;
                
            }

            execution += std::to_string(current_time) + ", " + std::to_string(1) + ", 1, IRET \n";
            current_time += 1;
        }

        else {
            std::cerr << "Error: Unknown activity type: " << activity << std::endl;
        }
        /************************************************************************/
    }

    input_file.close();

    write_output(execution);

    return 0;

}
