import subprocess
import sys

def run_benchmark(threads, points, iterations):
    executable = "./build/mandelbrot.elf"
    times = []

    print(f"Running {iterations} iterations for {threads} threads and {points} points...")

    for i in range(iterations):
        # Run executable
        result = subprocess.run(
            [executable, str(threads), str(points)],
            capture_output=True, text=True, check=True
        )

        # Parse time
        output = result.stdout
        time_str = None
        for line in output.split('\n'):
            if "Time:" in line:
                time_str = line.split(":")[1].strip()
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
    if len(sys.argv) != 4:
        print("Usage: python3 benchmark.py <nThreads> <nPoints> <nIterations>")
        sys.exit(1)

    t = int(sys.argv[1])
    p = int(sys.argv[2])
    iters = int(sys.argv[3])

    run_benchmark(t, p, iters)
