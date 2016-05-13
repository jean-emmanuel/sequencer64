#ifndef SEQ64_SEQEDIT_HPP
#define SEQ64_SEQEDIT_HPP

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
 * \file          seqedit.hpp
 *
 *  This module declares/defines the base class for editing a
 *  pattern/sequence.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2016-05-12
 * \license       GNU GPLv2 or above
 *
 *  The seqedit is a kind of master class for holding aseqroll, seqkeys,
 *  seqdata, and seqevent object.
 */

#include <list>
#include <string>
#include <gtkmm/window.h>

#include "gui_window_gtk2.hpp"

namespace Gtk
{
    class Adjustment;
    class Entry;
    class HScrollbar;
    class Image;
    class Menu;
    class MenuBar;
    class Table;
    class RadionButton;
    class ToggleButton;
    class Tooltips;
    class VBox;
    class VScrollbar;
}

namespace seq64
{

class perform;
class seqevent;
class seqdata;
class seqkeys;
class seqroll;
class seqtime;
class sequence;

/**
 *  Implements the Pattern Editor, which has references to:
 *
 *      -   perform
 *      -   seqroll
 *      -   seqkeys
 *      -   seqdata
 *      -   seqtime
 *      -   seqevent
 *      -   sequence
 *
 *  This class has a metric ton of user-interface objects and other
 *  members.
 */

class seqedit : public gui_window_gtk2
{

private:

    static int m_initial_snap;
    static int m_initial_note_length;

    /**
     *  Provides the zoom values: 1  2  3  4, and 1, 2, 4, 8, 16.
     *  The value of zoom is the same as the number of pixels per tick on the
     *  piano roll.
     */

    int m_zoom;

    /**
     *  Used in setting the snap-to value in pulses, off = 1.
     */

    int m_snap;

    /**
     *  The default length of a note to be inserted by a right-left-click
     *  operation.
     */

    int m_note_length;

    /**
     *  Settings for the music scale, key, and background sequence.
     */

    int m_scale;
    int m_key;
    int m_bgsequence;

    /**
     *  Provides the length of the sequence in measures.
     */

    long m_measures;

    /**
     *  Holds a copy of the current PPQN for the sequence (and the entire MIDI
     *  file).
     */

    int m_ppqn;

    /**
     *  Holds a reference to the sequence that this window represents.
     */

    sequence & m_seq;

    /**
     *  A number of user-interface objects.
     */

    Gtk::MenuBar * m_menubar;
    Gtk::Menu * m_menu_tools;
    Gtk::Menu * m_menu_zoom;
    Gtk::Menu * m_menu_snap;
    Gtk::Menu * m_menu_note_length;

    /**
     *  Provides the length in measures.
     */

    Gtk::Menu * m_menu_length;
    Gtk::Menu * m_menu_midich;
    Gtk::Menu * m_menu_midibus;
    Gtk::Menu * m_menu_data;
    Gtk::Menu * m_menu_key;
    Gtk::Menu * m_menu_scale;
    Gtk::Menu * m_menu_sequences;

    /**
     *  These member provife the time signature, beats per measure, and
     *  beat width menus.
     */

    Gtk::Menu * m_menu_bpm;
    Gtk::Menu * m_menu_bw;
    Gtk::Menu * m_menu_rec_vol;

    /**
     * Basically the sequence number.  We will eventually remove this member,
     * as we can get it via sequence::number() now.
     */

    int m_pos;

    /**
     *  Scrollbar and adjustment objects for horizontal and vertical panning.
     */

    Gtk::Adjustment * m_vadjust;
    Gtk::Adjustment * m_hadjust;
    Gtk::VScrollbar * m_vscroll_new;
    Gtk::HScrollbar * m_hscroll_new;

    /**
     *  Handles the piano-keys part of the user-interface.  This item draws the
     *  piano-keys at the left of the seqedit window.
     */

    seqkeys * m_seqkeys_wid;

    /**
     *  Handles the time-line (bar or measures) part of the user-interface.
     *  This is the location where the measure numbers and the END marker are
     *  shown.
     */

    seqtime * m_seqtime_wid;

    /**
     *  Handles the event-data part of the user-interface.  This is the area at
     *  the bottom of the window that shows value lines for the selected kinds
     *  of events.
     */

    seqdata * m_seqdata_wid;

    /**
     *  Handles the small event part of the user-interface, where events can be
     *  moved and added.
     */

    seqevent * m_seqevent_wid;

    /**
     *  Handles the piano-roll part of the user-interface.
     */

    seqroll * m_seqroll_wid;

    /**
     *  More user-interface elements.  These items provide a number of buttons
     *  and text-entry fields, as well as their layout.
     */

