import os

NSAMPLES = 8

FILENAME = "QRcode_Very_Large.png"
MAKE_CMD = "make very_large"
RUN_CMD = "./pqrcode.out ../images/input/" + FILENAME + " ../images/output/" + FILENAME + " >> out.txt"

# FILENAME = "QRcode_Large.png"
# MAKE_CMD = "make large"
# RUN_CMD = "./pqrcode.out ../images/input/" + FILENAME + " ../images/output/" + FILENAME + " >> out.txt"

averages = []

def average(l):
    return sum(l) / len(l)

os.chdir("../sequential/")
os.system(MAKE_CMD)

open("out.txt", 'w').close()
for i in range(0,NSAMPLES):
    os.system(RUN_CMD)

samples = []
for sample in open("out.txt"):
    samples.append(int(sample.strip()))

print("")
print("sequential:")
print("samples:" , samples)
avg = average(samples)
averages.append(avg)
print("average:", avg)

os.chdir("../parallel/")
print("")
os.system(MAKE_CMD)

for nt in ["2", "4", "6", "8"]:
    os.environ["OMP_NUM_THREADS"] = nt

    open("out.txt", 'w').close()
    for i in range(0,NSAMPLES):
        os.system(RUN_CMD)

    samples = []
    for sample in open("out.txt"):
        samples.append(int(sample.strip()))

    print("")
    print("parallel" , nt, "threads:")
    print("samples:" , samples)
    avg = average(samples)
    averages.append(avg)
    print("average:", avg)

print("")
print("averages list:", averages)
