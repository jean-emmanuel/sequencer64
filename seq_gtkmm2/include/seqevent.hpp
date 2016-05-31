#ifndef SEQ64_SEQEVENT_HPP
#define SEQ64_SEQEVENT_HPP

/*
 *  This file is part of seq24/sequencer64.
 *
 *  seq24 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  seq24 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with seq24; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          seqevent.hpp
 *
 *  This module declares/defines the base class for the event pane.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2016-05-31
 * \license       GNU GPLv2 or above
 *
 *  The event pane is the thin gridded drawing-area below the seqedit's piano
 *  roll, and contains small boxes representing the position of each event.
 */

#include <gdkmm/pixmap.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/window.h>

#include "globals.h"
#include "gui_drawingarea_gtk2.hpp"
#include "fruityseq.hpp"
#include "midibyte.hpp"                 /* seq64::midibyte, etc.        */
#include "seq24seq.hpp"

namespace Gtk
{
    class Adjustment;
}

namespace seq64
{
    class perform;
    class seqdata;
    class sequence;

/**
 *  Implements the piano event drawing area.
 */

class seqevent : public gui_drawingarea_gtk2
{
    friend struct FruitySeqEventInput;
    friend struct Seq24SeqEventInput;

private:

    /**
     *  Provides the mouse-handling paradigm for the fruity interaction.  Why
     *  should we need both at the same time?  Just load the one that is
     *  specified in the configuration.
     */

    FruitySeqEventInput m_fruity_interaction;

    /**
     *  Provides the normal mouse-handling for Sequencer64.
     */

    Seq24SeqEventInput m_seq24_interaction;

    /**
     *  Provides a reference to the sequence whose data is represented in this
     *  seqevent object.
     */

    sequence & m_seq;

    /**
     *  Zoom setting, means that one pixel == m_zoom ticks.
     */

    int m_zoom;

    /**
     *  The grid-snap setting for the event bar grid.  Same meaning as for the
     *  piano roll.  This value is the denominator of the note size used for
     *  the snap.
     */

    int m_snap;

    /**
     *  The value to use for the PPQN for this sequence.  Used in snap and
     *  zoom scaling.
     */

    int m_ppqn;

    GdkRectangle m_old;
    GdkRectangle m_selected;
    int m_scroll_offset_ticks;
    int m_scroll_offset_x;
    seqdata & m_seqdata_wid;

    /**
     *  Used when highlighting a bunch of events.
     */

    bool m_selecting;
    bool m_moving_init;
    bool m_moving;
    bool m_growing;
    bool m_painting;
    bool m_paste;
    int m_move_snap_offset_x;

    /**
     *  Indicates what is the data window currently editing?
     */

    midibyte m_status;
    midibyte m_cc;

public:

    seqevent
    (
        perform & p,
        sequence & seq,
        int zoom,
        int snap,
        seqdata & seqdata_wid,
        Gtk::Adjustment & hadjust,
        int ppqn = SEQ64_USE_DEFAULT_PPQN
    );

    /**
     *  Let's provide a do-nothing virtual destructor.
     */

    virtual ~seqevent ()
    {
        // I got nothin'
    }

    void reset ();
    void redraw ();
    void set_zoom (int zoom);

    /**
     * \setter m_snap
     *
     *  Simply sets the snap member.
     */

    void set_snap (int snap)
    {
        m_snap = snap;
    }

    void set_data_type (midibyte status, midibyte control);
    void update_sizes ();
    void draw_background ();
    void draw_events_on_pixmap ();
    void draw_pixmap_on_window ();
    void draw_selection_on_window ();
    void update_pixmap ();

private:

    virtual void force_draw ();

    int idle_redraw ();
    void x_to_w (int x1, int x2, int & x, int & w);
    void drop_event (midipulse tick);
    void draw_events_on (Glib::RefPtr<Gdk::Drawable> draw);
    void start_paste ();
    void change_horz ();

    /**
     *  Takes the screen x coordinate, multiplies it by the current zoom, and
     *  returns the tick value in the given parameter.  Why not just return it
     *  normally?
     */

    void convert_x (int x, midipulse & tick)
    {
        tick = x * m_zoom;
    }

    /**
     *  Converts the given tick value to an x corrdinate, based on the zoom,
     *  and returns it via the second parameter.  Why not just return it
     *  normally?
     */

    void convert_t (midipulse ticks, int & x)
    {
        x = ticks / m_zoom;
    }

    /**
     *  This function performs a 'snap' on y.
     */

    void snap_y (int & y)
    {
        y -= (y % c_key_y);
    }

    void snap_x (int & x);

private:        // callbacks

    void on_realize ();
    bool on_expose_event (GdkEventExpose * ev);
    bool on_button_press_event (GdkEventButton * ev);
    bool on_button_release_event (GdkEventButton * ev);
    bool on_motion_notify_event (GdkEventMotion * ev);
    bool on_focus_in_event (GdkEventFocus *);
    bool on_focus_out_event (GdkEventFocus *);
    bool on_key_press_event (GdkEventKey * p0);
    void on_size_allocate (Gtk::Allocation &);

};

}           // namespace seq64

#endif      // SEQ64_SEQEVENT_HPP

/*
 * seqevent.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

