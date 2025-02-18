#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "compiler.h"

void soundmng_sync(void *soundRef, int index, const SINT32 *pcm, size_t sz);
void soundmng_addStream(const void *soundRef, const char *name, int index);
UINT soundmng_getRate(void *soundRef);

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);
void soundmng_reset(void *soundRef);

#ifdef __cplusplus
} // #ifdef __cplusplus
#endif

