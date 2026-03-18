cmake_minimum_required(VERSION 3.22)

project(FreeSampler VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Pull JUCE at configure time so the repo stays lightweight for GitHub.
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(FreeSampler
    COMPANY_NAME "Tiz Wildin Entertainment"
    BUNDLE_ID com.garydoman.freesampler
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE TWEG
    PLUGIN_CODE FrSm
    FORMATS VST3 Standalone AU
    PRODUCT_NAME "FreeSampler")

juce_generate_juce_header(FreeSampler)

target_sources(FreeSampler PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SamplerSound.h
    Source/SamplerVoice.h)

target_compile_definitions(FreeSampler PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(FreeSampler PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
