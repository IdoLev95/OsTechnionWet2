/*
 * ReaderWriter.cpp
 *
 *  Created on: Dec 23, 2024
 *      Author: os
 */

#include "ReaderWriter.hpp"
class MutexLockException : public std::runtime_error {
public:
    MutexLockException(const std::string& message) : std::runtime_error(message) {}
};

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

void print_stack_trace() {
    const int max_frames = 64;  // Maximum number of stack frames to capture
    void *frames[max_frames];
    int frame_count = backtrace(frames, max_frames);  // Capture the stack trace

    // Print the stack trace
    char **frame_strings = backtrace_symbols(frames, frame_count);
    if (frame_strings) {
        std::cerr << "Stack trace:\n";
        for (int i = 0; i < frame_count; ++i) {
            std::cerr << frame_strings[i] << "\n";
        }
        free(frame_strings);  // Free the memory allocated by backtrace_symbols
    }
}


ReaderWriter::ReaderWriter(pthread_mutex_t* Lock_reader, pthread_mutex_t* Lock_writer) {
	lock_reader = Lock_reader;
	lock_writer = Lock_writer;
	readers = 0;
/*	if (pthread_mutex_init(lock_reader, NULL) != 0) {
		throw std::runtime_error("Mutex initialization failed");
	}
	if (pthread_mutex_init(lock_writer, NULL) != 0) {
			throw std::runtime_error("Mutex initialization failed");
		}
*/
}
ReaderWriter::ReaderWriter()
{
	readers = 0;
	lock_reader=NULL;
	lock_writer = NULL;
}
void ReaderWriter::SetLocks(pthread_mutex_t* Lock_reader,pthread_mutex_t* Lock_writer)
{
/*	if (pthread_mutex_init(lock_reader, NULL) != 0) {
		throw std::runtime_error("Mutex initialization failed");
	}
	if (pthread_mutex_init(lock_writer, NULL) != 0) {
		throw std::runtime_error("Mutex initialization failed");
	}
*/
	lock_reader = Lock_reader;
	lock_writer = Lock_writer;
}
ReaderWriter::~ReaderWriter() {
	// TODO Auto-generated destructor stub
}

int ReaderWriter::reader_locker() {
	pthread_mutex_lock(lock_reader);
	readers++;
	if(readers == 0)
	{
		//print_stack_trace();
		//throw MutexLockException("Readers value in reader_locker is not valid");
	}
	if (readers == 1)
	{
		pthread_mutex_lock(lock_writer);
	}
	pthread_mutex_unlock(lock_reader);
	return SUCCEDED_RETURN_VAL;
}

int ReaderWriter::reader_unlocker()
{
	pthread_mutex_lock(lock_reader);
	readers--;
	if(readers < 0 )
	{
		//print_stack_trace();
		//throw MutexLockException("Readers value in reader_unlocker is not valid");
	}
	if (readers == 0)
	{
		pthread_mutex_unlock(lock_writer);
	}
	pthread_mutex_unlock(lock_reader);
	return SUCCEDED_RETURN_VAL;

}
int ReaderWriter::writer_locker()
{
	pthread_mutex_lock(lock_writer);
	return SUCCEDED_RETURN_VAL;
}
int ReaderWriter::writer_unlocker()
{
	pthread_mutex_unlock(lock_writer);
	return SUCCEDED_RETURN_VAL;
}



