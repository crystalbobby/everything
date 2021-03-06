#pragma once

#define V_LENGTH_BASE 8
#define A_LENGTH_BASE 26
#define V_LENGTH_MAX 30
#define A_LENGTH_MAX 100

#define AV_DELAY_THRESH 900
#define AA_DELAY_THRESH 1000
#define POSTVARP 250
#define PREVARP 20
#define SAMPLES_PER_ADDTL_HEARTBEAT 128
#define INVERSE_FILTER_THRESHOLD 4

#define L_BLANK 50 // number of samples to blank leading the heartbeat
#define T_BLANK 50 // number of samples to blank trailing the heartbeat

#define N_BS_PTS 12
#define NUM_RECURSIVE_SUBDIV 3
#define SAMPLE_RATE 1000
#define MIN_HEARTRATE 10
#define MAX_HEARTRATE 150
#define PARAM_LEARN_SIZE SAMPLE_RATE*10
#define MINHEARTRATE_DIV_SAMPLRATE60 6000 //Defined as SampleRate*60/MinHeartRate
#define MAXHEARTRATE_DIV_SAMPLERATE60 400 //Defined as SampleRate*60/MaxHeartRate

#define MAX_EDGES (PARAM_LEARN_SIZE * 3) /MAXHEARTRATE_DIV_SAMPLERATE60

#define BUFFER_SIZE 128

#define NUM_PARAMS 4

#define IS_A 0xF000
#define IS_V 0x00F0
#define A_ST 0x0F00
#define V_ST 0X000F
