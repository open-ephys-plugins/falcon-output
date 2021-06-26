# Stream plugin

This plugin allows to stream with low latency continuous dataflow incoming in Open-Ephys via ZMQ packaged following a flatbuffer schema, enabling the creation of advanced visualization and monitoring add-ons. 


## Installation
### Dependencies (automatically installed at build time)

- ZMQ lib - shared lib for windows, linux, and mac already included for the plugin
- Flatbuffer lib - build at built time (via cmake FetchContent)

## Main usage 

Communication setup with the OpenEphysZMQ Falcon processor. 

## How to create your own client

- ZMQ communication as Subscriber (no source id)
- copy the schema fbs to decode the flatbuffer packet
