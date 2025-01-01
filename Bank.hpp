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
#include <deque>

using namespace std;
enum UserExistanceInBank
{
	NotExist,
	WrongPassword,
	Valid,
};

class Bank {
	private:
	Logger logger;
public:
	map<int,accounts*> bank_accounts;
	ReaderWriter reader_writer_bank_list;
	ReaderWriter reader_writer_atm_active_list;
	int numInitAtms;
	bool* isAtmActive;
	deque<map<int,account_no_locks*>> status_to_remeber;
	Bank(pthread_mutex_t& Lock_bank_list_reader,
	           pthread_mutex_t& Lock_bank_list_writer,
	           string Path_to_logger,
	           pthread_mutex_t& mutex_lock_logger_write,
	           pthread_mutex_t& mutex_lock_logger_read,
			   pthread_mutex_t& mutex_lock_atms_active_write,
			   pthread_mutex_t& mutex_lock_atms_active_read,
			   int num_init_atms);
	virtual ~Bank();
	void set_num_atms(int num_atms);
	void insert_new_account(int Account,int Amount, int Password,int Atm_id);
	void deposit(int Account,int Amount, int Password,int Atm_id);
	void withdraw(int Account,int Amount, int Password,int Atm_id);
	void close_existing_account(int Account,int Password,int Atm_id);
	void get_balance(int Account,int Password,int Atm_id);
	void transfer_money_between_accounts(int src_id_account,int src_password,int target_id_account,int amount,int Atm_id);
	void close_atm(int target_atm_id,int source_atm_id);
	void print_bank_status();
	void EraseLoggerContent();
	void insert_status_to_remember();
	void restore_status_from_remember(int ind);
private:
	bool IsExistingUser(int account);
	UserExistanceInBank IsPasswordCorrect(int account,int Password);
};

#endif /* BANK_HPP_ */
