/* EINA - EFL data type library
 * Copyright (C) 2008 Jorge Luis Zapata Muga, Cedric Bail
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EINA_H_
#define EINA_H_

/**
 * @mainpage Eina
 * @author Jorge Luis Zapata Muga
 * @date 2008
 *
 * @section intro_sec Introduction
 *
 * The Eina library is a library that implemente an API for data types
 * in an efficient way. It also provides some useful tools like
 * openin shared libraries, errors management, type conversion,
 * time accounting and memory pool.
 *
 * This library is cross-platform and can be compiled and used on
 * Linux and Windows (XP and CE).
 *
 * The data types that are available are
 * @li array
 * @li hash table
 * @li inlined list
 * @li inlined red black tree
 * @li list
 * @li shared string
 *
 * This document describes each data type by providing the algorithms
 * used in the implementation, the performance, a short tutorial and
 * the API.
 *
 * @section eina_data_types_sec Eina Data Types
 *
 * @subsection eina_array Array
 *
 * @subsection eina_hash Hash Table
 *
 * @subsection eina_inlist Inlined List
 *
 * @subsection eina_rbtree Inlined Red Black Tree
 *
 * @subsection eina_list List
 *
 * @subsection eina_stringshare Shared String
 *
 * @section eina_tools_sec Eina Tools
 *
 * @subsection eina_convert Convert Functions
 *
 * @subsection eina_counter Timing Functions
 *
 * @subsection eina_error Error Functions
 *
 * Take a look at the API of @ref Eina_Error_Group.
 *
 * Take a look at the @ref tutorial_error_page.
 *
 * @todo add debug function
 * @todo add magic function
 * @todo add other todo items :)
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "eina_types.h"
#include "eina_f16p16.h"
#include "eina_rectangle.h"
#include "eina_inlist.h"
#include "eina_file.h"
#include "eina_list.h"
#include "eina_hash.h"
#include "eina_lalloc.h"
#include "eina_module.h"
#include "eina_mempool.h"
#include "eina_error.h"
#include "eina_array.h"
#include "eina_stringshare.h"
#include "eina_magic.h"
#include "eina_counter.h"
#include "eina_rbtree.h"
#include "eina_accessor.h"
#include "eina_iterator.h"

EAPI int eina_init(void);
EAPI int eina_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* EINA_H */
