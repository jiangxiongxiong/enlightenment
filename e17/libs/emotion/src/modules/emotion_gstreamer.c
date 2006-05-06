#include <unistd.h>
#include <fcntl.h>

#include "Emotion.h"
#include "emotion_private.h"
#include "emotion_gstreamer.h"


static gboolean
_emotion_pipeline_pause (GstElement *pipeline)
{
   GstStateChangeReturn res;

   res = gst_element_set_state ((pipeline), GST_STATE_PAUSED);
   if (res == GST_STATE_CHANGE_FAILURE) {
     g_print ("Emotion-Gstreamer ERROR: could not pause\n");
     gst_object_unref (GST_OBJECT ((pipeline)));
     return 0;
   }

   res = gst_element_get_state ((pipeline), NULL, NULL, GST_CLOCK_TIME_NONE);
   if (res != GST_STATE_CHANGE_SUCCESS) {
     g_print ("Emotion-Gstreamer ERROR: could not complete pause\n");
     gst_object_unref (GST_OBJECT ((pipeline)));
     return 0;
   }

   return 1;
}



/* Callbacks to display the frame content */

static void cb_handoff (GstElement *fakesrc,
			GstBuffer  *buffer,
			GstPad     *pad,
			gpointer    user_data);
static void new_decoded_pad_cb (GstElement *decodebin,
                                GstPad     *new_pad,
                                gboolean    last,
                                gpointer    user_data);

/* Callbacks to get the eos */
static int _eos_timer_fct (void *data);

GstElement *
make_queue ()
{
  GstElement *queue = gst_element_factory_make ("queue", NULL);
  g_object_set (queue,
      "max-size-time", (guint64) 3 * GST_SECOND,
      "max-size-bytes", (guint32) 0,
      "max-size-buffers", (guint32) 0, NULL);

  return queue;
}

static int   _em_fd_ev_active(void *data, Ecore_Fd_Handler *fdh);

static Emotion_Video_Sink * _emotion_video_sink_new (Emotion_Gstreamer_Video *ev);
static Emotion_Audio_Sink * _emotion_audio_sink_new (Emotion_Gstreamer_Video *ev);
static GstElement         * _em_audio_sink_create   (Emotion_Gstreamer_Video *ev, int index);
static Emotion_Video_Sink * _visualization_sink_create ();

static int _cdda_pipeline_build (void *video, const char * device, unsigned int track);
static int _file_pipeline_build (void *video, const char *file);

static int _cdda_track_count_get(void *video);

/* Interface */

static unsigned char  em_init                     (Evas_Object     *obj,
                                                   void           **emotion_video);

static int            em_shutdown                 (void           *video);

static unsigned char  em_file_open                (const char     *file,
                                                   Evas_Object     *obj,
                                                   void            *video);

static void           em_file_close               (void            *video);

static void           em_play                     (void            *video,
                                                   double           pos);

static void           em_stop                     (void            *video);

static void           em_size_get                 (void            *video,
                                                   int             *width,
                                                   int             *height);

static void           em_pos_set                  (void            *video,
                                                   double           pos);

static void           em_vis_set                  (void            *video,
                                                   Emotion_Vis      vis);

static double         em_len_get                  (void            *video);

static int            em_fps_num_get              (void            *video);

static int            em_fps_den_get              (void            *video);

static double         em_fps_get                  (void            *video);

static double         em_pos_get                  (void            *video);

static Emotion_Vis    em_vis_get                  (void            *video);

static double         em_ratio_get                (void            *video);

static int            em_video_handled            (void            *video);

static int            em_audio_handled            (void            *video);

static int            em_seekable                 (void            *video);

static void           em_frame_done               (void            *video);

static Emotion_Format em_format_get               (void            *video);

static void           em_video_data_size_get      (void            *video,
                                                   int             *w,
                                                   int             *h);

static int            em_yuv_rows_get             (void            *video,
                                                   int              w,
                                                   int              h,
                                                   unsigned char  **yrows,
                                                   unsigned char  **urows,
                                                   unsigned char  **vrows);

static int            em_bgra_data_get            (void            *video,
                                                   unsigned char  **bgra_data);

static void           em_event_feed               (void            *video,
                                                   int              event);

static void           em_event_mouse_button_feed  (void            *video,
                                                   int              button,
                                                   int              x,
                                                   int              y);

static void           em_event_mouse_move_feed    (void            *video,
                                                   int              x,
                                                   int              y);

static int            em_video_channel_count      (void             *video);

static void           em_video_channel_set        (void             *video,
                                                   int               channel);

static int            em_video_channel_get        (void             *video);

static const char    *em_video_channel_name_get   (void             *video,
                                                   int               channel);

static void           em_video_channel_mute_set   (void             *video,
                                                   int               mute);

static int            em_video_channel_mute_get   (void             *video);

static int            em_audio_channel_count      (void             *video);

static void           em_audio_channel_set        (void             *video,
                                                   int               channel);

static int            em_audio_channel_get        (void             *video);

static const char    *em_audio_channel_name_get   (void             *video,
                                                   int               channel);

static void           em_audio_channel_mute_set   (void             *video,
                                                   int               mute);

static int            em_audio_channel_mute_get   (void             *video);

static void           em_audio_channel_volume_set (void             *video,
                                                   double             vol);

static double         em_audio_channel_volume_get (void             *video);

static int            em_spu_channel_count        (void             *video);

static void           em_spu_channel_set          (void             *video,
                                                   int               channel);

static int            em_spu_channel_get          (void             *video);

static const char    *em_spu_channel_name_get     (void             *video,
                                                   int               channel);

static void           em_spu_channel_mute_set     (void             *video,
                                                   int               mute);

static int            em_spu_channel_mute_get     (void             *video);

static int            em_chapter_count            (void             *video);

static void           em_chapter_set              (void             *video,
                                                   int               chapter);

static int            em_chapter_get              (void             *video);

static const char    *em_chapter_name_get         (void             *video,
                                                   int               chapter);

static void           em_speed_set                (void             *video,
                                                   double            speed);

static double         em_speed_get                (void             *video);

static int            em_eject                    (void             *video);

static const char    *em_meta_get                 (void             *video,
                                                   int               meta);

/* Module interface */

