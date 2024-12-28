/*
 * acounts.hpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#ifndef ACCOUNTS_HPP_
#define ACCOUNTS_HPP_
#include <pthread.h>
#include <cstdio>
#include "ReaderWriter.hpp"

class accounts {
public:
	int account_id;
	int amount;
	int password;
	ReaderWriter reader_writer_user_account;
	pthread_mutex_t lock_reader;
	pthread_mutex_t lock_writer;
	accounts(int Account_id,int Amount,int Password);
	bool validate_password(int password_entered);
	virtual ~accounts();
};

#endif /* ACCOUNTS_HPP_ */
