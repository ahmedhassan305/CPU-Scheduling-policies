# Use the official lightweight Alpine Linux image with GCC installed
FROM alpine:latest

# Install build dependencies and necessary tools
RUN apk add --no-cache g++ make diffutils

# Set the WD 
WORKDIR /app

# Copy the lab6.cpp file and the testcases directory to the container
COPY lab6.cpp ./
COPY testcases ./testcases

# Compile the C++ source file
RUN g++ -o lab6 lab6.cpp

# Set the entrypoint to a shell to allow interactive commands (zay el mini shell keda)
ENTRYPOINT ["/bin/sh"]


#it was posssible to run the commands directly using the 
#CMD instruction and give it the commands to run it immedietly