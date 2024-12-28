/*
 * Bank.hpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#ifndef BANK_HPP_
#define BANK_HPP_

#include "accounts.hpp"
#include "ReaderWriter.hpp"
#include "Logger.hpp"
#include <map>
#include <iostream>
#include <string> // Required for std::string and std::to_string
#include <pthread.h>

using namespace std;

class Bank {
	private:
	Logger logger;
public:
	map<int,accounts*> bank_accounts;
	ReaderWriter reader_writer_bank_list;
	Bank(pthread_mutex_t& Lock_bank_list_reader,
	           pthread_mutex_t& Lock_bank_list_writer,
	           string Path_to_logger,
	           pthread_mutex_t& mutex_lock_logger_write,
	           pthread_mutex_t& mutex_lock_logger_read);
	virtual ~Bank();
	void insert_new_account(int Account,int Amount, int Password,int Atm_id);
	void close_existing_account(int Account,int Password,int Atm_id);
	void EraseLoggerContent();

};

#endif /* BANK_HPP_ */
