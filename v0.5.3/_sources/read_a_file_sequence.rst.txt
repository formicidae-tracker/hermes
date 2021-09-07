.. read_a_file_sequence

Reading a file sequence
=======================

FORT's **artemis**/**leto** writes tracking data in sequence of files,
where each files ususally contains 2 hours worth of tracking
data. Theses files are usually named
``tracking.0000.hermes``, ``tracking.0001.hermes`` and so on..

**fort-hermes** provides utilities to read such a sequence of
files through :cpp:class:`fort::hermes::FileContext` or
:py:class:`py_fort_hermes.file.Context`


C++
---

**fort-hermes** provides the class
:cpp:class:`fort::hermes::FileContext` to access a sequence of
tracking file. By default it will read the first provided segment and
continue until the last one is fully successfully read and throw a
:cpp:class:`fort::hermes::EndOfFile` exception. If an error occurs
before the last segment is succesfully read a
:cpp:class:`fort::hermes::UnexpectedEndOfFileSequence` exception is
thrown.

.. code-block:: c++

    #include <fort/hermes/FileContext.hpp>

    FileContext context("tracking.0000.hermes")

    // to avoid ununcessary memory allocation, the same protobuf
    // message is re-used for each iteration
    fort::hermes::FrameReadout ro;

    while (true) { // enter an unknown loop
        try {
            // read a FrameReadout from the context
            context.Read(&ro);
            //do something with the readout
        } catch(const fort::hermes::EndOfFile &) {
            //EndOfFile is thrown when no data is available
            break;
        } catch (const fort::hermes::UnexpectedEndOfFileSequence & e) {
            // if an error occurs before the end of file sequence, we
            // can access the corrupted segment file path
            std::cerr << "Got an error in " << e.SegmentFilePath() << std::endl;
            break;
        }
    }

Python
------

The function :py:func:`py_fort_hermes.file.open` opens a
:py:class:`py_fort_hermes.file.Context` which is iterable in a **for**
loop. Again if the file sequence encounters an errors before the last
segment is fully read,a
:py:class:`py_fort_hermes.UnexpectedEndOfFileSequence` is raised.

.. code-block:: python

    import py_fort_hermes as fh

    with fh.file.open('tracking.0000.hermes') as s:
        try:
            for ro in s:
                # do something with readout
        except fh.UnexpectedEndOfFileSequence as e:
            print("%s is corrupted" % e.segmentPath)
