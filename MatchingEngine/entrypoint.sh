#!/bin/bash
# Start the executable in the background
./equityexchangetest &&

# Start Netcat in the foreground to keep the container running
cat inputFile.csv | nc -u -w 1 127.0.0.1 1234
