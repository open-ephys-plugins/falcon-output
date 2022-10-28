/*
 ------------------------------------------------------------------
 FalconOutput
 Copyright (C) 2021 - present Neuro-Electronics Research Flanders

 This file is part of the Open Ephys GUI
 Copyright (C) 2016 Open Ephys
 ------------------------------------------------------------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <zmq.h>
#include <iostream>
#include <string>
#include "channel_generated.h"
#include "flatbuffers/flatbuffers.h"


int main(int argc, char **argv) {

    // Parameters
    std::string address = "127.0.0.1";
    int port = 3335;

    // Step 1: Create your ZMQ socket
    auto context = zmq_ctx_new();
    auto tcp_address = "tcp://" + address + ":" + std::to_string(port);
    auto socket = zmq_socket(context, ZMQ_SUB);
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, nullptr, 0);
    zmq_connect(socket, tcp_address.c_str());

    // Step 2 : Loop to receive packets
    while(1){

        zmq_msg_t message;
        zmq_msg_init (&message);
        const openephysflatbuffer::ContinuousData* data;

        if (zmq_msg_recv(&message, socket, ZMQ_DONTWAIT) != -1)  // Non-blocking to wait to receive a message
        {

            // Step 3: Decode the message
            try {
                data = openephysflatbuffer::GetContinuousData(zmq_msg_data(&message));
            } catch (...) {
                std::cout << "Impossible to parse the packet received - skipping to the next." << std::endl;
                continue;
            }


            std::cout << "Received packet number: " << data->message_id()
                    << ", Stream: " << data->stream()->c_str()
                    << ", Sample_Number: " << data->sample_num()
                    << ", Samples: " << data->n_samples()
                    << ", Channels: " << data->n_channels() << std::endl;

            // Step 4: Process your data: [sample0/chan0, sample1/chan0, ..., sampleN/chan0, sample0/chan1, sample1/chan1...]
            // for(auto i = data->samples()->begin(); i < data->samples()->begin() + data->n_samples(); i++)  // Only processing the first channel
            // {
            //     std::cout << "Sample Value: " << *i << std::endl;
            // }

        }

    }

}