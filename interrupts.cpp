/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Fareen. Lavji
 *
 * Assignment assumptions:
 * --> ISR of both END/IO and SYSCALL for the same device takes the same amount of time
 * --> I/O devices are always available, i.e., no delay on I/O request, immediate start
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

    /*
    * @todo: milestone1.issue3 --> add variables
    * activity, eventDuration, deviceNumber, taskEstimate, isrAddress,
    * modeBit, deviceName, switchModeDuration = 1ms, saveContextDuration = 10ms,
    * isrAddressSearchDuration = 1ms, isrAddressExtractDuration = 1ms,
    * iretExecuteDuration, timeOfEvent, eventType, traceFileLineNumber
    */

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
