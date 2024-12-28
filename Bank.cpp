/*
 * Bank.cpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#include "Bank.hpp"

Bank::Bank(pthread_mutex_t& Lock_bank_list_reader,
           pthread_mutex_t& Lock_bank_list_writer,
           string Path_to_logger,
           pthread_mutex_t& mutex_lock_write,
           pthread_mutex_t& mutex_lock_read)
: logger(Path_to_logger, mutex_lock_write, mutex_lock_read), // Pass by reference,
reader_writer_bank_list(Lock_bank_list_reader, Lock_bank_list_writer) // Pass by reference
{
}

Bank::~Bank() {
	for (const auto & account: bank_accounts)
	{
		delete account.second;
	}
}
void Bank::insert_new_account(int Account,int Amount,int Password,int Atm_id)
	{
		// This is writing to bank_account. need to addres this as reader - writer problem.
		accounts* new_account = new accounts(Account,Amount,Password);
		reader_writer_bank_list.writer_locker();
		std::string str_to_logger;
		if(bank_accounts.find(Account)!= bank_accounts.end())
		{
			reader_writer_bank_list.writer_unlocker();
					str_to_logger = "Error " + to_string(Atm_id) + ": Your transaction failed – account with the same id exists";
					logger.WriteToLogger(str_to_logger);
					delete new_account;

		}
		else
		{
			bank_accounts[Account] = new_account;
						reader_writer_bank_list.writer_unlocker();
						str_to_logger = std::to_string(Atm_id) + ": New account id is " + std::to_string(Account) + " with password " + to_string(Password) + " and initial balance " + to_string(Amount);
						logger.WriteToLogger(str_to_logger);
		}
	}
void Bank::close_existing_account(int Account,int Password,int Atm_id)
{
	string str_to_write;
	reader_writer_bank_list.writer_locker();
	map<int,accounts*>::iterator it = bank_accounts.find(Account);
	if(it != bank_accounts.end())
	{
		accounts* account_to_erase = bank_accounts[Account];
		if(account_to_erase->validate_password(Password))
		{
			bank_accounts.erase(Account);
			reader_writer_bank_list.writer_unlocker();
			str_to_write = to_string(Atm_id) + ": Account "+to_string(Account)+" is now closed. Balance was "+to_string(account_to_erase->amount);
			logger.WriteToLogger(str_to_write);
			delete account_to_erase;
		}
		else{
			reader_writer_bank_list.writer_unlocker();
			str_to_write = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " +to_string(Account) + " is incorrect";
			logger.WriteToLogger(str_to_write);
		}

	}
	else
	{
		reader_writer_bank_list.writer_unlocker();
		str_to_write = "No account =" + to_string(Account) + " exists";
		logger.WriteToLogger(str_to_write);
	}

}
void Bank::EraseLoggerContent()
{
	logger.EraseLoggerContent();
}
