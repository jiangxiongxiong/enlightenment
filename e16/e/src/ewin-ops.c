/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "ewin-ops.h"
#include "snaps.h"
#include <sys/time.h>

static const WinOp  winops[] = {
   {"close", 2, 1, 0, EWIN_OP_CLOSE},
   {"kill", 0, 1, 0, EWIN_OP_KILL},
   {"iconify", 2, 1, 1, EWIN_OP_ICONIFY},
   {"opacity", 2, 1, 1, EWIN_OP_OPACITY},
   {"shadow", 0, 1, 1, EWIN_OP_SHADOW},	/* Place before "shade" */
   {"shade", 2, 1, 1, EWIN_OP_SHADE},
   {"stick", 2, 1, 1, EWIN_OP_STICK},
   {"fixedpos", 0, 1, 1, EWIN_OP_FIXED_POS},
   {"never_use_area", 0, 1, 1, EWIN_OP_NEVER_USE_AREA},
   {"focusclick", 0, 1, 1, EWIN_OP_FOCUS_CLICK},
   {"neverfocus", 0, 1, 1, EWIN_OP_FOCUS_NEVER},
   {"no_button_grabs", 0, 1, 1, EWIN_OP_NO_BUTTON_GRABS},
   {"title", 2, 1, 1, EWIN_OP_TITLE},
   {"toggle_width", 0, 1, 0, EWIN_OP_MAX_WIDTH},
   {"tw", 2, 1, 0, EWIN_OP_MAX_WIDTH},
   {"toggle_height", 0, 1, 0, EWIN_OP_MAX_HEIGHT},
   {"th", 0, 1, 0, EWIN_OP_MAX_HEIGHT},
   {"toggle_size", 0, 1, 0, EWIN_OP_MAX_SIZE},
   {"ts", 2, 1, 0, EWIN_OP_MAX_SIZE},
   {"raise", 2, 1, 0, EWIN_OP_RAISE},
   {"lower", 2, 1, 0, EWIN_OP_LOWER},
   {"layer", 2, 1, 1, EWIN_OP_LAYER},
   {"border", 2, 1, 0, EWIN_OP_BORDER},
   {"desk", 2, 1, 1, EWIN_OP_DESK},
   {"area", 2, 1, 1, EWIN_OP_AREA},
   {"move", 2, 1, 1, EWIN_OP_MOVE},
   {"resize", 0, 1, 1, EWIN_OP_SIZE},
   {"sz", 2, 1, 1, EWIN_OP_SIZE},
   {"move_relative", 0, 1, 0, EWIN_OP_MOVE_REL},
   {"mr", 2, 1, 0, EWIN_OP_MOVE_REL},
   {"resize_relative", 0, 1, 0, EWIN_OP_SIZE_REL},
   {"sr", 2, 1, 0, EWIN_OP_SIZE_REL},
   {"focus", 2, 1, 0, EWIN_OP_FOCUS},
   {"fullscreen", 2, 1, 1, EWIN_OP_FULLSCREEN},
   {"skiplists", 4, 1, 1, EWIN_OP_SKIP_LISTS},
   {"zoom", 2, 1, 0, EWIN_OP_ZOOM},
   {"snap", 0, 1, 0, EWIN_OP_SNAP},
   {NULL, 0, 0, 0, EWIN_OP_INVALID}	/* Terminator */
};

const WinOp        *
EwinOpFind(const char *op)
{
   const WinOp        *wop;

   wop = winops;
   for (; wop->name; wop++)
     {
	if (wop->len)
	  {
	     if (!strncmp(op, wop->name, wop->len))
		return wop;
	  }
	else
	  {
	     if (!strcmp(op, wop->name))
		return wop;
	  }
     }

   return NULL;
}

void
SlideEwinTo(EWin * ewin, int fx, int fy, int tx, int ty, int speed)
{
   int                 k, x, y, tmpx, tmpy;
   char                firstlast;

   firstlast = 0;
   Mode.doingslide = 1;
   SoundPlay("SOUND_WINDOW_SLIDE");

   if (Conf.slidemode > 0)
      ecore_x_grab();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	tmpx = x;
	tmpy = y;
	if (Conf.slidemode == 0)
	   EoMove(ewin, tmpx, tmpy);
	else
	   DrawEwinShape(ewin, Conf.slidemode, tmpx, tmpy,
			 ewin->client.w, ewin->client.h, firstlast);
	if (firstlast == 0)
	   firstlast = 1;

	k = ETimedLoopNext();
     }
   DrawEwinShape(ewin, Conf.slidemode, x, y, ewin->client.w, ewin->client.h, 2);
   MoveEwin(ewin, tx, ty);
   Mode.doingslide = 0;

   if (Conf.slidemode > 0)
      ecore_x_ungrab();

   SoundPlay("SOUND_WINDOW_SLIDE_END");
}

