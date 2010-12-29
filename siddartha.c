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
};

PrayFile*
pray_new ()
{
	PrayFile *pray = malloc (sizeof (*pray));
	pray->count = 0;
	return pray;
}

size_t
pray_get_number_of_blocks (const PrayFile *pray)
{
	return pray->count;
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
	
	ENSURE(READ(128,&block.name) == 128, PRAY_BAD_BLOCK_NAME);
	ENSURE(block.name[127] == 0, PRAY_BAD_BLOCK_NAME);
	
	ENSURE(READ(4, &block.size) == 4, PRAY_BAD_BLOCK_SIZE);
	ENSURE(READ(4, &block.size_uncompressed) == 4, PRAY_BAD_BLOCK_SIZE_UNCOMPRESSED);
	ENSURE(READ(4, &block.compressed) == 4, PRAY_BAD_BLOCK_COMPRESSED_FLAG);
	
	void *data = malloc (block.size);
	ENSURE(READ(block.size, data) == block.size, PRAY_BAD_BLOCK_DATA);
	free(data);
	
#undef ENSURE
	
	return block;
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
	
	while (true) {
		PrayBlock block = pray_block_make_from_stream (stream, iface, error);
		if (PRAY_ERROR_NULL != *error) break;
		pray->count++;
	}
	if (PRAY_END_OF_FILE == *error) *error = PRAY_ERROR_NULL;
	
	return pray;
}

PrayFile*
pray_new_from_data (const void *data, PrayError *error)
{	
	struct data_map map = { data, (void*)data };
	return pray_new_from_stream (&map, data_map_iface, error);
}
