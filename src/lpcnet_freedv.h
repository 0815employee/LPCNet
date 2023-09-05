/*
  lpcnet_freedv.h
  David Rowe April 2019

  LPCNet API functions for FreeDV.
*/

#ifndef __LPCNET_FREEDV__
#define __LPCNET_FREEDV__

#ifdef __cplusplus
  extern "C" {
#endif

#if defined(_MSC_VER)
	#ifdef LPCNET_LIBRARY_EXPORTS
		#define LPCNET_LIBRARY_API __declspec(dllexport)
	#else
		#define LPCNET_LIBRARY_API __declspec(dllimport)
	#endif
#else
	#define LPCNET_LIBRARY_API
#endif

// possible vq_type values in lpcnet_freedv_create()
#define LPCNET_PRED                   0
#define LPCNET_DIRECT_SPLIT           1
#define LPCNET_DIRECT_SPLIT_INDEX_OPT 2
      
typedef struct LPCNetFreeDV LPCNetFreeDV;

LPCNET_LIBRARY_API LPCNetFreeDV* lpcnet_freedv_create(int vq_type);
LPCNET_LIBRARY_API void lpcnet_freedv_destroy(LPCNetFreeDV *lf);
LPCNET_LIBRARY_API void lpcnet_enc(LPCNetFreeDV *lf, short *pcm, char *frame);
LPCNET_LIBRARY_API void lpcnet_dec(LPCNetFreeDV *lf, char *frame, short* pcm);
LPCNET_LIBRARY_API int lpcnet_bits_per_frame(LPCNetFreeDV *lf);
LPCNET_LIBRARY_API int lpcnet_samples_per_frame(LPCNetFreeDV *lf);
LPCNET_LIBRARY_API char *lpcnet_get_hash(void);

#ifdef __cplusplus
}
#endif

#endif
