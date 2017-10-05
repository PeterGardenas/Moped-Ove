import sys
import re
import os, urlparse
#import mosquitto
import paho.mqtt.client as mosquitto


# Define event callbacks
def on_connect(mosq, obj, rc):
    print("rc: " + str(rc))

def on_message_OLD(mosq, obj, msg):
    p = str(msg.payload)
    while p[-1] == '\n' or p[-1] == '\t':
        p = p[:-1]
    sys.stdout.write(msg.topic + " " + str(msg.qos) + " " + p + "\n")
    m = re.search("HONK", p)
    if m:
	m = re.search('current_value":"([0-9]+)', p)
	if m:
	    print m.group(1)
            os.system("aplay /home/pi/paho/%s.wav" % m.group(1))
    #print(msg.topic + " " + str(msg.qos))

lastlist = None

compass = ['north', 'northeast', 'east', 'southeast', 'south', 'southwest',
           'west', 'northwest']

def on_message(mosq, obj, msg):
    global lastlist
    p = str(msg.payload)
    while p[-1] == '\n' or p[-1] == '\t':
        p = p[:-1]
    sys.stdout.write(msg.topic + " " + str(msg.qos) + " " + p + "\n")
    m = re.search("POS", p)
    if m:
	m = re.search('current_value":"position ([0-9]+): ([0-9 ]+)', p)
	if m:
            data = m.group(2)
	    print data
            l = data.split(" ")
            print l
            x = int(l[0])
            y = int(l[1])
            ang = int(l[2])*360/256
            q = int(l[3])
            if q > 20 and x > 0 and y < 65000 and l != lastlist:
                lastlist = l
                print (x,y,ang)
                #os.system("espeak '%d %d'" % (x/50, y/50))
                a = int((ang+22.5+360)*8/360)%8
                print a
                #os.system("espeak '%d'" % (ang))
                os.system("espeak '%d %d %s'" % (x/100, y/100, compass[a]))
    #print(msg.topic + " " + str(msg.qos))

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(mosq, obj, level, string):
    print(string)

mqttc = mosquitto.Mosquitto()
# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

# Uncomment to enable debug messages
#mqttc.on_log = on_log

# Parse CLOUDMQTT_URL (or fallback to localhost)
url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://localhost:1883')
url = urlparse.urlparse(url_str)

# Connect
#mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)

# Start subscribe, with QoS level 0
#mqttc.subscribe("hello/world2", 0)
if False:
    mqttc.subscribe("hello/world", 0)
    mqttc.subscribe("hello/+", 0)
    mqttc.subscribe("/hello", 0)
    mqttc.subscribe("+/world2", 0)
    mqttc.subscribe("world2", 0)
    mqttc.subscribe("+/speed", 0)
    mqttc.subscribe("speed", 0)
    mqttc.subscribe("/zeni/speed", 0)
    mqttc.subscribe("/zeni/+", 0)
    mqttc.subscribe("fw", 0)
    mqttc.subscribe("fw/+", 0)

mqttc.subscribe("/zeni/+", 0)

mqttc.subscribe("/sics/moped/+", 0)

#mqttc.subscribe("+/+", 0)
#mqttc.subscribe("/+/+", 0)

#mqttc.subscribe("+", 0)

# Publish a message
mqttc.publish("eee/hui", "hallo")
mqttc.publish("eee/zwei", "hallo")

mqttc.publish("/sics/moped/to-car/hej", "hopp")
mqttc.publish("/sics/moped/to-car/hej", "happ")

# Continue the network loop, exit when an error occurs
rc = 0
while rc == 0:
    rc = mqttc.loop()
print("rc: " + str(rc))

