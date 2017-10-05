import requests
import time
import os
import picamera


def postImage(session, url):
    """
    Post an image to the server.
    """
    print("Posting image...")
    try:
        # Convert image to grayscale
#        im = Image.open("/dev/shm/optiposimage.jpg").convert("L").save("/dev/shm/optiposimage.jpg", quality = 25)
        # Send image to server

        response = session.post(url, files = {"file": ("", open("/dev/shm/optiposimage.jpg", "rb"))}).text
        #response = session.post(url, files = {"file": ("", open("yellowCircle.jpg", "rb"))}).text
    except Exception as e:
        print(e)
        response = "Connection broken"
    return response

def main():
    print("hai")

    server = 'http://192.168.137.1:8080/processimage'

    camera = picamera.PiCamera()
    resolution = 972
    camera.resolution = (resolution, resolution)
    camera.iso = 800
    camera.meter_mode = "backlit"

    with requests.Session() as session:

        time.sleep(2)

        response = None
        # Construct a stream to hold image data temporarily (we could write it directly to connection but in this
        # case we want to find out the size of each capture first to keep the protocol simple)
        if True:
        #for _ in camera.capture_continuous("/dev/shm/optiposimage.jpg", 'jpeg', use_video_port = True, quality = 10):
            try:
                # Get the start time, to be able to calculate response time
                start = time.time()
                response = postImage(session, server)
                end = time.time()
                print('Received [' + response + '] after ' + str(end - start) + ' s')
            except:
                pass


main()
