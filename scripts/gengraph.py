import matplotlib.pyplot as plt

nthreads = [1,2,4,6,8]
times = [9994614.75, 6764518.125, 5462597.0, 5317915.375, 4624276.75]

for i in range(0, len(times)):

    times[i] /= 1000000
    times[i] = round(times[i], 3)

x = nthreads
y = times

plt.plot(x, y)

plt.xlabel('N Threads')

plt.ylabel('Execution Time (seconds)')

plt.title('QR Code image (4340x4460 pixels)')

for a,b in zip(x, y): 
    plt.text(a, b, str(b))

plt.show()
