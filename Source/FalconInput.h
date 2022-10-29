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

#ifndef __FALCONINPUTH__
#define __FALCONINPUTH__

#include <DataThreadHeaders.h>

#include <zmq.h>
#include <iostream>
#include <string>

const int DEFAULT_PORT = 3335;
const float DEFAULT_SAMPLE_RATE = 40000.0f;
const int DEFAULT_NUM_CHANNELS = 16;
const int MAX_NUM_SAMPLES = 10000;
#define MAX_NUM_CHANNELS 384

class FalconInput : public DataThread
{

public:

    /** Constructor */
    FalconInput(SourceNode* sn);

    /** Destructor */
    ~FalconInput();

    // Interface fulfillment
    bool foundInputSource() override;

    void updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
        OwnedArray<EventChannel>* eventChannels,
        OwnedArray<SpikeChannel>* spikeChannels,
        OwnedArray<DataStream>* sourceStreams,
        OwnedArray<DeviceInfo>* devices,
        OwnedArray<ConfigurationObject>* configurationObjects);

    int getNumChannels() const;

    // User defined
    int port;
    float sample_rate;
    float data_scale;
    uint16_t data_offset;
    bool transpose = true;
    int num_samp;
    int num_channels;

    int64 total_samples;
    float relative_sample_rate;

    uint64 eventState;

    void resizeChanSamp();
    void tryToConnect();
    void closeConnection();

    std::unique_ptr<GenericEditor> createEditor(SourceNode* sn);
    static DataThread* createDataThread(SourceNode* sn);

private:

    bool updateBuffer() override;
    bool startAcquisition() override;
    bool stopAcquisition()  override;

    bool connected = false;

    void* socket;
    void* context;

    zmq_msg_t message;
    
    Array<int64> sampleNumbers;
    Array<double> timestamps;
    Array<uint64> ttlEventWords;

    float samples[MAX_NUM_SAMPLES * MAX_NUM_CHANNELS];
    double timestamp_s[MAX_NUM_SAMPLES];
    uint64 event_codes[MAX_NUM_SAMPLES];
    int64 sample_numbers[MAX_NUM_SAMPLES];


    int64 currentTimestamp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FalconInput);
};

#endif