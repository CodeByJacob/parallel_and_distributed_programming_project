import os
import sys
from datetime import datetime
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np


def create_results_folder_with_readable_timestamp():
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    results_folder = f'results/{timestamp}'
    if not os.path.exists(results_folder):
        os.makedirs(results_folder)
    return results_folder


def save_logs_to_file(logs, results_folder):
    with open(f'{results_folder}/logs.txt', 'w') as log_file:
        for log in logs:
            log_file.write(log + '\n')


def extract_data_from_logs(logs):
    data = defaultdict(lambda: defaultdict(list))
    for log in logs:
        parts = log.split("|")
        test_category, test_name, size, clock_time = [part.split(":")[1] for part in parts]
        # Convert seconds to milliseconds
        data[test_name][(test_category, int(size))].append(float(clock_time) * 1000)
    return data


def make_plot(data, results_folder):
    for test_name, categories in data.items():
        fig, ax = plt.subplots(figsize=(10, 5))
        plt.title(f'Test: {test_name}')
        ax.set_xlabel("Category - Size", fontsize=12)
        ax.set_ylabel("Average Time (milliseconds)", fontsize=12)

        unique_sizes = set(size for _, size in categories.keys())
        unique_categories = sorted(set(category for category, _ in categories.keys()))
        bar_width = 0.15
        x_base = range(len(unique_categories))

        colors = plt.cm.viridis(np.linspace(0, 1, len(unique_sizes)))

        for i, size in enumerate(sorted(unique_sizes)):
            times = [sum(categories[(category, size)]) / len(categories[(category, size)]) if (category,
                                                                                               size) in categories else 0
                     for category in unique_categories]
            rects = ax.bar([x + i * bar_width for x in x_base], times, width=bar_width, label=f'Size {size}',
                           color=colors[i])

            for rect in rects:
                height = rect.get_height()
                ax.annotate(f'{height:.2f}',
                            xy=(rect.get_x() + rect.get_width() / 2, height),
                            xytext=(0, 3),  # 3 points vertical offset
                            textcoords="offset points",
                            ha='center', va='bottom', fontsize=8)

        ax.set_xticks([x + bar_width * (len(unique_sizes) - 1) / 2 for x in x_base])
        ax.set_xticklabels(unique_categories, rotation=45, ha="right", fontsize=10)

        ax.legend(title="Size", fontsize='small', loc='upper left')

        plt.tight_layout()

        file_name = f"{results_folder}/{test_name.replace(' ', '_')}_plot.png"
        plt.savefig(file_name, dpi=300)
        plt.close()


def main():
    results_folder = create_results_folder_with_readable_timestamp()

    logs = [line.strip() for line in sys.stdin if line.startswith("TestCategory:")]
    save_logs_to_file(logs, results_folder)

    data = extract_data_from_logs(logs)
    make_plot(data, results_folder)


if __name__ == "__main__":
    main()
