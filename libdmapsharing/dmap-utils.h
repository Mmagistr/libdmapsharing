/*
 * Copyright (C) 2006 INDT
 *  Andre Moreira Magalhaes <andre.magalhaes@indt.org.br>
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*
 */

#ifndef __DMAP_UTILS_H__
#define __DMAP_UTILS_H__

#include <glib.h>

#include <libdmapsharing/dmap-config.h>

G_BEGIN_DECLS

#if DMAP_HAVE_UNALIGNED_ACCESS

#define _DMAP_GET(__data, __size, __end) \
    (GUINT##__size##_FROM_##__end (* ((guint##__size *) (__data))))

#define DMAP_READ_UINT32_BE(data) _DMAP_GET (data, 32, BE)
#define DMAP_READ_UINT16_BE(data) _DMAP_GET (data, 16, BE)
#define DMAP_READ_UINT8(data)     (* ((guint8 *) (data)))

#else

#define _DMAP_GET(__data, __idx, __size, __shift) \
    (((guint##__size) (((guint8 *) (__data))[__idx])) << __shift)

#define DMAP_READ_UINT32_BE(data) \
    (_DMAP_GET (data, 0, 32, 24) | \
     _DMAP_GET (data, 1, 32, 16) | \
     _DMAP_GET (data, 2, 32,  8) | \
     _DMAP_GET (data, 3, 32,  0))
#define DMAP_READ_UINT16_BE(data) \
    (_DMAP_GET (data, 0, 16,  8) | \
     _DMAP_GET (data, 1, 16,  0))
#define DMAP_READ_UINT8(data) (_DMAP_GET (data, 0,  8,  0))

#endif

G_END_DECLS

#endif
