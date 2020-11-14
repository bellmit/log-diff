#include <iostream>
#include <string>
#include <cstring>
#include "vtmerge/vt_merge.h"
#include "vtdiff/vtdiff.h"
#include "vtdiff/vtdiffcontext.h"
#include "vtclib/vtlog.h"
#include "vtsys/vtdiffcodec.h"
using namespace std;



int main_based_on_flashmem(char *filef1,char*filedelta,char*filef2){
	int retcode = -1;
	char* target_filepath =filef1;// (char*)"target.bin";
	char* delta_filepath =filedelta;// (char*)"delta.diff";
    char* updated_file = filef2;//(char*) "temp.bin";
	//char* original_file = (char*) "target2.bin";
	
	if (vt_merge_init() != 0)
		return retcode;
	
	if (vt_merge_check(target_filepath, delta_filepath) != 0)
		return retcode;

	retcode = vt_merge_file(target_filepath, delta_filepath,updated_file, 1);
    
	return retcode;
}
bool isArg(void *__c,const char *parg)
{
    char *c =(char*)__c;
   size_t s1 = strlen(c);
   if(s1 != strlen(parg))
   {
       return false;
   }
   for(size_t i=0;i<s1;i++)
   {
       if(toupper(c[i])!=toupper(parg[i]))
       {
           return false;
       }
   }
   return true;

}
extern uint32_t vt_diff_max_chunk_size;
extern uint32_t vt_diff_checksum_algorithm;
const char appversion[]="1.0";
int main(int argc,char**argv)
{
    if((argc ==2)&&isArg(argv[1],"-v"))
    {
        printf("%s\r\n",appversion);
        return 0;
    }
    /*
        vtmerge f1 f2 delta
    */
    if((argc >=5)&&isArg(argv[1],"-m"))
    { 
        char *pf1=argv[2];
        char *pf2=argv[3];
        char *pfdelta=argv[4];
        printf("merge file f1: %s delta: %s => f2: %s\r\n",pf1,pfdelta,pf2);
        main_based_on_flashmem(pf1,pfdelta,pf2);
        return 0;
    }else if((argc >=4)&&isArg(argv[1],"-d"))
    { 
        char *pf1=argv[2];
        char *pf2=argv[3];
        char *pfdelta=argv[4];
        // char *pfdelta=argv[4];
        /*
const char VTDM_DIFF_OP[]="sub";

const char VTDM_CHECKSUM_ALGORITHM[]="crc32";
const char VTDM_MAX_CHUNK_SIZE[]="2k";
// const char VTDM_DATA_GRAM_SIZE[]="8";
// const char VTDM_HASH_WINDOW_SIZE[]="8";
// const char VTDM_CONTINUE_MISMATCH_LIMIT[]="6";
// const char VTDM_MAX_MISMATCH_PERCENTAGE[]="50";
const char VTDM_MAX_IDENTICAL_FIRST_IN_MATCH[]="false";
const char VTDM_JMP_IDENTICAL_BLOCK_IN_MATCH[]="true";
const char VTDM_NEED_KEEP_INTERIM_TEMP_FILES[]="false";
const char VTDM_FLAG_OPT[]="";
const char VTDM_LOG_LEVEL[]="DEBUG";
*/
        printf("vt_hash_window_size[8] %d \r\n",vt_hash_window_size);
        printf("vt_continue_mismatch_limit[6] %d \r\n",vt_continue_mismatch_limit);
        printf("vt_max_mismatch_percentage[50] %d \r\n",vt_max_mismatch_percentage);
        printf("vt_diff_data_gram_size[8] %d \r\n",vt_diff_data_gram_size);
        printf("vt_diff_max_chunk_size[8] %d \r\n",vt_diff_max_chunk_size);
        printf("vt_diff_checksum_algorithm[8] %d \r\n",vt_diff_checksum_algorithm);

        printf("sizeof(vt_diff_delta_file_header_t)[64] %d \r\n",sizeof(vt_diff_delta_file_header_t));

        

        printf("generate delta file {f1: %s , f2: %s} => {delta: %s}\r\n",pf1,pf2,pfdelta);
       vt_diff_checksum_algorithm=1;
       vt_diff_max_chunk_size=2048;
       int i = vt_file_diff01(pfdelta,pf1,pf2);
        printf("result : %d \r\n",i);

        // i = vt_file_lzma_diff01("./testobd/delta.lzma.bin",pf1,pf2);
        // printf("result : %d \r\n",i);

        // i = vt_file_bz2_diff01("./testobd/delta.bz2.bin",pf1,pf2);
        // printf("result : %d \r\n",i);
        // main_based_on_flashmem(pf1,pfdelta,pf2);
        return 0;
    }
    else
    {
        printf("usages:\r\n\r\n");
        printf("\t-v : version\r\n");
        printf("\t-m <f1> <f2> <delta>: merge file f1 with delta and generate to f2\r\n");
        
    }

    return 0;
}