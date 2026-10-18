import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def main():
    filename = 'nbody_output.csv'
    if len(sys.argv) > 1:
        filename = sys.argv[1]

    print(f"Reading data from {filename}...")

    # Загружаем данные из CSV
    try:
        data = np.loadtxt(filename, delimiter=',')
    except Exception as e:
        print(f"Error reading {filename}: {e}")
        return

    if data.size == 0:
        print("Data is empty!")
        return

    # Разбираем столбцы:
    # 0 столбец - время (t)
    # 1, 3, 5... столбцы - координаты X
    # 2, 4, 6... столбцы - координаты Y
    times = data[:, 0]
    coords = data[:, 1:]

    n_points = coords.shape[1] // 2
    X = coords[:, 0::2]  # Берем каждый второй столбец начиная с 0-го
    Y = coords[:, 1::2]  # Берем каждый второй столбец начиная с 1-го

    print(f"Loaded {len(times)} frames for {n_points} bodies.")

    # Настраиваем график
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_aspect('equal')
    ax.grid(True, linestyle='--', alpha=0.6)

    # Определяем границы графика (с небольшим запасом)
    x_min, x_max = np.min(X), np.max(X)
    y_min, y_max = np.min(Y), np.max(Y)

    # Добавляем 10% отступы
    dx = (x_max - x_min) * 0.1 if x_max != x_min else 10
    dy = (y_max - y_min) * 0.1 if y_max != y_min else 10

    ax.set_xlim(x_min - dx, x_max + dx)
    ax.set_ylim(y_min - dy, y_max + dy)

    # Создаем объекты точек (разные цвета для каждой планеты)
    colors = plt.cm.jet(np.linspace(0, 1, n_points))
    scatter = ax.scatter(X[0], Y[0], s=50, c=colors, zorder=5)

    # Добавляем "хвосты" (траектории) для красоты
    lines = [ax.plot([], [], '-', color=colors[i], alpha=0.4)[0] for i in range(n_points)]

    time_text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=12)

    # Поскольку кадров может быть 10 000, отрисовывать каждый слишком долго.
    # Будем рисовать каждый N-й кадр, чтобы анимация была плавной и быстрой.
    frames_to_skip = max(1, len(times) // 500) # Отрисуем около 500 кадров
    tail_length = 50 # Длина хвоста траектории в кадрах

    def update(frame_idx):
        # Реальный индекс строки в массиве
        i = frame_idx * frames_to_skip
        if i >= len(times):
            i = len(times) - 1

        # Обновляем координаты точек
        scatter.set_offsets(np.c_[X[i], Y[i]])

        # Обновляем хвосты траекторий
        start_idx = max(0, i - tail_length * frames_to_skip)
        for p in range(n_points):
            lines[p].set_data(X[start_idx:i, p], Y[start_idx:i, p])

        time_text.set_text(f'Time: {times[i]:.2f}')
        return [scatter, time_text] + lines

    # Создаем анимацию
    num_frames = len(times) // frames_to_skip
    ani = animation.FuncAnimation(fig, update, frames=num_frames, interval=20, blit=True)

    plt.title("N-Body Simulation")
    plt.show()

if __name__ == '__main__':
    main()
