/*
 ------------------------------------------------------------------
 StreamPlugin
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

#ifndef STREAMPLUGIN_H_INCLUDED
#define STREAMPLUGIN_H_INCLUDED

#include <ProcessorHeaders.h>
#include <queue>
#include <zmq.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include <errno.h>

#include "StreamPluginEditor.h"
#include "flatbuffers/flatbuffers.h"
#include "channel_generated.h"

class StreamPlugin: public GenericProcessor
{
public:
    StreamPlugin();
    ~StreamPlugin();

    virtual bool isSource(){ return false;};
    virtual bool isSink(){return false;}

    /** Defines the functionality of the processor.

         The process method is called every time a new data buffer is available.

         Processors can either use this method to add new data, manipulate existing
         data, or send data to an external target (such as a display or other hardware).

         Continuous signals arrive in the "buffer" variable, event data (such as TTLs
         and spikes) is contained in the "events" variable, and "nSamples" holds the
         number of continous samples in the current buffer (which may differ from the
         size of the buffer).
         */

    virtual void process(AudioSampleBuffer& continuousBuffer);

    AudioProcessorEditor* createEditor();
    bool hasEditor() const {return true;};
    bool isReady() const {return true;};

    void setPort(uint32_t new_port){
        port = new_port;
        closeSocket();
        createSocket();
    }
    uint32_t getPort () const { return port; }

private:

    void createSocket();
    void closeSocket();

    void sendData(AudioSampleBuffer& continuousBuffer, int nSamples,
                  uint64 timestamp, int sampleRate);

    void *context;
    void *socket;

    int flag;
    int messageNumber;
    uint32_t port;
    flatbuffers::FlatBufferBuilder flatBuilder;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamPlugin);

};



#endif  // STREAMPLUGIN_H_INCLUDED
