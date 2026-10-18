import subprocess
import sys

def run_benchmark(iterations, threads, tEnd, inputPath):
    executable = "./build/n_body.elf"
    times = []

    print(f"tEnd={tEnd}, inputPath={inputPath}")
    print(f"Running {iterations} iterations for {threads} threads...")

    for i in range(iterations):
        # Run executable
        result = subprocess.run(
            [executable, str(threads), str(tEnd), str(inputPath)],
            capture_output=True, text=True, check=True
        )

        # Parse time
        output = result.stdout
        time_str = None
        for line in output.split('\n'):
            if "Time:" in line:
                time_str = line.split(":")[1].split()[0]
                break

        if time_str is None:
            print(f"Error: Couldn't parse time from output:\n{output}")
            sys.exit(1)

        exec_time = float(time_str)
        times.append(exec_time)
        print(f"  Iteration {i+1}: {exec_time:.6f} sec")

    min_time = min(times)
    print(f"\n[RESULT] Minimum time: {min_time:.6f} seconds")
    return min_time

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python3 benchmark.py <nIterations> <nThreads> <t_end> <input_path>")
        sys.exit(1)

    iters     = int(sys.argv[1])
    nThreads  = int(sys.argv[2])
    tEnd      = float(sys.argv[3])
    inputPath = str(sys.argv[4])

    run_benchmark(iters, nThreads, tEnd, inputPath)
