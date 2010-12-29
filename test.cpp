#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "siddartha.h"

#include <gtest/gtest.h>

TEST (Reader, FailsOnEmptyFile) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("", 0, &error);
	EXPECT_FALSE (pray);
	EXPECT_EQ (PRAY_BAD_FILE_HEADER, error);
}

TEST (Reader, FailsOnIncorrectFileHeader) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("Not a pray file", 15, &error);
	EXPECT_FALSE (pray);
	EXPECT_EQ (PRAY_BAD_FILE_HEADER, error);
}

TEST (Reader, PassesOnCorrectFileHeader) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("PRAY", 4, &error);
	EXPECT_NE (PRAY_BAD_FILE_HEADER, error);
	EXPECT_EQ (pray_get_number_of_blocks (pray), 0);
}

TEST (Reader, BlankPrayFileHasZeroBlocks) {
	PrayFile *pray = pray_new();
	EXPECT_EQ (pray_get_number_of_blocks (pray), 0);
}

TEST (Reader, ReadsCorrectNumberOfBlocks) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data
		("PRAY" // PRAY
		 
		 "TEST" // Block type
		 "Block1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // Block name
		 "\004\0\0\0" // Block data size
		 "\004\0\0\0" // Block data size uncompressed
		 "\0\0\0\0" // Block data Zlib-compressed
		 "DATA"
		 
		 "TEST" // Block type
		 "Block2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // Block name
		 "\0\0\0\0" // Block data size
		 "\0\0\0\0" // Block data size uncompressed
		 "\0\0\0\0", // Block data Zlib-compressed
		 296,
		 &error);
	EXPECT_EQ (PRAY_ERROR_NULL, error);
	ASSERT_TRUE (pray);
	EXPECT_EQ (pray_get_number_of_blocks (pray), 2);
}

TEST (Reader, ReadsBlockMetadata) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data
	("PRAY" // PRAY
	 
	 "TEST" // Block type
	 "Block1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // Block name
	 "\004\0\0\0" // Block data size
	 "\004\0\0\0" // Block data size uncompressed
	 "\0\0\0\0" // Block data Zlib-compressed
	 "DATA",
	 152,
	 &error);
	
	EXPECT_EQ (PRAY_ERROR_NULL, error);
	ASSERT_TRUE (pray);
	ASSERT_EQ (1, pray_get_number_of_blocks (pray));
	
	PrayBlock block = pray_get_block (pray, 0);
	EXPECT_STREQ ("TEST", pray_block_get_type (&block));
	EXPECT_STREQ ("Block1", pray_block_get_name (&block));
	EXPECT_EQ (4, pray_block_get_size (block));
	EXPECT_EQ (4, pray_block_get_size_uncompressed (block));
	EXPECT_EQ (0, pray_block_is_zlib_compressed (block));
	char *data = (char*) pray_block_get_data (block);
	EXPECT_EQ ('D', data[0]);
	EXPECT_EQ ('A', data[1]);
	EXPECT_EQ ('T', data[2]);
	EXPECT_EQ ('A', data[3]);
}