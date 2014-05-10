Vibox
Brian Connolly and Jared Rosen

Our repository has four subdirectories: gatherdata, data, vibox, and website.

gatherdata
This subdirectory contains the very simple webpage we made to log sensor values to a repository to build up our training dataset. To function properly, the computer running the website needs to be connected to the Galileo via Ethernet (see specifications below). Also, the website must be running on a webserver because it utilizes PUT requests. Lastly, in order to actually connect to our remote SQL database the IP address that the site is running on has to be whitelisted on the server. 

data
The data subdirectory has the raw data in csv format and the machine learning code in an iPython notebook. The code is very straight forward: it starts with exploratory data analysis and visualization and then moves into machine learning. 

vibox
The only file in the vibox subdirectory is the ino file that we uploaded to the Galileo. We wrote and compiled the code in the Arduino IDE version 1.5.3 (the only version that works with the Galileo). There are three primary portions of the code: reading the sensor values, running the web server, and computing the VibeCluster. In the void loop we are continually reading in sensor values and updating the weighted average for each variable and checking for incoming requests from the website. If a request is detected we call a function that calculates the VibeCluster. We then send a response with the VibeCluster and the raw sensor values and then go back to reading sensor vlaues and listening for more requests.

website
The website directory contains the final product, the website which plays music based on the VibeCluster from the Galileo. As such, in order to function properly, the website must be run on a webserver, which must be connected to the Intel Galileo as described below. The only page of the website is the index.html file.

Connecting to the Intel Galileo.
In order to connect a computer to the Galileo connect the two via Ethernet. Make sure the computers IP address is set to 192.168.1.xxx and the Subnet Mask is set to 255.255.255.0. 
