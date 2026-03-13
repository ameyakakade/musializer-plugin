#pragma once

#include "PluginProcessor.h"
#include <juce_opengl/juce_opengl.h>

class shaderC : public juce::OpenGLAppComponent
{
public:
    shaderC(AudioPluginAudioProcessor& p);
    ~shaderC() override;

    //==============================================================================
    // void paint (juce::Graphics&) override;
    void resized() override;
    void initialise() override; 
    void shutdown() override; 
    void render() override;

private:

    AudioPluginAudioProcessor& processorRef;

    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    GLuint VBO, VAO;
    GLuint dataTextureID = 0;
    int textureWidth;
    float startTime = juce::Time::getMillisecondCounterHiRes();

    juce::String vertexSource = 
        R"glsl(#version 120
    attribute vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
    )glsl" ;

    juce::String fragmentSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (shaderC)
};

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    AudioPluginAudioProcessor& processorRef;

    shaderC sh;

    size_t size = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};

