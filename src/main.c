#include "wav_parser.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "./wav_parser filename");
		return 1;
	}
	FILE *w = fopen(argv[1], "rb");
	if (w == NULL)
	{
		fprintf(stderr, "IO error");
		return 2;
	}
	wav_file_t wav = create_wav(w);
	print_wav(wav);
	destroy_wav(&wav);
	fclose(w);
	return 0;
}
