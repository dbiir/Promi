import os
os.chdir('..')
cmd = "make clean; make -j;"
print(cmd)
os.system(cmd)

Path = "/home/dzh/project/detest"
Path1 = "home/dzh"
Files = ["rundb","runcl","ifconfig.txt","benchmarks/YCSB_schema.txt"]
User = "dzh"
Machines = ["10.77.110.146","10.77.110.147","10.77.110.148",]
for m in Machines:




    
    for f in Files:
        cmd = "scp {}/{} {}@{}:/{}".format(Path, f, User, m, Path1)
        print(cmd)
        os.system(cmd)

