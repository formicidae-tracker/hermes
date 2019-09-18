
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


// Errors that are returned by any <fh_frame_readout_t>
typedef enum fh_readout_error {
	// Success
	NO_ERROR = 0,
	// A low-level tracking process overflow occured.
	PROCESS_OVERFLOW = 1,
	// One of the distributed tracking process did not finished this
	// frame in time.
	PROCESS_TIMEOUT = 2,
	// issue with the illumination system prevented the tag tracking.
	ILLUMINATION_ERROR = 3,
} fh_readout_error_e;


// Error types that are returned by libfort-hermes functions.
typedef enum fh_error_code {
	// Success
	FH_NO_ERROR = 0,
	// The selected traking stream did not gave any header.
	FH_STREAM_NO_HEADER = 10001,
	// The stream is in an incompatible version
	FH_STREAM_WRONG_VERSION = 10002,
	// The end of the stream has been reached
	FH_END_OF_STREAM = 10003,
	// The next message in the stream could not be decoded.
	FH_MESSAGE_DECODE_ERROR = 10004,
	// Could not connect to the network stream
	FH_COULD_NOT_CONNECT = 10005,
	// Network error
	FH_SOCKET_ERROR = 10006,
} fh_error_code_e;

/* Error reporting object.
 *
 * <fh_error_t> represent any error / exceptional condition that could
 * happens when using libfort-hermes functions. It contains an
 * internal <fh_error_code_e> and a describing string.
 *
 * <fh_error_t> are passed as initialized arguments to any function that
 * reports error. They should be initialized by the user using
 * <fh_error_create> and cleaned up using <fh_error_destroy>.
 *
 * The same error structure can be used repeatidly accros multiple
 * call to the library function.
 */
typedef struct fh_error {
	// Textual description of the error.
	char * what;
	// Type of the error
	fh_error_code_e code;
} fh_error_t;

// Creates a new fh_error_t structure for reporting
//
// @return a newly allocated <fh_error_t>
fh_error_t * fh_error_create();
// Frees an fh_error_t
// @err the <fh_error_t> to free.
void fh_error_destroy(fh_error_t * err);

/* Represents a tag frame readout.
 *
 * Frame readout are opaque objects. To read frame the user should
 * initialize one using <fh_frame_readout_create>, and read over an
 * opened <fh_context_t> using <fh_context_t::fh_context_read>. It is
 * a good practice to reuse for each read the same
 * <fh_frame_readout_t>, and **not** to create a new one for each
 * <fh_context_t::fh_context_read> calls.
 *
 * To clean up allocated ressources the user would need to use the
 * <fh_frame_readout_destroy> function.
 */
typedef void fh_frame_readout_t;

/* Represents a detected tag
 *
 * Its an opaque type. Its life cycle is from this interface always
 * tied up to a parent <fh_frame_readout_t>. Hence only accessor
 * functions are provided by this API.
 */
typedef void fh_tag_t;


/* Creates a new <fh_frame_readout_t> to be used with <fh_context_t::fh_context_read>.
 *
 * @return a newly allocated <fh_frame_readout_t>
 */
