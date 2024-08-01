"""
A real-time multi-channel plotter to display the data received from the Falcon Output plugin using ZMQ and Flatbuffers.
"""

import sys
import zmq
import flatbuffers
import numpy as np
import threading
from ContinuousData import *
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui, QtWidgets

# Address and port for the ZMQ connection
address = "127.0.0.1"
port = 3335 # <----- Change this value to match the port used by the Falcon Output plugin

# Initialize ZMQ context and socket
context = zmq.Context()
tcp_address = f"tcp://{address}:{port}"
socket = context.socket(zmq.SUB)
socket.setsockopt_string(zmq.SUBSCRIBE, "")
socket.connect(tcp_address)

# Buffer and plotting parameters
buffer_size = 40000  # Number of samples to keep in the buffer for each channel <----- Change this value as needed
num_channels_to_plot = 16  # Number of channels to plot <----- Change this value as needed
y_range = 250  # Adjust based on your expected amplitude range <----- Change this value as needed
channel_data = np.zeros((num_channels_to_plot, buffer_size))  # Initialize buffer
index = 0 # Index to keep track of the current position in the buffer
update_interval = 30  # Update interval for the plot in ms


class RealTimePlotter(QtWidgets.QMainWindow):
    """

    Real-time plotter class to display the data received from the GUI via the Faclon Output plugin.

    """

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        """
        Initializes the user interface for the real-time plotter.

        - Sets up the plot widget and adds it to the layout.
        - Creates individual plots for each channel and configures their appearance.
        - Adds a vertical scan line to the plot
        - Configures the Y and X ranges for the plot.
        - Sets the update interval and initializes a timer to periodically update the plot.
        """

        # Set up the main window
        self.setWindowTitle("Real-Time Channel Data")
        self.setGeometry(100, 100, 1200, 1000)

        # Set up the central widget and layout
        self.central_widget = QtWidgets.QWidget()
        self.setCentralWidget(self.central_widget)

        self.layout = QtWidgets.QVBoxLayout(self.central_widget)

        # Set up the plot widget
        self.plot_widget = pg.PlotWidget()
        self.layout.addWidget(self.plot_widget)

        self.plots = []
        self.ticks = []
        for i in range(num_channels_to_plot):
            # Create a plot for each channel
            self.plots.append(self.plot_widget.plot(pen=pg.mkPen(i, width=1)))
            self.ticks.append((i * y_range / 2, f"CH {num_channels_to_plot - i}"))
            self.plots[i].setSkipFiniteCheck(True)

        # Set the ticks for the left axis
        self.plot_widget.getAxis("left").setTicks([self.ticks])

        # Add a vertical line to the plot
        self.vline = pg.InfiniteLine(angle=90, movable=False, pen="w")
        self.plot_widget.addItem(self.vline)

        # Set the Y and X range for the plot
        self.plot_widget.setYRange(-(y_range / 2), num_channels_to_plot * (y_range / 2))
        self.plot_widget.setXRange(0, buffer_size)
        self.plot_widget.setLabel("left", "Channels")
        self.plot_widget.setLabel("bottom", "Samples")

        # Set up a timer to update the plot
        self.plot_timer = QtCore.QTimer()
        self.plot_timer.timeout.connect(self.update_plot)
        self.plot_timer.start(int(update_interval))

    def update_plot(self):
        global channel_data, index
        for i in range(num_channels_to_plot):
            self.plots[num_channels_to_plot - i - 1].setData(
                channel_data[i, :] + i * (y_range / 2)
            )  # Offset each line for better visibility
        self.vline.setPos(index)


def data_collection():
    """
    Function to collect data from the ZMQ socket and update the buffer with the received data.
    """

    global channel_data, index

    # Initialize buffer
    channel_data = np.zeros((num_channels_to_plot, buffer_size))

    while True:
        try:
            # Non-blocking wait to receive a message
            message = socket.recv(flags=zmq.NOBLOCK)

            # Decode the message
            try:
                buf = bytearray(message)
                data = ContinuousData.GetRootAsContinuousData(buf, 0)
            except Exception as e:
                print(
                    f"Impossible to parse the packet received - skipping to the next. Error: {e}"
                )
                continue

            # Access fields based on the schema
            num_samples = data.NSamples()
            num_channels = data.NChannels()
            samples_flat = data.SamplesAsNumpy() * 0.195  # Convert to microvolts

            # Check if the total size matches
            total_elements = samples_flat.size
            expected_elements = num_samples * num_channels

            if total_elements == expected_elements:
                samples_reshaped = samples_flat.reshape((num_channels, num_samples))

                # Update rolling buffer for each channel
                for i in range(num_channels_to_plot):
                    new_data = samples_reshaped[i, :]  # Collect data from each channel
                    if index + num_samples < buffer_size:
                        channel_data[i, index : index + num_samples] = new_data
                    else:
                        part1 = buffer_size - index
                        part2 = num_samples - part1
                        channel_data[i, index:] = new_data[:part1]
                        channel_data[i, :part2] = new_data[part1:]
                index = (index + num_samples) % buffer_size

            else:
                print(
                    f"Error: Expected {expected_elements} elements but got {total_elements}."
                )

        except zmq.Again:
            # No message received
            pass


def main():
    """
    Main function to start the real-time plotter application.
    """
    
    if socket is None:
        print("Error: ZMQ socket is not initialized.")
        sys.exit(1)

    # warn the user if the number of channels to plot is greater than 32
    if num_channels_to_plot > 32:
        print("\nWarning: Plotting more than 32 channels may slow down the application.\n")

    print(f"Starting the plot with {num_channels_to_plot} channels and a sampling rate of {buffer_size} Hz.")

    # Main function to start the application
    app = QtWidgets.QApplication(sys.argv)
    plotter = RealTimePlotter()
    plotter.show()

    # Start the data collection thread
    data_thread = threading.Thread(target=data_collection)
    data_thread.daemon = True
    data_thread.start()

    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
