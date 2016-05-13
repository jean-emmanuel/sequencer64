#ifndef SEQ64_EVENTEDIT_HPP
#define SEQ64_EVENTEDIT_HPP

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
 * \file          eventedit.hpp
 *
 *  This module declares/defines the base class for the Performance Editor,
 *  also known as the Song Editor.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-12-05
 * \updates       2016-05-12
 * \license       GNU GPLv2 or above
 *
 */

#include <gtkmm/widget.h>               /* can't forward-declare GdkEventAny */
#include <gtkmm/window.h>               /* ditto for Window                  */

#include "gui_window_gtk2.hpp"

namespace Gtk
{
    /*
     *  Since these items are pointers, we were able to move (most) of the
     *  included header files to the cpp file.   Except for the items that
     *  come from widget.h, perhaps because GdkEventAny was a typedef.
     */

    class Adjustment;
    class Button;
    class Entry;
    class HBox;
    class HScrollbar;
    class Label;
    class Menu;
    class Table;
    class ToggleButton;
    class Tooltips;
    class VBox;
    class VScrollbar;
}

namespace seq64
{

class eventslots;
class perform;
class sequence;

/**
 *  This class supports an Event Editor that is used to tweak the details of
 *  events and get a better idea of the mix of events in a sequence.
 */

class eventedit : public gui_window_gtk2
{

private:

    /**
     *  A whole horde of GUI elements.
     */

    Gtk::Table * m_table;
    Gtk::Adjustment * m_vadjust;
    Gtk::VScrollbar * m_vscroll;
    eventslots * m_eventslots;
    Gtk::HBox * m_htopbox;
    Gtk::VBox * m_showbox;
    Gtk::VBox * m_editbox;
    Gtk::VBox * m_optsbox;
    Gtk::HBox * m_bottbox;
    Gtk::VBox * m_rightbox;

    Gtk::Button * m_button_del;
    Gtk::Button * m_button_ins;
    Gtk::Button * m_button_modify;
    Gtk::Button * m_button_save;
    Gtk::Button * m_button_cancel;

    /**
     *  Items to size the m_indexslots member.
     */

    Gtk::Label * m_label_index;
    Gtk::Label * m_label_time;
    Gtk::Label * m_label_event;

    /**
     * Items for the inside of the m_showbox member.
     */

    Gtk::Label * m_label_seq_name;
    Gtk::Label * m_label_time_sig;
    Gtk::Label * m_label_ppqn;
    Gtk::Label * m_label_channel;
    Gtk::Label * m_label_ev_count;
    Gtk::Label * m_label_spacer;
    Gtk::Label * m_label_modified;

    /**
     *  Items for the inside of the m_editbox member.
     */

    Gtk::Label * m_label_category;
    Gtk::Entry * m_entry_ev_timestamp;
    Gtk::Entry * m_entry_ev_name;
    Gtk::Entry * m_entry_ev_data_0;
    Gtk::Entry * m_entry_ev_data_1;
    Gtk::Label * m_label_time_fmt;

    /**
     *  Padding for the right side of the user-interface.
     */

    Gtk::Label * m_label_right;

    /**
     *  A reference to the sequence being edited, to control its editing flag.
     */

    sequence & m_seq;

    /**
     *  Indicates that the focus has already been changed to this sequence.
     */

    bool m_have_focus;

public:

    eventedit (perform & p, sequence & seq);
    ~eventedit ();

    void enqueue_draw ();
    void set_seq_title (const std::string & title);
    void set_seq_time_sig (const std::string & sig);
    void set_seq_ppqn (const std::string & p);
    void set_seq_count ();
    void set_event_category (const std::string & c);
    void set_event_timestamp (const std::string & ts);
    void set_event_name (const std::string & n);
    void set_event_data_0 (const std::string & d);
    void set_event_data_1 (const std::string & d);
    void perf_modify ();
    void set_dirty (bool flag = true);
    void v_adjustment (int value);
    void v_adjustment (int value, int lower, int upper);
    void change_focus (bool set_it = true);
    void handle_close ();

private:

    /*
     * We don't need a timeout in this static editing window which doesn't
     * interact directly with other editing windows.
     *
     * bool timeout ();
     */

    void handle_delete ();
    void handle_insert ();
    void handle_modify ();
    void handle_save ();
    void handle_cancel ();

private:            // Gtkmm 2.4 callbacks

    void on_realize ();
    void on_set_focus (Widget * focus);
    bool on_focus_in_event (GdkEventFocus *);
    bool on_focus_out_event (GdkEventFocus *);
    bool on_key_press_event (GdkEventKey * ev);
    bool on_delete_event (GdkEventAny * event);

};          // class eventedit

}           // namespace seq64

#endif      // SEQ64_EVENTEDIT_HPP

/*
 * eventedit.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

