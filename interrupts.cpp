/**
 * @file interrupts.cpp
 * @author Aryan Singh
 *
 * A simple simulation of a CPU trace with interrupt handling.  This program
 * parses a trace file describing CPU bursts, system calls and end‑of‑IO
 * events and produces a textual log of the simulated execution.  Each
 * `SYSCALL` triggers a sequence of steps modelled after a typical interrupt
 * service routine (switch to kernel, save context, locate the vector, load
 * the ISR address, execute the ISR, return from interrupt and start the
 * device I/O).  The durations for each device and the ISR vector
 * addresses are provided via the `parse_args` helper declared in
 * interrupts.hpp.
 */

#include "interrupts.hpp"

#include <algorithm> // for std::max
#include <utility>   // for std::pair
#include <vector>    // for std::vector
#include <iostream>  // for std::cerr
#include <sstream>   // for std::ostringstream
#include <string>    // for std::string
#include <cstdint>   // for std::uint64_t
#include <cstdio>    // for std::snprintf
#include <fstream>   // for std::ifstream


// If the header doesn't define ADDR_BASE or VECTOR_SIZE, set default values here.
// They determine where the interrupt vector resides in memory.
// In real applications, these constants would typically be defined in another location.


#ifndef ADDR_BASE
#define ADDR_BASE 0x0000
#endif
#ifndef VECTOR_SIZE
#define VECTOR_SIZE 1
#endif

int main(int argc, char **argv) {
// Use the helper to parse command‑line arguments and extract the ISR
// vectors and device delays.  The concrete definition of parse_args
// resides in interrupts.hpp or another translation unit.  It is
// expected to return a pair of vectors: the first with ISR
// addresses and the second with the corresponding device latencies.
auto [vectors, delays] = parse_args(argc, argv);

// Expect at least one additional argument specifying the trace file to
// read.  If none is given, report usage and terminate.
if (argc < 2) {
std::cerr << "Usage: " << argv[0] << " <trace_file>\n";
return 1;
}

// Open the input trace file.  Abort if the file cannot be opened.
std::ifstream input_file(argv[1]);
if (!input_file.is_open()) {
std::cerr << "Could not open file: " << argv[1] << "\n";
return 1;
}

std::string trace;      // stores each line read from the trace file
std::string execution;  // accumulates the output log
std::uint64_t current_time = 0; // simulated time in milliseconds

// Process each line of the trace.  Blank lines are ignored.  The
// parse_trace helper returns a pair consisting of the activity name
// (e.g., "CPU", "SYSCALL" or "ENDIO") and an integer argument.  For
// a CPU burst this argument is its duration; for a SYSCALL or ENDIO
// it is the device number.
while (std::getline(input_file, trace)) {
if (trace.empty()) {
continue;
}
auto [activity, duration_intr] = parse_trace(trace);

if (activity == "CPU") {
int burst = duration_intr;
if (burst < 0) {
burst = 0;
}
// Log the CPU burst and advance time accordingly.
execution += std::to_string(current_time) + ", " +
        std::to_string(burst) +
        ", CPU burst of " + std::to_string(burst) + " ms\n";
current_time += static_cast<std::uint64_t>(burst);
} else if (activity == "SYSCALL") {
int devnum = duration_intr;
if (devnum < 0) {
// Invalid device numbers are ignored.
continue;
}
// Step 1: switch to kernel mode (1 ms)
execution += std::to_string(current_time) + ", 1, switch to kernel mode\n";
current_time += 1;
// Step 2: save context (10 ms)
execution += std::to_string(current_time) + ", 10, context saved\n";
current_time += 10;
// Step 3: find vector in memory (1 ms)
char vectoraddr[10];
std::snprintf(vectoraddr, sizeof(vectoraddr), "0x%04X",
        ADDR_BASE + devnum * VECTOR_SIZE);
execution += std::to_string(current_time) + ", 1, find vector " +
        std::to_string(devnum) + " in memory position " + vectoraddr +
        "\n";
current_time += 1;
// Step 4: load ISR address into the PC (1 ms).  Guard against
// references beyond the provided vector array.
std::string isr = (static_cast<std::size_t>(devnum) < vectors.size())
? vectors.at(devnum)
                : std::string("<unknown>");
execution += std::to_string(current_time) + ", 1, load address " + isr +
        " into the PC\n";
current_time += 1;
// Step 5: execute the ISR (40 ms)
execution += std::to_string(current_time) + ", 40, execute ISR for device " +
        std::to_string(devnum) + "\n";
current_time += 40;
// Step 6: return from interrupt (1 ms)
execution += std::to_string(current_time) + ", 1, IRET - end interrupt service\n";
current_time += 1;
// Step 7: start device I/O.  The device latency is looked up from
// the delays vector; if no entry exists, a default of zero is used.
int io_time = 0;
if (static_cast<std::size_t>(devnum) < delays.size()) {
io_time = delays.at(devnum);
if (io_time < 0) {
io_time = 0;
}
}
execution += std::to_string(current_time) + ", " + std::to_string(io_time) +
        ", start IO for device " + std::to_string(devnum) + "\n";
current_time += static_cast<std::uint64_t>(io_time);
} else if (activity == "ENDIO") {
int devnum = duration_intr;
if (devnum < 0) {
continue;
}
// ENDIO signals the completion of a device transfer.  We
// log the event and advance the simulated time by the device
// latency (again guarded by the size of the delays vector).
int io_time = 0;
if (static_cast<std::size_t>(devnum) < delays.size()) {
io_time = delays.at(devnum);
if (io_time < 0) {
io_time = 0;
}
}
execution += std::to_string(current_time) + ", " + std::to_string(io_time) +
        ", ENDIO device " + std::to_string(devnum) + "\n";
current_time += static_cast<std::uint64_t>(io_time);
} else {
// Unrecognised activity.  You may choose to log or ignore such
// entries.  Here we silently ignore them.
continue;
}
}

// Clean up the input file and write out the accumulated execution log.
input_file.close();
write_output(execution);

return 0;
}
