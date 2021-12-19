
/*
 * Clearly Broken Software
 * Copyright (C) 2021 Rob van den Berg <rghvdberg@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"
#include "MiniBpm.h"
#include <rubberband/RubberBandStretcher.h>
#include <sndfile.hh>
#include <memory>
#include <vector>
#include <iostream>

using namespace RubberBand;

START_NAMESPACE_DISTRHO

class Stretcher : public Plugin
{
public:
  Stretcher()
      : Plugin(kParameterCount, 0, 1),
        timeRatio(1.0f),
        estimatedTempo(0.0f),
        sampleLoaded(false),
        phase(0)
  {
    // setup Rubberband Stretcher
    stretcher.reset(new RubberBandStretcher(getSampleRate(), 2,
                                            RubberBandStretcher::OptionProcessRealTime));
    stretcher->setTimeRatio(timeRatio);

    // init buffer
    bufferSize = getBufferSize();
    stretcher_inputs.resize(2);
    stretcher_outputs.resize(2);
    stretcher_inL.resize(bufferSize * 2);
    stretcher_inR.resize(bufferSize * 2);
    stretcher_outL.resize(bufferSize * 2);
    stretcher_outR.resize(bufferSize * 2);
    stretcher_inputs[0] = stretcher_inL.data();
    stretcher_inputs[1] = stretcher_inR.data();
    stretcher_outputs[0] = stretcher_outL.data();
    stretcher_outputs[1] = stretcher_outR.data();
  }

protected:
  const char *getLabel() const override
  {
    return "Stretcher";
  }

  const char *getDescription() const override
  {
    return "POC Rubberband stretcher";
  }

  const char *getMaker() const override
  {
    return "Clearly Broken Software";
  }

  const char *getHomePage() const override
  {
    return "http://github.com/clearly-broken-software/stretcher";
  }

  const char *getLicense() const override
  {
    return "ISC";
  }

  uint32_t getVersion() const override
  {
    return d_version(1, 0, 0);
  }

  int64_t getUniqueId() const override
  {
    return d_cconst('s', 'T', 'r', 'E');
  }

  void initState(uint32_t index, String &stateKey, String &defaultStateValue) override
  {
    stateKey = "file";
    defaultStateValue = "";
  }

  bool isStateFile(uint32_t index) override
  {
    return true;
  }

  void initParameter(uint32_t index, Parameter &parameter) override
  {
    switch (index)
    {
    case kTimeRatio:
      parameter.name = "Time Ratio";
      parameter.symbol = "timeratio";
      parameter.hints = kParameterIsAutomable;
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = 0.5f;
      parameter.ranges.max = 2.0f;
      break;

    case kEstimatedTempo:
      parameter.name = "Estimated Tempo";
      parameter.symbol = "estimatedtempo";
      parameter.hints = kParameterIsOutput;
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1000.f;

    default:
      break;
    }
  }

  float getParameterValue(uint32_t index) const override
  {
    switch (index)
    {
    case kTimeRatio:
      return timeRatio;
      break;
    case kEstimatedTempo:
      return estimatedTempo;
    default:
      break;
    }
    return 0.0f;
  }

  void setParameterValue(uint32_t index, float value) override
  {
    switch (index)
    {
    case kTimeRatio:
      timeRatio = value;
      stretcher->setTimeRatio(timeRatio);
      break;

    default:
      break;
    }
  }

  void setState(const char *key, const char *value) override
  {

    if (strcmp(key, "file") == 0)
    {
      SndfileHandle fileHandle(value);
      sampleFrames = fileHandle.frames();
      const auto channels = fileHandle.channels();
      dataSize = sampleFrames * channels;
      if (dataSize)
      {
        sampleData.clear();
        sampleData.resize(dataSize);
        fileHandle.read(&sampleData.at(0), dataSize);
        estimatedTempo = getEstimatedTempo(channels);
        printf("Estimated Tempo %f\n", estimatedTempo);
        sampleLoaded = true;
      }
    }
  }

  void run(const float **, float **outputs, uint32_t frames) override
  {
    if (!sampleLoaded)
    {
      for (int i = 0; i < frames; i++)
      {
        outputs[0][i] = 0.0f;
        outputs[1][i] = 0.0f;
      }
      return;
    }

    int retrieveOffset = 0;
    int actualRetrieved = 0;
    // stretcher->setMaxProcessSize(frames);
    // TODO impliment latency
    // const int latency = stretcher->getLatency();
    while (retrieveOffset < frames)
    {
      // fill buffer
      auto samplesRequired = stretcher->getSamplesRequired();
      if (samplesRequired)
      {
        for (int i = 0; i < samplesRequired; i++)
        {
          stretcher_inL[i] = sampleData[phase];
          phase++;
          stretcher_inR[i] = sampleData[phase];
          phase++;
          if (phase >= dataSize)
            phase = 0;
        }
      }

      stretcher->process(stretcher_inputs.data(), samplesRequired, false);
      framesAvailable = stretcher->available();
      if (framesAvailable > 0)
      {
        const int framesToGet = std::min(frames - retrieveOffset, (uint32_t)framesAvailable);
        actualRetrieved = stretcher->retrieve(stretcher_outputs.data(), framesToGet);
        for (int i = 0; i < actualRetrieved; i++)
        {
          if (i + retrieveOffset > frames - 1)
            break;
          outputs[0][i + retrieveOffset] = stretcher_outL[i];
          outputs[1][i + retrieveOffset] = stretcher_outR[i];
        }
        retrieveOffset += actualRetrieved;
      }
    }
  }

  // void  bufferSizeChanged(uint32_t newBufferSize) override
  // {
  // }

private:
  // functions
  double getEstimatedTempo(int channels)
  {
    // init minibpm
    minibpm.reset(new breakfastquay::MiniBPM(getSampleRate()));
    if (channels = 2)
    {
      std::vector<float> monoSample(sampleFrames);
      for (sf_count_t i = 0, j = 0; i < sampleFrames; i++)
      {
        monoSample[i] = (sampleData[j] + sampleData[j + 1]) * .5f;
        j += 2;
      }
      return minibpm->estimateTempoOfSamples(monoSample.data(), sampleFrames);
    }
    // mono sample
    return minibpm->estimateTempoOfSamples(sampleData.data(), sampleFrames);
  }

  // Parameters
  float timeRatio;
  float estimatedTempo;
  // members
  std::unique_ptr<RubberBand::RubberBandStretcher> stretcher;
  std::unique_ptr<breakfastquay::MiniBPM> minibpm;
  std::vector<float> sampleData;
  sf_count_t phase;
  sf_count_t dataSize;
  sf_count_t sampleFrames;
  uint32_t bufferSize;
  size_t samplesRequired;
  std::vector<float *> stretcher_inputs;
  std::vector<float *> stretcher_outputs;
  std::vector<float> stretcher_inL;
  std::vector<float> stretcher_inR;
  std::vector<float> stretcher_outL;
  std::vector<float> stretcher_outR;
  int framesAvailable;
  bool sampleLoaded;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stretcher)
};

Plugin *createPlugin()
{
  return new Stretcher();
}

END_NAMESPACE_DISTRHO