static Emotion_Video_Module em_module =
{
   em_init, /* init */
   em_shutdown, /* shutdown */
   em_file_open, /* file_open */
   em_file_close, /* file_close */
   em_play, /* play */
   em_stop, /* stop */
   em_size_get, /* size_get */
   em_pos_set, /* pos_set */
   em_vis_set, /* vis_set */
   em_len_get, /* len_get */
   em_fps_num_get, /* fps_num_get */
   em_fps_den_get, /* fps_den_get */
   em_fps_get, /* fps_get */
   em_pos_get, /* pos_get */
   em_vis_get, /* vis_get */
   em_ratio_get, /* ratio_get */
   em_video_handled, /* video_handled */
   em_audio_handled, /* audio_handled */
   em_seekable, /* seekable */
   em_frame_done, /* frame_done */
   em_format_get, /* format_get */
   em_video_data_size_get, /* video_data_size_get */
   em_yuv_rows_get, /* yuv_rows_get */
   em_bgra_data_get, /* bgra_data_get */
   em_event_feed, /* event_feed */
   em_event_mouse_button_feed, /* event_mouse_button_feed */
   em_event_mouse_move_feed, /* event_mouse_move_feed */
   em_video_channel_count, /* video_channel_count */
   em_video_channel_set, /* video_channel_set */
   em_video_channel_get, /* video_channel_get */
   em_video_channel_name_get, /* video_channel_name_get */
   em_video_channel_mute_set, /* video_channel_mute_set */
   em_video_channel_mute_get, /* video_channel_mute_get */
   em_audio_channel_count, /* audio_channel_count */
   em_audio_channel_set, /* audio_channel_set */
   em_audio_channel_get, /* audio_channel_get */
   em_audio_channel_name_get, /* audio_channel_name_get */
   em_audio_channel_mute_set, /* audio_channel_mute_set */
   em_audio_channel_mute_get, /* audio_channel_mute_get */
   em_audio_channel_volume_set, /* audio_channel_volume_set */
   em_audio_channel_volume_get, /* audio_channel_volume_get */
   em_spu_channel_count, /* spu_channel_count */
   em_spu_channel_set, /* spu_channel_set */
   em_spu_channel_get, /* spu_channel_get */
   em_spu_channel_name_get, /* spu_channel_name_get */
   em_spu_channel_mute_set, /* spu_channel_mute_set */
   em_spu_channel_mute_get, /* spu_channel_mute_get */
   em_chapter_count, /* chapter_count */
   em_chapter_set, /* chapter_set */
   em_chapter_get, /* chapter_get */
   em_chapter_name_get, /* chapter_name_get */
   em_speed_set, /* speed_set */
   em_speed_get, /* speed_get */
   em_eject, /* eject */
   em_meta_get /* meta_get */
};

static unsigned char
em_init(Evas_Object  *obj,
	void        **emotion_video)
{
   Emotion_Gstreamer_Video *ev;
   GError                  *error;
   int                      fds[2];

   if (!emotion_video)
      return 0;

   printf ("Init gstreamer...\n");

   ev = calloc(1, sizeof(Emotion_Gstreamer_Video));
   if (!ev) return 0;

   ev->obj = obj;
   ev->obj_data = NULL;

   /* Initialization of gstreamer */
   if (!gst_init_check (NULL, NULL, &error))
     goto failure_gstreamer;

   ev->pipeline = gst_pipeline_new ("pipeline");
   if (!ev->pipeline)
     goto failure_pipeline;

   ev->eos_bus = gst_pipeline_get_bus (GST_PIPELINE (ev->pipeline));
   if (!ev->eos_bus)
     goto failure_bus;

   /* We allocate the sinks lists */
   ev->video_sinks = ecore_list_new ();
   if (!ev->video_sinks)
     goto failure_video_sinks;
   ecore_list_set_free_cb(ev->video_sinks, ECORE_FREE_CB(free));
   ev->audio_sinks = ecore_list_new ();
   if (!ev->audio_sinks)
     goto failure_audio_sinks;
   ecore_list_set_free_cb(ev->audio_sinks, ECORE_FREE_CB(free));

   *emotion_video = ev;

   /* Default values */
   ev->ratio = 1.0;
   ev->video_sink_nbr = 0;
   ev->audio_sink_nbr = 0;
   ev->vis = EMOTION_VIS_LIBVISUAL_GOOM;

   /* Create the file descriptors */
   if (pipe(fds) == 0) {
      ev->fd_ev_read = fds[0];
      ev->fd_ev_write = fds[1];
      fcntl(ev->fd_ev_read, F_SETFL, O_NONBLOCK);
      ev->fd_ev_handler = ecore_main_fd_handler_add(ev->fd_ev_read,
                                                    ECORE_FD_READ,
                                                    _em_fd_ev_active,
                                                    ev,
                                                    NULL, NULL);
      ecore_main_fd_handler_active_set(ev->fd_ev_handler, ECORE_FD_READ);
   }
   else
     goto failure_pipe;

   return 1;

 failure_pipe:
   ecore_list_destroy (ev->audio_sinks);
 failure_audio_sinks:
   ecore_list_destroy (ev->video_sinks);
 failure_video_sinks:
   gst_object_unref (GST_OBJECT (ev->eos_bus));
 failure_bus:
   /* this call is not really necessary */
   gst_element_set_state (ev->pipeline, GST_STATE_NULL);
   gst_object_unref (GST_OBJECT (ev->pipeline));
 failure_pipeline:
   gst_deinit ();
 failure_gstreamer:
   free (ev);

   return 0;
}

static int
em_shutdown(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev)
     return 0;

   gst_element_set_state (ev->pipeline, GST_STATE_NULL);
   gst_object_unref (GST_OBJECT (ev->pipeline));
   gst_object_unref (GST_OBJECT (ev->eos_bus));
   gst_deinit ();

   ecore_list_destroy (ev->video_sinks);
   ecore_list_destroy (ev->audio_sinks);

   /* FIXME: and the evas object ? */

   ecore_main_fd_handler_del(ev->fd_ev_handler);
   close(ev->fd_ev_write);
   close(ev->fd_ev_read);

   free(ev);

   return 1;
}

