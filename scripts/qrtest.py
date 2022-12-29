import os

csv = open("qrtestAll.csv")

def test():

    cmd = "make"
    os.system(cmd)
    os.environ["OMP_NUM_THREADS"] = "4"

    csv.seek(0)
    for line in csv:
        [filename,code] = line.split(",")
        code = code.strip()
        print("file: " + filename)
        cmd = "./pqrcode.out ../images/input/" + filename +" ../images/output/" + filename[:-4] + "_out.png > out.txt"
        os.system(cmd)
        out = open("out.txt").readline().strip()
        
        if out == code:
            print("passed")
        else:
            print("########### failed ###########")


os.chdir("../sequential/")
print("sequential:")
print("")
test()
os.chdir("../parallel/")
print("")
print("parallel:")
print("")
test()