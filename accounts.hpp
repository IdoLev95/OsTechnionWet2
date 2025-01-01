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
class account_no_locks{
public:
	const int password;
	int account_id;
	int amount;
	account_no_locks(int Account_id,int Amount,int Password):password(Password)
	{
		account_id = Account_id;
		amount = Amount;
	}
};
#endif /* ACCOUNTS_HPP_ */
