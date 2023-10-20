import sys
import matplotlib.pyplot as plt
from collections import defaultdict


def main():
    # Parsing the logs and organizing the data
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))

    for line in sys.stdin:
        log = line.strip()
        parts = log.split("|")
        test_category = parts[0].split(":")[1]
        test_name = parts[1].split(":")[1]
        for part in parts[2:]:
            key, value = part.split(":")
            data[test_category][test_name][key] = float(value)

    # Generate the plots with separate columns and different colors for each time type
    colors = {'TimeStandard': 'blue', 'TimeCPU': 'red', 'TimeClock': 'green'}

    for test_category in data:
        plt.figure(figsize=(10, 5))
        plt.title(test_category)
        plt.xlabel("Test Name")
        plt.ylabel("Time (seconds)")

        bar_width = 0.2  # Width of the bars
        index = 0  # Position index for bars
        x_labels = list(data[test_category].keys())
        x_ticks = range(len(x_labels))

        for time_type in ["TimeStandard", "TimeCPU", "TimeClock"]:
            times = [data[test_category][test_name][time_type] for test_name in x_labels]
            plt.bar([tick + index * bar_width for tick in x_ticks], times, width=bar_width, alpha=0.7, label=time_type,
                    color=colors[time_type])
            index += 1  # Increment the position index

        plt.xticks([tick + bar_width for tick in x_ticks], x_labels)  # Positioning the x-labels
        plt.legend()

        # Save the plot to a file
        file_name = f"{test_category.replace(' ', '_')}_plot.png"
        plt.savefig(file_name)
        plt.close()


if __name__ == "__main__":
    main()
