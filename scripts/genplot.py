import matplotlib.pyplot as plt

title = "QR Code image (4340x4460 pixels)"
nthreads = [1,2,4,6,8]
times = [10019368.875, 6803757.5, 5276054.625, 5070326.75, 4302069.625]

# title = "QR Code image (2170x2230 pixels)"
# nthreads = [1,2,4,6,8]
# times = [1964211.625, 1329297.25, 1016332.75, 980577.25, 888286.875]

for i in range(0, len(times)):
    times[i] /= 1000000
    times[i] = round(times[i], 3)

x = nthreads
y = times

plt.plot(x, y)

plt.xlabel('N Threads')
plt.ylabel('Execution Time (seconds)')
plt.title(title)

for a,b in zip(x, y): 
    plt.text(a, b, str(b))

plt.show()
