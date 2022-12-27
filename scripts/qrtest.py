import os
import subprocess

csv = open("qrtest.csv")
os.chdir("../sequential/")

for line in csv:
    [filename,code] = line.split(",")
    code = code.strip()
    print("file: " + filename)
    cmd = "./pqrcode.out ../images/input/" + filename +" ../images/output/" + filename + "_out.png > out.txt"
    os.system(cmd)
    out = open("out.txt").readline().strip()
    
    if out == code:
        print("passed")
    else:
        print("failed")
