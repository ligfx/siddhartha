#include "siddartha.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ~ Stream ~

typedef size_t (*sidd_read_t) (void*, size_t n, void *buf);
typedef void (*sidd_seek_t) (void*, size_t mark);
typedef size_t (*sidd_tell_t) (void*);

struct sidd_stream {
	sidd_read_t read;
	sidd_seek_t seek;
	sidd_tell_t tell;
};

struct data_map {
	const void *data;
	void *ptr;
};

// ~ Stream: Data Map ~

size_t
data_map_read (struct data_map *map, size_t n, void *buf)
{
	memcpy (buf, map->ptr, n);
	map->ptr = ((char*)map->ptr) + n;
	return n;
}

void
data_map_seek (struct data_map *map, size_t mark)
{
	map->ptr = ((char*)map->data) + mark;
}

size_t
data_map_tell (struct data_map *map)
{
	return ((char*)map->ptr) - ((char*)map->data);
}

static struct sidd_stream data_map_iface = {
	(sidd_read_t) data_map_read,
	(sidd_seek_t) data_map_seek,
	(sidd_tell_t) data_map_tell
};

// ~ PrayFile ~

struct PrayFile {
	size_t count;
	PrayBlock *blocks;
};

PrayFile*
pray_new ()
{
	PrayFile *pray = malloc (sizeof (*pray));
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

#define READ(count, ptr) iface.read(stream, count, ptr)
#define SEEK(mark) iface.seek(stream, mark)
#define TELL(mark) iface.tell(stream)


PrayBlock
pray_block_make_from_stream (void* stream, struct sidd_stream iface, PrayError *error)
{
	PrayBlock block;
	
	if (*(char*)(((struct data_map*)stream)->ptr) == '\0') {
		*error = PRAY_END_OF_FILE;
		return block;
	}

#define ENSURE(condition,err) if (!(condition)) { *error = err; return block; }
	ENSURE(READ(4,&block.type) == 4, PRAY_BAD_BLOCK_TYPE);
	block.type[4] = '\0';
	
	ENSURE(READ(128,&block.name) == 128, PRAY_BAD_BLOCK_NAME);
	ENSURE(block.name[127] == 0, PRAY_BAD_BLOCK_NAME);
	
	ENSURE(READ(4, &block.size) == 4, PRAY_BAD_BLOCK_SIZE);
	ENSURE(READ(4, &block.size_uncompressed) == 4, PRAY_BAD_BLOCK_SIZE_UNCOMPRESSED);
	ENSURE(READ(4, &block.compressed) == 4, PRAY_BAD_BLOCK_COMPRESSED_FLAG);
	
	block.data = malloc (block.size);
	ENSURE(READ(block.size, block.data) == block.size, PRAY_BAD_BLOCK_DATA);
#undef ENSURE
	
	return block;
}

PrayBlock*
pray_block_read_all (size_t *i, void *stream, struct sidd_stream iface, PrayError *error)
{
	PrayBlock block = pray_block_make_from_stream (stream, iface, error);
	PrayBlock *blocks;
	if (PRAY_ERROR_NULL == *error) {
		int j = (*i)++;
		blocks = pray_block_read_all (i, stream, iface, error);
		blocks[j] = block;
	} else {
		blocks = malloc (sizeof (*blocks) * *i);
	}
	return blocks;
}

PrayFile*
pray_new_from_stream (void* stream, struct sidd_stream iface, PrayError *error)
{
	char header[4];

#define ENSURE(condition,err) if (!(condition)) { *error = err; return NULL; }
	ENSURE(READ(4,&header) == 4, PRAY_BAD_FILE_HEADER);
	ENSURE(header[0] == 'P' &&
		   header[1] == 'R' &&
		   header[2] == 'A' &&
		   header[3] == 'Y',
		   PRAY_BAD_FILE_HEADER);
#undef ENSURE
	
	PrayFile *pray = pray_new();
	
	pray->blocks = pray_block_read_all (&pray->count, stream, iface, error);
	if (PRAY_END_OF_FILE == *error) *error = PRAY_ERROR_NULL;
	
	return pray;
}

PrayFile*
pray_new_from_data (const void *data, PrayError *error)
{	
	struct data_map map = { data, (void*)data };
	return pray_new_from_stream (&map, data_map_iface, error);
}