static unsigned char
em_file_open(const char   *file,
	     Evas_Object  *obj,
	     void         *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   /* Evas Object */
   ev->obj = obj;

   /* CD Audio */
   if (strstr (file,"cdda://")) {
      const char  *device = NULL;
      unsigned int track = 1;

      device = file + strlen ("cdda://");
      if (device[0] == '/') {
         char *tmp;

         if ((tmp = strchr (device, '?')) || (tmp = strchr (device, '#'))) {
            sscanf (tmp + 1,"%d", &track);
            tmp[0] = '\0';
         }
      }
      else {
         device = NULL;
         sscanf (file,"cdda://%d", &track);
      }
      printf ("build cdda pipeline\n");
      if (!(_cdda_pipeline_build (ev, device, track))) {
        printf ("error building CA Audio pipeline\n");
        return 0;
      }
   }
   /* Normal media file */
   else {
      const char *filename;

      filename = strstr (file,"file://")
        ? file + strlen ("file://")
        : file;

      printf ("build file pipeline \n");
      if (!(_file_pipeline_build (ev, filename))) {
        printf ("error building File pipeline\n");
        return 0;
      }
   }

   ev->position = 0.0;

   {
      /* on recapitule : */
     Emotion_Video_Sink *vsink;
     Emotion_Audio_Sink *asink;

     vsink = (Emotion_Video_Sink *)ecore_list_goto_first (ev->video_sinks);
     if (vsink) {
        g_print ("video : \n");
        g_print ("  size   : %dx%d\n", vsink->width, vsink->height);
        g_print ("  fps    : %d/%d\n", vsink->fps_num, vsink->fps_den);
        g_print ("  fourcc : %" GST_FOURCC_FORMAT "\n", GST_FOURCC_ARGS (vsink->fourcc));
        g_print ("  length : %" GST_TIME_FORMAT "\n\n",
                 GST_TIME_ARGS ((guint64)(vsink->length_time * GST_SECOND)));
     }

     asink = (Emotion_Audio_Sink *)ecore_list_goto_first (ev->audio_sinks);
     if (asink) {
        g_print ("audio : \n");
        g_print ("  chan   : %d\n", asink->channels);
        g_print ("  rate   : %d\n", asink->samplerate);
        g_print ("  length : %" GST_TIME_FORMAT "\n\n",
                 GST_TIME_ARGS ((guint64)(asink->length_time * GST_SECOND)));
     }
   }

   return 1;
}

static void
em_file_close(void *video)
{
   Emotion_Gstreamer_Video *ev;
   GstIterator             *iter;
   gpointer                 data;

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev)
     return;

   printf("EX pause end...\n");
   if (!emotion_object_play_get(ev->obj))
     {
	printf("  ... unpause\n");
        _emotion_pipeline_pause (ev->pipeline);
     }
   printf("EX stop\n");
   if (ev->pipeline)
     gst_element_set_state (ev->pipeline, GST_STATE_READY);

   /* we remove all the elements in the pipeline */
   iter = gst_bin_iterate_elements (GST_BIN (ev->pipeline));
   while (gst_iterator_next (iter, &data) == GST_ITERATOR_OK) {
     GstElement *element;

     element = GST_ELEMENT (data);
     gst_bin_remove (GST_BIN (ev->pipeline), element);
   }
   gst_iterator_free (iter);

   /* we clear the sink lists */
   ecore_list_clear (ev->video_sinks);
   ecore_list_clear (ev->audio_sinks);

   /* shutdown eos */
   if (ev->eos_timer) {
     ecore_timer_del (ev->eos_timer);
     ev->eos_timer = NULL;
   }
}

static void
em_play(void   *video,
	double  pos)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
   gst_element_set_state (ev->pipeline, GST_STATE_PLAYING);
   ev->play = 1;

   /* eos */
   ev->eos_timer = ecore_timer_add (0.1, _eos_timer_fct, ev);
}

static void
em_stop(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   gst_element_set_state (ev->pipeline, GST_STATE_PAUSED);
   ev->play = 0;

   /* shutdown eos */
   if (ev->eos_timer) {
     ecore_timer_del (ev->eos_timer);
     ev->eos_timer = NULL;
   }
}

static void
em_size_get(void  *video,
	    int   *width,
	    int   *height)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink) {
      if (width) *width = vsink->width;
      if (height) *height = vsink->height;
   }
   else {
      if (width) *width = 0;
      if (height) *height = 0;
   }
}

static void
em_pos_set(void   *video,
	   double  pos)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;
   Emotion_Audio_Sink      *asink;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ev->seek_to_pos == pos) return;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   asink = (Emotion_Audio_Sink *)ecore_list_goto_index (ev->video_sinks, ev->audio_sink_nbr);

   if (vsink) {
      gst_element_seek(vsink->sink, 1.0,
                       GST_FORMAT_TIME,
                       GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH,
                       GST_SEEK_TYPE_SET,
                       (gint64)(pos * (double)GST_SECOND),
                       GST_SEEK_TYPE_NONE,
                       -1);
   }
   if (asink) {
      gst_element_seek(asink->sink, 1.0,
                       GST_FORMAT_TIME,
                       GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH,
                       GST_SEEK_TYPE_SET,
                       (gint64)(pos * (double)GST_SECOND),
                       GST_SEEK_TYPE_NONE,
                       -1);
   }
   ev->seek_to_pos = pos;
}

static void
em_vis_set(void       *video,
	   Emotion_Vis vis)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ev->vis == vis) return;
   ev->vis = vis;
}

static double
em_len_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink)
      return (double)vsink->length_time;

   return 0.0;
}

static int
em_fps_num_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink)
      return vsink->fps_num;

   return 0;
}

static int
em_fps_den_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink)
      return vsink->fps_den;

   return 1;
}

static double
em_fps_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink)
      return (double)vsink->fps_num / (double)vsink->fps_den;

   return 0.0;
}

static double
em_pos_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->position;
}

static Emotion_Vis
em_vis_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->vis;
}

static double
em_ratio_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->ratio;
}

static int
em_video_handled(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ecore_list_is_empty (ev->video_sinks))
     return 0;

   return 1;
}

static int
em_audio_handled(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ecore_list_is_empty (ev->audio_sinks))
     return 0;

   return 1;
}

static int
em_seekable(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 1;
}

static void
em_frame_done(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static Emotion_Format
em_format_get (void *video)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink) {
      switch (vsink->fourcc) {
      case GST_MAKE_FOURCC ('I','4','2','0'):
         return EMOTION_FORMAT_I420;
      case GST_MAKE_FOURCC ('Y','V','1','2'):
         return EMOTION_FORMAT_YV12;
      case GST_MAKE_FOURCC ('Y','U','Y','2'):
         return EMOTION_FORMAT_YUY2;
      case GST_MAKE_FOURCC ('A','R','G','B'):
         return EMOTION_FORMAT_BGRA;
      default:
         return EMOTION_FORMAT_NONE;
      }
   }
   return EMOTION_FORMAT_NONE;
}

