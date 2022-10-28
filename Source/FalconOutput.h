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

#ifndef FALCONOUTPUT_H_INCLUDED
#define FALCONOUTPUT_H_INCLUDED

#include <ProcessorHeaders.h>
#include <queue>
#include <zmq.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <chrono>
#include <errno.h>

#include "FalconOutputEditor.h"
#include "flatbuffers/flatbuffers.h"
#include "channel_generated.h"

class FalconOutput: public GenericProcessor
{
public:
    FalconOutput();
    ~FalconOutput();

    /** Streams incoming data over a ZMQ socket */
    void process(AudioBuffer<float>& continuousBuffer) override;

    /** Called whenever the settings of upstream plugins have changed */
    void updateSettings() override;

    /** Called when a parameter is updated*/
    void parameterValueChanged(Parameter* param) override;

    AudioProcessorEditor* createEditor();

    /** Updates the output stream*/
	void setSelectedStream(int idx);

private:

    void createSocket();
    void closeSocket();

    void setPort(uint32_t new_port);

    void sendData(const float **bufferChanPtrs,
                  int nChannels, int nSamples,
                  int64 sampleNumber, int sampleRate);

    void *context;
    void *socket;

    uint16 selectedStream;

    int flag;
    int messageNumber;
    uint32_t port;
    flatbuffers::FlatBufferBuilder flatBuilder;

    Array<int> selectedChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FalconOutput);

};



#endif  // FALCONOUTPUT_H_INCLUDED
