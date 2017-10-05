import nav as n
import time
from nav import *
from nav1 import whole4, pause, cont
from driving import stop, drive, steer
init()
time.sleep(3)
g.limitspeed = None
driving.drive(7)
counterDrive = 0
while (True):
        print("g.can_ultra ")
        print(g.can_ultra)
        print("g.limitspeed: ")
        print(g.limitspeed)
        if (g.can_ultra >= 1):
                counterDrive = counterDrive + 1
                if (counterDrive >= 3):
                        driving.drive(7)
                        print("In if not and should drive")
        else:
                counterDrive = 0
                driving.drive(0)
                print("In if and should stop")

        time.sleep(0.001)