static void
em_video_data_size_get(void *video, int *w, int *h)
{
   Emotion_Gstreamer_Video *ev;
   Emotion_Video_Sink      *vsink;

   ev = (Emotion_Gstreamer_Video *)video;

   vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
   if (vsink) {
      *w = vsink->width;
      *h = vsink->height;
   }
   else {
      *w = 0;
      *h = 0;
   }
}

static int
em_yuv_rows_get(void           *video,
		int             w,
		int             h,
		unsigned char **yrows,
		unsigned char **urows,
		unsigned char **vrows)
{
   Emotion_Gstreamer_Video *ev;
   int                      i;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ev->obj_data)
     {
       if (em_format_get(video) == EMOTION_FORMAT_I420) {
         for (i = 0; i < h; i++)
           yrows[i] = &ev->obj_data[i * w];

         for (i = 0; i < (h / 2); i++)
           urows[i] = &ev->obj_data[h * w + i * (w / 2) ];

         for (i = 0; i < (h / 2); i++)
           vrows[i] = &ev->obj_data[h * w + h * (w /4) + i * (w / 2)];
       }
       else if (em_format_get(video) == EMOTION_FORMAT_YV12) {
         for (i = 0; i < h; i++)
           yrows[i] = &ev->obj_data[i * w];

         for (i = 0; i < (h / 2); i++)
           vrows[i] = &ev->obj_data[h * w + i * (w / 2) ];

         for (i = 0; i < (h / 2); i++)
           urows[i] = &ev->obj_data[h * w + h * (w /4) + i * (w / 2)];
       }
       else
         return 0;

       return 1;
     }

   return 0;
}

static int
em_bgra_data_get(void *video, unsigned char **bgra_data)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (ev->obj_data && em_format_get(video) == EMOTION_FORMAT_BGRA) {
      *bgra_data = ev->obj_data;
      return 1;
   }
   return 0;
}

static void
em_event_feed(void *video, int event)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static void
em_event_mouse_button_feed(void *video, int button, int x, int y)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static void
em_event_mouse_move_feed(void *video, int x, int y)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

/* Video channels */
static int
em_video_channel_count(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ecore_list_nodes(ev->video_sinks);
}

static void
em_video_channel_set(void *video,
		     int   channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (channel < 0) channel = 0;
   /* FIXME: a faire... */
}

static int
em_video_channel_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->video_sink_nbr;
}

static const char *
em_video_channel_name_get(void *video,
			  int   channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return NULL;
}

static void
em_video_channel_mute_set(void *video,
			  int   mute)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   ev->video_mute = mute;
}

static int
em_video_channel_mute_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->video_mute;
}

/* Audio channels */

static int
em_audio_channel_count(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ecore_list_nodes(ev->audio_sinks);
}

static void
em_audio_channel_set(void *video,
		     int   channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (channel < -1) channel = -1;
   /* FIXME: a faire... */
}

static int
em_audio_channel_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->audio_sink_nbr;
}

static const char *
em_audio_channel_name_get(void *video,
			  int   channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return NULL;
}

static void
em_audio_channel_mute_set(void *video,
			  int   mute)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   ev->audio_mute = mute;
   /* FIXME: a faire ... */
}

static int
em_audio_channel_mute_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return ev->audio_mute;
}

static void
em_audio_channel_volume_set(void  *video,
			    double vol)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   if (vol < 0.0)
     vol = 0.0;
   if (vol > 100.0)
     vol = 100.0;
   g_object_set (G_OBJECT (ev->pipeline), "volume",
		 vol / 100.0, NULL);
}

static double
em_audio_channel_volume_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   double                   vol;

   ev = (Emotion_Gstreamer_Video *)video;

   g_object_get (G_OBJECT (ev->pipeline), "volume", &vol, NULL);

   return vol*100.0;
}

/* spu stuff */

static int
em_spu_channel_count(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 0;
}

static void
em_spu_channel_set(void *video, int channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static int
em_spu_channel_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 1;
}

static const char *
em_spu_channel_name_get(void *video, int channel)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
   return NULL;
}

static void
em_spu_channel_mute_set(void *video, int mute)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static int
em_spu_channel_mute_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 0;
}

static int
em_chapter_count(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
   return 0;
}

static void
em_chapter_set(void *video, int chapter)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static int
em_chapter_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 0;
}

static const char *
em_chapter_name_get(void *video, int chapter)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return NULL;
}

static void
em_speed_set(void *video, double speed)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
}

static double
em_speed_get(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 1.0;
}

static int
em_eject(void *video)
{
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;

   return 1;
}

static const char *
em_meta_get(void *video, int meta)
{
   Emotion_Gstreamer_Video *ev;
   GstBus                  *bus;
   gchar                   *str = NULL;
   gboolean                 done;

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev) return NULL;

   done = FALSE;
   bus = gst_element_get_bus (ev->pipeline);
   if (!bus) return NULL;

   while (!done) {
      GstMessage *message;

      message = gst_bus_pop (bus);
      if (message == NULL)
        /* All messages read, we're done */
         break;

      switch (GST_MESSAGE_TYPE (message)) {
      case GST_MESSAGE_ERROR:
      case GST_MESSAGE_EOS:
         gst_message_unref (message);
         break;
      case GST_MESSAGE_TAG: {
         GstTagList *new_tags;

         gst_message_parse_tag (message, &new_tags);

         switch (meta) {
         case META_TRACK_TITLE:
            gst_tag_list_get_string (new_tags, GST_TAG_TITLE, &str);
            if (str) done = TRUE;
            break;
         case META_TRACK_ARTIST:
            gst_tag_list_get_string (new_tags, GST_TAG_ARTIST, &str);
            if (str) done = TRUE;
            break;
         case META_TRACK_GENRE:
            gst_tag_list_get_string (new_tags, GST_TAG_GENRE, &str);
            if (str) done = TRUE;
            break;
         case META_TRACK_COMMENT:
            gst_tag_list_get_string (new_tags, GST_TAG_COMMENT, &str);
            if (str) done = TRUE;
            break;
         case META_TRACK_ALBUM:
            gst_tag_list_get_string (new_tags, GST_TAG_ALBUM, &str);
            if (str) done = TRUE;
            break;
         case META_TRACK_YEAR: {
            const GValue *date;

            date = gst_tag_list_get_value_index (new_tags, GST_TAG_DATE, 0);
            if (date)
               str = g_strdup_value_contents (date);
            if (str) done = TRUE;
            break;
         }
         case META_TRACK_DISCID:
#ifdef GST_TAG_CDDA_CDDB_DISCID
            gst_tag_list_get_string (new_tags, GST_TAG_CDDA_CDDB_DISCID, &str);
#endif
            if (str) done = TRUE;
            break;
         }
         break;
      }
      default:
         break;
      }
      gst_message_unref (message);
   }

   gst_object_unref (GST_OBJECT (bus));

   return str;
}

