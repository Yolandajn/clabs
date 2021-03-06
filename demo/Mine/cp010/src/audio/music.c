
/************************************************************
 * cp: music.c
 *
 * Licensed under GPLv2.
 * ABSOLUTELY NO WARRANTY!
 *
 * Copyright (C) eXerigumo Clanjor (哆啦比猫/兰威举).
 ************************************************************/

#include "music.h"
#include <math.h>
#include <stdlib.h>

// info
// BPM = Beats Per Minute, RPB = Rows Per Beat
#define BPM_TO_MSPR(BPM,RPB)	60000.0f / (BPM) / (RPB)
float music_mspr = BPM_TO_MSPR(120, 3);		// MilliSeconds Per Row
float music_amplify = 1.0f;

// scores

// explanation:
// Take 0x4102 for instance:
// 		 4 --> Octave 4
// 		 1 --> Note   1
// 		02 --> Instrument ID (0x02)
// Special:
// 		0x0100 --> rest
// 		0x0000 --> sustain
static u16 s_lead[] = {
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0x4100, 0x0000, 0x3800, 0x0000, 0x4300, 0x0000, 0x3800, 0x0000,
	0xFFFF
};

static u16 s_low[] = {
	0x3101, 0x0000, 0x2800, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x2801, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x3101, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x2801, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x3101, 0x0000, 0x0100, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x2801, 0x0000, 0x0100, 0x0000, 0x2801, 0x0000, 0x0100, 0x0000,
	0x3101, 0x0000, 0x0100, 0x0000, 0x2801, 0x0000, 0x2801, 0x0000,
	0x2801, 0x0000, 0x0100, 0x0000, 0x2801, 0x0000, 0x0100, 0x0000,
	0xFFFF
};

static u16 s_drum[] = {
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x0000, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x4102, 0x5802, 0x0000, 0x4102, 0x0000,
	0x3502, 0x0000, 0x4102, 0x4102, 0x5802, 0x4102, 0x4102, 0x4102,
	0xFFFF
};

u8    music_nscore = 3;
u16 * music_scores[] = {s_lead, s_low, s_drum};

// insts
#define INST(NAME)	float NAME (float x)

INST(osc_pulse) { 
	x = mod(x, 2*PI);
	return (x<PI ? -1.0f : 1.0f);
}
INST(osc_tri) {
	x = mod(x, 2*PI);
	if (x < PI) return lerp(x, 0, PI, -1, 1);
	return lerp(x, PI, 2*PI, 1, -1);
}
INST(inst_hit) {
	if (x > 30.0f * 2*PI) return 0.0f;
	static float t;
	t = osc_pulse(x) * 0.7f;
	if (x < 10.0f * 2*PI) return t * lerp(x, 0, 10.0f * 2*PI, 1.0f, 0.2f);
	return t * lerp(x, 10.0f * 2*PI, 30.0f * 2*PI, 0.2f, 0.0f);
}
INST(inst_low) {
	return osc_tri(x) * 0.2f;
}
static float drum[2048];
INST(inst_drum) {
	x += 0.2f;	// make some offset so that it can be heard more clearly.
	if (x > 20.0f * 2*PI) return 0.0f;

	x = mod(x, 2048.0);
	static float t;
	t = (drum[(u32)x] + osc_tri(x/2.0f)) / 2.0f;
	if (x < 10.0f * 2*PI) return t * lerp(x, 0, 10.0f * 2*PI, 1.0f, 0.2f);
	return t * lerp(x, 10.0f * 2*PI, 20.0f * 2*PI, 0.2f, 0.0f);
}

u8 music_ninst = 3;
MusicOsc music_insts[] = {
/* 00 */	inst_hit,
/* 01 */	inst_low,
/* 02 */	inst_drum,
};

void music_init()
{
	// init instrument: drum
	int i;
	for (i=0; i<2048; i++)
		drum[i] = lerp(rand() % 256, 0.0f, 255.0f, -1.0f, 1.0f);
}

