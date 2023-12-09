import os
import sys
from datetime import datetime
from collections import defaultdict
import matplotlib.pyplot as plt


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
        data[test_name][(test_category, int(size))].append(float(clock_time))
    return data


def make_plot(data, results_folder):
    for test_name, categories in data.items():
        plt.figure(figsize=(10, 5))
        plt.title(f'Test: {test_name}')
        plt.xlabel("Category - Size")
        plt.ylabel("Average Time (seconds)")

        bar_width = 0.2
        categories_sorted = sorted(categories.keys())
        x_ticks = range(len(categories_sorted))

        times = [sum(categories[cat])/len(categories[cat]) for cat in categories_sorted]  # Średni czas
        labels = [f'{cat[0]}-{cat[1]}' for cat in categories_sorted]

        plt.bar(x_ticks, times, width=bar_width, alpha=0.7)

        plt.xticks(x_ticks, labels, rotation=45)
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
