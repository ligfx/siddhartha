#ifndef SIDDARTHA
#define SIDDARTHA

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #define SIDD_API __declspec(dllexport)
  #else
    #define SIDD_API __declspec(dllimport)
  #endif
#else
  #if __GNUC__ >= 4
    #define SIDD_API __attribute__ ((visibility("default")))
  #else
    #define SIDD_API
  #endif
#endif

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PrayFile PrayFile;
typedef struct PrayBlock PrayBlock;
	
struct PrayBlock {
	char type[4];
	char name[128];
	uint32_t size;
	uint32_t size_uncompressed;
	uint32_t compressed;
};
	
typedef enum {
	PRAY_ERROR_NULL,
	PRAY_BAD_FILE_HEADER,
	PRAY_END_OF_FILE,
	PRAY_BAD_BLOCK_TYPE,
	PRAY_BAD_BLOCK_NAME,
	PRAY_BAD_BLOCK_SIZE,
	PRAY_BAD_BLOCK_SIZE_UNCOMPRESSED,
	PRAY_BAD_BLOCK_COMPRESSED_FLAG,
	PRAY_BAD_BLOCK_DATA,
} PrayError;
	
SIDD_API PrayFile* pray_new ();
SIDD_API PrayFile* pray_new_from_data (const void*, PrayError*);
SIDD_API size_t pray_get_number_of_blocks (const PrayFile*);
	
#ifdef __cplusplus
}
#endif

#endif // SIDDARTHA