unsigned char
module_open(Evas_Object           *obj,
	    Emotion_Video_Module **module,
	    void                 **video)
{
   if (!module)
      return 0;

   if (!em_module.init(obj, video))
      return 0;

   *module = &em_module;
   return 1;
}

void
module_close(Emotion_Video_Module *module,
	     void                 *video)
{
   em_module.shutdown(video);
}

/* Send the video frame to the evas object */
static void
cb_handoff (GstElement *fakesrc,
	    GstBuffer  *buffer,
	    GstPad     *pad,
	    gpointer    user_data)
{
   GstQuery *query;
   void *buf[2];

   Emotion_Gstreamer_Video *ev = ( Emotion_Gstreamer_Video *) user_data;
   if (!ev)
     return;

   if (!ev->video_mute) {
      if (!ev->obj_data)
         ev->obj_data = (void*) malloc (GST_BUFFER_SIZE(buffer) * sizeof(void));

      memcpy ( ev->obj_data, GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer));
      buf[0] = GST_BUFFER_DATA(buffer);
      buf[1] = buffer;
      write(ev->fd_ev_write, buf, sizeof(buf));
   }

   query = gst_query_new_position (GST_FORMAT_TIME);
   if (gst_pad_query (gst_pad_get_peer (pad), query)) {
      gint64 position;

      gst_query_parse_position (query, NULL, &position);
      ev->position = (double)position / (double)GST_SECOND;
   }
   gst_query_unref (query);
}

static void
new_decoded_pad_cb (GstElement *decodebin,
                    GstPad     *new_pad,
                    gboolean    last,
                    gpointer    user_data)
{
   Emotion_Gstreamer_Video *ev;
   GstCaps *caps;
   gchar   *str;

   ev = (Emotion_Gstreamer_Video *)user_data;
   caps = gst_pad_get_caps (new_pad);
   str = gst_caps_to_string (caps);
   /* video stream */
   if (g_str_has_prefix (str, "video/")) {
      Emotion_Video_Sink *vsink;
      GstElement         *queue;
      GstPad             *videopad;

      vsink = (Emotion_Video_Sink *)malloc (sizeof (Emotion_Video_Sink));
      if (!vsink) return;
      if (!ecore_list_append (ev->video_sinks, vsink)) {
         free(vsink);
         return;
      }

      queue = gst_element_factory_make ("queue", NULL);
      vsink->sink = gst_element_factory_make ("fakesink", "videosink");
      gst_bin_add_many (GST_BIN (ev->pipeline), queue, vsink->sink, NULL);
      gst_element_link (queue, vsink->sink);
      videopad = gst_element_get_pad (queue, "sink");
      gst_pad_link(new_pad, videopad);
      gst_object_unref (videopad);
      if (ecore_list_nodes(ev->video_sinks) == 1) {
         ev->ratio = (double)vsink->width / (double)vsink->height;
      }
      gst_element_set_state (queue, GST_STATE_PAUSED);
      gst_element_set_state (vsink->sink, GST_STATE_PAUSED);
   }
   /* audio stream */
   else if (g_str_has_prefix (str, "audio/")) {
      Emotion_Audio_Sink *asink;
      GstPad             *audiopad;

      asink = (Emotion_Audio_Sink *)malloc (sizeof (Emotion_Audio_Sink));
      if (!asink) return;
      if (!ecore_list_append (ev->audio_sinks, asink)) {
         free(asink);
         return;
      }

      asink->sink = _em_audio_sink_create (ev, ecore_list_index (ev->audio_sinks));
      gst_bin_add (GST_BIN (ev->pipeline), asink->sink);
      audiopad = gst_element_get_pad (asink->sink, "sink");
      gst_pad_link(new_pad, audiopad);
      gst_element_set_state (asink->sink, GST_STATE_PAUSED);
   }
}

static int
_em_fd_ev_active(void *data, Ecore_Fd_Handler *fdh)
{
   int fd;
   int len;
   void *buf[1];
   unsigned char *frame_data;
   Emotion_Gstreamer_Video *ev;
   GstBuffer  *buffer;

   ev = data;
   fd = ecore_main_fd_handler_fd_get(fdh);

   while ((len = read(fd, buf, sizeof(buf))) > 0)
     {
        if (len == sizeof(buf))
          {
             Emotion_Video_Sink *vsink;

             frame_data = buf[0];
             buffer = buf[1];
             _emotion_frame_new(ev->obj);
             vsink = (Emotion_Video_Sink *)ecore_list_goto_index (ev->video_sinks, ev->video_sink_nbr);
             _emotion_video_pos_update(ev->obj, ev->position, vsink->length_time);
          }
     }
   return 1;
}

static Emotion_Video_Sink *
_emotion_video_sink_new (Emotion_Gstreamer_Video *ev)
{
   Emotion_Video_Sink *vsink;

   if (!ev) return NULL;

   vsink = (Emotion_Video_Sink *)malloc (sizeof (Emotion_Video_Sink));
   if (!vsink) return NULL;

   if (!ecore_list_append (ev->video_sinks, vsink)) {
     free (vsink);
     return NULL;
   }
   return vsink;
}

static void
_emotion_video_sink_free (Emotion_Gstreamer_Video *ev, Emotion_Video_Sink *vsink)
{
   if (!ev || !vsink) return;

   if (ecore_list_goto (ev->video_sinks, vsink)) {
      ecore_list_remove (ev->video_sinks);
      free (vsink);
   }
}

