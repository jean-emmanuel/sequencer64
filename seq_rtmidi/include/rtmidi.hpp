#ifndef SEQ64_RTMIDI_HPP
#define SEQ64_RTMIDI_HPP

/**
 * \file          rtmidi.hpp
 *
 *  An abstract base class for realtime MIDI input/output.
 *
 * \author        Gary P. Scavone; refactoring by Chris Ahlstrom
 * \date          2016-11-14
 * \updates       2016-12-09
 * \license       See the rtexmidi.lic file.  Too big for a header file.
 *
 *  The big difference between this class (seq64::rtmidi) and
 *  seq64::rtmidi_info is that it gets information via midi_api-derived
 *  functions, while the latter gets if via midi_api_info-derived functions.
 */

#include "midi_api.hpp"                     /* seq64::midi[_in][_out]_api   */
#include "rterror.hpp"                      /* seq64::rterror               */
#include "rtmidi_base.hpp"                  /* seq64::rtmidi_base           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq64
{

/**
 *  The main class of the rtmidi API.  We moved the enum Api definition into
 *  the new rtmidi_types.hpp module to make refactoring the code easier.
 */

class rtmidi : public rtmidi_base
{
    friend class midibus;

protected:

    rtmidi ();
    virtual ~rtmidi ();

public:

    /*
     *  Checks the input queue.  If the API object doesn't have an input
     *  queue, this function will throw an rterror, for now.
     *
     * \return
     *      Returns true if the input queue is not empty.
     */

    bool poll_queue () const
    {
        return get_api()->poll_queue();
    }

    virtual void open_port
    (
        unsigned portnumber = 0,
        const std::string & portname = "rtmidi"
    ) = 0;

    virtual void open_virtual_port (const std::string & portname = "rtmidi") = 0;
    virtual void close_port () = 0;
    virtual bool is_port_open () const = 0;

    /**
     *  Gets the buss/client ID for a MIDI interfaces.  This is the left-hand
     *  side of a X:Y pair (such as 128:0).
     *
     *  This function is a new part of the RtMidi interface.
     *
     * \param index
     *      The ordinal index of the desired interface to look up.
     *
     * \return
     *      Returns the buss/client value as provided by the selected API.
     */

    virtual unsigned get_client_id (unsigned index)
    {
        return get_api()->get_client_id(index);
    }

    virtual unsigned get_port_count () = 0;

    virtual unsigned get_port_number (unsigned /*index*/) //  = 0;
    {
        return SEQ64_BAD_PORT_ID;
    }

    virtual std::string get_port_name (unsigned index) = 0;

    /**
     *  Set an error callback function to be invoked when an error has
     *  occured.
     *
     *  The callback function will be called whenever an error has occured. It
     *  is best to set the error callback function before opening a port.
     */

    virtual void seterrorcallback
    (
        rterror_callback errorcallback = nullptr,
        void * userdata = nullptr
    ) = 0;

    virtual void send_message (const std::vector<midibyte> &) = 0;
    virtual void ignore_types (bool, bool, bool) = 0;
    virtual double get_message (std::vector<midibyte> &) = 0;

protected:

};          // class rtmidi

/**
 *  A realtime MIDI input class.
 *
 *  This class provides a common, platform-independent API for
 *  realtime MIDI input.  It allows access to a single MIDI input
 *  port.  Incoming MIDI messages are either saved to a queue for
 *  retrieval using the get_message() function or immediately passed to
 *  a user-specified callback function.  Create multiple instances of
 *  this class to connect to more than one MIDI device at the same
 *  time.  With the OS-X, Linux ALSA, and JACK MIDI APIs, it is also
 *  possible to open a virtual input port to which other MIDI software
 *  clients can connect.
 *
 *  By Gary P. Scavone, 2003-2014.
 */

class rtmidi_in : public rtmidi
{

public:

    /**
     *  Default constructor that allows an optional api, client name and queue
     *  size.
     *
     *  An exception will be thrown if a MIDI system initialization
     *  error occurs.  The queue size defines the maximum number of
     *  messages that can be held in the MIDI queue (when not using a
     *  callback function).  If the queue size limit is reached,
     *  incoming messages will be ignored.
     *
     *  If no API argument is specified and multiple API support has been
     *  compiled, the default order of use is ALSA, JACK (Linux) and CORE,
     *  JACK (OS-X).
     *
     * \param api
     *      An optional API id can be specified.
     *
     * \param clientname
     *      An optional client name can be specified. This will be used to
     *      group the ports that are created by the application.
     *
     * \param queuesizelimit
     *      An optional size of the MIDI input queue can be specified.
     */

    rtmidi_in
    (
        rtmidi_api api = RTMIDI_API_UNSPECIFIED,
        const std::string & clientname = "rtmidi input client",
        unsigned queuesizelimit = 100
    );

    /**
     *  If a MIDI connection is still open, it will be closed by the destructor.
     */

    virtual ~rtmidi_in ();

    /**
     *  Returns the MIDI API specifier for the current instance of rtmidi_in.
     *  This is an integer enumeration value, starting with
     *  RTMIDI_API_UNSPECIFIED.
     *
     *  This function could be moved into rtmidi_base, if we see the need for
     *  that.
     */

    rtmidi_api get_current_api () const
    {
        return get_api()->get_current_api();
    }

    /**
     *  Open a MIDI input connection given by enumeration number.
     *
     * \param portnumber
     *      An optional port number greater than 0 can be specified.
     *      Otherwise, the default or first port found is opened.
     *
     * \param portname
     *      An optional name for the application port that is used to connect
     *      to portId can be specified.
     */

    void open_port
    (
        unsigned portnumber,
        const std::string & portname = "rtmidi input"
    )
    {
       get_api()->open_port(portnumber, portname);
    }

    /**
     *  Create a virtual input port, with optional name, to allow software
     *  connections (OS X, JACK and ALSA only).
     *
     *  This function creates a virtual MIDI input port to which other
     *  software applications can connect.  This type of functionality
     *  is currently only supported by the Macintosh OS-X, any JACK,
     *  and Linux ALSA APIs (the function returns an error for the other APIs).
     *
     * \param portname
     *      An optional name for the application port that is used to connect
     *      to portId can be specified.
     */

    void open_virtual_port (const std::string & portname = "rtmidi input")
    {
       get_api()->open_virtual_port(portname);
    }

    /**
     *  Set a callback function to be invoked for incoming MIDI messages.
     *
     *  The callback function will be called whenever an incoming MIDI
     *  message is received.  While not absolutely necessary, it is best
     *  to set the callback function before opening a MIDI port to avoid
     *  leaving some messages in the queue.
     *
     * \param callback
     *      A callback function must be given.
     *
     * \param userdata
     *      Optionally, a pointer to additional data can be passed to the
     *      callback function whenever it is called.
     */

    void set_callback (rtmidi_callback_t callback, void * userdata = nullptr)
    {
       dynamic_cast<midi_in_api *>(get_api())->set_callback(callback, userdata);
    }

    /**
     *  Cancel use of the current callback function (if one exists).
     *
     *  Subsequent incoming MIDI messages will be written to the queue
     *  and can be retrieved with the \e get_message function.
     */

    void cancel_callback ()
    {
       dynamic_cast<midi_in_api *>(get_api())->cancel_callback();
    }

    /**
     *  Close an open MIDI connection (if one exists).
     */

    void close_port ()
    {
       get_api()->close_port();
    }

    /**
     *  Returns true if a port is open and false if not.
     */

    virtual bool is_port_open () const
    {
       return get_api()->is_port_open();
    }

    /**
     *  Return the number of available MIDI input ports.
     *
     * \return
     *      This function returns the number of MIDI ports of the selected API.
     */

    unsigned get_port_count ()
    {
       return get_api()->get_port_count();
    }

    /**
     *  Return a string identifier for the specified MIDI input port number.
     *
     * \return
     *      The name of the port with the given Id is returned.  An empty
     *      string is returned if an invalid port specifier is provided.
     */

    std::string get_port_name (unsigned portnumber)
    {
       return get_api()->get_port_name(portnumber);
    }

    /**
     *  Specify whether certain MIDI message types should be queued or ignored
     *  during input.
     *
     *  By default, MIDI timing and active sensing messages are ignored
     *  during message input because of their relative high data rates.
     *  MIDI sysex messages are ignored by default as well.  Variable
     *  values of "true" imply that the respective message type will be
     *  ignored.
     *
     *  TEMPORARILY VIRTUAL:
     */

    virtual void ignore_types
    (
        bool midisysex = true,
        bool miditime = true,
        bool midisense = true
    )
    {
       dynamic_cast<midi_in_api *>(get_api())->ignore_types(midisysex, miditime, midisense);
    }

    /**
     *  Fill the user-provided vector with the data bytes for the next
     *  available MIDI message in the input queue and return the event
     *  delta-time in seconds.
     *
     *  This function returns immediately whether a new message is
     *  available or not.  A valid message is indicated by a non-zero
     *  vector size.  An exception is thrown if an error occurs during
     *  message retrieval or an input connection was not previously
     *  established.
     *
     * \param message
     *      A string of characters for the messages.
     *
     * \return
     *      Returns the delta-time (timestamp) of the incoming message.  If an
     *      error occurs, or if there is not message, then 0.0 is returned.
     *
     *  TEMPORARILY VIRTUAL:
     */

    virtual double get_message (std::vector<midibyte> & message)
    {
       return dynamic_cast<midi_in_api *>(get_api())->get_message(message);
    }

    /**
     *  Set an error callback function to be invoked when an error has
     *  occured.
     *
     *  The callback function will be called whenever an error has occured. It
     *  is best to set the error callback function before opening a port.
     */

    virtual void seterrorcallback
    (
        rterror_callback errorcallback = nullptr,
        void * userdata = 0
    )
    {
       get_api()->seterrorcallback(errorcallback, userdata);
    }

    virtual void send_message (const std::vector<midibyte> &);

protected:

    void openmidi_api
    (
        rtmidi_api api,
        const std::string & clientname,
        unsigned queuesizelimit
    );

};

/**
 *  A realtime MIDI output class.
 *
 *  This class provides a common, platform-independent API for MIDI
 *  output.  It allows one to probe available MIDI output ports, to
 *  connect to one such port, and to send MIDI bytes immediately over
 *  the connection.  Create multiple instances of this class to
 *  connect to more than one MIDI device at the same time.  With the
 *  OS-X, Linux ALSA and JACK MIDI APIs, it is also possible to open a
 *  virtual port to which other MIDI software clients can connect.
 *
 *  by Gary P. Scavone, 2003-2014.
 */

class rtmidi_out : public rtmidi
{

public:

    /**
     *  Default constructor that allows an optional client name.
     *
     *  An exception will be thrown if a MIDI system initialization error occurs.
     *
     *  If no API argument is specified and multiple API support has been
     *  compiled, the default order of use is ALSA, JACK (Linux) and CORE,
     *  JACK (OS-X).
     */

    rtmidi_out
    (
        rtmidi_api api = RTMIDI_API_UNSPECIFIED,
        const std::string & clientname = "rtmidi output client"
    );

    /**
     *  The destructor closes any open MIDI connections.
     */

    virtual ~rtmidi_out ();

    virtual void ignore_types (bool, bool, bool);
    virtual double get_message (std::vector<midibyte> &);

    /**
     *  Returns the MIDI API specifier for the current instance of rtmidi_out.
     */

    rtmidi_api get_current_api () const
    {
       return get_api()->get_current_api();
    }

    /**
     *  Open a MIDI output connection.
     *
     *   An optional port number greater than 0 can be specified.
     *   Otherwise, the default or first port found is opened.  An
     *   exception is thrown if an error occurs while attempting to make
     *   the port connection.
     */

    void open_port
    (
        unsigned portnumber,
        const std::string & portname = "rtmidi output"
    )
    {
       get_api()->open_port(portnumber, portname);
    }

    /**
     *  Close an open MIDI connection (if one exists).
     */

    void close_port ()
    {
       get_api()->close_port();
    }

    /**
     *  Returns true if a port is open and false if not.
     */

    virtual bool is_port_open () const
    {
       return get_api()->is_port_open();
    }

    /**
     *  Create a virtual output port, with optional name, to allow software
     *  connections (OS X, JACK and ALSA only).
     *
     *  This function creates a virtual MIDI output port to which other
     *  software applications can connect.  This type of functionality is
     *  currently only supported by the Macintosh OS-X, Linux ALSA and JACK
     *  APIs (the function does nothing with the other APIs).  An exception is
     *  thrown if an error occurs while attempting to create the virtual port.
     */

    void open_virtual_port (const std::string & portname = "rtmidi output")
    {
       get_api()->open_virtual_port(portname);
    }

    /**
     *  Return the number of available MIDI output ports.
     */

    unsigned get_port_count ()
    {
       return get_api()->get_port_count();
    }

    /**
     *  Return a string identifier for the specified MIDI port type and number.
     *  An empty string is returned if an invalid port specifier is provided.
     */

    std::string get_port_name (unsigned portnumber)
    {
       return get_api()->get_port_name(portnumber);
    }

    /**
     *  Immediately send a single message out an open MIDI output port.
     *  An exception is thrown if an error occurs during output or an
     *  output connection was not previously established.
     *
     *  TEMPORARILY VIRTUAL:
     */

    virtual void send_message (const std::vector<midibyte> & message)
    {
       dynamic_cast<midi_out_api *>(get_api())->send_message(message);
    }

    /**
     *  Set an error callback function to be invoked when an error has
     *  occurred.  The callback function will be called whenever an error has
     *  occured. It is best to set the error callback function before opening
     *  a port.
     */

    virtual void seterrorcallback
    (
        rterror_callback errorcallback = nullptr,
        void * userdata = nullptr
    )
    {
       get_api()->seterrorcallback(errorcallback, userdata);
    }

protected:

  void openmidi_api (rtmidi_api api, const std::string & clientname);

};

}           // namespace seq64

#endif      // SEQ64_RTMIDI_HPP

/*
 * rtmidi.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
