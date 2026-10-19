import random
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 generate.py <N>")
        return

    n_points = int(sys.argv[1])
    filename = f"{n_points}.txt"

    with open(filename, 'w') as f:
        f.write(f"{n_points}\n")
        for _ in range(n_points):
            m = random.uniform(0.1, 10.0)
            x = random.uniform(-1000.0, 1000.0)
            y = random.uniform(-1000.0, 1000.0)
            vx = random.uniform(-10.0, 10.0)
            vy = random.uniform(-10.0, 10.0)
            f.write(f"{m:.5f} {x:.5f} {y:.5f} {vx:.5f} {vy:.5f}\n")

    print(f"File {filename} generated!")

if __name__ == '__main__':
    main()