void
SlideEwinsTo(EWin ** ewin, int *fx, int *fy, int *tx, int *ty, int num_wins,
	     int speed)
{
   int                 k, *x = NULL, *y = NULL, tmpx, tmpy, tmpw, tmph, i;
   char                firstlast;

   if (num_wins)
     {
	x = Emalloc(sizeof(int) * num_wins);
	y = Emalloc(sizeof(int) * num_wins);
     }

   firstlast = 0;
   Mode.doingslide = 1;
   SoundPlay("SOUND_WINDOW_SLIDE");

   if (Conf.slidemode > 0)
      ecore_x_grab();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num_wins; i++)
	  {
	     if (!ewin[i])
		continue;

	     x[i] = ((fx[i] * (1024 - k)) + (tx[i] * k)) >> 10;
	     y[i] = ((fy[i] * (1024 - k)) + (ty[i] * k)) >> 10;
	     tmpx = x[i];
	     tmpy = y[i];
	     tmpw = ewin[i]->client.w;
	     tmph = ewin[i]->client.h;
	     if (ewin[i]->type == EWIN_TYPE_MENU)
		EoMove(ewin[i], tmpx, tmpy);
	     else
		DrawEwinShape(ewin[i], 0, tmpx, tmpy, tmpw, tmph, firstlast);
	     if (firstlast == 0)
		firstlast = 1;
	  }
	/* We may loop faster here than originally intended */
	k = ETimedLoopNext();
     }

   for (i = 0; i < num_wins; i++)
     {
	if (ewin[i])
	  {
	     DrawEwinShape(ewin[i], 0, x[i], y[i], ewin[i]->client.w,
			   ewin[i]->client.h, 2);
	     MoveEwin(ewin[i], tx[i], ty[i]);
	  }
     }

   Mode.doingslide = 0;
   if (Conf.slidemode > 0)
      ecore_x_ungrab();

   SoundPlay("SOUND_WINDOW_SLIDE_END");

   if (x)
      Efree(x);
   if (y)
      Efree(y);
}

void
EwinFixPosition(EWin * ewin)
{
   int                 x, y;

   if (!EwinIsMapped(ewin))
      return;

   x = EoGetX(ewin);
   if ((x + ewin->border->border.left + 1) > VRoot.w)
      x = VRoot.w - ewin->border->border.left - 1;
   else if ((x + EoGetW(ewin) - ewin->border->border.right - 1) < 0)
      x = 0 - EoGetW(ewin) + ewin->border->border.right + 1;

   y = EoGetY(ewin);
   if ((y + ewin->border->border.top + 1) > VRoot.h)
      y = VRoot.h - ewin->border->border.top - 1;
   else if ((y + EoGetH(ewin) - ewin->border->border.bottom - 1) < 0)
      y = 0 - EoGetH(ewin) + ewin->border->border.bottom + 1;

   if (x != EoGetX(ewin) || y != EoGetY(ewin))
      MoveEwin(ewin, x, y);
}

#define MR_FLAGS_MOVE   1
#define MR_FLAGS_RESIZE 2

static void
doMoveResizeEwin(EWin * ewin, int x, int y, int w, int h, int flags)
{
   static int          call_depth = 0;
   int                 dx = 0, dy = 0, sw, sh, x0, y0;
   char                move = 0, resize = 0;
   EWin              **lst;
   int                 i, num;

   if (call_depth > 256)
      return;
   call_depth++;

   if (EventDebug(EDBUG_TYPE_MOVERESIZE))
      Eprintf("doMoveResizeEwin(%d) %#lx %d+%d %d*%d %d %s\n", call_depth,
	      ewin->client.win, x, y, w, h, flags, EwinGetName(ewin));

   if (Mode.mode == MODE_NONE && Mode.move.check)
     {
	/* Don't throw windows offscreen */
	sw = VRoot.w;
	sh = VRoot.h;
	if (EoIsSticky(ewin))
	  {
	     x0 = y0 = 0;
	  }
	else
	  {
	     int                 ax, ay;

	     DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	     x0 = -ax * sw;
	     y0 = -ay * sh;
	     sw *= Conf.desks.areas_nx;
	     sh *= Conf.desks.areas_ny;
	  }

	if (ewin->shaded)
	  {
	     /* Keep shaded windows entirely on-screen */
	     dx = EoGetW(ewin);
	     dy = EoGetH(ewin);
	  }
	else
	  {
	     /* Keep at least 8 pixels on-screen */
	     dx = EoGetW(ewin) / 4;
	     if (dx > 8)
		dx = 8;
	     dy = EoGetH(ewin) / 4;
	     if (dy > 8)
		dy = 8;
	  }

	if (x < x0 - EoGetW(ewin) + dx)
	   x = x0 - EoGetW(ewin) + dx;
	else if (x > x0 + sw - dx)
	   x = x0 + sw - dx;
	if (y < y0 - EoGetH(ewin) + dy)
	   y = y0 - EoGetH(ewin) + dy;
	else if (y > y0 + sh - dy)
	   y = y0 + sh - dy;
     }

   if (flags & MR_FLAGS_MOVE)
     {
	dx = x - EoGetX(ewin);
	dy = y - EoGetY(ewin);
	if ((dx != 0) || (dy != 0))
	   move = 1;
	EoSetX(ewin, x);
	EoSetY(ewin, y);
	ewin->client.x = x + ewin->border->border.left;
	ewin->client.y = y + ewin->border->border.top;
     }

   if (flags & MR_FLAGS_RESIZE)
     {
	if ((w != ewin->client.w) || (h != ewin->client.h))
	   resize = 2;
	ewin->client.w = w;
	ewin->client.h = h;
	ICCCM_MatchSize(ewin);

	if (!ewin->shaded)
	  {
	     EoSetW(ewin, ewin->client.w + ewin->border->border.left +
		    ewin->border->border.right);
	     EoSetH(ewin, ewin->client.h + ewin->border->border.top +
		    ewin->border->border.bottom);
	  }
     }

   EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin));

   if (ewin->shaded == 0)
     {
	EMoveResizeWindow(ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  ewin->client.w, ewin->client.h);
     }
   else
     {
	EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
     }

   EMoveResizeWindow(ewin->client.win, 0, 0, ewin->client.w, ewin->client.h);

   EwinBorderCalcSizes(ewin);

   if (Mode.mode == MODE_NONE || Conf.movres.update_while_moving)
      ICCCM_Configure(ewin);

   if (move)
     {
	lst = EwinListTransients(ewin, &num, 0);
	for (i = 0; i < num; i++)
	   MoveEwin(lst[i], EoGetX(lst[i]) + dx, EoGetY(lst[i]) + dy);
	if (lst)
	   Efree(lst);
     }

   if ((Mode.mode == MODE_NONE) /* && (move || resize) */ )
     {
	ewin->shape_x = x;
	ewin->shape_y = y;
	EwinUpdateAfterMoveResize(ewin, resize);
     }

   call_depth--;
}

