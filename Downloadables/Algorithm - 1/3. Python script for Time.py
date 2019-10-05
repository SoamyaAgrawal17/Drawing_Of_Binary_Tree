#!/usr/bin/python

#open a file
from matplotlib import pyplot as plt
fileobject = open("time-1.txt","r")
i = 0
xaxis = []
yaxis = []
print("name of the file is: ", fileobject.name)
try:
    while i!=501000:
        str = fileobject.readline()
        x = [float(i) for i in str.split()]
        print(x[0], x[1])
        xaxis.append(x[1])
        yaxis.append(x[0])
        i+=1000

except EOFError:
    pass
plt.plot(xaxis,yaxis)
plt.title("Time analysis of Algorithm-1")
plt.ylabel("Time in seconds")
plt.xlabel("Input size")
plt.show()
fileobject.close()

