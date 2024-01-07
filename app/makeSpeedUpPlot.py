import matplotlib.pyplot as plt
import re
from statistics import mean

log_data = """
TestCategory:Sequential_Test|TestName:KeyExpansion|Size:1391|ClockTime:0.000010
TestCategory:Sequential_Test|TestName:Encrypt|Size:1391|ClockTime:0.012527
TestCategory:Sequential_Test|TestName:Decrypt|Size:1391|ClockTime:0.014403
TestCategory:OpenMP_Test-2|TestName:KeyExpansion|Size:1391|ClockTime:0.000017
TestCategory:OpenMP_Test-2|TestName:Encrypt|Size:1391|ClockTime:0.007236
TestCategory:OpenMP_Test-2|TestName:Decrypt|Size:1391|ClockTime:0.007864
TestCategory:OpenMP_Test-3|TestName:KeyExpansion|Size:1391|ClockTime:0.000016
TestCategory:OpenMP_Test-3|TestName:Encrypt|Size:1391|ClockTime:0.004737
TestCategory:OpenMP_Test-3|TestName:Decrypt|Size:1391|ClockTime:0.005524
TestCategory:OpenMP_Test-4|TestName:KeyExpansion|Size:1391|ClockTime:0.000015
TestCategory:OpenMP_Test-4|TestName:Encrypt|Size:1391|ClockTime:0.003686
TestCategory:OpenMP_Test-4|TestName:Decrypt|Size:1391|ClockTime:0.003927
"""

sequential_times = {'Encrypt': 0, 'Decrypt': 0}
parallel_times = {2: {'Encrypt': [], 'Decrypt': []},
                  3: {'Encrypt': [], 'Decrypt': []},
                  4: {'Encrypt': [], 'Decrypt': []}}

# Regex pattern to match the log entries
pattern = re.compile(r"TestCategory:(\w+)_Test(-\d+)?\|TestName:(Encrypt|Decrypt)\|Size:1391\|ClockTime:(\d+\.\d+)")

# Parse the log data
for line in log_data.split('\n'):
    match = pattern.search(line)
    if match:
        test_category, num_threads, test_type, time = match.groups()
        time = float(time)

        print(test_category, num_threads, test_type, time)

        if 'Sequential' in test_category:
            # Only set the time once as the sequential time doesn't change with number of threads
            if sequential_times[test_type] == 0:
                sequential_times[test_type] = time
        else:
            # The number of threads is in the test_category string
            num_threads = int(num_threads[1])
            parallel_times[num_threads][test_type].append(time)

# Calculate average times for parallel tests and speedup for each thread count
speedups = {'Encrypt': [], 'Decrypt': []}

for num_threads in parallel_times:
    for test_type in parallel_times[num_threads]:
        if parallel_times[num_threads][test_type]:
            avg_parallel_time = mean(parallel_times[num_threads][test_type])
            speedup = sequential_times[test_type] / avg_parallel_time
            speedups[test_type].append((num_threads, speedup))

# Sort the speedup data by number of threads

speedups['Encrypt'].sort()
speedups['Decrypt'].sort()

# Plotting the speedup for Encrypt and Decrypt
fig, ax = plt.subplots()

# Extract number of threads and corresponding speedup for plotting
threads_encrypt, speedup_encrypt = zip(*speedups['Encrypt'])
threads_decrypt, speedup_decrypt = zip(*speedups['Decrypt'])

ax.plot(threads_encrypt, speedup_encrypt, marker='o', label='Encrypt')
ax.plot(threads_decrypt, speedup_decrypt, marker='o', label='Decrypt')

# Adding labels and title
ax.set_xlabel('Number of Threads')
ax.set_ylabel('Speedup')
ax.set_title('Speedup of Encrypt and Decrypt with Different Number of Threads')
ax.legend()

# Show the plot
plt.show()