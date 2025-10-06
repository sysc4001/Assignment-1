import random
import time
import os

process_types = ['CPU', 'SYSCALL ' + str(random.randint(1, 25)), 'END_IO ' + str(random.randint(1, 25))]

def get_next_trace_filename(base_name="trace", extension=".txt"):
    i = 1
    while True:
        filename = f"{base_name}{i}{extension}"
        if not os.path.exists(filename):
            return filename
        i += 1

def GenerateTraceFile():
    filename = get_next_trace_filename()
    with open(filename, 'w') as file:
        for _ in range(random.randint(15, 30)):
            process_type = random.choice(process_types)
            time_ms = random.randint(70, 700)
            file.write(f"{process_type}, {time_ms}\n")
    print(f"Trace file generated: {filename}")

# Run the function
for _ in range(20):
    GenerateTraceFile()