    Gtk::Table * m_table;
    Gtk::VBox * m_vbox;
    Gtk::HBox * m_hbox;
    Gtk::HBox * m_hbox2;
    Gtk::HBox * m_hbox3;
    Gtk::Button * m_button_undo;
    Gtk::Button * m_button_redo;
    Gtk::Button * m_button_quantize;
    Gtk::Button * m_button_tools;
    Gtk::Button * m_button_sequence;
    Gtk::Entry * m_entry_sequence;
    Gtk::Button * m_button_bus;
    Gtk::Entry * m_entry_bus;
    Gtk::Button * m_button_channel;
    Gtk::Entry * m_entry_channel;
    Gtk::Button * m_button_snap;
    Gtk::Entry * m_entry_snap;
    Gtk::Button * m_button_note_length;
    Gtk::Entry * m_entry_note_length;
    Gtk::Button * m_button_zoom;
    Gtk::Entry * m_entry_zoom;
    Gtk::Button * m_button_length;
    Gtk::Entry * m_entry_length;
    Gtk::Button * m_button_key;
    Gtk::Entry * m_entry_key;
    Gtk::Button * m_button_scale;
    Gtk::Entry * m_entry_scale;
    Gtk::Tooltips * m_tooltips;
    Gtk::Button * m_button_data;
    Gtk::Entry * m_entry_data;
    Gtk::Button * m_button_bpm;
    Gtk::Entry * m_entry_bpm;
    Gtk::Button * m_button_bw;
    Gtk::Entry * m_entry_bw;
    Gtk::Button * m_button_rec_vol;
    Gtk::ToggleButton * m_toggle_play;
    Gtk::ToggleButton * m_toggle_record;
    Gtk::ToggleButton * m_toggle_q_rec;
    Gtk::ToggleButton * m_toggle_thru;
    Gtk::RadioButton * m_radio_select;
    Gtk::RadioButton * m_radio_grow;
    Gtk::RadioButton * m_radio_draw;
    Gtk::Entry * m_entry_name;

    /**
     *  Indicates what is the data window currently editing?
     */

    midibyte m_editing_status;
    midibyte m_editing_cc;

    /**
     *  Indicates that the focus has already been changed to this sequence.
     */

    bool m_have_focus;

public:

    seqedit
    (
        perform & perf,
        sequence & seq,
        int pos,
        int ppqn = SEQ64_USE_DEFAULT_PPQN
    );
    ~seqedit ();

private:

    void set_zoom (int zoom);
    void set_snap (int snap);
    void set_note_length (int note_length);
    void set_beats_per_bar (int bpm);
    void set_beat_width (int bw);

    /**
     *  Passes the given parameter to sequence::set_rec_vol().
     */

    void set_rec_vol (int recvol)
    {
        m_seq.set_rec_vol(recvol);
    }

    /**
     *  This function provides optimization for the on_scroll_event() function.
     *  A duplicate of the one in seqroll.
     *
     * \param step
     *      Provides the step value to use for adjusting the horizontal
     *      scrollbar.  See gui_drawingarea_gtk2::scroll_adjust() for more
     *      information.
     */

    void horizontal_adjust (double step)
    {
        scroll_adjust(*m_hadjust, step);
    }

    void set_measures (int lim);
    void apply_length (int bpm, int bw, int measures);
    long get_measures ();
    void set_midi_channel (int midichannel);
    void set_midi_bus (int midibus);
    void set_scale (int scale);
    void set_key (int note);
    void set_background_sequence (int seq);
    void name_change_callback ();
    void play_change_callback ();
    void record_change_callback ();
    void q_rec_change_callback ();
    void thru_change_callback ();
    void undo_callback ();
    void redo_callback ();
    void set_data_type (midibyte status, midibyte control = 0);
    void update_all_windows ();
    void fill_top_bar ();
    void create_menus ();
    void popup_menu (Gtk::Menu * menu);
    void popup_event_menu ();
    void popup_midibus_menu ();
    void popup_sequence_menu ();
    void popup_tool_menu ();
    void popup_midich_menu ();
    Gtk::Image * create_menu_image (bool state = false);
    bool timeout ();
    void do_action (int action, int var);
    void mouse_action (mouse_action_e action);
    void change_focus (bool set_it = true);
    void handle_close ();

private:    // Gtkmm 2.4 callbacks

    void on_realize ();
    void on_set_focus (Widget * focus);
    bool on_focus_in_event (GdkEventFocus *);
    bool on_focus_out_event (GdkEventFocus *);
    bool on_delete_event (GdkEventAny * event);
    bool on_scroll_event (GdkEventScroll * ev);
    bool on_key_press_event (GdkEventKey * ev);
};

}           // namespace seq64

#endif      // SEQ64_SEQEDIT_HPP

/*
 * seqedit.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
