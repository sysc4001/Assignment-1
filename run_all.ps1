# -----------------------------
# run_all.ps1
# -----------------------------
# This script runs all available trace files (trace_1 to trace_5)
# and generates separate execution outputs for each trace.

# Folder where the input files and traces live
$traceDir1 = ".\input_files"
$traceDir2 = ".\testcases"

# Paths to vector and device tables
$vectorTable = "$traceDir1\vector_table.txt"
$deviceTable = "$traceDir1\device_table.txt"

# Make sure the executable exists
$exePath = ".\bin\interrupts.exe"
if (!(Test-Path $exePath)) {
    Write-Host "Error: interrupts.exe not found. Please build first using build.sh or g++"
    exit 1
}

# Loop through all trace files (1–5)
for ($i = 1; $i -le 5; $i++) {
    $traceFile1 = "$traceDir1\trace_$i.txt"
    $traceFile2 = "$traceDir2\trace_$i.txt"
    $outputFile = ".\execution_trace_$i.txt"

    if (Test-Path $traceFile1) {
        Write-Host "Running trace_$i from input_files..."
        & $exePath $traceFile1 $vectorTable $deviceTable 10 40
        Copy-Item -Path ".\execution.txt" -Destination $outputFile -Force
        Write-Host "→ Output saved to $outputFile"
    }
    elseif (Test-Path $traceFile2) {
        Write-Host "Running trace_$i from testcases..."
        & $exePath $traceFile2 $vectorTable $deviceTable 10 40
        Copy-Item -Path ".\execution.txt" -Destination $outputFile -Force
        Write-Host "→ Output saved to $outputFile"
    }
    else {
        Write-Host "Trace file trace_$i.txt not found. Skipping."
    }
}

Write-Host "`n✅ All traces executed successfully!"
