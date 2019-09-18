#<cldoc:index>

libfort-hermes C API

# libfort-hermes C API

The main entry point of this library is using a <fh_context_t> to open
either a file (<fh_open_file>) or connect remotely to a live tracking
application (<fh_connect>). Then <fh_frame_readout_t> can be obtained
through the <fh_context_t::fh_context_read> function.
