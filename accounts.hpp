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
enum DepositStatus{
	Successfull,
	Fail_not_enogh_money,
};
class accounts {
public:
	ReaderWriter reader_writer_user_account;
	const int password;
	int account_id;
	int amount;


	pthread_mutex_t lock_reader;
	pthread_mutex_t lock_writer;
	accounts(int Account_id,int Amount,int Password);
	bool validate_password(int password_entered);
	DepositStatus deposit(int money_to_deposit);
	virtual ~accounts();
};

#endif /* ACCOUNTS_HPP_ */
