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

#include "FalconOutputEditor.h"
#include "FalconOutput.h"

FalconOutputEditor::FalconOutputEditor (GenericProcessor* parentNode) : GenericEditor (parentNode)
{
    falconProcessor = (FalconOutput*) parentNode;

    desiredWidth = 190;

    addSelectedStreamParameterEditor (Parameter::PROCESSOR_SCOPE, "stream", 15, 34);
    addMaskChannelsParameterEditor (Parameter::STREAM_SCOPE, "channels", 15, 58);
    addTextBoxParameterEditor (Parameter::PROCESSOR_SCOPE, "data_port", 15, 82);

    ipAddressLabel = std::make_unique<Label> ("ipAddress", "Address");
    ipAddressLabel->setBounds (123, 105, 55, 20);
    ipAddressLabel->setFont (FontOptions ("Inter", "Regular", 13));
    addAndMakeVisible (ipAddressLabel.get());

    ipAddress = std::make_unique<Label> ("ipAddress", IPAddress::getLocalAddress().toString());
    ipAddress->setBounds (15, 105, 180, 20);
    addAndMakeVisible (ipAddress.get());

    for (auto ed : parameterEditors)
    {
        ed->setSize (210, 18);
    }
}

FalconOutputEditor::~FalconOutputEditor()
{
}
