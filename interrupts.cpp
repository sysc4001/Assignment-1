/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Fareen. Lavji
 * @author Dearell Tobenna Ezeoke
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
    std::string activity;
    int eventDuration = 0;
    int deviceNumber = 0;
    int taskEstimate = 0;
    std::string isrAddress;
    int modeBit = 0;     
    std::string deviceName;

    // Fixed constants (all measured in ms)
    const int switchModeDuration = 1;
    const int saveContextDuration = 10; 
    const int isrAddressSearchDuration  = 1;
    const int isrAddressExtractDuration = 1;
    const int iretExecuteDuration       = 1;

    // Simulation clocking and events
    long long timeOfEvent = 0;
    std::string eventType;
    int traceFileLineNumber = 0; 

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        ++traceFileLineNumber;

        // Keep the parsed label as our event type for logging
        eventType = activity;

        if (activity == "CPU") {
            eventDuration = duration_intr;

            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", CPU burst\n";

            timeOfEvent += eventDuration;        
            modeBit = 0;                        
            continue;
        }

        // For interrupts: SYSCALL or END_IO
        if (activity == "SYSCALL" || activity == "END_IO") {
            deviceNumber = duration_intr;

            isrAddress = vectors.at(deviceNumber);
            int totalISRTime = delays.at(deviceNumber);

            int overheadBefore =
                switchModeDuration + saveContextDuration
              + isrAddressSearchDuration + isrAddressExtractDuration;

            int overheadAfter = iretExecuteDuration;

            // Compute remaining ISR
            taskEstimate = totalISRTime - (overheadBefore + overheadAfter);
            if (taskEstimate < 0) taskEstimate = 0;

            // switch to kernel mode
            modeBit = 1;
            eventDuration = switchModeDuration;
            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", switch to kernel mode\n";
            timeOfEvent += eventDuration;

            // save context
            eventDuration = saveContextDuration;
            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", context saved\n";
            timeOfEvent += eventDuration;

            // search vector table entry
            eventDuration = isrAddressSearchDuration;
            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", find vector for device "
                      +  std::to_string(deviceNumber) + "\n";
            timeOfEvent += eventDuration;

            // ---- extract ISR address
            eventDuration = isrAddressExtractDuration;
            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", obtain ISR address "
                      +  isrAddress + "\n";
            timeOfEvent += eventDuration;

            // ---- execute ISR body (use the whole remaining budget in one activity)
            if (taskEstimate > 0) {
                eventDuration = taskEstimate;
                execution += std::to_string(timeOfEvent) + ", "
                          +  std::to_string(eventDuration) + ", execute ISR activity (device "
                          +  std::to_string(deviceNumber) + ")\n";
                timeOfEvent += eventDuration;
            }

            // ---- IRET
            eventDuration = iretExecuteDuration;
            execution += std::to_string(timeOfEvent) + ", "
                      +  std::to_string(eventDuration) + ", IRET\n";
            timeOfEvent += eventDuration;

            // back to user mode
            modeBit = 0;
            continue;
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
