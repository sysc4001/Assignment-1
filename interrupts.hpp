#ifndef INTERRUPTS_HPP_
#define INTERRUPTS_HPP_

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<random>
#include<utility>
#include<sstream>
#include<iomanip>
#include <algorithm>

#include<stdio.h>

#define ADDR_BASE   0
#define VECTOR_SIZE 2

// ---------------- Helper functions ----------------

// Parse CLI args: now expects 5 arguments
//   ./interrupts <trace_file> <vector_table> <device_table> <context_time> <isr_step>
std::tuple<std::vector<std::string>, std::vector<int>, int, int, std::string> parse_args(int argc, char** argv) {
    if(argc != 6) {
        std::cout << "ERROR!\nExpected 5 arguments, received " << argc - 1 << std::endl;
        std::cout << "Usage: ./interrupts <trace_file> <vector_table> <device_table> <context_time> <isr_step>\n";
        exit(1);
    }

    std::string trace_file = argv[1];

    std::ifstream input_vector_table(argv[2]);
    if (!input_vector_table.is_open()) {
        std::cerr << "Error: Unable to open vector table: " << argv[2] << std::endl;
        exit(1);
    }
    std::string vector;
    std::vector<std::string> vectors;
    while(std::getline(input_vector_table, vector)) {
        vectors.push_back(vector);
    }
    input_vector_table.close();

    std::ifstream device_table(argv[3]);
    if (!device_table.is_open()) {
        std::cerr << "Error: Unable to open device table: " << argv[3] << std::endl;
        exit(1);
    }
    std::string duration;
    std::vector<int> delays;
    while(std::getline(device_table, duration)) {
        if (!duration.empty())
            delays.push_back(std::stoi(duration));
    }

    int context_time = std::stoi(argv[4]);
    int isr_step = std::stoi(argv[5]);

    return {vectors, delays, context_time, isr_step, trace_file};
}

// String split by delimiter
std::vector<std::string> split_delim(std::string input, std::string delim) {
    std::vector<std::string> tokens;
    std::size_t pos = 0;
    std::string token;
    while ((pos = input.find(delim)) != std::string::npos) {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delim.length());
    }
    tokens.push_back(input);

    return tokens;
}

// Parse one line from trace file
std::tuple<std::string, int> parse_trace(std::string trace) {
    auto parts = split_delim(trace, ",");
    if (parts.size() < 2) {
        std::cerr << "Error: Malformed input line: " << trace << std::endl;
        return {"null", -1};
    }

    auto activity = parts[0];
    // remove spaces
    activity.erase(remove_if(activity.begin(), activity.end(), ::isspace), activity.end());
    auto duration_intr = std::stoi(parts[1]);

    return {activity, duration_intr};
}

// Boilerplate interrupt sequence
std::pair<std::string, int> intr_boilerplate(int current_time, int intr_num, int context_save_time, std::vector<std::string> vectors) {
    std::string execution = "";

    execution += std::to_string(current_time) + ", 1, switch to kernel mode\n";
    current_time++;

    execution += std::to_string(current_time) + ", " + std::to_string(context_save_time) + ", context saved\n";
    current_time += context_save_time;

    char vector_address_c[10];
    snprintf(vector_address_c, sizeof(vector_address_c), "0x%04X", (ADDR_BASE + (intr_num * VECTOR_SIZE))); //terminal gave me a werid error, I added the "sizeof" function
    std::string vector_address(vector_address_c);

    execution += std::to_string(current_time) + ", 1, find vector " + std::to_string(intr_num)
                 + " in memory position " + vector_address + "\n";
    current_time++;

    execution += std::to_string(current_time) + ", 1, load address " + vectors.at(intr_num) + " into the PC\n";
    current_time++;

    return std::make_pair(execution, current_time);
}

// Write output file
void write_output(std::string execution) {
    std::ofstream output_file("execution.txt");
    if (output_file.is_open()) {
        output_file << execution;
        output_file.close();
        std::cout << "Output generated in execution.txt" << std::endl;
    } else {
        std::cerr << "Error opening file!" << std::endl;
    }
}

#endif