void
MoveEwin(EWin * ewin, int x, int y)
{
   doMoveResizeEwin(ewin, x, y, 0, 0, MR_FLAGS_MOVE);
}

void
ResizeEwin(EWin * ewin, int w, int h)
{
   doMoveResizeEwin(ewin, 0, 0, w, h, MR_FLAGS_RESIZE);
}

void
MoveResizeEwin(EWin * ewin, int x, int y, int w, int h)
{
   doMoveResizeEwin(ewin, x, y, w, h, MR_FLAGS_MOVE | MR_FLAGS_RESIZE);
}

void
EwinIconify(EWin * ewin)
{
   static int          call_depth = 0;
   EWin              **lst, *e;
   int                 i, num;
   char                was_shaded;

   if (!ewin)
      return;

   if (GetZoomEWin() == ewin)
      Zoom(NULL);

   if (ewin->props.inhibit_iconify)
      return;

   if (ewin->state != EWIN_STATE_MAPPED)
      return;

   if (call_depth > 256)
      return;
   call_depth++;

   was_shaded = ewin->shaded;

   if (!ewin->client.transient)
      ModulesSignal(ESIGNAL_EWIN_ICONIFY, ewin);

   HideEwin(ewin);

   /* Save position at which the window was iconified */
   EwinRememberPositionSet(ewin);

   if (was_shaded != ewin->shaded)
      EwinInstantShade(ewin, 0);

   if (ewin->client.transient)
     {
	/* We should only get here during restart */
	ewin->iconified = 4;
	goto done;
     }

   ewin->iconified = 3;
   ICCCM_Iconify(ewin);

   lst = EwinListTransients(ewin, &num, 0);
   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (e->iconified)
	   continue;

	HideEwin(e);
	e->iconified = 4;
	EwinRememberPositionSet(e);
     }
   if (lst)
      Efree(lst);
#if ENABLE_GNOME
   if (lst)
      GNOME_SetClientList();
#endif

   HintsSetWindowState(ewin);

 done:
   call_depth--;
}

void
EwinDeIconify(EWin * ewin)
{
   static int          call_depth = 0;
   EWin              **lst, *e;
   int                 i, num;
   int                 x, y, ox, oy, dx, dy;

   if (call_depth > 256)
      return;
   call_depth++;

   if (ewin->state != EWIN_STATE_ICONIC)
      return;

   EwinRememberPositionGet(ewin, &ox, &oy);
   x = ox;
   y = oy;

   if (!ewin->st.showingdesk)
     {
	/* If we iconified an offscreen window, get it back on screen */
	if (x + EoGetW(ewin) <= 4 || x > VRoot.w - 4 ||
	    y + EoGetH(ewin) <= 4 || y > VRoot.h - 4)
	  {
	     dx = EoGetW(ewin) / 2;
	     dy = EoGetH(ewin) / 2;
	     x = (EoGetX(ewin) + dx) % VRoot.w;
	     if (x < 0)
		x += VRoot.w;
	     x -= dx;
	     y = (EoGetY(ewin) + dy) % VRoot.h;
	     if (y < 0)
		y += VRoot.h;
	     y -= dy;
	  }
     }

   dx = x - ox;
   dy = y - oy;

   if (EoIsSticky(ewin))
      MoveEwin(ewin, x, y);
   else
      MoveEwinToDesktopAt(ewin, DesksGetCurrent(), x, y);

   ewin->iconified = 0;

   ModulesSignal(ESIGNAL_EWIN_DEICONIFY, ewin);

   ewin->st.showingdesk = 0;

   RaiseEwin(ewin);
   ShowEwin(ewin);
   ICCCM_DeIconify(ewin);

   lst = EwinListTransients(ewin, &num, 0);
   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (e->iconified != 4)
	   continue;

	EwinRememberPositionGet(e, &ox, &oy);
	if (EoIsSticky(e))
	   MoveEwin(e, ox + dx, oy + dy);
	else
	   MoveEwinToDesktopAt(e, DesksGetCurrent(), ox + dx, oy + dy);

	RaiseEwin(e);
	ShowEwin(e);
	e->iconified = 0;
     }
   if (lst)
      Efree(lst);
#if ENABLE_GNOME
   if (lst)
      GNOME_SetClientList();
#endif

   HintsSetWindowState(ewin);

   call_depth--;
}

void
EwinUnStick(EWin * ewin)
{

   if (!ewin)
      return;

   EoSetSticky(ewin, 0);
   MoveEwinToDesktopAt(ewin, DesksGetCurrent(), EoGetX(ewin), EoGetY(ewin));
   EwinBorderUpdateState(ewin);
   HintsSetWindowState(ewin);
}

