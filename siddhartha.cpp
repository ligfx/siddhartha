#include "siddhartha.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

namespace io = boost::iostreams;

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>

// ~ PrayFile ~

struct PrayFile {
	size_t count;
	PrayBlock *blocks;
};

PrayFile*
pray_new ()
{
	PrayFile *pray = (PrayFile*) malloc (sizeof (*pray));
	pray->count = 0;
	pray->blocks = NULL;
	return pray;
}

size_t
pray_get_number_of_blocks (const PrayFile *pray)
{
	return pray->count;
}

PrayBlock
pray_get_block (const PrayFile *pray, size_t i)
{
	return pray->blocks[i];
}

const char*
pray_block_get_type (const PrayBlock *block)
{
	return block->type;
}

const char*
pray_block_get_name (const PrayBlock *block)
{
	return block->name;
}

size_t
pray_block_get_size (const PrayBlock block)
{
	return block.size;
}

size_t
pray_block_get_size_uncompressed (const PrayBlock block)
{
	return block.size_uncompressed;
}

bool
pray_block_is_zlib_compressed (const PrayBlock block)
{
	return ((char*)&block.compressed)[0];
}

void*
pray_block_get_data (PrayBlock block)
{
	return block.data;
}

#define READ(count, ptr) (stream.read((char*)ptr, count), !stream.fail())

PrayBlock
pray_block_make_from_stream (std::istream& stream, PrayError *error)
{
	PrayBlock block;
	
	stream.peek();
	if (stream.eof()) {
		*error = PRAY_END_OF_FILE;
		return block;
	}

#define ENSURE(condition,err) if (!(condition)) { *error = err; return block; }
	ENSURE(READ(4, block.type), PRAY_BAD_BLOCK_TYPE);
	block.type[4] = '\0';
	
	//ENSURE(READ(128, &block.name), PRAY_BAD_BLOCK_NAME);
	READ(128, block.name);
	
	ENSURE(block.name[127] == 0, PRAY_BAD_BLOCK_NAME);
	
	ENSURE(READ(4, &block.size), PRAY_BAD_BLOCK_SIZE);
	ENSURE(READ(4, &block.size_uncompressed), PRAY_BAD_BLOCK_SIZE_UNCOMPRESSED);
	ENSURE(READ(4, &block.compressed), PRAY_BAD_BLOCK_COMPRESSED_FLAG);
	
	block.data = malloc (block.size);
	ENSURE(READ(block.size, block.data), PRAY_BAD_BLOCK_DATA);
#undef ENSURE
	
	return block;
}

PrayBlock*
pray_block_read_all (size_t *i, std::istream &stream, PrayError *error)
{
	PrayBlock block = pray_block_make_from_stream (stream, error);
	PrayBlock *blocks;
	if (PRAY_ERROR_NULL == *error) {
		int j = (*i)++;
		blocks = pray_block_read_all (i, stream, error);
		blocks[j] = block;
	} else {
		blocks = (PrayBlock*) malloc (sizeof (*blocks) * *i);
	}
	return blocks;
}

PrayFile*
pray_new_from_stream (std::istream& stream, PrayError *error)
{
	char header[4];

#define ENSURE(condition,err) if (!(condition)) { *error = err; return NULL; }
	ENSURE(READ(4,&header), PRAY_BAD_FILE_HEADER);
	ENSURE(header[0] == 'P' &&
		   header[1] == 'R' &&
		   header[2] == 'A' &&
		   header[3] == 'Y',
		   PRAY_BAD_FILE_HEADER);
#undef ENSURE
	
	PrayFile *pray = pray_new();
	
	pray->blocks = pray_block_read_all (&pray->count, stream, error);
	if (PRAY_END_OF_FILE == *error) *error = PRAY_ERROR_NULL;
	
	return pray;
}

PrayFile*
pray_new_from_data (char *data, size_t data_length, PrayError *error)
{	
	io::basic_array<char> buf (data, data_length);
	io::stream<io::array> stream (buf);
	return pray_new_from_stream (stream, error);
}
