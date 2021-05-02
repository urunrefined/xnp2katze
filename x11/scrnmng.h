#ifndef	NP2_X11_SCRNMNG_H__
#define	NP2_X11_SCRNMNG_H__

G_BEGIN_DECLS

enum {
	RGB24_B	= 2,
	RGB24_G	= 1,
	RGB24_R	= 0
};

typedef struct {
	UINT8	*ptr;
	int	xalign;
	int	yalign;
	int	width;
	int	height;
	UINT	bpp;
} SCRNSURF;

enum {
	SCRNMODE_HIGHCOLOR	= 0x02
};

enum {
	SCRNFLAG_FULLSCREEN	= 0x01,
	SCRNFLAG_HAVEEXTEND	= 0x02,
	SCRNFLAG_ENABLE		= 0x80
};

typedef struct {
	UINT8	flag;
	UINT8	bpp;
	UINT8	palchanged;
} SCRNMNG;

extern SCRNMNG *scrnmngp;

void scrnmng_initialize(void);
BRESULT scrnmng_create(UINT8 scrnmode);
void scrnmng_destroy(void);

void scrnmng_setwidth(int posx, int width);
void scrnmng_setextend(int extend);
void scrnmng_setheight(int posy, int height);
const SCRNSURF* scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_update(void);
#define	scrnmng_dispclock()

#define	scrnmng_isfullscreen()	(scrnmngp->flag & SCRNFLAG_FULLSCREEN)
#define	scrnmng_haveextend()	(scrnmngp->flag & SCRNFLAG_HAVEEXTEND)
#define	scrnmng_getbpp()	(scrnmngp->bpp)
#define	scrnmng_palchanged()	do { scrnmngp->palchanged = TRUE; } while (0)

RGB16 scrnmng_makepal16(RGB32 pal32);

/* -- for X11 */

void scrnmng_setmultiple(int multiple);

G_END_DECLS

#endif	/* NP2_X11_SCRNMNG_H__ */
