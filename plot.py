import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt

size = []
time = []

cache_size = {}

with open('memory_latency.log', 'r') as f:
	for line in f:
		if ',' in line:
			parts = line.split(',')
			size.append(int(parts[0]))
			time.append(float(parts[1]))
		else:
			parts = line.split(' ')
			name = parts[0]
			s = int(parts[2])
			cache_size[name] = s

plt.plot(size, time)
for name in cache_size:
	plt.axvline(cache_size[name], label=name)
	plt.legend()
plt.xlabel('Size (byte)')
plt.ylabel('Latency (ns)')
plt.xscale('log')
plt.savefig('memory_latency.png')
