/*
Copyright (c) 2012, Pavel Sountsov
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "fast_draw.h"

#include <allegro5/allegro_primitives.h>
#include <sallegro5/allegro_opengl.h>

#include <stdlib.h>

typedef struct FAST_DRAW_CACHE
{
	ALLEGRO_VERTEX* vertices;
	int* indices;
	size_t capacity;
	size_t size;
	ALLEGRO_BITMAP* bitmap;
	bool use_indices;
} FAST_DRAW_CACHE;

static void set_capacity(FAST_DRAW_CACHE* cache, size_t new_capacity)
{
	assert(cache);

	if(cache->use_indices)
	{
		cache->vertices = realloc(cache->vertices, 4 * new_capacity * sizeof(ALLEGRO_VERTEX));
		cache->indices = realloc(cache->indices, 6 * new_capacity * sizeof(int));
	}
	else
	{
		cache->vertices = realloc(cache->vertices, 6 * new_capacity * sizeof(ALLEGRO_VERTEX));
	}

	if(new_capacity)
		assert(cache->vertices);
	if(cache->use_indices && new_capacity)
		assert(cache->indices);

	cache->capacity = new_capacity;
}

static void get_pointers(FAST_DRAW_CACHE* cache, ALLEGRO_VERTEX** vertices, int** indices)
{
	if(cache->size + 1 > cache->capacity)
		set_capacity(cache, 3 * cache->capacity / 2 + 1);

	if(cache->use_indices)
	{
		*vertices = cache->vertices + 4 * cache->size;
		*indices = cache->indices + 6 * cache->size;
	}
	else
	{
		*vertices = cache->vertices + 6 * cache->size;
	}

	cache->size++;
}

FAST_DRAW_CACHE* fd_create_cache(size_t initial_size, bool use_indices)
{
	FAST_DRAW_CACHE* cache = calloc(1, sizeof(FAST_DRAW_CACHE));
	cache->use_indices = use_indices;
	set_capacity(cache, initial_size);
	return cache;
}

void fd_destroy_cache(FAST_DRAW_CACHE* cache)
{
	if(cache)
	{
		free(cache->indices);
		free(cache->vertices);
		free(cache);
	}
}

void fd_flush_cache(FAST_DRAW_CACHE* cache)
{
	assert(cache);

	if(cache->size == 0)
		return;

	if(cache->use_indices)
		al_draw_indexed_prim(cache->vertices, 0, cache->bitmap, cache->indices, 6 * cache->size, ALLEGRO_PRIM_TRIANGLE_LIST);
	else
		al_draw_prim(cache->vertices, 0, cache->bitmap, 0, 6 * cache->size, ALLEGRO_PRIM_TRIANGLE_LIST);

	cache->bitmap = 0;
	cache->size = 0;
}

void fd_draw_bitmap(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, float x, float y)
{
	int w = al_get_bitmap_width(bmp);
	int h = al_get_bitmap_height(bmp);
	fd_draw_tinted_scaled_bitmap(cache, bmp, al_map_rgba_f(1, 1, 1, 1), 0, 0, w, h, x, y, w, h);
}

void fd_draw_bitmap_region(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, float sx, float sy, float sw, float sh, float dx, float dy)
{
	fd_draw_tinted_scaled_bitmap(cache, bmp, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, sw, sh);
}

void fd_draw_scaled_bitmap(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh)
{
	fd_draw_tinted_scaled_bitmap(cache, bmp, al_map_rgba_f(1, 1, 1, 1), sx, sy, sw, sh, dx, dy, dw, dh);
}

void fd_draw_tinted_bitmap(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR tint, float x, float y)
{
	int w = al_get_bitmap_width(bmp);
	int h = al_get_bitmap_height(bmp);
	fd_draw_tinted_scaled_bitmap(cache, bmp, tint, 0, 0, w, h, x, y, w, h);
}

void fd_draw_tinted_bitmap_region(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy)
{
	fd_draw_tinted_scaled_bitmap(cache, bmp, tint, sx, sy, sw, sh, dx, dy, sw, sh);
}

void fd_draw_tinted_scaled_bitmap(FAST_DRAW_CACHE* cache, ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh)
{
	int ii;
	int offx = 0;
	int offy = 0;
	ALLEGRO_VERTEX* vertices;
	int* indices;
	ALLEGRO_BITMAP* parent = al_get_parent_bitmap(bmp);

	if(parent == 0)
		parent = bmp;
	else
		al_get_opengl_texture_position(bmp, &offx, &offy); /* HACK */

	if(parent != cache->bitmap)
		fd_flush_cache(cache);

	cache->bitmap = parent;

	get_pointers(cache, &vertices, &indices);

	if(cache->use_indices)
	{
		/* 0,3    4    0     1
		 *   o---o      o---o
		 *   |\  |      |\  |
		 *   | \ |      | \ |
		 *   |  \|      |  \|
		 *   o---o      o---o
		 *  2    1,5   2     3
		 */
		int offi = (cache->size - 1) * 4;
		indices[0] = offi + 0;
		indices[1] = offi + 2;
		indices[2] = offi + 3;
		indices[3] = offi + 0;
		indices[4] = offi + 1;
		indices[5] = offi + 2;

		vertices[0].x = dx;
		vertices[0].y = dy;
		vertices[0].u = sx + offx;
		vertices[0].v = sy + offy;

		vertices[1].x = dx + dw;
		vertices[1].y = dy;
		vertices[1].u = sx + sw + offx;
		vertices[1].v = sy + offy;

		vertices[2].x = dx + dw;
		vertices[2].y = dy + dh;
		vertices[2].u = sx + sw + offx;
		vertices[2].v = sy + sh + offy;

		vertices[3].x = dx;
		vertices[3].y = dy + dh;
		vertices[3].u = sx + offx;
		vertices[3].v = sy + sh + offy;

		for(ii = 0; ii < 4; ii++)
		{
			vertices[ii].z = 0;
			vertices[ii].color = tint;
		}
	}
	else
	{
		/* 0,3    4
		 *   o---o
		 *   |\  |
		 *   | \ |
		 *   |  \|
		 *   o---o
		 *  2    1,5
		 */

		vertices[0].x = dx;
		vertices[0].y = dy;
		vertices[0].u = sx + offx;
		vertices[0].v = sy + offy;

		vertices[1].x = dx + dw;
		vertices[1].y = dy + dh;
		vertices[1].u = sx + sw + offx;
		vertices[1].v = sy + sh + offy;

		vertices[2].x = dx;
		vertices[2].y = dy + dh;
		vertices[2].u = sx + offx;
		vertices[2].v = sy + sh + offy;

		vertices[3].x = dx;
		vertices[3].y = dy;
		vertices[3].u = sx + offx;
		vertices[3].v = sy + offy;

		vertices[4].x = dx + dw;
		vertices[4].y = dy;
		vertices[4].u = sx + sw + offx;
		vertices[4].v = sy + offy;

		vertices[5].x = dx + dw;
		vertices[5].y = dy + dh;
		vertices[5].u = sx + sw + offx;
		vertices[5].v = sy + sh + offy;

		for(ii = 0; ii < 6; ii++)
		{
			vertices[ii].z = 0;
			vertices[ii].color = tint;
		}
	}
}
