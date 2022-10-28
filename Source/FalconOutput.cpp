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


#include "FalconOutput.h"

FalconOutput::FalconOutput()
    : GenericProcessor("Falcon Output"),
      flatBuilder(1024),
      selectedStream(0)
{
    context = zmq_ctx_new();
    socket = 0;
    flag = 0;
    messageNumber = 0;
    port = 3335;

    if (!socket)
        createSocket();

    addMaskChannelsParameter(Parameter::STREAM_SCOPE, "Channels", "The input channel data to send", true);

    addIntParameter(Parameter::GLOBAL_SCOPE, "data_port", "Port number to send data", port, 1000, 65535, true);

}

FalconOutput::~FalconOutput()
{
    closeSocket();
    if (context)
    {
        zmq_ctx_destroy(context);
        context = 0;
    }
}

void FalconOutput::createSocket()
{
    if (!socket)
    {
        socket = zmq_socket(context, ZMQ_PUB);

        if (!socket)
        {
            LOGD("Couldn't create a socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }

        auto urlstring = "tcp://*:" + std::to_string(port);

        if (zmq_bind(socket, urlstring.c_str()))
        {
            LOGD("Couldn't open data socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }
    }
}

void FalconOutput::closeSocket()
{
    if (socket)
    {
        LOGD("Closing data socket");
        zmq_close(socket);
        socket = 0;
    }
}

void FalconOutput::sendData(const float **bufferChanPtrs,
                            int nChannels, int nSamples,
                            int64 sampleNumber, int sampleRate)
{
    
    messageNumber++;

    // Create message
    auto streamName = flatBuilder.CreateString(getDataStream(selectedStream)->getName().toStdString());
    auto samples = flatBuilder.CreateVector(*(bufferChanPtrs), nChannels*nSamples);
    auto zmqBuffer = openephysflatbuffer::CreateContinuousData(flatBuilder, samples, streamName,
                                                               nChannels, nSamples, sampleNumber,
                                                               messageNumber, sampleRate);
    flatBuilder.Finish(zmqBuffer);

    uint8_t *buf = flatBuilder.GetBufferPointer();
    int size = flatBuilder.GetSize();

    // Send packet
    zmq_msg_t request;
    zmq_msg_init_size(&request, size);
    memcpy(zmq_msg_data(&request), (void *)buf, size);
    int size_m = zmq_msg_send(&request, socket, 0);
    zmq_msg_close(&request);

    flatBuilder.Clear();
}

AudioProcessorEditor* FalconOutput::createEditor()
{
    editor = std::make_unique<FalconOutputEditor>(this);
    return editor.get();
}

void FalconOutput::updateSettings()
{
    FalconOutputEditor * ed = (FalconOutputEditor*) getEditor();
    ed->updateStreamSelectorOptions();
}

void FalconOutput::process(AudioBuffer<float>& buffer)
{
    if (!socket)
        createSocket();

    for (auto stream : dataStreams)
    {        
        if ((*stream)["enable_stream"]
            && stream->getStreamId() == selectedStream)
        {
            // Send the sample number of the first sample in the buffer block
            int64 sampleNum = getFirstSampleNumberForBlock(selectedStream) ;
            int numSamples = getNumSamplesInBlock(selectedStream);
            int numChannels = selectedChannels.size();

            if(numSamples == 0)
                continue;

            int i = 0;
            for(auto chan : selectedChannels)
            {
                int globalChanIndex = stream->getContinuousChannels().getUnchecked(chan)->getGlobalIndex();

                bufferPtrs[i] = buffer.getReadPointer(globalChanIndex);

                i++;
            }

            sendData(bufferPtrs, numChannels, numSamples, sampleNum, (int)stream->getSampleRate());
        }
    }
}

void FalconOutput::parameterValueChanged(Parameter* param)
{
    if (param->getName().equalsIgnoreCase("Channels"))
    {   
        if(param->getStreamId() == selectedStream)
            selectedChannels = static_cast<MaskChannelsParameter*>(param)->getArrayValue();
    }
    else if (param->getName().equalsIgnoreCase("data_port"))
    {
        int dataPort = static_cast<IntParameter*>(param)->getIntValue();
        setPort(dataPort);
    }
}

void FalconOutput::setSelectedStream(int idx)
{
    selectedStream = idx;
}

void FalconOutput::setPort(uint32_t new_port)
{
    port = new_port;
    closeSocket();
    createSocket();
}

