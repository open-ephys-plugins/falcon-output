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

#include <zmq.hpp>
#include <iostream>
#include <string>
#include "channel_generated.h"
#include "flatbuffers/flatbuffers.h"


int main(int argc, char **argv) {

    // Parameters
    std::string address = "127.0.0.1";
    int port = 3335;

    // Step 1: Create your ZMQ socket
    zmq::context_t context;
    auto tcp_address = "tcp://" + address + ":" + std::to_string(port);
    zmq::socket_t socket = zmq::socket_t(context, ZMQ_SUB);
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, nullptr, 0);
    socket.connect(tcp_address);

    // Step 2 : Loop to receive packets

    while(1){

      zmq_msg_t message;
      zmq_msg_init (&message);
      const openephysflatbuffer::ContinuousData* data;

      if (zmq_msg_recv(&message, socket, ZMQ_DONTWAIT) != -1) {   // Non-blocking to wait to receive a message

      // Step 3: Decode the message
          try {
              data = openephysflatbuffer::GetContinuousData(zmq_msg_data(&message));
          } catch (...) {
              std::cout << "Impossible to parse the packet received - skipping to the next." << std::endl;
              continue;
          }


          std::cout << "Received a packet numero " << data->message_id()
                    << " (OE packet sample_num = " << data->sample_num()
                    << ") with " << data->n_samples()
                    << " samples from " << data->n_channels() << " channels." << std::endl;

          // Step 4: Process your data: [sample0/chan0, sample1/chan0, ..., sampleN/chan0, sample0/chan1, sample1/chan1...]

          auto data_in_start_iter = data->samples()->begin();
          auto data_start_channel2 = data->samples()->begin() + 2* data->n_samples();
          // process



       }



    }




}