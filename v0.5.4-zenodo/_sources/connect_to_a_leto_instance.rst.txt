.. connect_to_a_leto_instance

Connect to a **leto** instance
==============================

**fort-hermes** provides utility to connect to a live **leto**
 instance. Usually those broadcast live tracking data on TCP
 port 4002. Those broadcast can be accessed with
 :cpp:class:`fort::hermes::FileContext` or
 :py:func:`py_fort_hermes.network.connect`

C++
---


.. code-block:: c++

    #include <fort/hermes/NetworkContext.hpp>

    NetworkContext context("host",4002,true);

    // Like for FileContext, we re-use the same FrameReadout through the loop;
    FrameReadout ro;
    while (true) {
        try {
            context.Read(&ro);
        } catch ( const fort::Hermes::WouldBlock &) {
            //No message ready yet
        }
    }

Python
------


.. code-block:: python

    import py_fort_hermes as fh

    with fh.network.connect("host") as c:
         try:
            ro = next(c)
         except StopIteration:
            break
