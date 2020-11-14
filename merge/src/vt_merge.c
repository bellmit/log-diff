#ifndef DISABLE_VTDIFF01

#include "vtclib/vtdef.h"
#include "vtclib/vterrno.h"
#include "vtclib/vtlog.h"
#include "vtmerge/vt_merge01.h"
#include "vtmerge/vt_merge01util.h"
#include "vtmerge/vt_merge02.h"
#include "vtoemadpt/oem_malloc.h"
#include "vtoemadpt/oem_flashfile.h"
#include "vtmerge/vt_merge.h"


int vt_merge_init(void)
{
    return 0;
	//return oem_memInit();
}

int vt_merge_check(const char* target_filepath,
									 const char* delta_filepath)
{
// 	unsigned int flash_file_addr;
//   long flash_file_size;
	
// 	if (oem_addr_and_size(&flash_file_addr, &flash_file_size, target_filepath) != 0)
// 		return -1;
	
// 	return oem_addr_and_size(&flash_file_addr, &flash_file_size, delta_filepath);
    return 0;
}

int vt_merge_file(const char* target_filepath,
									const char* delta_filepath,
                                    const char *megered_filepath,
									int merge_type)
{
	int retcode = 0;
	//const char temp_file[] = "./temp.bin";
			
	if (merge_type == 0x01){
		if(vt_sjaonse_file_merge01(megered_filepath, target_filepath, delta_filepath) != 0)
		{
			retcode = -1;
			logError("merge 01 failed");
		} 
	}else if (merge_type == 0x02){
#ifndef DISABLE_VTDIFF02
		if(vt_sjaonse_file_merge02(megered_filepath, target_filepath, delta_filepath, 0) != 0)
		{
			retcode = -1;
			logError("merge 02 failed");
		} 
#endif
	} else {
		retcode = -1;
		logError("unknown merge type");
	}

	return retcode;
}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

__END_DECLS

#endif // DISABLE_VTDIFF01
