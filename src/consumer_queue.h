#ifndef CONSUMER_QUEUE_H
#define CONSUMER_QUEUE_H

#include <boost/lockfree/spsc_queue.hpp>
#include "common.h"
#include "forward.h"

namespace lsl {
	/**
	* A thread-safe producer-consumer queue of unread samples.
	* Erases the oldest samples if max capacity is exceeded. Implemented as a circular buffer.
	*/
	class consumer_queue: private lslboost::noncopyable {
		using buffer_type = lslboost::lockfree::spsc_queue<sample_p>;

	public:
		/**
		* Create a new queue with a given capacity.
		* @param max_capacity The maximum number of samples that can be held by the queue. Beyond that, the oldest samples are dropped.
		* @param registry Optionally a pointer to a registration facility, for multiple-reader arrangements.
		*/
		consumer_queue(std::size_t max_capacity, send_buffer_p registry=send_buffer_p());

		/**
		* Destructor.
		* Unregisters from the send buffer, if any.
		*/
		~consumer_queue();

		/**
		* Push a new sample onto the queue.
		*/
		void push_sample(const sample_p &sample);

		/**
		* Pop a sample from the queue. 
		* Blocks if empty.
		* @param timeout Timeout for the blocking, in seconds. If expired, an empty sample is returned.
		*/
		sample_p pop_sample(double timeout=FOREVER);

		/**
		* Check whether the buffer is empty.
		*/ 
		bool empty();

	private:
		send_buffer_p registry_;				// optional consumer registry
		buffer_type buffer_;					// the sample buffer
	};

}

#endif
