# CMake file for DISTRHO Plugins # ------------------------------ #
cmake_minimum_required(VERSION 3.16)
project(Stretcher)

add_subdirectory(DPF)

dpf_add_plugin(Stretcher
  TARGETS jack lv2 vst2

  FILES_DSP
  MiniBpm.cpp
  StretcherPlugin.cpp
  
  FILES_UI
  StretcherUI.cpp
  NanoButton.cpp
  ExtraEventHandlers.cpp
  NanoSlider.cpp
  )

target_link_libraries(Stretcher PUBLIC -lsndfile -lrubberband)

target_include_directories(Stretcher PUBLIC ".")
