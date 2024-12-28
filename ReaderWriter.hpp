/*
 * ReaderWriter.hpp
 *
 *  Created on: Dec 23, 2024
 *      Author: os
 */

#ifndef READERWRITER_HPP_
#define READERWRITER_HPP_
#include <pthread.h>
using namespace std;

#define SUCCEDED_RETURN_VAL 1;
class ReaderWriter {
public:
 	pthread_mutex_t lock_reader;
	pthread_mutex_t lock_writer;
	int readers;
	ReaderWriter(pthread_mutex_t Lock_reader, pthread_mutex_t Lock_writer);
	virtual ~ReaderWriter();
	int reader_locker();
	int reader_unlocker();
	int writer_locker();
	int writer_unlocker();
};

#endif /* READERWRITER_HPP_ */
