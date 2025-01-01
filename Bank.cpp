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
void Bank::print_bank_status()
{
	printf("\033[2J");     // Clear the screen
	printf("\033[1;1H");   // Move the cursor to row 1, column 1
	reader_writer_bank_list.reader_locker();
	for(const auto& pair: bank_accounts)
	{
		accounts* account = pair.second;
		account->reader_writer_user_account.reader_locker();
	}
	for(const auto& pair: bank_accounts)
	{
		accounts* account = pair.second;
		string str_to_cout = "‫‪Account‬‬ ‫‪" + to_string(account->account_id) + ":‬‬ ‫‪Balance‬‬ ‫‪-‬‬ " + to_string(account->amount) + " ‫‪$,‬‬ ‫‪Account‬‬ ‫‪Password‬‬ ‫‪-‬‬ " + to_string(account->password);
		cout << str_to_cout << endl;
	}
	insert_status_to_remember();
	for(const auto& pair: bank_accounts)
	{
		accounts* account = pair.second;
		account->reader_writer_user_account.reader_unlocker();
	}
	reader_writer_bank_list.reader_unlocker();
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


// Insert status into the deque with memory management
void Bank::insert_status_to_remember()
{
    // Create a new map with fresh allocations for accounts*
    map<int, account_no_locks*> new_allocated_status;

    for (const auto& pair : bank_accounts) {
        int account_id = pair.first;
        accounts* old_account = pair.second;

        // Allocate a new account with copied data
        account_no_locks* new_account = new account_no_locks(
            old_account->account_id, // Copy ID
            old_account->amount,    // Copy balance
            old_account->password   // Copy password
        );

        // Insert the new account into the map
        new_allocated_status[account_id] = new_account;
    }

    // Push the newly allocated status into the deque
    status_to_remeber.push_front(new_allocated_status);

    // Check if deque size exceeds 120
    if (status_to_remeber.size() > 120) {
        // Get the oldest status (back of the deque)
        map<int, account_no_locks*> old_status = status_to_remeber.back();

        // De lete all dynamically allocated accounts in the old status
        for (auto& pair : old_status) {
            delete pair.second; // Free memory for each account
        }

        // Remove the oldest status from the deque
        status_to_remeber.pop_back();
    }
}
void Bank::restore_status_from_remember(int ind)
{
    // Bounds check for valid index
    if (ind < 0 || ind >= static_cast<int>(status_to_remeber.size())) {
        cerr << "Index out of bounds!" << endl;
        return;
    }

    // Lock the bank accounts for safe modification
    reader_writer_bank_list.writer_locker();

    // Get the target status from the deque at position [ind]
    map<int, account_no_locks*> target_status = status_to_remeber[ind];

    // Step 1: Process existing accounts in bank_accounts
    for (auto it = bank_accounts.begin(); it != bank_accounts.end(); ) {
        int account_id = it->first;
        accounts* account = it->second;
        account->reader_writer_user_account.writer_locker();
        // Check if the account exists in the target status
        if (target_status.find(account_id) != target_status.end()) {

            // Account exists in target, update its data
            account_no_locks* target_account = target_status[account_id];
            account->amount = target_account->amount;         // Update balance
            account->reader_writer_user_account.writer_unlocker();
            // Password and ID are constant, no need to update
            ++it; // Move to the next element
        } else {
            // Account does not exist in target, delete it


            it = bank_accounts.erase(it); // Remove from the map
            account->reader_writer_user_account.writer_unlocker();
            delete account;               // Free memory
        }
    }

    // Step 2: Add new accounts from target_status that are not in bank_accounts
    for (const auto& target_pair : target_status) {
        int target_id = target_pair.first;

        // If the account does not exist in bank_accounts, create a new one
        if (bank_accounts.find(target_id) == bank_accounts.end()) {
            account_no_locks* target_account = target_pair.second;

            // Create a new accounts object using target_account data
            accounts* new_account = new accounts(
                target_account->account_id,   // ID
                target_account->amount,      // Balance
                target_account->password     // Password
            );

            // Insert the new account into bank_accounts
            bank_accounts[target_id] = new_account;
        }
    }

    // Unlock the bank accounts after modification
    reader_writer_bank_list.writer_unlocker();
}
