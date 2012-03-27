#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>

#include "fast_draw.h"

#define N 15000
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

	index_cache = fd_create_cache(0, true);
	norm_cache = fd_create_cache(0, false);

	al_clear_to_color(al_map_rgb_f(0, 0, 0));

	al_flip_display();

	/*********************************/

	srand(seed);

	for(trial = 0; trial < TRIALS; trial++)
	{
		if(trial == 1)
			t_start = al_get_time();
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
	}

	//al_flip_display();

	t_end = al_get_time();

	printf("Standard allegro (hold off): %f msec\n", 1000.0 * (t_end - t_start));

	/*********************************/

	srand(seed);

	for(trial = 0; trial < TRIALS; trial++)
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
	}

	//al_flip_display();

	t_end = al_get_time();

	printf("Standard allegro (hold on): %f msec\n", 1000.0 * (t_end - t_start));

	/*********************************/

	srand(seed);

	for(trial = 0; trial < TRIALS; trial++)
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
	}

	//al_flip_display();

	t_end = al_get_time();

	printf("Fast drawing (indexed): %f msec\n", 1000.0 * (t_end - t_start));

	/*********************************/

	srand(seed);

	for(trial = 0; trial < TRIALS; trial++)
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
	}

	//al_flip_display();

	t_end = al_get_time();

	printf("Fast drawing (non-indexed): %f msec\n", 1000.0 * (t_end - t_start));

	printf("\n");

	al_rest(1.0f);

	return 0;
}
