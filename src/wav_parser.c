#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wav_parser.h"

static int process_fmt(FILE *wav, wav_file_t *res, uint32_t chunk_size)
{
	uint16_t audio_format;
	fread(&audio_format, 2, 1, wav);
	if (audio_format != 1)
	{
		return audio_format;
	}
	fread(&res->channels, 2, 1, wav);
	fread(&res->sample_rate, 4, 1, wav);
	fseek(wav, 4 + 2, SEEK_CUR); //skip byterate and block align
	fread(&res->bits_per_sample, 2, 1, wav);
	if (res->bits_per_sample != 16)
	{
		return -1 * (int)res->bits_per_sample;
	}
	if (chunk_size > 16)
	{
		fseek(wav, chunk_size - 16, SEEK_CUR);
	}
	if (chunk_size % 2 == 1)
	{
		fseek(wav, 1, SEEK_CUR);
	}
	return 0;
}

static int process_data(FILE *wav, wav_file_t *res, uint32_t chunk_size)
{
	res->samples = malloc(chunk_size);
	if (!res->samples)
	{
		return 1;
	}
	res->data_size = chunk_size;
	res->num_samples = chunk_size / 2;
	fread(res->samples, 2, res->num_samples, wav);
	return 0;
}

wav_file_t create_wav(FILE *wav)
{
	int rc;
	wav_file_t res;
	memset(&res, 0, sizeof(wav_file_t));
	uint32_t riff_size;
	char riff[4], wave[4];
	fread(riff, 1, 4, wav);
	fread(&riff_size, 4, 1, wav);
	fread(wave, 1, 4, wav);

	if(memcmp(riff, "RIFF", 4) != 0 || memcmp(wave, "WAVE", 4) != 0)
	{
		fprintf(stderr, "Incorrect file format");
		return res;
	}
	
	char chunk_id[4];
	uint32_t chunk_size;
	while (fread(chunk_id, 1, 4, wav) == 4)
	{
		fread(&chunk_size, 4, 1, wav);
		if (memcmp(chunk_id, "fmt ", 4) == 0)
		{
			rc = process_fmt(wav, &res, chunk_size);
			if (rc > 0)
			{
				fprintf(stderr, "Only PCM supported, got %u format", rc);
				return res;
			}
			if (rc < 0)
			{
				fprintf(stderr, "Only 16 bps supported, got %d", rc);
				return res;
			}
		}
		else if (memcmp(chunk_id, "data", 4) == 0)
		{
			rc = process_data(wav, &res, chunk_size);
			if (rc != 0)
			{
				fprintf(stderr, "Memory error");
			}
			return res;
		}
		else
		{
			fseek(wav, chunk_size, SEEK_CUR);
			continue;
		}
	}
	return res;
}

void destroy_wav(wav_file_t *wav)
{
	free(wav->samples);
}

void print_wav(wav_file_t wav)
{
    printf("=== WAV FILE INFO ===\n");
    printf("Sample Rate:     %u Hz\n", wav.sample_rate);
    printf("Channels:        %u\n", wav.channels);
    printf("Bits per Sample: %u\n", wav.bits_per_sample);
    printf("Data Size:       %u bytes\n", wav.data_size);
    printf("Num Samples:     %u\n", wav.num_samples);
    printf("=====================\n\n");

    if (!wav.samples || wav.num_samples == 0)
    {
        printf("No samples to display.\n");
        return;
    }

    uint32_t display_samples = 200;
    if (wav.num_samples < display_samples)
    {
        display_samples = wav.num_samples;
    }

    int real_min = 32767;
    int real_max = -32768;
    for (uint32_t i = 0; i < display_samples; i++)
    {
        if (wav.samples[i] < real_min) real_min = wav.samples[i];
        if (wav.samples[i] > real_max) real_max = wav.samples[i];
    }

    if (real_max == real_min)
    {
        real_max += 1;
        real_min -= 1;
    }

    printf("First 10 samples: ");
    uint32_t print_count = 10;
    if (wav.num_samples < print_count) print_count = wav.num_samples;
    for (uint32_t i = 0; i < print_count; i++)
    {
        printf("%d ", wav.samples[i]);
    }
    printf("\n\n");

    uint32_t width = 60;
    uint32_t samples_per_col = display_samples / width;
    if (samples_per_col == 0) samples_per_col = 1;

    int height = 20;

    printf("Waveform (first %u samples, range: %d .. %d):\n",
           display_samples, real_min, real_max);
    printf("  +----------------------+\n");

    for (int row = 0; row < height; row++)
    {
        int row_max = real_max - (row * (real_max - real_min) / height);
        int row_min = real_max - ((row + 1) * (real_max - real_min) / height);

        printf("  |");

        for (uint32_t col = 0; col < width; col++)
        {
            uint32_t start = col * samples_per_col;
            uint32_t end = start + samples_per_col;
            if (end > display_samples) end = display_samples;

            int local_max = real_min;
            int local_min = real_max;

            for (uint32_t i = start; i < end; i++)
            {
                if (wav.samples[i] > local_max) local_max = wav.samples[i];
                if (wav.samples[i] < local_min) local_min = wav.samples[i];
            }

            if (local_max >= row_min && local_min <= row_max)
            {
                if (local_max - local_min > (real_max - real_min) / 4)
                {
                    printf("*");
                }
                else
                {
                    printf("|");
                }
            }
            else
            {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("  +----------------------+\n");
    printf("  (min = %d, max = %d)\n\n", real_min, real_max);
}
