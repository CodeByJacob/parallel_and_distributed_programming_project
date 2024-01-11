import dataclasses
import os
import sys
from dataclasses import dataclass
from datetime import datetime
from collections import defaultdict
from typing import List

import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator


@dataclass
class TestResult:
    test_category: str
    test_name: str
    number_of_processes: int
    speedup: float
    efficiency: float


@dataclass
class TestData:
    category: str
    number_of_processes: int
    size: int
    times: List[float] = dataclasses.field(default_factory=list)

    def avg_times(self):
        return sum(self.times) / len(self.times)


@dataclass
class Data:
    test_name: str
    test_data: List[TestData] = dataclasses.field(default_factory=list)


def create_results_folder_with_readable_timestamp():
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    results_folder = f'results/{timestamp}-speedup-and-efficiency'
    if not os.path.exists(results_folder):
        os.makedirs(results_folder)
    return results_folder


def save_logs_to_file(logs, results_folder):
    with open(f'{results_folder}/logs.txt', 'w') as log_file:
        for log in logs:
            log_file.write(log + '\n')


def extract_number_of_processes(test_name: str) -> int:
    if 'Sequential_Test' in test_name:
        return 1
    elif 'OpenMP_Test' in test_name or 'MPI_Test' in test_name:
        # Extract the number after the test name
        parts = test_name.split('-')
        return int(parts[1]) if len(parts) > 1 else 1
    elif 'Hybrid_Test' in test_name:
        # Multiply the two numbers after the test name
        parts = test_name.split('-')
        return int(parts[1]) * int(parts[2]) if len(parts) > 2 else 1
    else:
        return 1  # Default case


def clean_test_category_name(test_name: str) -> str:
    if 'Sequential_Test' in test_name:
        return 'Sequential'
    elif 'OpenMP_Test' in test_name:
        return 'OpenMP'
    elif 'MPI_Test' in test_name:
        return 'MPI'
    elif 'Hybrid_Test' in test_name:
        return 'Hybrid'
    else:
        return 'Unknown'


def extract_data_from_logs(logs) -> List[Data]:
    data = []
    for log in logs:
        parts = log.split("|")
        test_category, test_name, size, clock_time = [part.split(":")[1].strip() for part in parts]

        clock_time_ms = float(clock_time) * 1000

        number_of_processes = extract_number_of_processes(test_category)
        test_category = clean_test_category_name(test_category)

        data_instance = next((d for d in data if d.test_name == test_name), None)
        if data_instance is None:
            data_instance = Data(test_name=test_name)
            data.append(data_instance)

        test_data_instance = next(
            (td for td in data_instance.test_data if
             td.category == test_category and td.number_of_processes == number_of_processes and td.size == int(size)),
            None)
        if test_data_instance is None:
            test_data_instance = TestData(category=test_category, size=int(size),
                                          number_of_processes=number_of_processes)
            data_instance.test_data.append(test_data_instance)

        test_data_instance.times.append(clock_time_ms)

    return data


def filter_data(data_list: List[Data], target_size: int = 1391) -> List[Data]:
    filtered_data = []
    for data in data_list:
        if data.test_name in ["Encrypt", "Decrypt"]:
            filtered_test_data = [test for test in data.test_data if test.size == target_size]
            if filtered_test_data:
                filtered_data.append(Data(test_name=data.test_name, test_data=filtered_test_data))
    return filtered_data


def generate_test_results(data_list: List[Data]) -> List[TestResult]:
    test_results = []
    for data in data_list:
        sequential_test_data = next((td for td in data.test_data if td.category == "Sequential"), None)
        if sequential_test_data:
            sequential_avg_time = sequential_test_data.avg_times()

            for test_data in data.test_data:
                if test_data.category != "Sequential":
                    speedup = sequential_avg_time / test_data.avg_times() if test_data.avg_times() else 0
                    efficiency = (speedup / test_data.number_of_processes) * 100 if test_data.number_of_processes else 0
                    test_results.append(TestResult(test_category=test_data.category, test_name=data.test_name,
                                                   number_of_processes=test_data.number_of_processes,
                                                   speedup=speedup, efficiency=efficiency))
    return test_results


def plot_speedup_by_category(results: List[TestResult], test_name: str, results_folder: str):
    category_data = defaultdict(list)
    for result in results:
        if result.test_name == test_name:
            category_data[result.test_category].append((result.number_of_processes, result.speedup))

    plt.figure(figsize=(10, 6))

    for category, values in category_data.items():
        values.sort(key=lambda x: x[0])  # Sorting by number of processes
        processes, speedups = zip(*values)
        plt.plot(processes, speedups, marker='o', label=category)

    plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True))

    plt.title(f"Speed-up Plot for {test_name}")
    plt.xlabel("Number of Processes")
    plt.ylabel("Speedup")
    plt.legend()

    plt.grid(True)

    os.makedirs(results_folder, exist_ok=True)
    plot_filename = f"Speed_up_plot_{test_name}.png"
    plot_path = os.path.join(results_folder, plot_filename)
    plt.savefig(plot_path)
    plt.close()  # Close the plot to free memory


def plot_efficiency_by_category(results: List[TestResult], test_name: str, results_folder: str):
    # Organizing data by test_category
    category_data = defaultdict(list)
    for result in results:
        if result.test_name == test_name:
            category_data[result.test_category].append((result.number_of_processes, result.efficiency))

    plt.figure(figsize=(10, 6))

    # Plotting each category
    for category, values in category_data.items():
        values.sort(key=lambda x: x[0])  # Sorting by number of processes
        processes, efficiencies = zip(*values)
        plt.plot(processes, efficiencies, marker='o', label=category)

    plt.gca().xaxis.set_major_locator(MaxNLocator(integer=True))

    plt.title(f"Efficiency Plot for {test_name}")
    plt.xlabel("Number of Processes")
    plt.ylabel("Efficiency (%)")
    plt.legend()

    plt.grid(True)

    os.makedirs(results_folder, exist_ok=True)
    plot_filename = f"Efficiency_plot_{test_name}.png"
    plot_path = os.path.join(results_folder, plot_filename)
    plt.savefig(plot_path)
    plt.close()


def main():
    results_folder = create_results_folder_with_readable_timestamp()

    logs = [line.strip() for line in sys.stdin if line.startswith("TestCategory:")]
    save_logs_to_file(logs, results_folder)

    data = extract_data_from_logs(logs)
    data = filter_data(data)
    test_results = generate_test_results(data)

    print(test_results)

    for test_name in ["Encrypt", "Decrypt"]:
        plot_speedup_by_category(test_results, test_name, results_folder)
        plot_efficiency_by_category(test_results, test_name, results_folder)


if __name__ == "__main__":
    main()
