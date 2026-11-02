#ifndef WAV_PARSER_H__
#define WAV_PARSER_H__

#include <stdint.h>
#include <stdio.h>

typedef struct{
	uint32_t sample_rate;
	uint16_t channels;
	uint16_t bits_per_sample;
	uint32_t data_size;
	int16_t *samples;
	uint32_t num_samples;
}wav_file_t;

wav_file_t create_wav(FILE *wav);
void destroy_wav(wav_file_t *wav);

void print_wav(wav_file_t wav);

#endif