void
EwinStick(EWin * ewin)
{
   int                 x, y, dx, dy;

   if (!ewin)
      return;

   /* Avoid "losing" windows made sticky while not in the current viewport */
   dx = EoGetW(ewin) / 2;
   dy = EoGetH(ewin) / 2;
   x = (EoGetX(ewin) + dx) % VRoot.w;
   if (x < 0)
      x += VRoot.w;
   x -= dx;
   y = (EoGetY(ewin) + dy) % VRoot.h;
   if (y < 0)
      y += VRoot.h;
   y -= dy;

   MoveEwinToDesktopAt(ewin, DesksGetCurrent(), x, y);
   EoSetSticky(ewin, 1);
   EwinBorderUpdateState(ewin);
   HintsSetWindowState(ewin);
}

void
EwinInstantShade(EWin * ewin, int force)
{
   XSetWindowAttributes att;
   int                 b, d;
   char                pq;

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0)
       && (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      return;
   if (GetZoomEWin() == ewin)
      return;
   if (ewin->shaded && !force)
      return;

   pq = Mode.queue_up;
   Mode.queue_up = 0;
   switch (ewin->border->shadedir)
     {
     default:
     case 0:
	att.win_gravity = EastGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	EoSetW(ewin, b);
	break;
     case 1:
	att.win_gravity = WestGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	d = EoGetX(ewin) + EoGetW(ewin) - b;
	EoSetW(ewin, b);
	if (!Mode.wm.startup)
	   EoSetX(ewin, d);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	b = d;
	EoSetH(ewin, b);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	b = d;
	d = EoGetY(ewin) + EoGetH(ewin) - b;
	EoSetH(ewin, b);
	if (!Mode.wm.startup)
	   EoSetY(ewin, d);
	break;
     }

   ewin->shaded = 2;
   EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin));
   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
   EwinBorderCalcSizes(ewin);
   ecore_x_sync();

   EwinPropagateShapes(ewin);
   Mode.queue_up = pq;
   HintsSetWindowState(ewin);
   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
}

void
EwinInstantUnShade(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 b, d;
   char                pq;

   if (GetZoomEWin() == ewin)
      return;
   if (!ewin->shaded)
      return;
   pq = Mode.queue_up;
   Mode.queue_up = 0;

   switch (ewin->border->shadedir)
     {
     default:
     case 0:
	att.win_gravity = EastGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	EoSetW(ewin, b);
	break;
     case 1:
	att.win_gravity = WestGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	b = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	d = EoGetX(ewin) + EoGetW(ewin) - (ewin->border->border.right +
					   ewin->client.w +
					   ewin->border->border.left);
	EoSetW(ewin, b);
	EoSetX(ewin, d);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	EoSetH(ewin, b);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	b = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	d = EoGetY(ewin) + EoGetH(ewin) - (ewin->border->border.bottom +
					   ewin->client.h +
					   ewin->border->border.top);
	EoSetH(ewin, b);
	EoSetY(ewin, d);
	break;
     }

   /* Reset gravity */
   att.win_gravity = NorthWestGravity;
   EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);

   ewin->shaded = 0;
   MoveResizeEwin(ewin, EoGetX(ewin), EoGetY(ewin), ewin->client.w,
		  ewin->client.h);
   ecore_x_sync();

   EwinPropagateShapes(ewin);
   Mode.queue_up = pq;
   HintsSetWindowState(ewin);
   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
}

void
EwinShade(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 i, j, k, speed, a, b, c, d, ww, hh;
   char                pq;

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0)
       && (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      return;
   if (GetZoomEWin() == ewin)
      return;
   if (ewin->shaded || ewin->iconified)
      return;
   if ((ewin->border) && (!strcmp(ewin->border->name, "BORDERLESS")))
      return;

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   speed = Conf.shadespeed;

#if 0
   Eprintf("EwinShade-B\n");
   ecore_x_grab();
#endif

   switch (ewin->border->shadedir)
     {
     default:
     case 0:
	att.win_gravity = EastGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	a = EoGetW(ewin);
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  EoSetW(ewin, i);
		  if (EoGetW(ewin) < 1)
		     EoSetW(ewin, 1);
		  ww = EoGetW(ewin) - ewin->border->border.left -
		     ewin->border->border.right;
		  if (ww < 1)
		     ww = 1;
		  hh = ewin->client.h;
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ww, hh);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, -(ewin->client.w - ww),
					0, ewin->client.win, ShapeBounding,
					ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetW(ewin, b);
	break;
     case 1:
	att.win_gravity = WestGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	a = EoGetW(ewin);
	c = EoGetX(ewin);
	d = EoGetX(ewin) + EoGetW(ewin) - b;
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  j = ((c * (1024 - k)) + (d * k)) >> 10;
		  EoSetW(ewin, i);
		  EoSetX(ewin, j);
		  if (EoGetW(ewin) < 1)
		     EoSetW(ewin, 1);
		  ww = EoGetW(ewin) - ewin->border->border.left -
		     ewin->border->border.right;
		  if (ww < 1)
		     ww = 1;
		  hh = ewin->client.h;
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ww, hh);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0, 0, ewin->client.win,
					ShapeBounding, ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetW(ewin, b);
	EoSetX(ewin, d);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	a = EoGetH(ewin);
	EwinBorderMinShadeSize(ewin, &b, &d);
	b = d;
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  EoSetH(ewin, i);
		  if (EoGetH(ewin) < 1)
		     EoSetH(ewin, 1);
		  hh = EoGetH(ewin) - ewin->border->border.top -
		     ewin->border->border.bottom;
		  if (hh < 1)
		     hh = 1;
		  ww = ewin->client.w;
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ww, hh);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0,
					-(ewin->client.h - hh),
					ewin->client.win, ShapeBounding,
					ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetH(ewin, b);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	EwinBorderMinShadeSize(ewin, &b, &d);
	a = EoGetH(ewin);
	b = d;
	c = EoGetY(ewin);
	d = EoGetY(ewin) + EoGetH(ewin) - b;
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  j = ((c * (1024 - k)) + (d * k)) >> 10;
		  EoSetH(ewin, i);
		  EoSetY(ewin, j);
		  if (EoGetH(ewin) < 1)
		     EoSetH(ewin, 1);
		  hh = EoGetH(ewin) - ewin->border->border.top -
		     ewin->border->border.bottom;
		  if (hh < 1)
		     hh = 1;
		  ww = ewin->client.w;
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ww, hh);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0, 0, ewin->client.win,
					ShapeBounding, ShapeSet);
		  EwinPropagateShapes(ewin);
		  k = ETimedLoopNext();
	       }
	  }
	EoSetH(ewin, b);
	EoSetY(ewin, d);
	break;
     }

   ewin->shaded = 2;
   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
   EwinBorderCalcSizes(ewin);
   MoveEwin(ewin, EoGetX(ewin), EoGetY(ewin));
   ecore_x_sync();

