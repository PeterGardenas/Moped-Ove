import time

import nav as n
from nav import *
from nav1 import whole4, pause, cont
from driving import stop, drive, steer
init()
n = int(sys.argv[2])
time.sleep(1.0*n)
wm.putcar(2.5, 14, 0)
whole4()
