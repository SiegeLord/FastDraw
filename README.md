FastDraw
--------

by SiegeLord

A mini-library that implements a possibly faster deferred bitmap drawing than the one that comes with Allegro. It does this by removing the ability to change the transformations while the drawing is held.

It is contained in two files that you're probably best including in your project:

fast_draw.h
fast_draw.c

Usage:

    // Typically using indices and not using buffers is the fastest.
    FAST_DRAW_CACHE* cache = fd_create_cache(0, true, false);
    fd_draw_bitmap(cache, bmp, 0, 0);
    fd_flush_cache(cache);

test.c is a test file that benchmarks the various drawing techniques. You can try it out like this:

    gcc test.c fast_draw.c -o test -O2 -lallegro -lallegro_primitives -lallegro_image
    ./test
