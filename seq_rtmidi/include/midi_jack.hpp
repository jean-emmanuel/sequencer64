#ifndef SEQ64_MIDI_JACK_HPP
#define SEQ64_MIDI_JACK_HPP

/**
 * \file          midi_jack.hpp
 *
 *    A class for realtime MIDI input/output via JACK.
 *
 * \author        Gary P. Scavone; refactoring by Chris Ahlstrom
 * \date          2016-11-14
 * \updates       2016-12-02
 * \license       See the rtexmidi.lic file.  Too big for a header file.
 *
 *    In this refactoring...
 */

#include <string>

#include "midi_api.hpp"

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq64
{

/**
 *  The class for handling JACK MIDI input.
 */

class midi_in_jack: public midi_in_api
{

protected:

    std::string m_clientname;

public:

    midi_in_jack (const std::string & clientname, unsigned queuesize);
    virtual ~midi_in_jack ();

    /**
     * \getter RTMIDI_API_UNIX_JACK
     */

    virtual rtmidi_api get_current_api () const
    {
        return RTMIDI_API_UNIX_JACK;
    }

    virtual void open_port (unsigned portnumber, const std::string & portname);
    virtual void open_virtual_port (const std::string & portname);
    virtual void close_port ();
    virtual unsigned get_port_count ();
    virtual std::string get_port_name (unsigned portnumber);

private:

    /* virtual */ void initialize (const std::string & clientname);

    void connect ();

};          // midi_in_jack

/**
 *  The JACK MIDI output API class.
 */

class midi_out_jack: public midi_out_api
{

protected:

    std::string m_clientname;

public:

    midi_out_jack (const std::string & clientname);
    virtual ~midi_out_jack ();

    /**
     * \getter RTMIDI_API_UNIX_JACK
     */

    virtual rtmidi_api get_current_api () const
    {
        return RTMIDI_API_UNIX_JACK;
    }

    virtual void open_port (unsigned portnumber, const std::string & portname);
    virtual void open_virtual_port (const std::string & portname);
    virtual void close_port ();
    virtual unsigned get_port_count ();
    virtual std::string get_port_name (unsigned portnumber);
    virtual void send_message (const std::vector<midibyte> & message);

private:

    /* virtual */ void initialize (const std::string & clientname);

    void connect ();

};          // midi_out_jack

}           // namespace seq64

#endif      // SEQ64_MIDI_JACK_HPP

/*
 * midi_jack.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
