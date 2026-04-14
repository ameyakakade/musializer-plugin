#include "PluginProcessor.h"
#include "PluginEditor.h"

int height = 800;
int width  = 800;

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), sh(p)
{


    juce::ignoreUnused (processorRef);
    setSize (width, height);
    setResizable(true, true);
    startTimerHz(60);
    size = processorRef.fft_analyze(0.016f);
    addAndMakeVisible(sh);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //
    // float m = processorRef.fft_analyze(0.016f);
    // int step = width/m;
    // g.setColour (juce::Colours::white);
    // for(size_t i=0; i<m-1; i++)
    // {
    //     float t = processorRef.out_smooth[i];
    //     g.fillRect((float)(i*step) , 0.0, (float)step/2, t*2/3*height);
    // }
}
void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}

void AudioPluginAudioProcessorEditor::resized()
{
    sh.setBounds(getLocalBounds());
}

shaderC::shaderC(AudioPluginAudioProcessor& p) : processorRef(p)
{
    fragmentSource =
#import "Shader.frag"
    setOpaque(true);
    openGLContext.setComponentPaintingEnabled(false);
    openGLContext.setContinuousRepainting(true);
    textureWidth = processorRef.fft_analyze(0.016f); 
}

shaderC::~shaderC()
{

}

void shaderC::resized()
{

}

void shaderC::initialise()
{

    shader.reset(new juce::OpenGLShaderProgram(openGLContext));

    if(shader -> addVertexShader(vertexSource) && shader -> addFragmentShader(fragmentSource) && shader -> link())
    {
        DBG(shader->getLastError());
    }

    float vertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };


    openGLContext.extensions.glGenVertexArrays(1, &VAO);

    openGLContext.extensions.glGenBuffers(1, &VBO);

    openGLContext.extensions.glBindVertexArray(VAO);

    openGLContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, VBO);

    openGLContext.extensions.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(vertices), vertices, juce::gl::GL_STATIC_DRAW);

    openGLContext.extensions.glVertexAttribPointer(0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 3 * sizeof(float), 0);
    openGLContext.extensions.glEnableVertexAttribArray(0);

    juce::gl::glGenTextures(1, &dataTextureID);
    juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, dataTextureID);


    // Setup scaling filters (Linear will interpolate between samples for a smooth look)
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MIN_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MAG_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_S, juce::gl::GL_CLAMP_TO_EDGE);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_T, juce::gl::GL_CLAMP_TO_EDGE);

    // Initialize with NULL (empty container)
    // GL_R32F is the "Native Float" format for modern GPUs
    juce::gl::glTexImage2D(juce::gl::GL_TEXTURE_2D, 0, juce::gl::GL_R32F, textureWidth, 2, 0, juce::gl::GL_RED, juce::gl::GL_FLOAT, nullptr);

}

void shaderC::render()
{

    // Clear background
    juce::OpenGLHelpers::clear(juce::Colour(0xddddddff));

    float m = processorRef.fft_analyze(0.016f);
    

    // 2. Bind and Upload
    openGLContext.extensions.glActiveTexture(juce::gl::GL_TEXTURE0);
    juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, dataTextureID);


    // glTexSubImage2D is much faster than glTexImage2D because it doesn't re-allocate
    juce::gl::glTexSubImage2D(juce::gl::GL_TEXTURE_2D, 0, 0, 0, textureWidth, 1, juce::gl::GL_LUMINANCE, juce::gl::GL_FLOAT, processorRef.out_smooth);

    juce::gl::glTexSubImage2D(juce::gl::GL_TEXTURE_2D, 0, 0, 1, textureWidth, 1, juce::gl::GL_LUMINANCE, juce::gl::GL_FLOAT, processorRef.out_smear);

    // 3. Link to the shader
    shader->setUniform("uDataTexture", 0); // Use Texture Unit 0

    float scale = openGLContext.getRenderingScale();
    shader->setUniform("uRes", (float)getWidth()*scale, (float)getHeight()*scale);

    // Pass uniforms (like time)
    shader->setUniform("steps", m);

    shader->use();

    // Draw the quad
    openGLContext.extensions.glBindVertexArray(VAO);
    juce::gl::glDrawArrays(juce::gl::GL_TRIANGLES, 0, 6);

    
}

void shaderC::shutdown()
{
    shader.reset();
    openGLContext.extensions.glDeleteVertexArrays(1, &VAO);
    openGLContext.extensions.glDeleteBuffers(1, &VBO);
}
