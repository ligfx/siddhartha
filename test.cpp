#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "siddartha.h"

#include <gtest/gtest.h>

TEST (Reader, FailsOnEmptyFile) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("", &error);
	EXPECT_FALSE (pray);
	EXPECT_EQ (PRAY_BAD_FILE_HEADER, error);
}

TEST (Reader, FailsOnIncorrectFileHeader) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("Not a pray file", &error);
	EXPECT_FALSE (pray);
	EXPECT_EQ (PRAY_BAD_FILE_HEADER, error);
}

TEST (Reader, PassesOnCorrectFileHeader) {
	PrayError error = PRAY_ERROR_NULL;
	PrayFile *pray = pray_new_from_data ("PRAY", &error);
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
		 "\0\0\0\0" // Block data size uncompressed
		 "\0\0\0\0" // Block data Zlib-compressed
		 "DATA"
		 
		 "TEST" // Block type
		 "Block1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		 "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // Block name
		 "\0\0\0\0" // Block data size
		 "\0\0\0\0" // Block data size uncompressed
		 "\0\0\0\0", // Block data Zlib-compressed
		 &error);
	EXPECT_EQ (PRAY_ERROR_NULL, error);
	ASSERT_TRUE (pray);
	EXPECT_EQ (pray_get_number_of_blocks (pray), 2);
}