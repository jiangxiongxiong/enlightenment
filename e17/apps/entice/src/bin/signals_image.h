#ifndef ENTICE_SIGNALS_H
#define ENTICE_SIGNALS_H

/**
 * These are callbacks for all the signals EnticeImage responds to
 * If you want to add a new callback for EnticeImage do the following
 * 1. Add a function to signal.[ch]
 * 2. Add a function to entice.[ch]
 * 3. Have your signal.c function call your entice.c function
 *
 * Keep the data in entice.c and the request for state changes separate
 */
#include<Edje.h>

void _entice_delete_current(void *data, Evas_Object * o, const char *emission,
                            const char *source);
void _entice_remove_current(void *data, Evas_Object * o, const char *emission,
                            const char *source);
void _entice_image_next(void *data, Evas_Object * o, const char *emission,
                        const char *source);
void _entice_image_prev(void *data, Evas_Object * o, const char *emission,
                        const char *source);
void _entice_zoom_in(void *data, Evas_Object * o, const char *emission,
                     const char *source);
void _entice_zoom_out(void *data, Evas_Object * o, const char *emission,
                      const char *source);
void _entice_zoom_default(void *data, Evas_Object * o, const char *emission,
                          const char *source);
void _entice_zoom_fit(void *data, Evas_Object * o, const char *emission,
                      const char *source);
void _entice_zoom_in_focused(void *data, Evas_Object * o,
                             const char *emission, const char *source);
void _entice_zoom_out_focused(void *data, Evas_Object * o,
                              const char *emission, const char *source);
void _entice_fit_window(void *data, Evas_Object * o, const char *emission,
                        const char *source);
void _entice_rotate_left(void *data, Evas_Object * o, const char *emission,
                         const char *source);
void _entice_rotate_right(void *data, Evas_Object * o, const char *emission,
                          const char *source);
void _entice_flip_horizontal(void *data, Evas_Object * o,
                             const char *emission, const char *source);
void _entice_flip_vertical(void *data, Evas_Object * o, const char *emission,
                           const char *source);
void _entice_fullscreen(void *data, Evas_Object * o, const char *emission,
                        const char *source);
void _entice_thumbs_scroll_next_start(void *data, Evas_Object * o,
                                      const char *emission,
                                      const char *source);
void _entice_thumbs_scroll_prev_start(void *data, Evas_Object * o,
                                      const char *emission,
                                      const char *source);
void _entice_thumbs_scroll_stop(void *data, Evas_Object * o,
                                const char *emission, const char *source);
void _entice_image_scroll_east_start(void *data, Evas_Object * o,
                                     const char *emission,
                                     const char *source);
void _entice_image_scroll_west_start(void *data, Evas_Object * o,
                                     const char *emission,
                                     const char *source);
void _entice_image_scroll_north_start(void *data, Evas_Object * o,
                                      const char *emission,
                                      const char *source);
void _entice_image_scroll_south_start(void *data, Evas_Object * o,
                                      const char *emission,
                                      const char *source);
void _entice_image_scroll_stop(void *data, Evas_Object * o,
                               const char *emission, const char *source);
void _entice_quit(void *data, Evas_Object * o, const char *emission,
                  const char *source);
void _entice_image_modified(void *data, Evas_Object * o, const char *emission,
                            const char *source);
void _entice_image_save(void *data, Evas_Object * o, const char *emission,
                        const char *source);
#endif
