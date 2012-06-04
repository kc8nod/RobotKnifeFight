======
pyTUIO
======

A Python library that understands the TUIO protocol.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This library is able to receive and parse data following the `TUIO protocol`_,
which was specially designed for transmitting the state of tangible objects and multi-touch control on a table surface.

Installation
============

In order to use this library you need to have the cross-plattform  
reacTIVision_ application that does the heavy lifting of tracking the tangible 
objects, so-called fiducials_. It transmits the received data (e.g. positional 
information) as OSC_ messages via an UDP socket to your client software which
uses this library.

If you haven't downloaded a copy of django-registration already,
you'll need to do so. You can download a packaged version of the
latest release here::

    http://pytuio.googlecode.com/files/pytuio-0.1.tar.gz

Open up the package (on most operating systems you can double-click, or you
can use the command ``tar zxvf pytuio-0.1.tar.gz`` to manually unpack it), 
and, at a command line, navigate to the directory ``pytuio-0.1``, then type::

    python setup.py install

This will install pytuio into a directory on your Python import path. For 
system-wide installation on Linux/Unix and Mac OS, you can use ``sudo``::

    sudo python setup.py install

Alternatively, you can do a Subversion checkout to get the latest
development code (though this may also include bugs which have not yet
been fixed)::

    svn co http://pytuio.googlecode.com/svn/trunk/tuio/

For best results, do that in a directory that's on your Python import
path.

If you prefer you can also simply place the included ``tuio`` directory 
somewhere on your Python path, or symlink to it from somewhere on your Python 
path; this is useful if you're working from a Subversion checkout.

If you plan to use this library together with Nodebox.app please copy the 
``tuio`` directory to ``~/Library/Applications Support/Nodebox/`` to enable
Nodebox to find it.

.. _TUIO protocol: http://modin.yuri.at/publications/tuio_gw2005.pdf
.. _reacTIVision: http://reactable.iua.upf.edu/?software
.. _fiducials: http://reactable.iua.upf.edu/pdfs/fiducials.pdf
.. _OSC: http://en.wikipedia.org/wiki/OpenSound_Control

Basic use
=========

To use this library in general you should follow these steps:

1. Get a camera or webcam, like iSight, Quickcam, etc., install its drivers if 
   necessary, try it with the reacTIVision_ software
2. Look in the ``examples`` directory to get started with Python code. Ask 
   your local Python guru if needed.
3. Build the tangible interface, table, stage, vehicle, game, whatever.
4. Combine it with Blender_, Pygame_ or Nodebox_
5. Use the source, Luke.

.. _Blender: http://blender.org 
.. _Pygame: http://pygame.org
.. _Nodebox: http://nodebox.net

What is in the library
======================

The library consists of several parts and submodules:

    * Tracking_
    * Objects_
    * Profiles_
    * OSC_

Tracking
--------

The ``Tracking`` class should be used to initialize a socket connection for
receiving the OSC messages from reacTIVision_. It handles all incoming data
and calls the appropriate functions, depending on the type of message.

When started it loads every possible profile from the ``profiles`` submodule
and initializes a callback manager from the ``OSC`` module.

A simple example can be found in the ``examples`` directory in 
``example1.py``:

1. Import it::

    import tuio

2. Initializes the receiving of tracking data::

    tracking = tuio.Tracking()

3. Print all TUIO profiles that have been found and loaded::

    print "loaded profiles:", tracking.profiles.keys()

4. Print available helper functions, that can be used to access the objects of
   each loaded profile::
   
    print "list functions to access tracked objects:", tracking.get_helpers()

5. Prepare to receive the data in an infinite or event loop::

    try:
        while 1:
            tracking.update()
            for obj in tracking.objects():
                print obj
    except KeyboardInterrupt:
        tracking.stop()

  a) You need to update the tracking information on each loop manually.

  b) Access the tracked objects by using one of the helper function that 
     return a list of these objects.

  c) Stop the tracking manually on every exception to prevent socket 
     errors

