# ProxyServer.py
# Author: Sean Watson
# Date Oct 8, 2012

from socket import *
import sys
import string

port = 8888
max_connections = 1

if len(sys.argv) <= 1:
	print 'Usage : "python ProxyServer.py server_ip"\n[server_ip : It is the IP Address Of Proxy Server]'
	sys.exit(2)

# Create a server socket, bind it to a port and start listening
tcpSerSock = socket(AF_INET, SOCK_STREAM)
tcpSerSock.bind((sys.argv[1],port))
tcpSerSock.listen(max_connections)

while 1:

	# Start receiving data from the client
	print 'Ready to serve...'
	
	tcpCliSock, addr = tcpSerSock.accept()
	print 'Received a connection from:', addr
	
	message = tcpCliSock.recv(1024)
	print message
	
	# Extract the filename from the given message
	print message.split()[1]
	filename = message.split()[1].partition("//")[2]
	print filename
	
	fileExist = "false"
	filetouse = "/" + filename.replace("/","")
	
	print filetouse
	
	try:
		# Check whether the file exist in the cache
		f = open(filetouse[1:], "r")
		outputdata = f.readlines()
		fileExist = "true"
		
		# ProxyServer finds a cache hit and generates a response message
		resp = ""
		for s in outputdata:
			resp += s
		
		tcpCliSock.send(resp)
		
		print 'Read from cache'
	
	# Error handling for file not found in cache
	except IOError:
		if fileExist == "false":
			# Create a socket on the proxyserver
			c = socket(AF_INET, SOCK_STREAM)
			hostn = filename.split('/')[0].replace("www.","",1)
			print hostn
			try:
				# Connect to the socket to port 80
				c.connect((hostn, 80))
		
				# Create a temporary file on this socket and ask port 80 for the file requested by the client
				fileobj = c.makefile('r', 0)
				fileobj.write("GET "+"http://" + filename + " HTTP/1.0\n\n")
				
				# Show what request was made
				print "GET "+"http://" + filename + " HTTP/1.0"

				# Read the response into buffer
				resp = c.recv(4096)
				response = ""
				while resp:
					response += resp
					resp = c.recv(4096)
				
				# Create a new file in the cache for the requested file.
				# Also send the response in the buffer to client socket and the corresponding file in the cache
				if(filename[-1:] == '/'):
					filename = filename[:-1]
					
				tmpFile = open("./" + filename.replace("/","") ,"wb")
				tmpFile.write(response)
				tmpFile.close()
				
				tcpCliSock.send(response)
			except Exception, e:
				print str(e)
				print "Illegal request"
		else:
			# HTTP response message for file not found
			pass
	
	# Close the client and the server sockets
	tcpCliSock.close()
