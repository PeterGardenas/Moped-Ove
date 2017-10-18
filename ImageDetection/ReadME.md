# Image detection
The detection consists of three components, a Java server on an external device which analyse the images and sends the response, 
a python script which captures images and sends them to the external java server and a Java server on the MOPED which receives and handles the response.
The process can be graphically displayed as: 
![alt text](https://github.com/PeterGardenas/Moped-Ove/blob/master/ImageDetection/imageDetectionFlowChart.png "Logo Title Text 1")


## Running image detection ##
1. Start the java server on a computer by running ImageDetectionServer.java in Moped-Ove/ImageDetection.
2. Start sending images by running python3 runCamera.py on the MOPED. The scripts needs the ip- address of the java server, either change the server variable in the script or add the ip as an argument to python3 runCamera.py, eg. python3 runCamera.py 192.168.1.1
3. Start the ACC and image detection by running run.sh on the MOPED.