static Emotion_Audio_Sink *
_emotion_audio_sink_new (Emotion_Gstreamer_Video *ev)
{
   Emotion_Audio_Sink *asink;

   if (!ev) return NULL;

   asink = (Emotion_Audio_Sink *)malloc (sizeof (Emotion_Audio_Sink));
   if (!asink) return NULL;

   if (!ecore_list_append (ev->audio_sinks, asink)) {
     free (asink);
     return NULL;
   }
   return asink;
}

static void
_emotion_audio_sink_free (Emotion_Gstreamer_Video *ev, Emotion_Audio_Sink *asink)
{
   if (!ev || !asink) return;

   if (ecore_list_goto (ev->audio_sinks, asink)) {
      ecore_list_remove (ev->audio_sinks);
      free (asink);
   }
}

static Emotion_Video_Sink *
_visualization_sink_create (Emotion_Gstreamer_Video *ev, Emotion_Audio_Sink *asink)
{
   Emotion_Video_Sink *vsink;

   if (!ev) return NULL;

   vsink = _emotion_video_sink_new (ev);
   if (!vsink) return NULL;

   vsink->sink = gst_bin_get_by_name (GST_BIN (asink->sink), "vissink1");
   if (!vsink->sink) {
      _emotion_video_sink_free (ev, vsink);
      return NULL;
   }
   vsink->width = 320;
   vsink->height = 200;
   ev->ratio = (double)vsink->width / (double)vsink->height;
   vsink->fps_num = 25;
   vsink->fps_den = 1;
   vsink->fourcc = GST_MAKE_FOURCC ('A','R','G','B');
   vsink->length_time = asink->length_time;

   g_object_set (G_OBJECT (vsink->sink), "sync", TRUE, NULL);
   g_object_set (G_OBJECT (vsink->sink), "signal-handoffs", TRUE, NULL);
   g_signal_connect (G_OBJECT (vsink->sink),
                     "handoff",
                     G_CALLBACK (cb_handoff), ev);
   return vsink;
}

static GstElement *
_em_audio_sink_create (Emotion_Gstreamer_Video *ev, int index)
{
   gchar       buf[128];
   GstElement *bin;
   GstElement *audiobin;
   GstElement *visbin = NULL;
   GstElement *tee;
   GstPad     *teepad;
   GstPad     *binpad;

   /* audio sink */
   bin = gst_bin_new (NULL);
   if (!bin) return NULL;

   g_snprintf (buf, 128, "tee%d", index);
   tee = gst_element_factory_make ("tee", buf);

   /* audio part */
   {
     GstElement *queue;
     GstElement *conv;
     GstElement *resample;
     GstElement *sink;
     GstPad     *audiopad;

     audiobin = gst_bin_new (NULL);

     queue = gst_element_factory_make ("queue", NULL);
     conv = gst_element_factory_make ("audioconvert", NULL);
     resample = gst_element_factory_make ("audioresample", NULL);
     if (index == 1)
       sink = gst_element_factory_make ("autoaudiosink", NULL);
     else
       sink = gst_element_factory_make ("fakesink", NULL);

     gst_bin_add_many (GST_BIN (audiobin),
                       queue, conv, resample, sink, NULL);
     gst_element_link_many (queue, conv, resample, sink, NULL);

     audiopad = gst_element_get_pad (queue, "sink");
     gst_element_add_pad (audiobin, gst_ghost_pad_new ("sink", audiopad));
     gst_object_unref (audiopad);
   }

   /* visualisation part */
   {
     GstElement *vis = NULL;
     char       *vis_name;

     switch (ev->vis) {
     case EMOTION_VIS_GOOM:
       vis_name = "goom";
       break;
     case EMOTION_VIS_LIBVISUAL_BUMPSCOPE:
       vis_name = "libvisual_bumpscope";
       break;
     case EMOTION_VIS_LIBVISUAL_CORONA:
       vis_name = "libvisual_corona";
       break;
     case EMOTION_VIS_LIBVISUAL_DANCING_PARTICLES:
       vis_name = "libvisual_dancingparticles";
       break;
     case EMOTION_VIS_LIBVISUAL_GDKPIXBUF:
       vis_name = "libvisual_gdkpixbuf";
       break;
     case EMOTION_VIS_LIBVISUAL_G_FORCE:
       vis_name = "libvisual_G-Force";
       break;
     case EMOTION_VIS_LIBVISUAL_GOOM:
       vis_name = "libvisual_goom";
       break;
     case EMOTION_VIS_LIBVISUAL_INFINITE:
       vis_name = "libvisual_infinite";
       break;
     case EMOTION_VIS_LIBVISUAL_JAKDAW:
       vis_name = "libvisual_jakdaw";
       break;
     case EMOTION_VIS_LIBVISUAL_JESS:
       vis_name = "libvisual_jess";
       break;
     case EMOTION_VIS_LIBVISUAL_LV_ANALYSER:
       vis_name = "libvisual_lv_analyzer";
       break;
     case EMOTION_VIS_LIBVISUAL_LV_FLOWER:
       vis_name = "libvisual_lv_flower";
       break;
     case EMOTION_VIS_LIBVISUAL_LV_GLTEST:
       vis_name = "libvisual_lv_gltest";
       break;
     case EMOTION_VIS_LIBVISUAL_LV_SCOPE:
       vis_name = "libvisual_lv_scope";
       break;
     case EMOTION_VIS_LIBVISUAL_MADSPIN:
       vis_name = "libvisual_madspin";
       break;
     case EMOTION_VIS_LIBVISUAL_NEBULUS:
       vis_name = "libvisual_nebulus";
       break;
     case EMOTION_VIS_LIBVISUAL_OINKSIE:
       vis_name = "libvisual_oinksie";
       break;
     case EMOTION_VIS_LIBVISUAL_PLASMA:
       vis_name = "libvisual_plazma";
       break;
     default:
       vis_name = "goom";
       break;
     }

     g_snprintf (buf, 128, "vis%d", index);
     if ((vis = gst_element_factory_make (vis_name, buf))) {
       GstElement *queue;
       GstElement *conv;
       GstElement *cspace;
       GstElement *sink;
       GstPad     *vispad;
       GstCaps    *caps;

       g_snprintf (buf, 128, "visbin%d", index);
       visbin = gst_bin_new (buf);

       queue = gst_element_factory_make ("queue", NULL);
       conv = gst_element_factory_make ("audioconvert", NULL);
       cspace = gst_element_factory_make ("ffmpegcolorspace", NULL);
       g_snprintf (buf, 128, "vissink%d", index);
       sink = gst_element_factory_make ("fakesink", buf);

       gst_bin_add_many (GST_BIN (visbin),
                         queue, conv, vis, cspace, sink, NULL);
       gst_element_link_many (queue, conv, vis, cspace, NULL);
       caps = gst_caps_new_simple ("video/x-raw-rgb",
                                   "bpp", G_TYPE_INT, 32,
                                   "width", G_TYPE_INT, 320,
                                   "height", G_TYPE_INT, 200,
                                   NULL);
       gst_element_link_filtered (cspace, sink, caps);

       vispad = gst_element_get_pad (queue, "sink");
       gst_element_add_pad (visbin, gst_ghost_pad_new ("sink", vispad));
       gst_object_unref (vispad);
     }
   }

   gst_bin_add_many (GST_BIN (bin), tee, audiobin, NULL);
   if (visbin)
     gst_bin_add (GST_BIN (bin), visbin);

   binpad = gst_element_get_pad (audiobin, "sink");
   teepad = gst_element_get_request_pad (tee, "src%d");
   gst_pad_link (teepad, binpad);
   gst_object_unref (teepad);
   gst_object_unref (binpad);

   if (visbin) {
      binpad = gst_element_get_pad (visbin, "sink");
      teepad = gst_element_get_request_pad (tee, "src%d");
      gst_pad_link (teepad, binpad);
      gst_object_unref (teepad);
      gst_object_unref (binpad);
   }

   teepad = gst_element_get_pad (tee, "sink");
   gst_element_add_pad (bin, gst_ghost_pad_new ("sink", teepad));
   gst_object_unref (teepad);

   return bin;
}