#if 0
   ecore_x_ungrab();
   Eprintf("EwinShade-E\n");
#endif

   if (ewin->client.shaped)
      EShapeCombineShape(ewin->win_container, ShapeBounding, 0, 0,
			 ewin->client.win, ShapeBounding, ShapeSet);
   EwinPropagateShapes(ewin);

   Mode.queue_up = pq;

   HintsSetWindowState(ewin);
   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
}

void
EwinUnShade(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 i, j, k, speed, a, b, c, d;
   char                pq;

   if (GetZoomEWin() == ewin)
      return;
   if (!ewin->shaded || ewin->iconified)
      return;

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   speed = Conf.shadespeed;

#if 0
   Eprintf("EwinUnShade-B\n");
   ecore_x_grab();
#endif

   switch (ewin->border->shadedir)
     {
     default:
     case 0:
	att.win_gravity = EastGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.left + ewin->border->border.right;
	b = ewin->client.w + a;
	a++;
	ewin->shaded = 0;
	EMoveResizeWindow(ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  1, ewin->client.h);
	EWindowSync(ewin->client.win);	/* Gravity - recache */
	EMoveResizeWindow(ewin->client.win, -ewin->client.w, 0,
			  ewin->client.w, ewin->client.h);
	EMapWindow(ewin->client.win);
	EMapWindow(ewin->win_container);
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  EoSetW(ewin, i);
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top,
				    EoGetW(ewin) - ewin->border->border.left -
				    ewin->border->border.right, ewin->client.h);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding,
					-(ewin->client.w -
					  (EoGetW(ewin) -
					   ewin->border->border.left -
					   ewin->border->border.right)), 0,
					ewin->client.win, ShapeBounding,
					ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetW(ewin, b);
	break;
     case 1:
	att.win_gravity = WestGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.left + ewin->border->border.right;
	b = ewin->client.w + a;
	c = EoGetX(ewin);
	d = EoGetX(ewin) + EoGetW(ewin) - (ewin->client.w + a);
	a++;
	ewin->shaded = 0;
	EMoveResizeWindow(ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  1, ewin->client.h);
	EWindowSync(ewin->client.win);	/* Gravity - recache */
	EMoveResizeWindow(ewin->client.win, 0, 0, ewin->client.w,
			  ewin->client.h);
	EMapWindow(ewin->client.win);
	EMapWindow(ewin->win_container);
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  j = ((c * (1024 - k)) + (d * k)) >> 10;
		  EoSetW(ewin, i);
		  EoSetX(ewin, j);
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top,
				    EoGetW(ewin) - ewin->border->border.left -
				    ewin->border->border.right, ewin->client.h);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0, 0, ewin->client.win,
					ShapeBounding, ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetW(ewin, b);
	EoSetX(ewin, d);
	break;
     case 2:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.top + ewin->border->border.bottom;
	b = ewin->client.h + a;
	a++;
	ewin->shaded = 0;
	EMoveResizeWindow(ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  ewin->client.w, 1);
	EWindowSync(ewin->client.win);	/* Gravity - recache */
	EMoveResizeWindow(ewin->client.win, 0, -ewin->client.h,
			  ewin->client.w, ewin->client.h);
	EMapWindow(ewin->client.win);
	EMapWindow(ewin->win_container);
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  EoSetH(ewin, i);
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ewin->client.w,
				    EoGetH(ewin) - ewin->border->border.top -
				    ewin->border->border.bottom);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0,
					-(ewin->client.h -
					  (EoGetH(ewin) -
					   ewin->border->border.top -
					   ewin->border->border.bottom)),
					ewin->client.win, ShapeBounding,
					ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetH(ewin, b);
	break;
     case 3:
	att.win_gravity = SouthGravity;
	EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);
	a = ewin->border->border.top + ewin->border->border.bottom;
	b = ewin->client.h + a;
	c = EoGetY(ewin);
	d = EoGetY(ewin) + EoGetH(ewin) - (ewin->client.h + a);
	a++;
	ewin->shaded = 0;
	EMoveResizeWindow(ewin->win_container,
			  ewin->border->border.left, ewin->border->border.top,
			  ewin->client.w, 1);
	EWindowSync(ewin->client.win);	/* Gravity - recache */
	EMoveResizeWindow(ewin->client.win, 0, 0, ewin->client.w,
			  ewin->client.h);
	EMapWindow(ewin->client.win);
	EMapWindow(ewin->win_container);
	if ((Conf.animate_shading) || (ewin->type == EWIN_TYPE_MENU))
	  {
	     ETimedLoopInit(0, 1024, speed);
	     for (k = 0; k <= 1024;)
	       {
		  i = ((a * (1024 - k)) + (b * k)) >> 10;
		  j = ((c * (1024 - k)) + (d * k)) >> 10;
		  EoSetH(ewin, i);
		  EoSetY(ewin, j);
		  EMoveResizeWindow(ewin->win_container,
				    ewin->border->border.left,
				    ewin->border->border.top, ewin->client.w,
				    EoGetH(ewin) - ewin->border->border.top -
				    ewin->border->border.bottom);
		  EoMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
			       EoGetW(ewin), EoGetH(ewin));
		  EwinBorderCalcSizes(ewin);
		  if (ewin->client.shaped)
		     EShapeCombineShape(ewin->win_container,
					ShapeBounding, 0, 0, ewin->client.win,
					ShapeBounding, ShapeSet);
		  EwinPropagateShapes(ewin);

		  k = ETimedLoopNext();
	       }
	  }
	EoSetH(ewin, b);
	EoSetY(ewin, d);
	break;
     }

   /* Reset gravity */
   att.win_gravity = NorthWestGravity;
   EChangeWindowAttributes(ewin->client.win, CWWinGravity, &att);

   MoveResizeEwin(ewin, EoGetX(ewin), EoGetY(ewin), ewin->client.w,
		  ewin->client.h);
   ecore_x_sync();

