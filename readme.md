# WAV Parser

A simple WAV file parser written in C.

## Features

* Parses RIFF/WAVE headers.
* Supports **PCM 16-bit** only.
* Extracts samples and prints file info with a terminal waveform visualization.

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```bash
./build/wav_parser tests/data/wav.wav
```

The file `tests/data/wav.wav` is a small sample included for testing.

## Plans

* [ ] Support more audio formats (IEEE float, A-law, ADPCM).
* [ ] Support other bit depths (8, 24, 32).
* [ ] Support multi-channel files beyond stereo.
