# UDPPingerServer.py
# We will need the following module to generate randomized lost packets
import random
from socket import *

# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(('', 12000))

# The timeout for a heartbeat before we consider client disconnected
HEARTBEAT_TIMEOUT = 10000

# The last sequence number and the time it came at
lastNum = 0
lastTime = 0.000000

while True:

	# Generate random number in the range of 0 to 10
	rand = random.randint(0, 10)
	
	# Receive the client packet along with the address it is coming from
	message, address = serverSocket.recvfrom(1024)
	
	# Get the sequence number and time
	thisNum = int(message.split()[1])
	thisTime = float(message.split()[2])
	
	# If the sequence numbers aren't in order print the number of lost packets
	if(thisNum != lastNum+1 and thisTime - lastTime < HEARTBEAT_TIMEOUT):
		print "Detected packet loss"
	
	# If the sequence numbers were in order and the heartbeat is still going (less than 10s since last one) print the time
	elif(lastTime != 0 and thisTime - lastTime < HEARTBEAT_TIMEOUT):
		print "Heartbeat"
	
	# Capitalize the message from the client
	message = message.upper()
	
	# If rand is less is than 4, we consider the packet lost and do not respond
	if rand < 4:
		continue
	
	# Otherwise, the server responds
	serverSocket.sendto(message, address)
	
	# Save the sequence number and time
	lastNum = thisNum
	lastTime = thisTime
