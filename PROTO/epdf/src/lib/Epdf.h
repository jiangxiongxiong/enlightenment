#ifndef __EPDF_H__
#define __EPDF_H__


/**
 * @mainpage Epdf library
 *
 * @section intro_sec Introduction
 *
 * Epdf is a library that provides a C interface to the Poppler library
 * to render PDF documents in different ways, using an Evas frontend.
 * Straight Evas can be used, but a widget implementations can
 * integrate a PDF document in an Edje user interface using an Evas
 * smart object.
 *
 * @section install_sec Installation
 *
 * The Epdf library requires Evas, Ecore and Poppler to be installed.
 *
 * @subsection api_sec API Documentation
 *
 * For the Epdf library, the complete api can be read in the
 * section @ref Epdf.
 *
 * For the Esmart object library, the complete api can be read in the
 * section @ref Esmart_Pdf.
 *
 */


/**
 * @file Epdf.h
 * @defgroup Epdf  Epdf
 * @brief A Pdf library that renders PDF documents
 *
 * The Epdf library provides a set of functions to manage PDF documents.
 * It wraps the functions of the Poppler library in a C API and uses
 * Evas as front end.
 *
 * The simplest way to use Epdf is to load a document using
 * epdf_document_new(), to create a page using epdf_page_new() and to
 * render the whole page in an Evas object using epdf_page_render(). If
 * you want to render only a part of the page, use epdf_page_render_slice().
 * If you want to view a specific page, use epdf_page_page_set().
 * Here is an example:
 *
 * @code
 * Epdf_Document *document;
 * Epdf_Page     *page;
 * char          *filename;
 *
 * document = epdf_document_new (filename);
 * if (!document) {
 *   // manage error here
 * }
 *
 * page = epdf_page_new (document);
 * if (!page) {
 *   // manage error here
 * }
 *
 * o = evas_object_image_add (evas);
 * evas_object_move (o, 0, 0);
 * epdf_page_render (page, o);
 * evas_object_show (o);
 *
 * evas_object_del (o);
 * epdf_page_delete (page);
 * epdf_document_delete (document);
 * @endcode
 */


#include <Eina.h>
#include <Evas.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EPDF_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EPDF_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif


#include "epdf_enum.h"
#include "epdf_fontinfo.h"
#include "epdf_page_transition.h"
#include "epdf_page.h"
#include "epdf_document.h"
#include "epdf_index.h"
#include "epdf_postscript.h"
#include "epdf_main.h"


#ifdef __cplusplus
}
#endif


#endif /* __EPDF_H__ */
