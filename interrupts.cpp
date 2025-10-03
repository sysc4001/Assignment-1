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
    int negligible_time = 1; //1ms
    int context_save_time = 10; 
    int ISR_runtime = 40;
    int current_time = 0;
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU") 
        {
            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            current_time += duration_intr;
        }
        else if(activity == "SYSCALL")
        {
            //to kernal mode, save context, find vector, load ISR into PC
            auto [inter_out, new_time] = intr_boilerplate(current_time,duration_intr,context_save_time,vectors);
            execution += inter_out;
            current_time = new_time;

            //execution of ISR
            execution += (std::to_string(current_time) + "," + std::to_string(ISR_runtime ) + ",Executing ISR for device " +std::to_string(duration_intr) + "\n");
            current_time += ISR_runtime;

            //device operation. find device operation time from device table
            int opr_time = 0; 
            if(duration_intr >(int)sizeof(delays))
            {
                std::cout<<"ERROR: DEVICE " + std::to_string(duration_intr) +  " NOT FOUND"<<std::endl;
            }
            else
            {
                opr_time = delays[duration_intr];
            }

            execution += std::to_string(current_time) + ", " + std::to_string(opr_time) + ", device " + std::to_string(duration_intr) + " I/O operation\n";
            current_time += opr_time;

            //IRET
            execution += std::to_string(current_time) + ", "+  std::to_string(negligible_time) + ",IRET\n";
            current_time += negligible_time;

        }
        else if(activity == "END_IO"){
        
		// checks if device number out of bounds
		if (duration_intr < 0 || duration_intr >= (int)vectors.size()) {
			execution += std::to_string(current_time) + ", 0, END_IO received for device " + std::to_string(duration_intr) + "\n";
			continue;    
		//to kernel mode, save context, find vector, load ISR into PC
		auto [inter_out, new_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
		execution += inter_out;
		current_time = new_time;
		
		//execution of ISR
		execution += std::to_string(current_time) + ", " + std::to_string(ISR_runtime) + ", Executing ISR for END_IO device " + std::to_string(duration_intr) + "\n"
		current_time += ISR_runtime;
		
		//IRET
		execution += std::to_string(current_time) + ", " + std::to_string(negligible_time) + ", IRET\n";
		current_time += negligible_time;
 
        }
        else {
            execution += std::to_string(current_time) + ", 0, UNKOWN ACTIVITY";
        }
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
