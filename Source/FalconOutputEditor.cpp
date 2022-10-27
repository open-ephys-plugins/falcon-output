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


FalconOutputEditor::FalconOutputEditor(GenericProcessor *parentNode): GenericEditor(parentNode)
{
    falconProcessor = (FalconOutput*)parentNode;

    desiredWidth = 200;

	streamSelection = std::make_unique<ComboBox>("Stream Selector");
    streamSelection->setBounds(30, 40, 140, 20);
    streamSelection->setTooltip("Output stream");
    streamSelection->addListener(this);
    addAndMakeVisible(streamSelection.get());

    addMaskChannelsParameterEditor("channels", 15, 90);

    addTextBoxParameterEditor("data_port", 110, 70);

}

FalconOutputEditor::~FalconOutputEditor()
{

}

void FalconOutputEditor::comboBoxChanged(ComboBox* cb)
{
    if (cb == streamSelection.get())
    {
        setOutputStream(cb->getSelectedId());
    }
}
	
// void FalconOutputEditor::buttonClicked(Button* button)
// {
//     if (button == portButton)
//     {
//         String dport = portEditor->getText();
//         int dportVal = dport.getIntValue();
//         if ( (dportVal == 0) && !dport.containsOnly("0")) {
//             // wrong integer input
//             CoreServices::sendStatusMessage("Invalid data port value");
//             portEditor->setText(std::to_string(StreamProcessor->getPort()));
//         }else {
//             StreamProcessor->setPort(dportVal);
//             CoreServices::sendStatusMessage("ZMQ port updated");
//         }
//     }
// }

void FalconOutputEditor::startAcquisition()
{

}


void FalconOutputEditor::stopAcquisition()
{

}


void FalconOutputEditor::updateStreamSelectorOptions()
{
    bool needsUpdate = false;
	int subprocessorToSet = streamSelection->getSelectedId();

	for (auto stream: falconProcessor->getDataStreams())
	{
		if(!inputStreamIds.contains(stream->getStreamId()))
		{
			needsUpdate = true;
			break;
		}
	}

	if(falconProcessor->getNumDataStreams() != inputStreamIds.size())
		needsUpdate = true;

	if(needsUpdate || subprocessorToSet == 0)
	{	
		inputStreamIds.clear();
		streamSelection->clear(dontSendNotification);

		// Add all datastreams to combobox
		for (auto stream: falconProcessor->getDataStreams())
		{
			int streamID = stream->getStreamId();

			inputStreamIds.add(streamID);
			streamSelection->addItem("[" + String(stream->getSourceNodeId()) + "] " +
									 stream->getName(), streamID);
		}

		// Check and select datastream if available
		if (inputStreamIds.size() > 0)
		{
			if(!inputStreamIds.contains(subprocessorToSet))
				subprocessorToSet = inputStreamIds[0];

			streamSelection->setSelectedId(subprocessorToSet, dontSendNotification);
		}
		else
		{
			subprocessorToSet = -1;
		}

		setOutputStream(subprocessorToSet);
	}
}


void FalconOutputEditor::setOutputStream(int index)
{
	if (index > 0)
		falconProcessor->setSelectedStream(index);
	else
		falconProcessor->setSelectedStream(-1);
}


