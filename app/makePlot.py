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
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))
    for log in logs:
        parts = log.split("|")
        test_category = parts[0].split(":")[1]
        test_name = parts[1].split(":")[1]
        for part in parts[2:]:
            key, value = part.split(":")
            data[test_category][test_name][key] = float(value)
    return data


def make_plot(data, results_folder):
    colors = {'TimeStandard': 'blue', 'TimeCPU': 'red', 'TimeClock': 'green'}
    for test_category in data:
        plt.figure(figsize=(10, 5))
        plt.title(test_category)
        plt.xlabel("Test Name")
        plt.ylabel("Time (seconds)")

        bar_width = 0.2
        index = 0
        x_labels = list(data[test_category].keys())
        x_ticks = range(len(x_labels))

        for time_type in ["TimeStandard", "TimeCPU", "TimeClock"]:
            times = [data[test_category][test_name][time_type] for test_name in x_labels]
            plt.bar([tick + index * bar_width for tick in x_ticks], times, width=bar_width, alpha=0.7, label=time_type,
                    color=colors[time_type])
            index += 1

        plt.xticks([tick + bar_width for tick in x_ticks], x_labels)
        plt.legend()

        file_name = f"{results_folder}/{test_category.replace(' ', '_')}_plot.png"
        plt.savefig(file_name)
        plt.close()


def main():
    results_folder = create_results_folder_with_readable_timestamp()

    logs = [line.strip() for line in sys.stdin]
    save_logs_to_file(logs, results_folder)

    data = extract_data_from_logs(logs)
    make_plot(data, results_folder)


if __name__ == "__main__":
    main()
