/*
 * ReaderWriter.cpp
 *
 *  Created on: Dec 23, 2024
 *      Author: os
 */

#include "ReaderWriter.hpp"

ReaderWriter::ReaderWriter(pthread_mutex_t Lock_reader, pthread_mutex_t Lock_writer) {
	lock_reader = Lock_reader;
	lock_writer = Lock_writer;
	readers = 0;
}

ReaderWriter::~ReaderWriter() {
	// TODO Auto-generated destructor stub
}

int ReaderWriter::reader_locker() {
	pthread_mutex_lock(&lock_reader);
	readers++;
	if (readers == 1)
	{
		pthread_mutex_lock(&lock_writer);
	}
	pthread_mutex_unlock(&lock_reader);
	return SUCCEDED_RETURN_VAL;
}

int ReaderWriter::reader_unlocker()
{
	pthread_mutex_lock(&lock_reader);
	readers--;
	if (readers == 0)
	{
		pthread_mutex_unlock(&lock_writer);
	}
	pthread_mutex_unlock(&lock_reader);
	return SUCCEDED_RETURN_VAL;

}
int ReaderWriter::writer_locker()
{
	pthread_mutex_lock(&lock_writer);
	return SUCCEDED_RETURN_VAL;
}
int ReaderWriter::writer_unlocker()
{
	pthread_mutex_unlock(&lock_writer);
	return SUCCEDED_RETURN_VAL;
}



