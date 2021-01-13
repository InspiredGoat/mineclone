#ifndef CHUNK_H
#define CHUNK_H

#include "mesh.h"
#include "types.h"

#define CHUNK_LENGTH 8 
#define CHUNK_HEIGHT 32
#define CHUNKS_RADIUS 40 

// blocks are stored as ids that can be referenced later on
typedef byte block_id;

// raw chunk mmmmmmmm
typedef struct {
	int x;
	int y;
	block_id data[CHUNK_LENGTH * CHUNK_LENGTH * CHUNK_HEIGHT];
} Chunk;

void Chunks_init();
void Chunks_free();

void Chunk_test(int amount);
void Chunks_setBlock(uint chunk_id, Vector2 block_pos, block_id type);
void Chunks_setBlockRadius(uint chunk_id, Vector2 block_pos, uint radius, block_id type);

Vector2 chunk_to_world(Vector2 chunk_pos);
Vector2 world_to_chunk(Vector2 world_pos);

#endif