static int
_cdda_pipeline_build (void *video, const char * device, unsigned int track)
{
   GstElement              *cdiocddasrc;
   Emotion_Video_Sink      *vsink;
   Emotion_Audio_Sink      *asink;
   Emotion_Gstreamer_Video *ev;
/*    GstFormat                format; */
/*    gint64                  tracks_count; */

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev) return 0;

   cdiocddasrc = gst_element_factory_make ("cdiocddasrc", "src");
   if (!cdiocddasrc) {
     g_print ("cdiocddasrc element missing. Install it.\n");
     return 0;
   }

   if (device)
      g_object_set (G_OBJECT (cdiocddasrc), "device", device, NULL);

   g_object_set (G_OBJECT (cdiocddasrc), "track", track, NULL);

   asink = _emotion_audio_sink_new (ev);
   if (!asink)
     goto failure_emotion_sink;

   asink->sink = _em_audio_sink_create (ev,  1);
   if (!asink->sink)
     goto failure_gstreamer_sink;

   gst_bin_add_many((GST_BIN (ev->pipeline)), cdiocddasrc, asink->sink, NULL);

   if (!gst_element_link (cdiocddasrc, asink->sink))
     goto failure_link;

   vsink = _visualization_sink_create (ev, asink);
   if (!vsink) goto failure_link;

   if (!_emotion_pipeline_pause (ev->pipeline))
     goto failure_gstreamer_pause;

   {
     gint tracks_count;
     tracks_count = _cdda_track_count_get(ev);
     g_print ("Tracks count : %d\n", tracks_count);
   }

   {
      GstQuery *query;
      GstPad   *pad;
      GstCaps  *caps;
      GstStructure *structure;

      /* should always be found */
      pad = gst_element_get_pad (cdiocddasrc, "src");

      caps = gst_pad_get_caps (pad);
      structure = gst_caps_get_structure (GST_CAPS (caps), 0);

      gst_structure_get_int (structure, "channels", &asink->channels);
      gst_structure_get_int (structure, "rate", &asink->samplerate);

      gst_caps_unref (caps);

      query = gst_query_new_duration (GST_FORMAT_TIME);
      if (gst_pad_query (pad, query)) {
         gint64 time;

         gst_query_parse_duration (query, NULL, &time);
         asink->length_time = (double)time / (double)GST_SECOND;
         vsink->length_time = asink->length_time;
      }
      gst_query_unref (query);
      gst_object_unref (GST_OBJECT (pad));
   }

   return 1;

 failure_gstreamer_pause:
   _emotion_video_sink_free (ev, vsink);
 failure_link:
   gst_bin_remove (GST_BIN (ev->pipeline), asink->sink);
   gst_bin_remove (GST_BIN (ev->pipeline), cdiocddasrc);
 failure_gstreamer_sink:
   _emotion_audio_sink_free (ev, asink);
 failure_emotion_sink:
   /*  may be NULL because of gst_bin_remove above */
   if (cdiocddasrc)
      gst_object_unref (GST_OBJECT (cdiocddasrc));

   return 0;
}

