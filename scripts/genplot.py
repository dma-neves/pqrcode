import matplotlib.pyplot as plt

title = "QR Code image (4340x4460 pixels)"
nthreads = [1,2,4,6,8]
times = [1386452.0, 760978.875, 572731.25, 517555.625, 481059.75]

# title = "QR Code image (2170x2230 pixels)"
# nthreads = [1,2,4,6,8]
# times = [402892.375, 198650.625, 149227.25, 143830.625, 130349.625]

for i in range(0, len(times)):
    times[i] /= 1000
    times[i] = round(times[i], 3)

x = nthreads
y = times

plt.plot(x, y)

plt.xlabel('N Threads')
plt.ylabel('Execution Time (milliseconds)')
plt.title(title)

for a,b in zip(x, y): 
    plt.text(a, b, str(b))

plt.ylim([0, plt.ylim()[1]])

plt.show()
