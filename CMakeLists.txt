cmake_minimum_required(VERSION 3.22)
project(SWATronix2A VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.0
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(${PROJECT_NAME}
  PRODUCT_NAME "SWA Tronix 2A"
  COMPANY_NAME "StreetWave Audio"
  BUNDLE_ID "com.streetwaveaudio.swatronix2a"
  FORMATS VST3
  VERSION "1.0.0"
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_SYNTH FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  SOURCES
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/La2aEmulation.cpp
    src/Themes.cpp
    src/Presets.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_gui_basics
  juce::juce_graphics
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
)
