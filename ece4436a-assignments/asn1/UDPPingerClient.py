# UDPPingerClient.py
# Author: Sean Watson
# Date: Oct 8, 2012

from socket import *
import time

TIMEOUT = 1 	# Response timeout
REQUESTS = 10 	# The number of packets to send
IP = "127.0.0.1"	# IP address to ping
PORT = 12000	# Port to ping on

lost_packets = 0
response_times = []

# Create a UDP socket for the client
csocket = socket(AF_INET, SOCK_DGRAM)
csocket.settimeout(TIMEOUT)

# Loop through the packet sending procedure 10 times
for i in range(REQUESTS):
	
	# Set up all the parameters
	sequence = i + 1
	current_time = time.time() * 1000
	message = "Ping " + str(sequence) + " " + str(current_time)
	
	# Send the packet
	csocket.sendto(message,(IP,PORT))
	
	# Listen for a response
	try:
		response, address = csocket.recvfrom(1024)
		
		# Find the time the ping was sent at
		sending_time = response.split()[2]
		
		# Calculate the round trip time (RTT)
		rtt = (time.time() * 1000) - float(sending_time)
		
		# Print the response and the RTT
		print response + " : time=" + str(rtt) + " ms"
		
		# Store the RTT to calculate statistics
		response_times.append(rtt)
		
	except timeout:
		
		# No response received, print the timeout message
		print "Request timed out"
		
		# Increment the number of lost packets
		lost_packets += 1

# Calculate the statistics
loss_percent = float(lost_packets) / REQUESTS * 100
min_rtt = min(response_times)
max_rtt = max(response_times)
avg_rtt = sum(response_times) / len(response_times)

# Print the summary
print "--- Ping statistics for " + IP + ":" + str(PORT) + " ---"
print str(REQUESTS) + " packets transmitted, " + str(REQUESTS - lost_packets) + " packets received, " + str(loss_percent) + "% packet loss"
print "RTT (min/avg/max): " + str(min_rtt) + " ms / " + str(avg_rtt) + " ms / " + str(max_rtt) + " ms"
