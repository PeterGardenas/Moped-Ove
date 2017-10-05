from __future__ import division #Avoid division problems in Python 2
import math
import pyaudio
import sys

PyAudio = pyaudio.PyAudio
RATE1 = int(sys.argv[1]) #18000
RATE2 = int(sys.argv[2]) #3000
WAVE = float(sys.argv[3]) #2000.0
data = ''.join([chr(int(math.sin(x/((RATE1/WAVE)/math.pi))*100+100)) for x in xrange(RATE2)])

data2 = [int(math.sin(x/((RATE1/WAVE)/math.pi))*127+128) for x in xrange(RATE2)]

p = PyAudio()

print data2

stream = p.open(format =
                p.get_format_from_width(1),
                channels = 1,
                rate = RATE1,
                output = True)

stream.write(data)

stream.stop_stream()
stream.close()
p.terminate()
