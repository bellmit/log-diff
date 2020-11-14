#ifndef VTNEEDLE_H_INCLUDED
#define VTNEEDLE_H_INCLUDED

/**  2016.02 Dehua Tan **/

#include "vtsys/vtdef.h"

#define VT_MIN_NEEDLE_REPEATING_TIMES   3
#define VT_MIN_NEEDLE_REPEATING_SIZE    15
#define VT_MIN_NEEDLE_DISTANCE          3
#define VT_MAX_NEEDLE_DISTANCE          7

__BEGIN_DECLS

typedef struct _vt_repeating_needle_description_t{
    uint8_t needle_value;
    uint16_t needle_distance;
    uint32_t start_offset;
    uint32_t repeating_times;
}   vt_repeating_needle_description_t;

typedef struct _vt_repeating_needle_descriptions_t{
    vt_repeating_needle_description_t* rnd_array;
    uint32_t rnd_count;
    uint32_t rnd_allocated;
}   vt_repeating_needle_descriptions_t;

/**
 * (*rnds_ptr) has been initialized into 0;
**/
int vt_search_needle_descriptions_on_distances(
    vt_repeating_needle_descriptions_t* rnds_ptr,
    const uint8_t* buf,
    const uint32_t bufsize,
    const uint32_t min_repeating_times,
    const uint32_t min_repeating_size,
    const uint16_t needle_distance
);

/**
 * (*rnds_ptr) has been initialized into 0;
**/
int vt_search_all_repeating_needle_descriptions(
    vt_repeating_needle_descriptions_t* rnds_ptr,
    const uint8_t* buf,
    const uint32_t bufsize,
    const uint32_t min_repeating_times,
    const uint32_t min_repeating_size,
    const uint16_t min_needle_distance,
    const uint16_t max_needle_distance
);

int vt_free_repeating_needle_descriptions(vt_repeating_needle_description_t* rnd_array);

typedef struct _vt_needled_common_block_t{
    uint32_t block_len;
    uint16_t needle_distance;
    uint8_t needle1_value;
    uint32_t offset1;
    uint8_t needle2_value;
    uint32_t offset2;
}   vt_needled_common_block_t;

typedef struct _vt_needled_common_blocks_t{
    vt_needled_common_block_t* block_array;
    uint32_t block_count;
    uint32_t block_allocated;
}   vt_needled_common_blocks_t;

/**
 * (*ncbs_ptr) has been initialized into 0;
**/
int vt_search_repeating_needled_common_blocks(
    vt_needled_common_blocks_t* ncbs_ptr,
    const uint8_t* buf1,
    const uint32_t buf1_size,
    const vt_repeating_needle_descriptions_t* rnds1_ptr,
    const uint8_t* buf2,
    const uint32_t buf2_size,
    const vt_repeating_needle_descriptions_t* rnds2_ptr
);

int vt_free_repeating_needled_common_blocks(vt_needled_common_block_t* block_array);

__END_DECLS

#endif // VTHALLOW_H_INCLUDED
