"""
A Python test client to receive data from the Falcon Output plugin using ZMQ and Flatbuffers.

"""

import sys
import zmq
import flatbuffers
import numpy as np
import threading
from ContinuousData import *

# Address and port for the ZMQ connection
address = "127.0.0.1"
port = 3335 # <----- Change this value to match the port used by the Falcon Output plugin

# Initialize ZMQ context and socket
context = zmq.Context()
tcp_address = f"tcp://{address}:{port}"
socket = context.socket(zmq.SUB)
socket.setsockopt_string(zmq.SUBSCRIBE, "")
socket.connect(tcp_address)

def data_collection():
    """Function to collect data from the ZMQ socket and update the buffer with the received data.
    """
    while True:
        try:
            # Non-blocking wait to receive a message
            message = socket.recv(flags=zmq.NOBLOCK)
            
            # Decode the message
            try:
                buf = bytearray(message)
                data = ContinuousData.GetRootAsContinuousData(buf, 0)
            except Exception as e:
                print(f"Impossible to parse the packet received - skipping to the next. Error: {e}")
                continue

            print(f"Message id: {data.MessageId()} received {data.NSamples()} samples from {data.NChannels()} channels for stream {data.Stream()}.")

            # Access fields based on the schema
            num_samples = data.NSamples()
            num_channels = data.NChannels()
            samples_flat = data.SamplesAsNumpy()

            # Check if the total size matches
            total_elements = samples_flat.size
            expected_elements = num_samples * num_channels
            
            if total_elements == expected_elements:
                # Reshape the samples to a 2D array in channel-major order
                samples_reshaped = samples_flat.reshape((num_channels, num_samples))
                # Do something with the data
            else:
                print(f"Error: Expected {expected_elements} elements but got {total_elements}.")

        except zmq.Again:
            # No message received
            pass

def main():
    """
    Main function to start the data collection.
    """
    if socket is not None:
        print(f"Subscribed to {tcp_address}. Waiting for data...")
        data_collection() # Start the data collection
    else:
        print("Error: ZMQ socket is not initialized.")
        sys.exit(1)

if __name__ == '__main__':
    main()