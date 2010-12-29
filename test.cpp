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