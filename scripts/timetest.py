import os

NSAMPLES = 8

FILENAME = "QRcode_Very_Large_out.png"
MAKE_CMD = "make very large"
RUN_CMD = "./pqrcode.out ../images/input/" + FILENAME + " ../images/output/" + FILENAME + " >> out.txt"

# FILENAME = "QRcode_Large.png"
# MAKE_CMD = "make large"
# RUN_CMD = "./pqrcode.out ../images/input/" + FILENAME + " ../images/output/" + FILENAME + " >> out.txt"

def average(l):
    sum = 0

    for e in l:
        sum += e
    
    return sum/len(l)

os.chdir("../sequential/")
cmd = MAKE_CMD
os.system(cmd)

open("out.txt", 'w').close()
for i in range(0,NSAMPLES):
    cmd = RUN_CMD
    os.system(cmd)

samples = []
for sample in open("out.txt"):
    samples.append(int(sample.strip()))

print("")
print("sequential:")
print("samples:" , samples)
print("average:", average(samples))

os.chdir("../parallel/")
print("")
cmd = MAKE_CMD
os.system(cmd)

for nt in ["2", "4", "6", "8"]:
    os.environ["OMP_NUM_THREADS"] = nt

    open("out.txt", 'w').close()
    for i in range(0,NSAMPLES):
        cmd = RUN_CMD
        os.system(cmd)

    samples = []
    for sample in open("out.txt"):
        samples.append(int(sample.strip()))

    print("")
    print("parallel" , nt, "threads:")
    print("samples:" , samples)
    print("average:", average(samples))
