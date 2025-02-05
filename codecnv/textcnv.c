#include	"compiler.h"
#include	"codecnv.h"
#include	"textcnv.h"


UINT textcnv_getinfo(TCINF *inf, const UINT8 *hdr, UINT hdrsize) {

	UINT	textcode;
	TCINF	info;

	textcode = TEXTCNV_DEFAULT;
	ZeroMemory(&info, sizeof(info));
	if ((hdrsize >= 3) &&
		(hdr[0] == 0xef) && (hdr[1] == 0xbb) && (hdr[2] == 0xbf)) {
		// UTF-8
		textcode = TEXTCNV_UTF8;
		info.width = 1;
		info.hdrsize = 3;
	}
	else if ((hdrsize >= 2) && (hdr[0] == 0xff) && (hdr[1] == 0xfe)) {
		// UCSLE
		textcode = TEXTCNV_UCS2;
#if defined(BYTESEX_BIG)
		info.xendian = 1;
#endif
		info.width = 2;
		info.hdrsize = 2;
	}
	else if ((hdrsize >= 2) && (hdr[0] == 0xfe) && (hdr[1] == 0xff)) {
		// UCS2BE
		// UCSLE
		textcode = TEXTCNV_UCS2;
#if defined(BYTESEX_LITTLE)
		info.xendian = 1;
#endif
		info.width = 2;
		info.hdrsize = 2;
	}
	else {
		info.width = 1;
	}

	switch(textcode) {
		case TEXTCNV_UTF8:
			info.caps = TEXTCNV_READ | TEXTCNV_WRITE;
			break;

		case TEXTCNV_UCS2:
			info.caps = TEXTCNV_READ | TEXTCNV_WRITE;
			info.tooem = (TCTOOEM)codecnv_ucs2toutf8;
			info.fromoem = (TCFROMOEM)codecnv_utf8toucs2;
			break;
	}

	if (inf) {
		*inf = info;
	}
	return(info.caps);
}

