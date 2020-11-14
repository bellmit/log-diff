#include <stdlib.h>
#include <errno.h>

#include "vtoemadpt/oem_malloc.h"
#include "vtclib/vtlog.h"

// /*#define LENGTH_36_NUM   (44)
// #define LENGTH_52_NUM   (26)
// #define LENGTH_64_NUM   (4)
// #define LENGTH_128_NUM  (5)
// #define LENGTH_132_NUM  (8)
// #define LENGTH_256_NUM  (8)
// #define LENGTH_512_NUM  (18)    
// #define LENGTH_640_NUM  (8)
// #define LENGTH_1536_NUM (6) */
// #define LENGTH_CHUNK_NUM (1)
// #define LENGTH_CHUNK_LEN (4096)

// #define CUS_MEM_USED        (1)
// #define CUS_MEM_NO_USED     (0)

// #define CALC_CNT    (0)
// #define CALC_MAX    (1)

// #define __Ram_Loc__         (0x20001000) ///This is my RAM address
// #define __TOP_Ram_Loc__     (0x20001000 + 0x2000 -0x10) //Total 32K RAM and last 16 bytes reserved for some data storage

// typedef struct _OEM_MEM_BLOCK_S {
//     char used;
//     int block_size;
//     char *ptr;
//     char *next;
// } oem_mem_block_s;

// static struct _MEM_INFO_TBL_S {
//     int block_size;
//     int num_max;
//     oem_mem_block_s *wm_head;
//     int calc[2];
// } memInfoTbl[] = {
//  /*{36,  LENGTH_36_NUM  , 0, {0,0} },
//  {52,  LENGTH_52_NUM  , 0, {0,0} },
//  {64,  LENGTH_64_NUM  , 0, {0,0} },
//  {128, LENGTH_128_NUM , 0, {0,0} },
//  {132, LENGTH_132_NUM , 0, {0,0} },
//  {256, LENGTH_256_NUM , 0, {0,0} },
//  {512, LENGTH_512_NUM , 0, {0,0} },
//  {640, LENGTH_640_NUM , 0, {0,0} },
//  {1536,LENGTH_1536_NUM, 0, {0,0} },*/
//  {LENGTH_CHUNK_LEN, LENGTH_CHUNK_NUM, 0, {0,0} }
// };

// #define MEM_TBL_MAX     (sizeof(memInfoTbl)/sizeof(struct _MEM_INFO_TBL_S))

// int memHeapHasBeenInitialised = 0;

int oem_memInit(void)
{
    return 0;
    // int i,j;
    // oem_mem_block_s *head=NULL;
    // unsigned int addr;

    // addr = __Ram_Loc__;

    // for(i=0; i<MEM_TBL_MAX; i++) 
    // {
    //     head = (oem_mem_block_s *)addr;
    //     memInfoTbl[i].wm_head = head;
    //     for(j=0;j<memInfoTbl[i].num_max; j++)
    //     {
    //         head->used = CUS_MEM_NO_USED;
    //         head->block_size = memInfoTbl[i].block_size;
    //         head->ptr = (char *)(addr + sizeof(oem_mem_block_s));
    //         addr += (memInfoTbl[i].block_size + sizeof(oem_mem_block_s));
    //         head->next =(char *)addr;
    //         head = (oem_mem_block_s *) head->next;
    //         if(head > (oem_mem_block_s *) __TOP_Ram_Loc__) 
    //         {
	// 						  //error
    //             return 1;
    //         }
    //     }
    // }
    // head->ptr = 0;
    // head->block_size = 0;
    // head->next = (char *) __Ram_Loc__;

    // memHeapHasBeenInitialised = 1;
		
	// 	return 0;
}

// ** BEGIN of adapting my**** functions **
void oem_free(void* ptr){
    free(ptr);
    // int i;
    // oem_mem_block_s *head;

    // if( (ptr == NULL) || (memHeapHasBeenInitialised == 0) )
    //     return;
		
    // //if( (ptr < __RamAHB32__) && (ptr > (void *) __TOP_Ram_Loc__) )
    // if( ptr > (void *) __TOP_Ram_Loc__ )
    // {
    //     //error
    //     return;
    // }

	// 	i = sizeof(oem_mem_block_s);
	// 	i = (unsigned int) ptr;
    // //head = (oem_mem_block_s *) ptr-sizeof(oem_mem_block_s);
    // head = (oem_mem_block_s *) (i - sizeof(oem_mem_block_s));


    // if(head->used)
    //     head->used = CUS_MEM_NO_USED;
    // else
    // {
    //     //error;
    //     return;
    // }

    // for(i=0;i<MEM_TBL_MAX;i++)
    // {
    //     if(memInfoTbl[i].block_size == head->block_size)
    //     {
    //         memInfoTbl[i].calc[CALC_CNT]--;
    //         return;
    //     }
    // }
}

void* oem_malloc(const size_t size){
    return malloc(size);
    // int i;
    // void* prtReturn = NULL;
    // oem_mem_block_s *head;

    // if(memHeapHasBeenInitialised == 0) 
    //     return NULL;

    // for(i=0; i<MEM_TBL_MAX; i++)
    // {
    //     if(size <= memInfoTbl[i].block_size)
    //     {
    //         head = memInfoTbl[i].wm_head;
    //         while(head->ptr)
    //         {
    //             if(head->used == CUS_MEM_NO_USED)
    //             {
    //                 head->used = CUS_MEM_USED;
    //                 prtReturn = head->ptr;
    //                 i = MEM_TBL_MAX;
	// 									head = (oem_mem_block_s *) head->next;
	// 								  break;
    //             }
    //             head = (oem_mem_block_s *) head->next;
    //         }
    //     }
    // }
		
    // if(prtReturn != NULL)
    // {
    //     for(i=0; i<MEM_TBL_MAX; i++)
    //     {
    //         if(memInfoTbl[i].block_size == head->block_size)
    //         {

    //             memInfoTbl[i].calc[CALC_CNT]++;
    //             if(memInfoTbl[i].calc[CALC_CNT] > memInfoTbl[i].calc[CALC_MAX] )
    //                 memInfoTbl[i].calc[CALC_MAX]=memInfoTbl[i].calc[CALC_CNT];
    //             break;
    //         }
    //     }
    // }

	// 	return prtReturn;
}

void* oem_realloc(void* ptr, const size_t size){
	    if (ptr != NULL)
		{
			  oem_free(ptr);
			  ptr = NULL;
		}
		
		return oem_malloc(size);
}
// ** END of adapting my**** functions **
