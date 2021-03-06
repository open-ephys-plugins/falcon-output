# Falcon Output

This is an Open Ephys GUI plugin that streams incoming continuous data with very low latency. It is optimized for use with [Falcon](https://falcon-core.readthedocs.io/en/latest/), Linux-based real-time analysis software maintained by the [Kloosterman Lab](https://www.nerf.be/research/nerf-labs/fabian-kloosterman) at NERF. However, because it uses a general-purpose data packaging format (based on [ZeroMQ](https://zeromq.org/) and [Flatbuffers](https://google.github.io/flatbuffers/)), it can be used to create advanced visualization and monitoring add-ons in almost any programming language.

## Installation
### Dependencies (automatically installed at build time)

- ZMQ lib - shared lib for windows, linux, and mac already included for the plugin
- Flatbuffer lib - build at built time (via cmake FetchContent)

## Main usage 

Communication setup with the OpenEphysZMQ Falcon processor. 

## How to create your own client

- ZMQ communication as Subscriber (no source id)
- copy the schema fbs to decode the Flatbuffer packet

For more details, look in the `client` folder in the repository

## Benchmarking

Two points can be optimized here to reduce the streaming latency (known as "fast and small").

This benchmark has been done by measuring, for different frequencies set in Open Ephys GUI, the time to 
encode the package and sent it.

![Excution time is a function of the frequency rate to send packets](doc/execution_time.png)

In terms of size, the Flatbuffer packaging is always adding exactly 64 bits to the raw data. 

## Special use-case and round trip obtained 

This plugin has been originally developed to stream Neuropixels data with low latency from Open-Ephys to Falcon.
[Falcon](https://falcon-core.readthedocs.io/en/latest/) is a software used for low-latency real-time processing such as ripple detection and decoding. 

To run the latency tests, a sinusoidal signal was generated in the saline liquid. All 384 channels + the digital signal were streamed by Open Ephys. In falcon, a simple threshold applied on one of the channel was used to generate events transferred by an Arduino as a 
square signal in the digital input of the Neuropixels.

![setup of the falcon streaming use-case](doc/falcon_use_case.png)

The round-trip latency was measured by comparing the Neuropixels digital input (rising edge of the square wave signal) 
recorded in Falcon with the timestamps of events generated by the Falcon threshold crossing node. 

![round trip executing time for sending neuropixels in falcon is around 9.2ms](doc/round_trip_falcon.png)

- Median delay: 9.241 ms
- Standard deviation: 1.302 ms