static int
_file_pipeline_build (void *video, const char *file)
{
   GstElement              *filesrc;
   GstElement              *decodebin;
   Emotion_Gstreamer_Video *ev;

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev) return 0;

   filesrc = gst_element_factory_make ("filesrc", "src");
   if (!filesrc)
      return 0;
   g_object_set (G_OBJECT (filesrc), "location", file, NULL);

   decodebin = gst_element_factory_make ("decodebin", "decodebin");
   if (!decodebin)
     goto failure_decodebin;
   g_signal_connect (decodebin, "new-decoded-pad",
                     G_CALLBACK (new_decoded_pad_cb), ev);

   gst_bin_add_many (GST_BIN (ev->pipeline), filesrc, decodebin, NULL);
   if (!gst_element_link (filesrc, decodebin))
     goto failure_link;

   if (!_emotion_pipeline_pause (ev->pipeline))
     goto failure_gstreamer_pause;

   /* We get the informations of streams */
   ecore_list_goto_first (ev->video_sinks);
   ecore_list_goto_first (ev->audio_sinks);

   {
      GstIterator *it;
      gpointer     data;

      it = gst_element_iterate_src_pads (decodebin);
      while (gst_iterator_next (it, &data) == GST_ITERATOR_OK) {
         GstPad  *pad;
         GstCaps *caps;
         gchar   *str;

         pad = GST_PAD (data);

         caps = gst_pad_get_caps (pad);
         str = gst_caps_to_string (caps);
         /* video stream */
         if (g_str_has_prefix (str, "video/")) {
            Emotion_Video_Sink *vsink;
            GstStructure       *structure;
            const GValue       *val;
            GstQuery           *query;

            vsink = (Emotion_Video_Sink *)ecore_list_next (ev->video_sinks);
            structure = gst_caps_get_structure (GST_CAPS (caps), 0);

            gst_structure_get_int (structure, "width", &vsink->width);
            gst_structure_get_int (structure, "height", &vsink->height);

            vsink->fps_num = 1;
            vsink->fps_den = 1;
            val = gst_structure_get_value (structure, "framerate");
            if (val) {
               vsink->fps_num = gst_value_get_fraction_numerator (val);
               vsink->fps_den = gst_value_get_fraction_denominator (val);
            }
            if (g_str_has_prefix(str, "video/x-raw-yuv")) {
               val = gst_structure_get_value (structure, "format");
               vsink->fourcc = gst_value_get_fourcc (val);
            }
            else if (g_str_has_prefix(str, "video/x-raw-rgb")) {
              vsink->fourcc = GST_MAKE_FOURCC ('A','R','G','B');
            }
            else
               vsink->fourcc = 0;

            query = gst_query_new_duration (GST_FORMAT_TIME);
            if (gst_pad_query (pad, query)) {
               gint64 time;

               gst_query_parse_duration (query, NULL, &time);
               vsink->length_time = (double)time / (double)GST_SECOND;
            }
            gst_query_unref (query);
         }
         /* audio stream */
         else if (g_str_has_prefix (str, "audio/")) {
            Emotion_Audio_Sink *asink;
            GstStructure       *structure;
            GstQuery           *query;
            gint                index;

            asink = (Emotion_Audio_Sink *)ecore_list_next (ev->audio_sinks);

            structure = gst_caps_get_structure (GST_CAPS (caps), 0);
            gst_structure_get_int (structure, "channels", &asink->channels);
            gst_structure_get_int (structure, "rate", &asink->samplerate);

            query = gst_query_new_duration (GST_FORMAT_TIME);
            if (gst_pad_query (pad, query)) {
               gint64 time;

               gst_query_parse_duration (query, NULL, &time);
               asink->length_time = (double)time / (double)GST_SECOND;
            }
            gst_query_unref (query);

            index = ecore_list_index (ev->audio_sinks);

            if (ecore_list_nodes (ev->video_sinks) == 0) {
              if (index == 1) {
                 Emotion_Video_Sink *vsink;

                 vsink = _visualization_sink_create (ev, asink);
                 if (!vsink) goto finalize;
              }
            }
            else {
               gchar               buf[128];
               GstElement         *visbin;

               g_snprintf (buf, 128, "visbin%d", index);
               visbin = gst_bin_get_by_name (GST_BIN (ev->pipeline), buf);
               if (visbin) {
                  GstPad *srcpad;
                  GstPad *sinkpad;

                  sinkpad = gst_element_get_pad (visbin, "sink");
                  srcpad = gst_pad_get_peer (sinkpad);
                  gst_pad_unlink (srcpad, sinkpad);

                  gst_object_unref (srcpad);
                  gst_object_unref (sinkpad);
               }
            }
         }
      finalize:
          gst_caps_unref (caps);
          g_free (str);
          gst_object_unref (pad);
      }
      gst_iterator_free (it);
   }

   /* The first vsink is a valid Emotion_Video_Sink * */
   /* If no video stream is found, it's a visualisation sink */
   {
      Emotion_Video_Sink *vsink;

      vsink = (Emotion_Video_Sink *)ecore_list_goto_first (ev->video_sinks);
      if (vsink && vsink->sink) {
         g_object_set (G_OBJECT (vsink->sink), "sync", TRUE, NULL);
         g_object_set (G_OBJECT (vsink->sink), "signal-handoffs", TRUE, NULL);
         g_signal_connect (G_OBJECT (vsink->sink),
                           "handoff",
                           G_CALLBACK (cb_handoff), ev);
      }
   }

   return 1;

 failure_gstreamer_pause:
 failure_link:
   gst_element_set_state (ev->pipeline, GST_STATE_NULL);
   gst_bin_remove (GST_BIN (ev->pipeline), filesrc);
   gst_bin_remove (GST_BIN (ev->pipeline), decodebin);
 failure_decodebin:
   /*  may be NULL because of gst_bin_remove above */
   if (filesrc)
     gst_object_unref (GST_OBJECT (filesrc));

   return 0;
}

int _eos_timer_fct (void *data)
{
   Emotion_Gstreamer_Video *ev;
   GstMessage              *msg;

   ev = (Emotion_Gstreamer_Video *)data;
   while ((msg = gst_bus_poll (ev->eos_bus, GST_MESSAGE_ERROR | GST_MESSAGE_EOS, 0))) {
     switch (GST_MESSAGE_TYPE(msg)) {
     case GST_MESSAGE_ERROR: {
       gchar *debug;
       GError *err;

       gst_message_parse_error (msg, &err, &debug);
       g_free (debug);

       g_print ("Error: %s\n", err->message);
       g_error_free (err);

       break;
     }
     case GST_MESSAGE_EOS:
       if (ev->eos_timer)
         {
           ecore_timer_del(ev->eos_timer);
           ev->eos_timer = NULL;
         }
       ev->play = 0;
       _emotion_decode_stop(ev->obj);
       _emotion_playback_finished(ev->obj);
       break;
     default:
       break;
     }
     gst_message_unref (msg);
   }
   return 1;
}

static int
_cdda_track_count_get(void *video)
{
   Emotion_Gstreamer_Video *ev;
   GstBus                  *bus;
   guint                    tracks_count = 0;
   gboolean                 done;

   ev = (Emotion_Gstreamer_Video *)video;
   if (!ev) return tracks_count;

   done = FALSE;
   bus = gst_element_get_bus (ev->pipeline);
   if (!bus) return tracks_count;

   while (!done) {
      GstMessage *message;

      message = gst_bus_pop (bus);
      if (message == NULL)
        /* All messages read, we're done */
         break;

      switch (GST_MESSAGE_TYPE (message)) {
      case GST_MESSAGE_TAG: {
         GstTagList *tags;

         gst_message_parse_tag (message, &tags);

         gst_tag_list_get_uint (tags, GST_TAG_TRACK_COUNT, &tracks_count);
         if (tracks_count) done = TRUE;
         break;
      }
      case GST_MESSAGE_ERROR:
      default:
         break;
      }
      gst_message_unref (message);
   }

   gst_object_unref (GST_OBJECT (bus));

   return tracks_count;
}
