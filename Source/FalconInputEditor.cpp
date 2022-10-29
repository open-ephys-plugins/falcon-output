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

#include "FalconInputEditor.h"
#include "FalconInput.h"

#include <string>
#include <iostream>

FalconInputEditor::FalconInputEditor(GenericProcessor* parentNode, FalconInput *socket) : GenericEditor(parentNode)
{
    node = socket;

    desiredWidth = 240;

    // Add connect button
    connectButton = new UtilityButton("CONNECT", Font("Small Text", 12, Font::bold));
    connectButton->setRadius(3.0f);
    connectButton->setBounds(10, 35, 70, 20);
    connectButton->addListener(this);
    addAndMakeVisible(connectButton);

    // Port
    portLabel = new Label("Port", "Port");
    portLabel->setFont(Font("Small Text", 10, Font::plain));
    portLabel->setBounds(5, 60, 65, 8);
    portLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(portLabel);

    portInput = new Label("Port", String(node->port));
    portInput->setFont(Font("Small Text", 10, Font::plain));
    portInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    portInput->setEditable(true);
    portInput->addListener(this);
    portInput->setBounds(10, 70, 65, 15);
    addAndMakeVisible(portInput);

    //---
    bufferSizeMainLabel = new Label("BUFFER SIZE", "BUFFER SIZE");
    bufferSizeMainLabel->setFont(Font("Small Text", 12, Font::plain));
    bufferSizeMainLabel->setBounds(114, 30, 95, 15);
    bufferSizeMainLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(bufferSizeMainLabel);

    // Num chans
    channelCountLabel = new Label("CHANNELS", "CHANNELS");
    channelCountLabel->setFont(Font("Small Text", 10, Font::plain));
    channelCountLabel->setBounds(92, 48, 65, 8);
    channelCountLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(channelCountLabel);

    channelCountInput = new Label("Channel count", String(node->num_channels));
    channelCountInput->setFont(Font("Small Text", 10, Font::plain));
    channelCountInput->setBounds(100, 60, 50, 15);
    channelCountInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    channelCountInput->setEditable(true);
    channelCountInput->addListener(this);
    addAndMakeVisible(channelCountInput);

    // Fs
    sampleRateLabel = new Label("FREQ (HZ)", "FREQ (HZ)");
    sampleRateLabel->setFont(Font("Small Text", 10, Font::plain));
    sampleRateLabel->setBounds(5, 92, 85, 8);
    sampleRateLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(sampleRateLabel);

    sampleRateInput = new Label("Fs (Hz)", String((int) node->sample_rate));
    sampleRateInput->setFont(Font("Small Text", 10, Font::plain));
    sampleRateInput->setBounds(10, 105, 65, 15);
    sampleRateInput->setEditable(true);
    sampleRateInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    sampleRateInput->addListener(this);
    addAndMakeVisible(sampleRateInput);

}

void FalconInputEditor::labelTextChanged(Label* label)
{

    if (label == channelCountInput)
    {

        std::cout << "Label text changed" << std::endl;

        int num_channels = channelCountInput->getText().getIntValue();

        if (num_channels > 0 && num_channels < 1000)
        {
            node->num_channels = num_channels;
            CoreServices::updateSignalChain(this);
        }
        else {
            channelCountInput->setText(String(node->num_channels), dontSendNotification);
        }
        
    }
    else if (label == sampleRateInput)
    {
        float sampleRate = sampleRateInput->getText().getFloatValue();

        if (sampleRate > 0 && sampleRate < 50000.0f)
        {
            node->sample_rate = sampleRate;
            CoreServices::updateSignalChain(this);
        }
        else {
            sampleRateInput->setText(String(node->sample_rate), dontSendNotification);
        }
        
    }
    else if (label == portInput)
    {
        int port = portInput->getText().getIntValue();

        if (port > 1023 && port < 65535)
        {
            node->port = port;
        }
        else {
            portInput->setText(String(node->port), dontSendNotification);
        }
    }


}

void FalconInputEditor::startAcquisition()
{
    // Disable the whole gui
    portInput->setEnabled(false);
    channelCountInput->setEnabled(false);
    sampleRateInput->setEnabled(false);
    connectButton->setEnabled(false);

    node->resizeChanSamp();
}

void FalconInputEditor::stopAcquisition()
{
    // Reenable the whole gui
    portInput->setEnabled(true);
    channelCountInput->setEnabled(true);
    sampleRateInput->setEnabled(true);
    connectButton->setEnabled(true);
}

void FalconInputEditor::buttonClicked(Button* button)
{

    if (button == connectButton)
    {
        node->port = portInput->getText().getIntValue();
        node->tryToConnect();
    }
  
}

void FalconInputEditor::saveCustomParametersToXml(XmlElement* xmlNode)
{
    XmlElement* parameters = xmlNode->createNewChildElement("PARAMETERS");

    parameters->setAttribute("port", portInput->getText());
    parameters->setAttribute("numchan", channelCountInput->getText());
    parameters->setAttribute("fs", sampleRateInput->getText());
}

void FalconInputEditor::loadCustomParametersFromXml(XmlElement* xmlNode)
{
    forEachXmlChildElement(*xmlNode, subNode)
    {
        if (subNode->hasTagName("PARAMETERS"))
        {
            portInput->setText(subNode->getStringAttribute("port", ""), dontSendNotification);
            node->port = subNode->getIntAttribute("port", DEFAULT_PORT);

            channelCountInput->setText(subNode->getStringAttribute("numchan", ""), dontSendNotification);
            node->num_channels = subNode->getIntAttribute("numchan", DEFAULT_NUM_CHANNELS);

            sampleRateInput->setText(subNode->getStringAttribute("fs", ""), dontSendNotification);
            node->sample_rate = subNode->getDoubleAttribute("fs", DEFAULT_SAMPLE_RATE);

        }
    }
}

