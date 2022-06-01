#ifndef	NP2_X11_NP2_H__
#define	NP2_X11_NP2_H__

#include <signal.h>
#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT32 gettick(void);

typedef struct {
	UINT8	port;
	UINT8	def_en;
	UINT8	param;
	UINT32	speed;
	char	mout[MAX_PATH];
	char	min[MAX_PATH];
	char	mdl[64];
	char	def[MAX_PATH];
} COMCFG;

typedef struct {
	UINT8	DISPCLK;

	UINT8	KEYBOARD;
	UINT8	F12KEY;

	UINT8	JOYPAD1;
	UINT8	JOYPAD2;
	char	JOYDEV[2][MAX_PATH];

	COMCFG	mpu;
	COMCFG	com[3];

	UINT8	confirm;

	UINT8	statsave;
	UINT8	hostdrv_write;
	UINT8	jastsnd;

	char	MIDIDEV[2][MAX_PATH];
	UINT32	MIDIWAIT;
} NP2OSCFG;


enum {
	SCREEN_WBASE		= 80,
	SCREEN_HBASE		= 50,
	SCREEN_DEFMUL		= 8,
};

enum {
	MMXFLAG_DISABLE		= 1,
	MMXFLAG_NOTSUPPORT	= 2
};

/* np2.c */
extern volatile sig_atomic_t np2running;

extern UINT framecnt;
extern UINT waitcnt;
extern UINT framemax;

extern BOOL s98logging;
extern int s98log_count;

extern char bmpfilefolder[MAX_PATH];
extern char modulefile[MAX_PATH];
extern char statpath[MAX_PATH];

int flagload(const char* ext, BOOL force);
int flagsave(const char* ext);
void flagdelete(const char* ext);

void framereset();
void processwait(UINT cnt);
int mainloop(void *);

#ifdef __cplusplus
}
#endif

#endif	/* NP2_X11_NP2_H__ */
