#ifndef INFO_RECEIVER_H
#define INFO_RECEIVER_H

#include <boost/thread/thread_only.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include "common.h"
#include "forward.h"

namespace lsl {
	class inlet_connection;

	/// Internal class of an inlet that is responsible for retrieving the info of the inlet.
	/// The actual communication runs in an internal background thread, while the public function (info()) waits for the thread to finish.
	/// The public function has an optional timeout after which it gives up, while the background thread continues to do its job (so the next public-function call may succeed within the timeout).
	/// The background thread terminates only if the info_receiver is destroyed or the underlying connection is lost or shut down.
	class info_receiver {
	public:
		/// Construct a new info receiver for a given connection.
		info_receiver(inlet_connection &conn);

		/// Destructor. Stops the background activities.
		~info_receiver();

		/**
		* Retrieve the complete information of the given stream, including the extended description (.desc() field).
		* @param timeout Timeout of the operation (default: no timeout).
		* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
		*/
		const stream_info_impl &info(double timeout=FOREVER);

	private:
		/// The info reader thread.
		void info_thread();
		
		/// function polled by the condition variable
		bool info_ready();


		// the underlying connection
		inlet_connection &conn_;					// reference to our connection

		// background reader thread and the data generated by it
		lslboost::thread info_thread_;					// pulls the info in the background
		stream_info_impl_p fullinfo_;				// the full stream_info_impl object (retrieved by the info thread)
		lslboost::mutex fullinfo_mut_;					// mutex to protect the fullinfo
		lslboost::condition_variable fullinfo_upd_;	// condition variable to indicate that an update for the fullinfo is available
	};

}

#endif

