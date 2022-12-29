import os

NSAMPLES = 8

def average(l):
    sum = 0

    for e in l:
        sum += e
    
    return sum/len(l)

os.chdir("../sequential/")
cmd = "make large"
os.system(cmd)

open("out.txt", 'w').close()
for i in range(0,NSAMPLES):
    cmd = "./pqrcode.out ../images/input/QRcode_Very_Large.png ../images/output/QRcode_Very_Large_out.png >> out.txt"
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
cmd = "make large"
os.system(cmd)

for nt in ["2", "4", "6", "8"]:
    os.environ["OMP_NUM_THREADS"] = nt

    open("out.txt", 'w').close()
    for i in range(0,NSAMPLES):
        cmd = "./pqrcode.out ../images/input/QRcode_Very_Large.png ../images/output/QRcode_Very_Large_out.png >> out.txt"
        os.system(cmd)

    samples = []
    for sample in open("out.txt"):
        samples.append(int(sample.strip()))

    print("")
    print("parallel" , nt, "threads:")
    print("samples:" , samples)
    print("average:", average(samples))