#if 0
   ecore_x_ungrab();
   Eprintf("EwinUnShade-E\n");
#endif

   if (ewin->client.shaped)
      EShapeCombineShape(ewin->win_container, ShapeBounding, 0, 0,
			 ewin->client.win, ShapeBounding, ShapeSet);
   EwinPropagateShapes(ewin);

   Mode.queue_up = pq;

   HintsSetWindowState(ewin);
   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
}

void
EwinSetFullscreen(EWin * ewin, int on)
{
   int                 x, y, w, h, ww, hh;
   EWin              **lst;
   int                 i, num;
   const Border       *b;

   if (ewin->st.fullscreen == on)
      return;

   if (on)
     {
	ewin->lx = EoGetX(ewin);
	ewin->ly = EoGetY(ewin);
	ewin->lw = ewin->client.w;
	ewin->lh = ewin->client.h;
	ewin->ll = EoGetLayer(ewin);
	ScreenGetAvailableArea(EoGetX(ewin), EoGetY(ewin), &x, &y, &w, &h);

	/* Fixup if available space doesn't match ICCCM size constraints */
	ICCCM_SizeMatch(ewin, w, h, &ww, &hh);
	if (w == ww && h == hh)
	  {
	     b = FindItem("BORDERLESS", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	  }
	else
	  {
	     int                 l, t;

	     l = (w - ww) / 2;
	     l = (l < 0) ? 0 : l;
	     t = (h - hh) / 2;
	     t = (t < 0) ? 0 : t;
	     b = BorderCreateFiller(l, w - ww - l, t, h - hh - t);
	     w = ww;
	     h = hh;
	  }

	if (Conf.place.raise_fullscreen)
	  {
	     EoSetLayer(ewin, 8);
	     lst = EwinListTransients(ewin, &num, 0);
	     for (i = 0; i < num; i++)
	       {
		  lst[i]->ll = EoGetLayer(lst[i]);
		  EoSetLayer(lst[i], lst[i]->ll + EoGetLayer(ewin) - ewin->ll);
	       }
	     if (lst)
		Efree(lst);
	  }

	ewin->fixedpos = 1;
     }
   else
     {
	x = ewin->lx;
	y = ewin->ly;
	w = ewin->lw;
	h = ewin->lh;
	ewin->fixedpos = 0;	/* Yeah - well */
	b = ewin->normal_border;

	if (Conf.place.raise_fullscreen)
	  {
	     lst = EwinListTransients(ewin, &num, 0);
	     for (i = 0; i < num; i++)
		EoSetLayer(lst[i], lst[i]->ll);
	     if (lst)
		Efree(lst);
	  }
	EoSetLayer(ewin, ewin->ll);
     }
   ewin->st.fullscreen = on;
   RaiseEwin(ewin);
   MoveResizeEwin(ewin, x, y, w, h);
   HintsSetWindowState(ewin);
   EwinSetBorder(ewin, b, 1);
}

void
EwinsShowDesktop(int on)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListGetForDesk(&num, DesksGetCurrent());

   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	if (on)
	  {
	     if (EwinIsInternal(ewin) || ewin->iconified || ewin->props.donthide
		 || ewin->client.transient)
		continue;

	     ewin->st.showingdesk = 1;
	     EwinIconify(ewin);
	  }
	else
	  {
	     if (!ewin->st.showingdesk)
		continue;

	     EwinDeIconify(ewin);
	  }
     }
   EWMH_SetShowingDesktop(on);
}

void
MoveEwinToArea(EWin * ewin, int ax, int ay)
{
   AreaFix(&ax, &ay);
   MoveEwin(ewin, EoGetX(ewin) + (VRoot.w * (ax - ewin->area_x)),
	    EoGetY(ewin) + (VRoot.h * (ay - ewin->area_y)));
}

