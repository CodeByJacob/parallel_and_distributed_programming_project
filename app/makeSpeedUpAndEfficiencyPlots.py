import dataclasses
import os
import sys
from dataclasses import dataclass
from datetime import datetime
from collections import defaultdict
from typing import List

import matplotlib.pyplot as plt
import numpy as np


@dataclass
class TestResult:
    test_category: str
    test_name: str
    number_of_processes: int
    speedup: float
    efficiency: float


@dataclass
class TestData:
    test_name: str
    number_of_processes: int
    size: int
    times: List[float] = dataclasses.field(default_factory=list)


@dataclass
class Data:
    test_category: str
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


def clean_test_name(test_name: str) -> str:
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

        test_name = clean_test_name(test_name)
        number_of_processes = extract_number_of_processes(test_name)

        data_instance = next((d for d in data if d.test_category == test_category), None)
        if data_instance is None:
            data_instance = Data(test_category=test_category)
            data.append(data_instance)

        test_data_instance = next(
            (td for td in data_instance.test_data if
             td.test_name == test_name and td.number_of_processes == number_of_processes and td.size == int(size)),
            None)
        if test_data_instance is None:
            test_data_instance = TestData(test_name=test_name, size=int(size), number_of_processes=number_of_processes)
            data_instance.test_data.append(test_data_instance)

        test_data_instance.times.append(clock_time_ms)

    return data


def filter_data(data: List[Data]):
    data = filter(lambda d: d.test_category in ["Decrypt", "Encrypt"], data)
    data = [filter(lambda td: td.size == 1391, d) for d in data]
    return data


def calculate_test_results(data) -> List[TestResult]:
    test_results = []
    for test_name, categories in data.items():
        for category, size in categories.keys():
            times = categories[(category, size)]
            if len(times) > 1:
                raise ValueError(
                    f"Expected only one time for test {test_name} with category {category} and size {size}")
            test_results.append(
                TestResult(test_category=category, test_name=test_name, number_of_processes=size, speedup=0,
                           efficiency=0))
    return test_results


def main():
    results_folder = create_results_folder_with_readable_timestamp()

    logs = [line.strip() for line in sys.stdin if line.startswith("TestCategory:")]
    # save_logs_to_file(logs, results_folder)

    data = extract_data_from_logs(logs)
    data = filter_data(data)

    print(data)


if __name__ == "__main__":
    main()
