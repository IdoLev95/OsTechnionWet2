/*
 * Logger.hpp
 *
 *  Created on: Dec 28, 2024
 *      Author: os
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_
#include "ReaderWriter.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <pthread.h>
class Logger {
private:
	ReaderWriter reader_writer_bank_list;
	string path_to_logger;
public:
	Logger(string Path_to_logger,pthread_mutex_t mutex_lock_write,pthread_mutex_t mutex_lock_read);

	virtual ~Logger();
	void WriteToLogger(string str_to_write);
};

#endif /* LOGGER_HPP_ */