fh_frame_readout_t * fh_frame_readout_create();
// Frame timetamp.
// @re the <fh_frame_readout_t>
//
// Its an internal timestamp in microsecond of the frame, generated
// by the camera/framegrabber. It should be used to measure precise
// timing between frames The <fh_frame_readout_time> is an actual
// date that could be used to find a frame in time.
//
// @return the frame timestamp in microsecond.
uint64_t fh_frame_readout_timestamp(fh_frame_readout_t * re);
// Frame UID
// @re the <fh_frame_readout_t>
//
// @return the unique frame ID thte readout refers to.
uint64_t fh_frame_readout_frame_id(fh_frame_readout_t * re);
// Numbers of tags detected in the frame.
// @re the <fh_frame_readout_t>
//
// @return the number of tag detected in the frame
size_t fh_frame_readout_tag_size(fh_frame_readout_t * re);
// A <fh_tag_t> accessor.
// @re the <fh_frame_readout_t>
// @i the index of the tag that should be < to <fh_frame_readout_tag_size>.
//
// @return a <fh_tag_t> at the given index
fh_tag_t * fh_frame_readout_tag(fh_frame_readout_t * re, size_t i);
// Original frame emission date.
// @re the <fh_frame_readout_t>
//
// Due to realtime issue implied by jitter and clock synchronization,
// this field could present jitter and irregularities (in some extreme
// cases, date will not even be monotonous in time). It should only be
// used to find a particular event in time. To measure time between
// frame, **use the monotonous <fh_frame_readout_timestamp>
// function**.
//
// @return a struct timeval at which the frame was originally received
// by the node who processed the tracking.
void fh_frame_readout_time(fh_frame_readout_t * re,struct timeval * res);
// Tracking processing errors
// @re the <fh_frame_readout_t>
//
// @return a <fh_readout_error_e> representing the tracking process error.
fh_readout_error_e fh_frame_readout_error(fh_frame_readout_t * re);
// Frame width
// @re the <fh_frame_readout_t>
//
// @return the original frame width
int32_t fh_frame_readout_width(fh_frame_readout_t * re);
// Frame height
// @re the <fh_frame_readout_t>
//
// @return the original frame height
int32_t fh_frame_readout_height(fh_frame_readout_t * re);
// Frees all resources associated with a <fh_frame_readout_t>
// @re the <fh_frame_readout_t> to free
void fh_frame_readout_destroy(fh_frame_readout_t * re);


// ID of the detected tag.
// @ a the <fh_tag_t>
//
// @return the Unique tag ID.
uint32_t fh_tag_id(fh_tag_t * a);
// X position in image space.
// @ a the <fh_tag_t>
//
// @return the x position in image space.
double fh_tag_x(fh_tag_t * a);
// Y position in image space.
// @ a the <fh_tag_t>
//
// @return the y position in image space.
double fh_tag_y(fh_tag_t * a);
// Orientation in image space.
// @ a the <fh_tag_t>
//
// As convetionally in image processing the Y axis is oriented top to
// bottom, this angle is positive clockwise, as poositive angle are
// defined positive when going from the X to Y axis.
//
// @return the orientation of the tag.
double fh_tag_theta(fh_tag_t * a);


/* A context is used to read <fh_frame_readout_t>
 *
 * It can represents an offline list of tracking datafile opened using
 * <fh_open_file> or a connection to an online broadcast of tracking
 * data opened using <fh_connect>.
 */
typedef void fh_context_t;

/* Opens a list of tracking data file.
 * @filename filepath to the file.
 * @err <fh_error_t> used for error reporting
 *
 * @return A new opened <fh_context_t> upon success, otherwise NULL
 *         and an appropriate error in <err>.
 */
fh_context_t * fh_open_file(const char * filename, fh_error_t * err);

/* Connects to a remote for live tracking data processing.
 * @host the host to connect to
 * @port the port to connect to
 * @nonblocking if true, operates as a nonblocking socket.
 * @err <fh_error_t> used for error reporting
 *
 * If nonblocking is set to true, any call to fh_context_read will
 * return immediatly even if no data is available. The error return
 * value should be used to distinguish the End Of Stream case from the
 * no data currently available case.
 *
 * @return A new opened <fh_context_t> upon success, otherwise NULL
 * with an appropriate error in <err>.
 */
fh_context_t * fh_connect(const char * host,int port , bool nonblocking, fh_error_t * err);


/* Frees any ressources associated with an opened context.
 * @ctx the <fh_context_t> to free
 */
void fh_context_destroy(fh_context_t * ctx);

/* Reads a new fh_frame_readout_t from a fh_context_t.
 * @ctx the fh_context_t to read from
 * @ro the fh_frame_readout_t to read to
 * @err fh_error_t used for error reporting including FH_END_OF_STREAM cases.
 *
 * It returns true if a new fh_frame_readout_t has been successfully
 * received and parsed. In the special case of a non blocking netwrok
 * context, the <err> field should be used to distinguish case where
 * no data is currently available (return value is false and err is
 * FH_NO_ERROR) and end of stream cse (return value is false and err
 * is FH_END_OF_STREAM).
 *
 * @return true if a new fh_frame_readout_t has been successfully read.
 *
 */
bool fh_context_read(fh_context_t * ctx,fh_frame_readout_t * ro, fh_error_t * err);

#ifdef __cplusplus
}  // extern "C"
#endif //__cplusplus
