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
 * \file          seqmenu.cpp
 *
 *  This module declares/defines the class that handles the right-click
 *  menu of the sequence slots in the pattern window.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2016-05-12
 * \license       GNU GPLv2 or above
 *
 *  This object also does some minor coordination of editing a sequence via
 *  the pattern editor versus the event editor.
 *
 * \warning
 *      We have currently disabled the "Event Edit..." menu entry at present,
 *      by default, because of a difficult issue with segfaults caused when
 *      events are deleted by the event editor (classes eventedit and
 *      eventslots).  If you want to enable the event editor for your own
 *      experiments, use the "--enable-eveditor" option with the configure
 *      script.
 */

#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>

#include "eventedit.hpp"
#include "font.hpp"
#include "perform.hpp"
#include "seqedit.hpp"
#include "seqmenu.hpp"

using namespace Gtk::Menu_Helpers;

namespace seq64
{

/**
 *  Principal constructor.  Apart from filling in some of the members,
 *  this function initializes the clipboard, so that we don't get a crash
 *  on a paste with no previous copy.
 */

seqmenu::seqmenu (perform & p)
 :
    m_menu          (nullptr),
    m_mainperf      (p),
    m_clipboard     (),
    m_seqedit       (nullptr),
    m_eventedit     (nullptr),
    m_current_seq   (-1)            /* (0) is not really current yet    */
{
    m_clipboard.set_master_midi_bus(&m_mainperf.master_bus());
}

/**
 *  A rote destructor.  If we determine that we need to delete the
 *  m_seqedit pointer, we can do it here.  But that is not likely,
 *  because we can have many new seqedit objects in play, because we can
 *  edit many at once.
 */

seqmenu::~seqmenu ()
{
    /*
     * if (not_nullptr(m_seqedit))
     *      delete(m_seqedit)
     *
     * if (not_nullptr(m_eventedit))
     *      delete(m_eventedit)
     */
}

/**
 *  This function sets up the File menu entries.  It also sets up the pattern
 *  popup menu entries that are used in mainwid.  Note that, for the selected
 *  sequence, the "Edit" and "Event Edit" menu entries are not included if a
 *  pattern editor or event editor is already running.
 */

void
seqmenu::popup_menu ()
{
    if (not_nullptr(m_menu))
        delete m_menu;

    m_menu = manage(new Gtk::Menu());
    if (is_current_seq_active())
    {
        if (! get_current_sequence()->get_editing())
        {
            m_menu->items().push_back
            (
                MenuElem("Edit...", mem_fun(*this, &seqmenu::seq_edit))
            );

#if SEQ64_ENABLE_EVENT_EDITOR

            /**
             * The new event editor seems to create far-reaching problems that
             * we do not yet understand, so it is now possible to disable it at
             * build time.  We have mitigated most of those problems by not
             * allowing both a seq_edit() and a seq_event_edit() at the same
             * time.
             */

            m_menu->items().push_back
            (
                MenuElem
                (
                    "Event Edit...", mem_fun(*this, &seqmenu::seq_event_edit)
                )
            );
#endif
            m_menu->items().push_back(SeparatorElem());
        }
    }
    else
    {
        m_menu->items().push_back
        (
            MenuElem("New", mem_fun(*this, &seqmenu::seq_edit))
        );
        m_menu->items().push_back(SeparatorElem());
    }

    if (is_current_seq_active())
    {
        m_menu->items().push_back
        (
            MenuElem("Cut", mem_fun(*this, &seqmenu::seq_cut))
        );
        m_menu->items().push_back
        (
            MenuElem("Copy", mem_fun(*this, &seqmenu::seq_copy))
        );
    }
    else
    {
        m_menu->items().push_back
        (
            MenuElem("Paste", mem_fun(*this, &seqmenu::seq_paste))
        );
    }
    m_menu->items().push_back(SeparatorElem());
    Gtk::Menu * menu_song = manage(new Gtk::Menu());
    m_menu->items().push_back(MenuElem("Song", *menu_song));
    if (is_current_seq_active())
    {
        menu_song->items().push_back
        (
            MenuElem("Clear Song Data", mem_fun(*this, &seqmenu::seq_clear_perf))
        );
    }
    menu_song->items().push_back
    (
        MenuElem("Mute All Tracks", mem_fun(*this, &seqmenu::mute_all_tracks))
    );

    /*
     * This is the MIDI channel menu accessible from a non-empty pattern slot
     * on the main window.
     */

    if (is_current_seq_active())
    {
        m_menu->items().push_back(SeparatorElem());
        Gtk::Menu * menu_buses = manage(new Gtk::Menu());
        m_menu->items().push_back(MenuElem("MIDI Bus", *menu_buses));

        /* Get the MIDI buses */

        mastermidibus & masterbus = m_mainperf.master_bus();
        for (int bus = 0; bus < masterbus.get_num_out_buses(); ++bus)
        {
            Gtk::Menu * menu_channels = manage(new Gtk::Menu());
            menu_buses->items().push_back
            (
                MenuElem(masterbus.get_midi_out_bus_name(bus), *menu_channels)
            );

            char b[4];
            for (int channel = 0; channel < 16; channel++) /* channel menu */
            {
                snprintf(b, sizeof(b), "%d", channel + 1);
                std::string name = std::string(b);
                std::string s = usr().instrument_name(bus, channel);
                if (! s.empty())
                    name += (std::string(" ") + s);

#define SET_BUS     mem_fun(*this, &seqmenu::set_bus_and_midi_channel)

                menu_channels->items().push_back
                (
                    MenuElem(name, sigc::bind(SET_BUS, bus, channel))
                );
            }
        }
    }
    m_menu->popup(0, 0);
}

/**
 *  Sets up the bus, MIDI channel, and dirtiness flag of the current
 *  sequence in the main perform object, as per the give parameters.
 */

void
seqmenu::set_bus_and_midi_channel (int a_bus, int a_ch)
{
    if (is_current_seq_active())
    {
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
        {
            s->set_midi_bus(a_bus);
            s->set_midi_channel(a_ch);
            s->set_dirty();
        }
    }
}

/**
 *  Mutes all tracks in the main perform object.
 */

void
seqmenu::mute_all_tracks ()
{
    m_mainperf.mute_all_tracks();
}

/**
 *  This menu callback launches the sequence-editor (pattern editor)
 *  window.  If it is already open for that sequence, this function just
 *  raises it.
 *
 *  Note that the m_seqedit member to which we save the new pointer is
 *  currently there just to avoid a compiler warning.
 *
 *  Also, if a new sequences is created, we set the m_modified flag to
 *  true, even though the sequence might later be deleted.  Too much
 *  modification to keep track of!
 *
 *  An oddity is that calling show_all() here does not work unless the
 *  seqedit() constructor makes its show_all() call.
 */

void
seqmenu::seq_edit ()
{
    if (is_current_seq_active())
    {
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
        {
            if (! s->get_editing())
                m_seqedit = new seqedit(m_mainperf, *s, current_seq());
            else
                s->set_raise(true);
        }
    }
    else
    {
        seq_new();
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
            m_seqedit = new seqedit(m_mainperf, *s, current_seq());
    }
#ifdef SEQ64_EDIT_SEQUENCE_HIGHLIGHT
    set_edit_sequence(current_seq());
#endif
}

/**
 *  This menu callback launches the new event editor window.  If it is already
 *  open for that sequence, this function just raises it.
 *
 *  Note that the m_eventedit member to which we save the new pointer is
 *  currently there just to avoid a compiler warning.
 *
 *  This menu entry is available only if the selected sequence is active.
 *  That is, if the sequence has already been created.
 *
 *  An oddity is that we need the show_all() call here in order to see the
 *  dialog.  A situation different from that for seqedit!  However, now it
 *  doesn't seem to be needed, and we have put it back into the eventedit
 *  constructor.
 */

void
seqmenu::seq_event_edit ()
{
    if (is_current_seq_active())
    {
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
        {
            if (! s->get_editing())
                m_eventedit = new eventedit(m_mainperf, *s);
            else
                s->set_raise(true);
        }
    }
    else
    {
        seq_new();
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
            m_eventedit = new eventedit(m_mainperf, *s);
    }
#ifdef SEQ64_EDIT_SEQUENCE_HIGHLIGHT
    set_edit_sequence(current_seq());
#endif
}

/**
 *  This function sets the new sequence into the perform object, a bit
 *  prematurely, though.  For one thing, if current_seq() is either a -1
 *  or is greater than the maximum allowed sequence number,
 *  perform::is_active() will return false, and we have no idea whether the
 *  sequence is not active or the sequence number is just invalid.  So we need
 *  to check the pointer we got before trying to use it.
 */

void
seqmenu::seq_new ()
{
    if (! is_current_seq_active())
    {
        new_current_sequence();
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
            s->set_dirty();
    }
}

/**
 *  Copies the selected (current) sequence to the clipboard sequence.
 *  We use a more appropriate function than operator =() here:
 *  sequence::partial_assign().
 *
 * \todo
 *      Can be offloaded to a perform member function that accepts a
 *      sequence clipboard non-const reference parameter.
 */

void
seqmenu::seq_copy ()
{
    if (is_current_seq_active())
        m_clipboard.partial_assign(*get_current_sequence());
}

/**
 *  Deletes the selected (current) sequence and copies it to the clipboard
 *  sequence, <i> if </i> it is not in edit mode.
 *
 * \todo
 *      A lot of seq_cut() can be offloaded to a (new) perform member
 *      function that takes a sequence clipboard non-const reference
 *      parameter.
 */

void
seqmenu::seq_cut ()
{
    if (is_current_seq_active() && ! is_current_seq_in_edit())
    {
        m_clipboard.partial_assign(*get_current_sequence());
        m_mainperf.delete_sequence(current_seq());
        redraw(current_seq());
    }
}

/**
 *  Pastes the sequence clipboard into the current sequence, if the
 *  current sequence slot is not active.  Then it sets the dirty flag for
 *  the destination sequence.
 *
 * \todo
 *      All of seq_paste() can be offloaded to a (new) perform member
 *      function with a const clipboard reference parameter.
 */

void
seqmenu::seq_paste ()
{
    if (! is_current_seq_active())
    {
        new_current_sequence();
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
        {
            s->partial_assign(m_clipboard);
            s->set_dirty();
        }
    }
}

/**
 *  If the current sequence is active, this function pushes a trigger
 *  undo in the main perform object, clears its sequence triggers for the
 *  current sequence, and sets the dirty flag of the sequence.
 *
 * \todo
 *      All of seq_paste() can be offloaded to a (new) perform member
 *      function.
 */

void
seqmenu::seq_clear_perf ()
{
    if (is_current_seq_active())
    {
        m_mainperf.push_trigger_undo();
        m_mainperf.clear_sequence_triggers(current_seq());
        sequence * s = get_current_sequence();
        if (not_nullptr(s))
            s->set_dirty();
    }
}

}           // namespace seq64

/*
 * seqmenu.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

