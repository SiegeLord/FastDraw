#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>

#include "fast_draw.h"

#define N 100000
#define TRIALS 40

float randf()
{
	return (float)rand() / RAND_MAX;
}

int main()
{
	ALLEGRO_DISPLAY* disp;
	FAST_DRAW_CACHE* index_cache;
	FAST_DRAW_CACHE* norm_cache;
	FAST_DRAW_CACHE* buffer_index_cache;
	FAST_DRAW_CACHE* buffer_norm_cache;
	ALLEGRO_BITMAP* bmp1;
	ALLEGRO_BITMAP* sub1;
	ALLEGRO_BITMAP* bmp2;
	float dw = 800;
	float dh = 600;
	int ii;
	int trial;
	double t_start = 0;
	double t_end = 0;
	int seed = 115;

	al_init();
	al_init_primitives_addon();
	al_init_image_addon();

	disp = al_create_display(dw, dh);

	bmp1 = al_load_bitmap("test.tga");
	sub1 = al_create_sub_bitmap(bmp1, 32, 32, 32, 32);
	bmp2 = al_clone_bitmap(bmp1);

	index_cache = fd_create_cache(N, true, false);
	if(!index_cache)
	{
		printf("Couldn't create index_cache.\n");
		return -1;
	}
	norm_cache = fd_create_cache(N, false, false);
	if(!norm_cache)
	{
		printf("Couldn't create norm_cache.\n");
		return -1;
	}
	buffer_index_cache = fd_create_cache(N, true, true);
	if(!buffer_index_cache)
	{
		printf("Couldn't create buffer_index_cache.\n");
		return -1;
	}
	buffer_norm_cache = fd_create_cache(N, false, true);
	if(!buffer_norm_cache)
	{
		printf("Couldn't create buffer_norm_cache\n");
		return -1;
	}

	al_clear_to_color(al_map_rgb_f(0, 0, 0));

	al_flip_display();

	/*********************************/

	//~ al_set_window_title(disp, "Standard allegro (hold off)");

	//~ srand(seed);

	//~ for(trial = 0; trial < TRIALS + 1; trial++)
	//~ {
		//~ if(trial == 1)
			//~ t_start = al_get_time();
		//~ for(ii = 0; ii < N; ii++)
		//~ {
			//~ al_draw_bitmap(bmp1, randf() * dw, randf() * dh, 0);
		//~ }

		//~ for(ii = 0; ii < N; ii++)
		//~ {
			//~ al_draw_bitmap(sub1, randf() * dw, randf() * dh, 0);
		//~ }

		//~ for(ii = 0; ii < N; ii++)
		//~ {
			//~ al_draw_bitmap(bmp2, randf() * dw, randf() * dh, 0);
		//~ }
		//~ al_flip_display();
	//~ }

	//~ t_end = al_get_time();

	//~ printf("Standard allegro (hold off): %f FPS\n", TRIALS / (t_end - t_start));

	/*********************************/

	srand(seed);

	al_set_window_title(disp, "Standard allegro (hold on)");

	for(trial = 0; trial < TRIALS + 1; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
		al_hold_bitmap_drawing(true);
		for(ii = 0; ii < N; ii++)
		{
			al_draw_bitmap(bmp1, randf() * dw, randf() * dh, 0);
		}

		for(ii = 0; ii < N; ii++)
		{
			al_draw_bitmap(sub1, randf() * dw, randf() * dh, 0);
		}

		for(ii = 0; ii < N; ii++)
		{
			al_draw_bitmap(bmp2, randf() * dw, randf() * dh, 0);
		}
		al_hold_bitmap_drawing(false);
		al_flip_display();
	}

	t_end = al_get_time();

	printf("Standard allegro (hold on): %f FPS\n", TRIALS / (t_end - t_start));

	/*********************************/

	srand(seed);

	al_set_window_title(disp, "Fast drawing (indexed)");

	for(trial = 0; trial < TRIALS + 1; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(index_cache, bmp1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(index_cache, sub1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(index_cache, bmp2, randf() * dw, randf() * dh);
		}
		fd_flush_cache(index_cache);
		al_flip_display();
	}

	t_end = al_get_time();

	printf("Fast drawing (indexed): %f FPS\n", TRIALS / (t_end - t_start));

	/*********************************/

	srand(seed);

	al_set_window_title(disp, "Fast drawing (non-indexed)");

	for(trial = 0; trial < TRIALS + 1; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(norm_cache, bmp1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(norm_cache, sub1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(norm_cache, bmp2, randf() * dw, randf() * dh);
		}
		fd_flush_cache(norm_cache);
		al_flip_display();
	}

	t_end = al_get_time();

	printf("Fast drawing (non-indexed): %f FPS\n", TRIALS / (t_end - t_start));

	/*********************************/

	srand(seed);

	al_set_window_title(disp, "Fast drawing (indexed + buffer)");

	for(trial = 0; trial < TRIALS + 1; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_index_cache, bmp1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_index_cache, sub1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_index_cache, bmp2, randf() * dw, randf() * dh);
		}
		fd_flush_cache(buffer_index_cache);
		al_flip_display();
	}

	t_end = al_get_time();

	printf("Fast drawing (indexed + buffer): %f FPS\n", TRIALS / (t_end - t_start));

	/*********************************/

	srand(seed);

	al_set_window_title(disp, "Fast drawing (non-indexed + buffer)");

	for(trial = 0; trial < TRIALS + 1; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_norm_cache, bmp1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_norm_cache, sub1, randf() * dw, randf() * dh);
		}

		for(ii = 0; ii < N; ii++)
		{
			fd_draw_bitmap(buffer_norm_cache, bmp2, randf() * dw, randf() * dh);
		}
		fd_flush_cache(buffer_norm_cache);
		al_flip_display();
	}

	t_end = al_get_time();

	printf("Fast drawing (non-indexed + buffer): %f FPS\n", TRIALS / (t_end - t_start));

	printf("\n");

	al_rest(1.0f);

	return 0;
}
