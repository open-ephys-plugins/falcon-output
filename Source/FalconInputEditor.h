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

#ifndef __FALCONINPUTEDITORH__
#define __FALCONINPUTEDITORH__

#ifdef _WIN32
#include <Windows.h>
#endif

#include <VisualizerEditorHeaders.h>

class FalconInput;

class FalconInputEditor : public GenericEditor, 
                            public Label::Listener,
                            public Button::Listener
{

public:

    /** Constructor */
    FalconInputEditor(GenericProcessor* parentNode, FalconInput *node);

    /** Button listener callback, called by button when pressed. */
    void buttonClicked(Button* button);

    /** Called by processor graph in beginning of the acqusition, disables editor completly. */
    void startAcquisition();

    /** Called by processor graph at the end of the acqusition, reenables editor completly. */
    void stopAcquisition();

    /** Called when configuration is saved. Adds editors config to xml. */
    void saveCustomParametersToXml(XmlElement* xml) override;

    /** Called when configuration is loaded. Reads editors config from xml. */
    void loadCustomParametersFromXml(XmlElement* xml) override;

    /** Called when label is changed */
    void labelTextChanged(Label* label);

private:

    // Button that tried to connect to client
    ScopedPointer<UtilityButton> connectButton;

    // Port
    ScopedPointer<Label> portLabel;
    ScopedPointer<Label> portInput;

    // Buffer size
    ScopedPointer<Label> bufferSizeMainLabel;

    // Chans
    ScopedPointer<Label> channelCountLabel;
    ScopedPointer<Label> channelCountInput;

    // Fs
    ScopedPointer<Label> sampleRateLabel;
    ScopedPointer<Label> sampleRateInput;

    // Parent node
    FalconInput* node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FalconInputEditor);
};


#endif