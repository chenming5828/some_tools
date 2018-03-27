import numpy as np
import matplotlib.pyplot as plt
import string

f = open('x1.txt')
lines = f.readlines()

arr1 = []

for line in lines:
    str = line[0:3]
    if (str != '---'):
        arr1.append(string.atof(line))

arr2 = []
f = open('x2.txt')
lines = f.readlines()
for line in lines:
    str = line[0:3]
    if (str != '---'):
        arr2.append(string.atof(line))

plt.figure(1)

plt.plot(arr1)
plt.plot(arr2)

plt.show()