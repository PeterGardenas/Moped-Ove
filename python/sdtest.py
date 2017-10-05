import sys
import time

import os


text = "0.032504 0.061341 0.001573 -0.000263 0.000042 -0.000019 0.032503 0.061341 -0.000000 0.000000 2.644563 14.078739 359.999447 0.012207 0.000000 0.000000 0.000000 0.000000 0.000000 -0.017198 0.400000 -23.000000 47.000000 0.071519 0.000000 0.000000 0.232163 0.000000 0.000000 0.000000 0.000000 0.000000 %f\n"

f = open("tmpfile", "w", 1000)

t0 = time.time()

n = 0

length = 0

pid = os.getpid()

while True:
    n += 1
    t = time.time()
    text1 = text % t
    length += len(text1)
    f.write(text1)
    if n%10000 == 0:
        pass
        print((t, length))
    if t-t0 >= 10.0:
        os._exit(0)
        os.system("kill -9 %d" % pid)
        break

f.close()

print((t0,t))
