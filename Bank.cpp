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
           pthread_mutex_t& mutex_lock_read,
		   pthread_mutex_t& mutex_lock_atms_active_write,
		   pthread_mutex_t& mutex_lock_atms_active_read,
		   int num_init_atms)
: logger(Path_to_logger, mutex_lock_write, mutex_lock_read), // Pass by reference,
reader_writer_bank_list(Lock_bank_list_reader, Lock_bank_list_writer), // Pass by reference
reader_writer_atm_active_list(mutex_lock_atms_active_read,mutex_lock_atms_active_write)
{
	numInitAtms = num_init_atms;
	isAtmActive = NULL;
}

Bank::~Bank() {
	for (const auto & account: bank_accounts)
	{
		delete account.second;
	}
}
void Bank::set_num_atms(int num_atms)
{
	numInitAtms = num_atms;
	isAtmActive = new bool[num_atms];
	fill(isAtmActive, isAtmActive + num_atms, true);
}
void Bank::insert_new_account(int Account,int Amount,int Password,int Atm_id)
{
	std::string str_to_logger;
	reader_writer_bank_list.writer_locker();
	if(!IsExistingUser(Account)){


		accounts* new_account = new accounts(Account,Amount,Password);
		new_account->reader_writer_user_account.reader_locker();

		bank_accounts[Account] = new_account;
		reader_writer_bank_list.writer_unlocker();
		str_to_logger = std::to_string(Atm_id) + ": New account id is " + std::to_string(Account) + " with password " + to_string(Password) + " and initial balance " + to_string(Amount);
		new_account->reader_writer_user_account.reader_unlocker();
		logger.WriteToLogger(str_to_logger);
	}
	else{
		reader_writer_bank_list.writer_unlocker();
		str_to_logger = "Error " + to_string(Atm_id) + ": Your transaction failed – account with the same id exists";
		logger.WriteToLogger(str_to_logger);
	}
}
void Bank::close_existing_account(int Account,int Password,int Atm_id)
{

	string str_to_write;
	reader_writer_bank_list.writer_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(Account,Password);
	if(user_res == Valid)
	{
		accounts* account_to_erase = bank_accounts[Account];
		account_to_erase->reader_writer_user_account.writer_locker();

		bank_accounts.erase(Account);
		reader_writer_bank_list.writer_unlocker();
		str_to_write = to_string(Atm_id) + ": Account "+to_string(Account)+" is now closed. Balance was "+to_string(account_to_erase->amount);
		logger.WriteToLogger(str_to_write);
		account_to_erase->reader_writer_user_account.writer_unlocker();
		delete account_to_erase;

	}
	else if(user_res == WrongPassword || user_res == NotExist)
	{
		reader_writer_bank_list.writer_unlocker();
		str_to_write = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " +to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_write);

	}

}
void Bank::deposit(int Account,int Amount, int Password,int Atm_id)
{
	string str_to_log;
	reader_writer_bank_list.reader_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(Account,Password);
	if(user_res == Valid)
	{
		accounts* account_to_deposit = bank_accounts[Account];
		account_to_deposit->reader_writer_user_account.writer_locker();
		reader_writer_bank_list.reader_unlocker();
		account_to_deposit->deposit(Amount);
		str_to_log =to_string(Atm_id) + ": Account " + to_string(Account) +  " new balance is "+to_string(account_to_deposit->amount) + " after " + to_string(Amount) + " $ was deposited";
		account_to_deposit->reader_writer_user_account.writer_unlocker();
		logger.WriteToLogger(str_to_log);

	}
	else if(user_res == WrongPassword || user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " + to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::withdraw(int Account,int Amount, int Password,int Atm_id)
{
	string str_to_log;
	reader_writer_bank_list.reader_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(Account,Password);
	if(user_res == Valid)
	{
		accounts* account_to_deposit = bank_accounts[Account];
		account_to_deposit->reader_writer_user_account.writer_locker();
		reader_writer_bank_list.reader_unlocker();
		Amount = -Amount; //Since using withdraw, we take negative deposit
		DepositStatus depositStatus = account_to_deposit->deposit(Amount);
		Amount = -Amount;
		if(depositStatus == Successfull)
		{
			str_to_log =to_string(Atm_id) + ": Account " + to_string(Account) +  " new balance is "+to_string(account_to_deposit->amount) + " after " + to_string(Amount) + " $ was withdrawn";
			logger.WriteToLogger(str_to_log);
		}
		else{
			str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – account id " + to_string(Account) + " balance is lower than "+to_string(Amount);
			logger.WriteToLogger(str_to_log);
		}

		account_to_deposit->reader_writer_user_account.writer_unlocker();

	}
	else if(user_res == WrongPassword || user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " + to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::get_balance(int Account,int Password,int Atm_id)
{
	string str_to_log;
	reader_writer_bank_list.reader_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(Account,Password);
	if(user_res == Valid)
	{
		accounts* account_to_get_balance = bank_accounts[Account];
		account_to_get_balance->reader_writer_user_account.reader_locker();
		int balance = account_to_get_balance->amount;
		account_to_get_balance->reader_writer_user_account.reader_unlocker();
		reader_writer_bank_list.reader_unlocker();
		str_to_log = to_string(Atm_id) + ": Account "+ to_string(Account) + " balance is "+ to_string(balance);
		logger.WriteToLogger(str_to_log);
	}
	else
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id "+to_string(Account)+" is incorrec";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::transfer_money_between_accounts(int src_id_account,int src_password,int target_id_account,int amount,int Atm_id)
{
	string str_to_log;
	reader_writer_bank_list.reader_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(src_id_account,src_password);
	bool is_target_user_exist = IsExistingUser(target_id_account);
	if(is_target_user_exist && user_res == Valid)
	{
		accounts* src_account = bank_accounts[src_id_account];
		accounts* dst_account = bank_accounts[target_id_account];
		if(src_id_account < target_id_account)
		{
			src_account->reader_writer_user_account.writer_locker();
			dst_account->reader_writer_user_account.writer_locker();
		}
		else
		{
			dst_account->reader_writer_user_account.writer_locker();
			src_account->reader_writer_user_account.writer_locker();
		}
		reader_writer_bank_list.reader_unlocker();
		DepositStatus deposit_status = src_account->deposit(-amount);
		if(deposit_status == Successfull)
		{
			dst_account->deposit(amount);
			int src_balance = src_account->amount;
			int dst_balance = dst_account->amount;
			str_to_log = to_string(Atm_id) + ": Transfer "+to_string(amount) + " from account "+to_string(src_id_account) + " to account " + to_string(target_id_account) + " new account balance is " + to_string(src_balance) + " new target account balance is " + to_string(dst_balance);
			logger.WriteToLogger(str_to_log);
		}
		else{
			str_to_log = "Error "+to_string(Atm_id) + ": Your transaction failed – account id "+to_string(src_id_account) + " is balance is lower than "+to_string(amount);
			logger.WriteToLogger(str_to_log);
		}
		src_account->reader_writer_user_account.writer_unlocker();
		dst_account->reader_writer_user_account.writer_unlocker();
	}
	else{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error "+ to_string(Atm_id) + ": Your transaction failed – password for account id "+to_string(src_id_account) +" is incorrect";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::close_atm(int target_atm_id,int source_atm_id)
{
	string str_to_log;
	if(target_atm_id < numInitAtms)
	{
		reader_writer_atm_active_list.writer_locker();
		if(isAtmActive[target_atm_id])
		{
			isAtmActive[target_atm_id] = false;
			str_to_log = "‫‪Bank:‬‬ ‫‪ATM‬‬ ‫‪" + to_string(source_atm_id) + " ‫‪closed‬‬ ‫‪"+ to_string(target_atm_id) +" ‫‪successfully‬‬";
		}
		else
		{
			str_to_log = "‫‪Error‬‬ " + to_string(source_atm_id) + ":‬‬ ‫‪Your‬‬ ‫‪close‬‬ ‫‪operation‬‬ ‫‪failed‬‬ ‫–‬ ‫‪ATM‬‬ ‫‪ID‬‬ "+ to_string(target_atm_id) +" ‫‪is‬‬ ‫‪already‬‬ ‫‪in‬‬ ‫‪a‬‬ ‫‪closed‬‬ state";

		}
		logger.WriteToLogger(str_to_log);
		reader_writer_atm_active_list.writer_unlocker();
	}
	else{
		str_to_log = "Error " + to_string(source_atm_id) + ": Your transaction failed – ATM ID "+ to_string(target_atm_id) +" does not exist";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::EraseLoggerContent()
{
	logger.EraseLoggerContent();
}
bool Bank::IsExistingUser(int account)
{

	bool is_exist = bank_accounts.find(account)!= bank_accounts.end();

	return is_exist;
}
UserExistanceInBank Bank::IsPasswordCorrect(int account,int Password)
{
	UserExistanceInBank user;
	bool is_exist = bank_accounts.find(account)!= bank_accounts.end();
	if(!is_exist)
	{
		user = NotExist;

	}
	else
	{
		accounts* account_user= bank_accounts[account];
		if(account_user->validate_password(Password))
		{
			user = Valid;
		}
		else
		{
			user = WrongPassword;
		}
	}
	return user;
}

