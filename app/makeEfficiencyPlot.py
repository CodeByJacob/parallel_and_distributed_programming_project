import matplotlib.pyplot as plt
import re
from statistics import mean

log_data = """
TestCategory:Sequential_Test|TestName:KeyExpansion|Size:1391|ClockTime:0.000012
TestCategory:Sequential_Test|TestName:Encrypt|Size:1391|ClockTime:0.012463
TestCategory:Sequential_Test|TestName:Decrypt|Size:1391|ClockTime:0.014362
TestCategory:MPI_Test-2|TestName:KeyExpansion|Size:1391|ClockTime:0.000074
TestCategory:MPI_Test-2|TestName:Encrypt|Size:1391|ClockTime:0.007387
TestCategory:MPI_Test-2|TestName:Decrypt|Size:1391|ClockTime:0.007964
TestCategory:MPI_Test-3|TestName:KeyExpansion|Size:1391|ClockTime:0.004411
TestCategory:MPI_Test-3|TestName:Encrypt|Size:1391|ClockTime:0.005212
TestCategory:MPI_Test-3|TestName:Decrypt|Size:1391|ClockTime:0.006780
TestCategory:MPI_Test-4|TestName:KeyExpansion|Size:1391|ClockTime:0.003351
TestCategory:MPI_Test-4|TestName:Encrypt|Size:1391|ClockTime:0.010694
TestCategory:MPI_Test-4|TestName:Decrypt|Size:1391|ClockTime:0.008242
TestCategory:Sequential_Test|TestName:KeyExpansion|Size:1391|ClockTime:0.000009
TestCategory:Sequential_Test|TestName:Encrypt|Size:1391|ClockTime:0.012480
TestCategory:Sequential_Test|TestName:Decrypt|Size:1391|ClockTime:0.015584
TestCategory:MPI_Test-2|TestName:KeyExpansion|Size:1391|ClockTime:0.000017
TestCategory:MPI_Test-2|TestName:Encrypt|Size:1391|ClockTime:0.007205
TestCategory:MPI_Test-2|TestName:Decrypt|Size:1391|ClockTime:0.007843
TestCategory:MPI_Test-3|TestName:KeyExpansion|Size:1391|ClockTime:0.002206
TestCategory:MPI_Test-3|TestName:Encrypt|Size:1391|ClockTime:0.008061
TestCategory:MPI_Test-3|TestName:Decrypt|Size:1391|ClockTime:0.008702
TestCategory:MPI_Test-4|TestName:KeyExpansion|Size:1391|ClockTime:0.002236
TestCategory:MPI_Test-4|TestName:Encrypt|Size:1391|ClockTime:0.011128
TestCategory:MPI_Test-4|TestName:Decrypt|Size:1391|ClockTime:0.008325
TestCategory:Sequential_Test|TestName:KeyExpansion|Size:1391|ClockTime:0.000009
TestCategory:Sequential_Test|TestName:Encrypt|Size:1391|ClockTime:0.012402
TestCategory:Sequential_Test|TestName:Decrypt|Size:1391|ClockTime:0.014432
TestCategory:MPI_Test-2|TestName:KeyExpansion|Size:1391|ClockTime:0.000017
TestCategory:MPI_Test-2|TestName:Encrypt|Size:1391|ClockTime:0.007279
TestCategory:MPI_Test-2|TestName:Decrypt|Size:1391|ClockTime:0.008231
TestCategory:MPI_Test-3|TestName:KeyExpansion|Size:1391|ClockTime:0.004276
TestCategory:MPI_Test-3|TestName:Encrypt|Size:1391|ClockTime:0.018739
TestCategory:MPI_Test-3|TestName:Decrypt|Size:1391|ClockTime:0.019039
TestCategory:MPI_Test-4|TestName:KeyExpansion|Size:1391|ClockTime:0.003367
TestCategory:MPI_Test-4|TestName:Encrypt|Size:1391|ClockTime:0.018285
TestCategory:MPI_Test-4|TestName:Decrypt|Size:1391|ClockTime:0.009911
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

        if 'Sequential' in test_category:
            # Only set the time once as the sequential time doesn't change with number of threads
            if sequential_times[test_type] == 0:
                sequential_times[test_type] = time
        else:
            # The number of threads is in the test_category string
            num_threads = int(num_threads[1])
            parallel_times[num_threads][test_type].append(time)

# Calculate average times for parallel tests and speedup for each thread count
efficiencies = {'Encrypt': [], 'Decrypt': []}

for num_threads in parallel_times:
    for test_type in parallel_times[num_threads]:
        if parallel_times[num_threads][test_type]:
            avg_parallel_time = mean(parallel_times[num_threads][test_type])
            speedup = (sequential_times[test_type] / avg_parallel_time) * 100
            efficiency = speedup / num_threads
            efficiencies[test_type].append((num_threads, efficiency))

# Sort the speedup data by number of threads

efficiencies['Encrypt'].sort()
efficiencies['Decrypt'].sort()

# Plotting the speedup for Encrypt and Decrypt
fig, ax = plt.subplots()

# Extract number of threads and corresponding speedup for plotting
threads_encrypt, speedup_encrypt = zip(*efficiencies['Encrypt'])
threads_decrypt, speedup_decrypt = zip(*efficiencies['Decrypt'])

ax.plot(threads_encrypt, speedup_encrypt, marker='o', label='Encrypt')
ax.plot(threads_decrypt, speedup_decrypt, marker='o', label='Decrypt')

# Adding labels and title
ax.set_xlabel('Number of Threads')
ax.set_ylabel('Efficiency [%]')
ax.set_title('MPI Efficiency of Encrypt and Decrypt for test size 1391')
ax.legend()

plt.savefig('results/mpi_efficiency_plot.png', dpi=300)

# Show the plot
# plt.show()