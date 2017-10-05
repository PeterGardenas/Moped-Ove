from __future__ import division #Avoid division problems in Python 2
import math
import pyaudio
import sys

PyAudio = pyaudio.PyAudio
RATE = 18000
WAVE = 2000
data = ''.join([chr(int(math.sin(x/((RATE/WAVE)/math.pi))*127+128)) for x in xrange(RATE)])
p = PyAudio()

stream = p.open(format =
                p.get_format_from_width(1),
                channels = 1,
                rate = RATE,
                output = True)
for DISCARD in xrange(1):
    stream.write(data)
stream.stop_stream()
stream.close()
p.terminate()
