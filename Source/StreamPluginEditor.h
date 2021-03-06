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

#ifndef STREAMPLUGINEDITOR_H_INCLUDED
#define STREAMPLUGINEDITOR_H_INCLUDED

#include <EditorHeaders.h>

class StreamPlugin;

struct StreamApplication;

class StreamPluginEditor: public GenericEditor
{
public:
    StreamPluginEditor(GenericProcessor *parentNode, bool useDefaultParameters);
    virtual ~StreamPluginEditor();
    void setPort(uint32_t port, void callback());
    void buttonClicked(Button* button) override;
	void startAcquisition() override;
	void stopAcquisition()  override;

private:
    TextEditor *portEditor;
    TextButton *portButton;
    StreamPlugin *StreamProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StreamPluginEditor)

    
};

#endif  // STREAMPLUGINEDITOR_H_INCLUDED