Objects
-------

The ``objects`` submodule contains a series of classes that represent types
of tangible objects. They all are subclasses of the also included
``objects.TuioObject``. The following object types are defined at the moment:

1. ``Tuio2DCursor`` - An abstract cursor object, e.g. a finger.
   This object has limited information and is only sent by reacTIVision if the
   smallest possible fiducial marker was found: a point. In combination with a
   tangible table this can also be achieved by using fingers on the table
   surface.

   It has the following attributes:
   
   - ``sessionid`` - The unique sessionid it belongs to
   - ``xpos`` - The relative position on the x-axis
   - ``ypos`` - The relative position on the y-axis
   - ``xmot`` - The movement vector on the x-axis
   - ``ymot`` - The movement vector on the y-axis
   - ``mot_accel`` - The motion acceleration

2. ``Tuio2DObject`` - An abstract object representing a fiducial.
   This object has detailed information about its state and is sent by
   reacTIVision if a fiducial was recognized.
   
   It has the following attributes:
   
   - ``sessionid`` - The unique sessionid it belongs to
   - ``xpos`` - The relative position on the x-axis
   - ``ypos`` - The relative position on the y-axis
   - ``angle`` - The current angle in degrees
   - ``xmot`` - The movement vector on the x-axis
   - ``ymot`` - The movement vector on the y-axis
   - ``rot_vector`` - The rotation vector
   - ``mot_accel`` - The motion acceleration
   - ``rot_accel`` - The rotation acceleration

The TUIO protocol provides even more possible object types, depending on the
purpose of the intactive surface, e.g.:

   - 2.5D Interactive Surface - ``Tuio25DCursor`` and ``Tuio25DObject``
   - 3D Interactive Surface - ``Tuio3DCursor`` and ``Tuio3DObject``
   - raw profile - at the moment only ``dtouch`` specs are supported

But these profiles are left to be implemented by the user. Just have a look
in ``objects.py`` and ``profiles.py`` and subclass the base classes there.

Profiles
--------

The ``profiles`` submodule contains a number of abstract descriptions of what
should happen if a certain object type is used. Depending on the desirable
tangible object attributes you can customize the profiles for your own need.

For example, if you want to receive the data for a 2D tracking object you need
to use the according profile, because it knows how to handle the dataset of
this type of object.

Every profile subclasses from a ``TuioProfile`` base class that has the
following required methods whose names originate from the name of the raw
OSC message:

   - ``set`` - The state of each alive (but unchanged) fiducial is periodically
     resent with 'set' messages. The attributes are sent as a list or tuple.

   - ``alive`` - The 'alive' message contains the session ids of all alive
     fiducials known to reacTIVision.

   - ``fseq`` - fseq messages associate a unique frame id with a set of ``set``
     and ``alive`` messages

Other methods and attributes are:

   - ``list_label`` - Defines the names of the helper methods that are
     automatically created while initialization of the ``Tracking`` instance
     and maps to the ``objs`` method of the used profile.

   - ``address`` - Defines the OSC address to bind to the ``CallBackmanager``
     and start listening to while starting the ``Tracking`` instance.

   - ``objs`` - Returns a generator list of all tracked objects which are
     recognized with this profile and are in the current session. Though
     please use the helper methods whose names are defined in the class
     variable ``list_label``.

OSC
---

This submodule does most of the heavy lifting of decoding the OSC messages
used in the TUIO protocol and provides a convenient ``CallbackManager``.
It was written by Daniel Holth and Clinton McChesney.

What happens next?
==================

This library should be the start of lecturing about tangible interfaces in
combination with the ease of use of the Python programming language.

Feel free to contact the Author Jannis Leidel <jannis@leidel.info> to get to
know more about tangible user interfaces, integration into Pygame and future
features.

You can of course use the issue tracking service of its Google Code project::

    http://code.google.com/p/pytuio/issues/list
    
to ask for new features, report bugs or become a project member.
