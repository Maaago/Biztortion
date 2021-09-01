/*
  ==============================================================================

    GUIModule.cpp
    Created: 30 Aug 2021 10:59:59am
    Author:  gabri

  ==============================================================================
*/

#include "GUIModule.h"

GUIModule::GUIModule(unsigned int gp)
    : gridPosition(gp)
{

}

unsigned int GUIModule::getGridPosition()
{
    return gridPosition;
}

void GUIModule::setGridPosition(unsigned int gp)
{
    gridPosition = gp;
}

void GUIModule::drawContainer(juce::Graphics& g)
{
    // container margin
    g.setColour(juce::Colours::orange);
    g.drawRoundedRectangle(getContainerArea().toFloat(), 4.f, 1.f);
    // content margin
    g.setColour(juce::Colours::yellow);
    g.drawRoundedRectangle(getContentRenderArea().toFloat(), 4.f, 1.f);
}

juce::Rectangle<int> GUIModule::getContainerArea()
{
    // returns a dimesion reduced rectangle as bounds in order to avoid margin collisions
    auto bounds = getLocalBounds();

    /*bounds.reduce(JUCE_LIVE_CONSTANT(5), 
        JUCE_LIVE_CONSTANT(5)
        );*/
    bounds.reduce(10, 10);

    return bounds;
}

juce::Rectangle<int> GUIModule::getContentRenderArea()
{
    auto bounds = getContainerArea();

    /*bounds.reduce(JUCE_LIVE_CONSTANT(5),
        JUCE_LIVE_CONSTANT(5)
    );*/
    bounds.reduce(5, 5);

    return bounds;
}

