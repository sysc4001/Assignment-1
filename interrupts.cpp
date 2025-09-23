/**
 *
 * @file interrupts.cpp
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
    int currentTime;
    int contextSaveTime = 10; //10ms context save time
    int ISR_Runtime = 40;

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if(activity== "SYSCALL")
        {
            auto [inter_out, new_time] = intr_boilerplate(currentTime,duration_intr,contextSaveTime,vectors);
            execution.append(inter_out);
            currentTime = new_time;
            execution.append(inter_out);

            execution.append(std::to_string(currentTime) + "," + std::to_string(ISR_Runtime ) + ",Executing ISR");
            currentTime+= ISR_Runtime;

            execution.append(std::to_string(currentTime) + "," + std::to_string(delays.at(duration_intr)) + "Calling Device Driver" );
            currentTime += delays.at(duration_intr);

            execution += std::to_string(currentTime) + ", 1, IRET\n";
            currentTime += 1;
        }
        else if(activity == "END_IO"){
            
            //execution = execution + std::to_string(currentTime) + "what goes here?," + "End IO " + std::to_string(duration_intr) + ":Interrupt";
           // execution += smth
        }
        else {
            
            //log the cpu one 
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
