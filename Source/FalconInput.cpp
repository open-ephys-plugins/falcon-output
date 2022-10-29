/*
 ------------------------------------------------------------------
 FalconInput
 Copyright (C) 2021 - present Neuro-Electronics Research Flanders

 This file is part of the Open Ephys GUI
 Copyright (C) 2022 Open Ephys
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


#ifdef _WIN32
#include <Windows.h>
#endif


#include "channel_generated.h"
#include "flatbuffers/flatbuffers.h"


#include "FalconInput.h"
#include "FalconInputEditor.h"


DataThread* FalconInput::createDataThread(SourceNode *sn)
{
    return new FalconInput(sn);
}


FalconInput::FalconInput(SourceNode* sn) : DataThread(sn),
    port(DEFAULT_PORT),
    num_channels(DEFAULT_NUM_CHANNELS),
    sample_rate(DEFAULT_SAMPLE_RATE),
    socket(nullptr),
    context(nullptr)
{
    sourceBuffers.add(new DataBuffer(num_channels, MAX_NUM_SAMPLES)); // start with 16 channels and automatically resize

    tryToConnect();

    zmq_msg_init(&message);
}

std::unique_ptr<GenericEditor> FalconInput::createEditor(SourceNode* sn)
{

    std::unique_ptr<FalconInputEditor> editor = std::make_unique<FalconInputEditor>(sn, this);

    return editor;
}



FalconInput::~FalconInput()
{
    closeConnection();
}

void FalconInput::resizeChanSamp()
{
    sourceBuffers[0]->resize(num_channels, MAX_NUM_SAMPLES);
}

int FalconInput::getNumChannels() const
{
    return num_channels;
}

void FalconInput::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
    OwnedArray<EventChannel>* eventChannels,
    OwnedArray<SpikeChannel>* spikeChannels,
    OwnedArray<DataStream>* sourceStreams,
    OwnedArray<DeviceInfo>* devices,
    OwnedArray<ConfigurationObject>* configurationObjects)
{

    continuousChannels->clear();
    eventChannels->clear();
    devices->clear();
    spikeChannels->clear();
    configurationObjects->clear();
    sourceStreams->clear();

    DataStream::Settings settings
    {
        "FalconInputStream",
        "description",
        "identifier",

        sample_rate

    };

    sourceStreams->add(new DataStream(settings));
    sourceBuffers[0]->resize(num_channels, 10000);

    for (int ch = 0; ch < num_channels; ch++)
    {

        ContinuousChannel::Settings settings{
            ContinuousChannel::Type::ELECTRODE,
            "CH" + String(ch + 1),
            "description",
            "identifier",

            0.195,

            sourceStreams->getFirst()
        };

        continuousChannels->add(new ContinuousChannel(settings));
    }

    EventChannel::Settings eventSettings{
           EventChannel::Type::TTL,
           "Events",
           "description",
           "identifier",
           sourceStreams->getFirst(),
           1
    };

    eventChannels->add(new EventChannel(eventSettings));

}

bool FalconInput::foundInputSource()
{
    return connected;
}

bool FalconInput::startAcquisition()
{
    resizeChanSamp();

    total_samples = 0;

    startThread();

    return true;
}

void FalconInput::closeConnection()
{
    if (socket)
    {
        LOGD("Closing data socket");
        zmq_close(socket);
        socket = nullptr;
    }

    if (context)
    {
        zmq_ctx_destroy(context);
        context = nullptr;
    }
}

void  FalconInput::tryToConnect()
{

    closeConnection();

    // Create your ZMQ socket
    context = zmq_ctx_new();
    auto tcp_address = "tcp://" + address + ":" + std::to_string(port);
    socket = zmq_socket(context, ZMQ_SUB);
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, nullptr, 0);
    zmq_connect(socket, tcp_address.c_str());

    connected = true;
}

bool FalconInput::stopAcquisition()
{
    if (isThreadRunning())
    {
        signalThreadShouldExit();
    }

    waitForThreadToExit(500);

    sourceBuffers[0]->clear();
    return true;
}

bool FalconInput::updateBuffer()
{
   
    const openephysflatbuffer::ContinuousData* data;

    if (zmq_msg_recv(&message, socket, ZMQ_DONTWAIT) != -1)  // Non-blocking to wait to receive a message
    {

        try {
            data = openephysflatbuffer::GetContinuousData(zmq_msg_data(&message));
        }
        catch (...) {
            std::cout << "Impossible to parse the packet received - skipping to the next." << std::endl;
            return true;
        }

        /*std::cout << "Received packet number: " << data->message_id()
            << ", Stream: " << data->stream()->c_str()
            << ", Sample_Number: " << data->sample_num()
            << ", Samples: " << data->n_samples()
            << ", Channels: " << data->n_channels() << std::endl;*/

        const int num_samples = data->n_samples();
        
        for (int ch = 0; ch < num_channels; ch++)
        {

            const flatbuffers::Vector<float>* d = data->samples() + ch;
            
            int offset = 0;

            int zero_values = 0;

            for (int i = 0; i < num_samples; i++)
            {
                if (offset < d->size())
                {
                    samples[num_channels * i + ch] = d->Get(offset);
                }
                
                offset++;

                if (ch == 1)
                {

                    sample_numbers[i] = total_samples + i;
                    timestamp_s[i] = -1;
                    event_codes[i] = 0;
                    
                }

                if (samples[num_channels * i + ch] == 0)
                    zero_values++;

            }

        }

        sourceBuffers[0]->addToBuffer(samples, sample_numbers, timestamp_s, event_codes, num_samples);

        total_samples += num_samples;
    }

    return true;
}

