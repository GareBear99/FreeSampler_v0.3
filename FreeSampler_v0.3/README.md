# FreeSampler

FreeSampler is a lightweight JUCE virtual instrument built to close the exact gap Kush Audio pointed out:

- sample-based instrument architecture
- MIDI note input
- polyphonic voice allocation
- ADSR envelopes
- root-note pitch mapping
- sample start/end shaping
- looping / one-shot behavior
- velocity response
- gain control
- drag-and-load workflow

This package is intentionally small, clean, and interview-friendly.

## What it does

- Loads a single WAV/AIFF sample into RAM
- Maps that sample chromatically across the keyboard
- Uses JUCE `Synthesiser`, custom `SamplerVoice`, and custom `SamplerSound`
- Supports 16 voices by default
- Exposes ADSR, gain, root note, start, end, and velocity amount as parameters
- Supports loop mode and one-shot mode
- Includes a playable on-screen keyboard
- Stores parameter state and attempts to restore the last sample path when available locally

## Why this exists

This repo is meant to demonstrate direct virtual instrument competency on top of effect-plugin experience like FreeEQ8.

## Core architecture

- `PluginProcessor` handles parameters, sample loading, state, and synth setup
- `SamplerSound` stores the loaded audio buffer and metadata
- `SamplerVoice` handles playback, resampling, ADSR, looping, and note lifecycle
- `PluginEditor` provides a clean UI for loading a sample and shaping playback

## Features included

- Polyphony
- Sample playback
- Root note selection
- Attack / Decay / Sustain / Release
- Master gain
- Sample start / end region
- Velocity scaling
- Loop mode
- One-shot mode
- On-screen keyboard
- Basic state serialization

## Easy next upgrades

- waveform display
- sample reverse
- pitch bend range
- filter / drive section
- modulation matrix
- round-robin groups
- multi-sample layers / velocity zones
- disk streaming for large libraries

## Build

This project uses CMake + JUCE.

### Requirements

- CMake 3.22+
- JUCE 7+
- C++17 compiler
- macOS / Windows

### Example configure

```bash
cmake -B build -S . -DJUCE_DIR=/path/to/JUCE
cmake --build build --config Release
```

Depending on your JUCE setup, you may either:

- use `find_package(JUCE CONFIG REQUIRED)`
- or replace that with `add_subdirectory(path/to/JUCE)`

## Suggested repo pitch

> FreeSampler is a minimal sample-based JUCE instrument demonstrating MIDI handling, voice allocation, sample playback, ADSR shaping, loop/start/end playback control, and root-note pitch mapping.

## Notes

This is an intentionally lean first-instrument repo, not a finished commercial sampler.


## GitHub-ready notes

This package has been slimmed down for GitHub:
- removed `build/` output
- removed macOS zip metadata (`__MACOSX`, `.DS_Store`)
- removed vendored `JUCE/` copy
- CMake now fetches JUCE automatically during configure

### Build

```bash
cmake -S . -B build
cmake --build build -j
```

On first configure, CMake will download JUCE automatically.