void
MoveEwinToDesktop(EWin * ewin, int desk)
{
   MoveEwinToDesktopAt(ewin, desk, EoGetX(ewin), EoGetY(ewin));
}

void
MoveEwinToDesktopAt(EWin * ewin, int desk, int x, int y)
{
   EWin              **lst;
   int                 i, num;
   int                 pdesk, dx, dy;

   EoSetFloating(ewin, 0);

#if 0
   Eprintf("MoveEwinToDesktopAt: %#lx %d->%d\n", ewin->client.win,
	   EoGetDesk(ewin), desk);
#endif

   pdesk = ewin->ld;
   desk = desk % Conf.desks.num;
   EoSetDesk(ewin, desk);

   if (desk != pdesk && !EoIsSticky(ewin))
     {
	SnapshotEwinUpdate(ewin, SNAP_USE_DESK);
	if (pdesk >= 0)
	   ModulesSignal(ESIGNAL_DESK_CHANGE, (void *)pdesk);
     }

   dx = x - EoGetX(ewin);
   dy = y - EoGetY(ewin);
   EoSetX(ewin, x);
   EoSetY(ewin, y);
   EwinConformToDesktop(ewin);

   lst = EwinListTransients(ewin, &num, 0);
   for (i = 0; i < num; i++)
      MoveEwinToDesktopAt(lst[i], desk, EoGetX(lst[i]) + dx,
			  EoGetY(lst[i]) + dy);
   if (lst)
      Efree(lst);

   ModulesSignal(ESIGNAL_DESK_CHANGE, (void *)desk);
}

void
EwinOpClose(EWin * ewin)
{
   EWin              **gwins;
   int                 num, i;

   if (!ewin)
      return;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_KILL,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	ICCCM_Delete(gwins[i]);
	SoundPlay("SOUND_WINDOW_CLOSE");
     }
   if (gwins)
      Efree(gwins);
}

void
EwinOpKill(EWin * ewin)
{
   SoundPlay("SOUND_WINDOW_CLOSE");
   /* NB! Got to use X- not E- to get properly through the event handling. */
   XDestroyWindow(disp, ewin->client.win);
}

void
EwinOpRaise(EWin * ewin)
{
   EWin              **gwins = NULL;
   int                 i, num;

   SoundPlay("SOUND_RAISE");
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_RAISE,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
      RaiseEwin(gwins[i]);
   Efree(gwins);
}

void
EwinOpLower(EWin * ewin)
{
   EWin              **gwins = NULL;
   int                 i, num;

   SoundPlay("SOUND_LOWER");
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_LOWER,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
      LowerEwin(gwins[i]);
   Efree(gwins);
}

static int
FindEwinInList(EWin * ewin, EWin ** gwins, int num)
{
   int                 i;

   if (ewin && gwins)
     {
	for (i = 0; i < num; i++)
	  {
	     if (ewin == gwins[i])
		return 1;
	  }
     }
   return 0;
}

void
EwinOpRaiseLower(EWin * ewin)
{
   EWin              **gwins, *const *lst;
   int                 gnum, j, raise = 0;
   int                 i, num;

   lst = EwinListGetForDesk(&num, EoGetDesk(ewin));
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_RAISE_LOWER,
				      Mode.nogroup, &gnum);
   for (j = 0; j < gnum; j++)
     {
	ewin = gwins[j];
	for (i = 0; i < num - 1; i++)
	  {
	     if (EoGetLayer(lst[i]) == EoGetLayer(ewin) &&
		 (lst[i] == ewin || !FindEwinInList(lst[i], gwins, gnum)))
	       {
		  if (lst[i] != ewin)
		     raise = 1;

		  j = gnum;
		  break;
	       }
	  }
     }

   if (!raise)
     {
	SoundPlay("SOUND_LOWER");
	for (j = 0; j < gnum; j++)
	   LowerEwin(gwins[j]);
     }
   else
     {
	SoundPlay("SOUND_RAISE");
	for (j = 0; j < gnum; j++)
	   RaiseEwin(gwins[j]);
     }

   if (gwins)
      Efree(gwins);
}

void
EwinOpStick(EWin * ewin, int on)
{
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_STICK,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (EoIsSticky(gwins[i])
	    && ((curr_group && !curr_group->cfg.mirror) || !on))
	  {
	     SoundPlay("SOUND_WINDOW_UNSTICK");
	     EwinUnStick(gwins[i]);
	  }
	else if (!EoIsSticky(gwins[i])
		 && ((curr_group && !curr_group->cfg.mirror) || on))
	  {
	     SoundPlay("SOUND_WINDOW_STICK");
	     EwinStick(gwins[i]);
	  }
	SnapshotEwinUpdate(gwins[i], SNAP_USE_STICKY);
     }
   if (gwins)
      Efree(gwins);
}

