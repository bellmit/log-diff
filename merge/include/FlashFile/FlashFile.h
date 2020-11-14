#ifndef __FLASHFILE_H__
#define __FLASHFILE_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
	
// typedef struct FILECTRL{
// 	unsigned int baseAddr;
// 	long curPos;
// 	long totalLen;
// 	char self_malloc;
//     void *pfileobj;
// }FILECTRL;
typedef FILE FILECTRL;

FILECTRL * flashopen(unsigned int baseAddr, long len);
long  flashread(FILECTRL *f, char * buf , int size);
long  flashwrite(FILECTRL *f, char * buf , int size);
long flashtell(FILECTRL* f);
long flashseek(FILECTRL * f, long pos, long mode);
char flasheof(FILECTRL*f);
char flashclose(FILECTRL*f);
char flasherror(FILECTRL *f);
char flashflush(FILECTRL *f);
int flashgetc(FILECTRL*f);

#ifdef __cplusplus
}
#endif
#endif
