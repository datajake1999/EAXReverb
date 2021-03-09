#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "ReverbEffect.h"

int main()
{
    int sampleCount = 0;
    int frequency = 44100;


    printf("Loading raw samples (only 16-bit, %d mono supported in this program)\n" ,frequency);
    FILE *fp = fopen("test.raw","rb");
    if( fp == NULL )
    {
        perror("Error while opening the file...\n");
        exit(EXIT_FAILURE);
    }

    // get file size
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);

    // seek back to beginning
    fseek(fp, 0L, SEEK_SET);

    // calculate number of samples (assune 16-bit, mono, so, 2 bytes per sample)

    sampleCount = size / 2;
    int16_t *samples = new int16_t[sampleCount];
    fread(samples, 1, size, fp);
    fclose(fp);

    printf("Converting now samples...\n");
    float *floatSamplesIn =  new float[sampleCount];

    // now convert samples into floats
    int i;
    for (i=0; i<sampleCount; i++)
    {
        floatSamplesIn[i] = (float)samples[i] / 32767.0f;
    }
    delete[] samples;
    printf("Found %d samples\n", sampleCount);


    printf("Initializing effect...\n");
    ReverbEffect effect;
    effect.Create(frequency);


    printf("Applying enviroment reverb effect...\n");

    fp = fopen("output.raw","wb");
    if( fp == NULL )
    {
        perror("Error while creating the output file...\n");
        exit(EXIT_FAILURE);
    }

//castle
    effect.LoadPreset(EFX_REVERB_PRESET_CASTLE_SMALLROOM);

//stadium
    //effect.LoadPreset(EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM);

// heaven
    //effect.LoadPreset(EFX_REVERB_PRESET_MOOD_HEAVEN);

//sewer
    //effect.LoadPreset(EFX_REVERB_PRESET_SEWERPIPE);

//psychotic
    //effect.LoadPreset(EFX_REVERB_PRESET_PSYCHOTIC);

    effect.Update(frequency);

    int offset = 0;
    float floatSamplesOut[REVERB_BUFFERSIZE * OUTPUT_CHANNELS];

    do {

        int workSamples = REVERB_BUFFERSIZE / 4;
        if (workSamples>sampleCount)
        {
            workSamples = sampleCount;
        }

        effect.Process(workSamples, &floatSamplesIn[offset],  floatSamplesOut);

        for (i=0; i<workSamples; i++)
        {
            int16_t outSample = (int16_t) (floatSamplesOut[i*2 + 0] * 32767.0f);
            fwrite(&outSample, 1, 2, fp);

            outSample = (int16_t) (floatSamplesOut[i*2 + 1] * 32767.0f);
            fwrite(&outSample, 1, 2, fp);
        }

        sampleCount -= workSamples;
        offset += workSamples;
    } while (sampleCount>0);

    delete[] floatSamplesIn;


    fclose(fp);

    effect.Destroy();

    printf("Done!\n");

    return 0;
}