void
EwinOpSkipLists(EWin * ewin, int skip)
{
   ewin->skiptask = skip;
   ewin->skipwinlist = skip;
   ewin->skipfocus = skip;
   HintsSetWindowState(ewin);
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpSkipTask(EWin * ewin, int skip)
{
   ewin->skiptask = skip;
   HintsSetWindowState(ewin);
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpSkipFocus(EWin * ewin, int skip)
{
   ewin->skipfocus = skip;
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpSkipWinlist(EWin * ewin, int skip)
{
   ewin->skipwinlist = skip;
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpNeverFocus(EWin * ewin, int on)
{
   ewin->neverfocus = on;
   SnapshotEwinUpdate(ewin, SNAP_USE_FOCUS_NEVER);
}

void
EwinOpIconify(EWin * ewin, int on)
{
   Group              *curr_group = NULL;
   EWin              **gwins = NULL;
   int                 i, num;

   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ICONIFY, Mode.nogroup,
				 &num);
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->iconified
	    && ((curr_group && !curr_group->cfg.mirror) || !on))
	  {
	     EwinDeIconify(gwins[i]);
	  }
	else if (!gwins[i]->iconified
		 && ((curr_group && !curr_group->cfg.mirror) || on))
	  {
	     EwinIconify(gwins[i]);
	  }
     }
   if (gwins)
      Efree(gwins);
}

void
EwinOpShade(EWin * ewin, int on)
{
   EWin              **gwins = NULL;
   Group              *curr_group = NULL;
   int                 i, num;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_SHADE,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	curr_group = EwinsInGroup(ewin, gwins[i]);
	if (gwins[i]->shaded
	    && ((curr_group && !curr_group->cfg.mirror) || !on))
	  {
	     SoundPlay("SOUND_UNSHADE");
	     EwinUnShade(gwins[i]);
	  }
	else if (!gwins[i]->shaded
		 && ((curr_group && !curr_group->cfg.mirror) || on))
	  {
	     SoundPlay("SOUND_SHADE");
	     EwinShade(gwins[i]);
	  }
	SnapshotEwinUpdate(gwins[i], SNAP_USE_SHADED);
     }
   Efree(gwins);
}

void
EwinOpSetLayer(EWin * ewin, int layer)
{
   if (EoGetLayer(ewin) > layer)
     {
	SoundPlay("SOUND_WINDOW_CHANGE_LAYER_DOWN");
     }
   else if (EoGetLayer(ewin) < layer)
     {
	SoundPlay("SOUND_WINDOW_CHANGE_LAYER_UP");
     }
   EoSetLayer(ewin, layer);
   RaiseEwin(ewin);
   HintsSetWindowState(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_LAYER);
}

void
EwinOpSetBorder(EWin * ewin, const char *name)
{
   EWin              **gwins = NULL;
   int                 i, num;
   char                has_shaded;
   Border             *b;
   char                shadechange = 0;

   b = (Border *) FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
   if (!b)
      return;

   has_shaded = 0;
   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_SET_WINDOW_BORDER,
				 Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (gwins[i]->shaded)
	  {
	     has_shaded = 1;
	     break;
	  }
     }
   if (has_shaded)
     {
	if ((b->border.left == 0) && (b->border.right == 0)
	    && (b->border.top == 0) && (b->border.bottom == 0))
	   return;
     }

   for (i = 0; i < num; i++)
     {
	if (b != gwins[i]->border)
	  {
	     SoundPlay("SOUND_WINDOW_BORDER_CHANGE");
	     shadechange = 0;
	     if (gwins[i]->shaded)
	       {
		  shadechange = 1;
		  EwinInstantUnShade(gwins[i]);
	       }
	     EwinSetBorder(gwins[i], b, 1);
	     if (shadechange)
		EwinInstantShade(gwins[i], 0);
	  }
	SnapshotEwinUpdate(gwins[i], SNAP_USE_BORDER);
     }
   if (gwins)
      Efree(gwins);
}

unsigned int
OpacityExt(int op)
{
   /* op is 0-255, extend to 32 bit */
   /* op = 0 is mapped to 255 (opaque) */
   if (op <= 0 || op > 255)
      op = 255;
   return (op << 24) | (op << 16) | (op << 8) | op;
}

void
EwinOpSetOpacity(EWin * ewin, int opacity)
{
   unsigned int        op;

   op = OpacityExt(opacity);
   ewin->ewmh.opacity = op;
   HintsSetWindowOpacity(ewin);
   EoChangeOpacity(ewin, op);
   SnapshotEwinUpdate(ewin, SNAP_USE_OPACITY);
}

void
EwinOpMoveToDesk(EWin * ewin, int desk)
{
   MoveEwinToDesktop(ewin, desk);
   RaiseEwin(ewin);
   EoSetSticky(ewin, 0);
   SnapshotEwinUpdate(ewin, SNAP_USE_STICKY);
}

void
EwinOpMoveToArea(EWin * ewin, int x, int y)
{
   MoveEwinToArea(ewin, x, y);
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
}

#if 0				/* Not used? */
static int
doMoveWinToLinearArea(EWin * ewin, const char *params)
{
   int                 da;

   if (params)
     {
	sscanf(params, "%i", &da);
	MoveEwinToLinearArea(ewin, da);
     }
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
   return 0;
}

static int
doMoveWinByLinearArea(EWin * ewin, const char *params)
{
   EWin               *ewin;
   int                 da;

   if (params)
     {
	sscanf(params, "%i", &da);
	MoveEwinLinearAreaBy(ewin, da);
     }
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
   return 0;
}
#endif

#if 0				/* FIXME - Unused? */
static int
doScrollWindows(EWin * edummy __UNUSED__, const char *params)
{
   int                 x, y, num, i;
   EWin               *const *lst;

   if (!params)
      return 0;

   x = 0;
   y = 0;
   sscanf(params, "%i %i", &x, &y);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((lst[i]->desktop == DesksGetCurrent()) && (!lst[i]->sticky) &&
	    (!lst[i]->floating))
	   MoveEwin(lst[i], lst[i]->x + x, lst[i]->y + y);
     }
   return 0;
}
#endif
