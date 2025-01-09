/*
 * Bank.cpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#include "Bank.hpp"

Bank::Bank(pthread_mutex_t* Lock_bank_list_reader,
           pthread_mutex_t* Lock_bank_list_writer,
           string Path_to_logger,
           pthread_mutex_t* mutex_lock_write,
           pthread_mutex_t* mutex_lock_read,
		   pthread_mutex_t* mutex_lock_atms_active_write,
		   pthread_mutex_t* mutex_lock_atms_active_read,
		   pthread_mutex_t* mutex_lock_restore_req_list_write,
		   pthread_mutex_t* mutex_lock_restore_req_list_read,
		   int num_init_atms)
: logger(Path_to_logger, mutex_lock_write, mutex_lock_read), // Pass by reference,
reader_writer_bank_list(Lock_bank_list_reader, Lock_bank_list_writer), // Pass by reference
reader_writer_atm_active_list(mutex_lock_atms_active_read,mutex_lock_atms_active_write),
reader_writer_restore_req_list(mutex_lock_restore_req_list_read,mutex_lock_restore_req_list_write)
{
	numInitAtms = num_init_atms;
	isAtmActive = NULL;
	srand(static_cast<unsigned int>(time(0)));
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
void Bank::insert_new_account(int Account,int Amount,int Password,int Atm_id,bool PERSISTENT_flag)
{
	std::string str_to_logger;
	reader_writer_bank_list.writer_locker();
	if(!IsExistingUser(Account)){


		accounts* new_account = new accounts(Account,Amount,Password);
		new_account->reader_writer_user_account.reader_locker();

		bank_accounts[Account] = new_account;
		reader_writer_bank_list.writer_unlocker();
		str_to_logger = std::to_string(Atm_id) + ": New account id is " + std::to_string(Account) + " with password " + to_string(Password) + " and initial balance " + to_string(Amount);
		//cout <<"Here" << endl;
		logger.WriteToLogger(str_to_logger);

		//logger.reader_writer_logger.writer_unlocker();
		//cout << "Error?" << endl;
		new_account->reader_writer_user_account.reader_unlocker();

	}
	else{

		if(PERSISTENT_flag){
		reader_writer_bank_list.writer_unlocker();
		PERSISTENT_flag = false;
		sleep(1);
		insert_new_account(Account,Amount,Password,Atm_id,PERSISTENT_flag);
		return;
		}
		else{
			reader_writer_bank_list.writer_unlocker();
			str_to_logger = "Error " + to_string(Atm_id) + ": Your transaction failed – account with the same id exists";
			logger.WriteToLogger(str_to_logger);
		}
	}
}
void Bank::close_existing_account(int Account,int Password,int Atm_id,bool PERSISTENT_flag)
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
	else if(PERSISTENT_flag){
		reader_writer_bank_list.writer_unlocker();
		PERSISTENT_flag = false;
		sleep(1);
		close_existing_account(Account,Password,Atm_id,PERSISTENT_flag);
		return;
	}
	else if(user_res == WrongPassword)
	{
		reader_writer_bank_list.writer_unlocker();
		str_to_write = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " +to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_write);

	}
	else if(user_res == NotExist)
	{
		reader_writer_bank_list.writer_unlocker();
		PrintNotExistingUesr(Account,Atm_id);
	}

}
void Bank::deposit(int Account,int Amount, int Password,int Atm_id,bool PERSISTENT_flag)
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
		logger.WriteToLogger(str_to_log);
		account_to_deposit->reader_writer_user_account.writer_unlocker();
	}
	else if(PERSISTENT_flag){
		reader_writer_bank_list.reader_unlocker();
		PERSISTENT_flag = false;
		sleep(1);
		deposit(Account,Amount,Password,Atm_id,PERSISTENT_flag);
		return;
	}
	else if(user_res == WrongPassword)
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " + to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_log);
	}
	else if(user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		PrintNotExistingUesr(Account,Atm_id);
	}
}
void Bank::withdraw(int Account,int Amount, int Password,int Atm_id,bool PERSISTENT_flag)
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
			if(PERSISTENT_flag){
						account_to_deposit->reader_writer_user_account.writer_unlocker();
						PERSISTENT_flag = false;
						sleep(1);
						withdraw(Account,Amount,Password,Atm_id,PERSISTENT_flag);
						return;
					}
			else{
				str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – account id " + to_string(Account) + " balance is lower than "+to_string(Amount);
				logger.WriteToLogger(str_to_log);
			}
		}

		account_to_deposit->reader_writer_user_account.writer_unlocker();

	}
	else if(PERSISTENT_flag){
			reader_writer_bank_list.reader_unlocker();
			PERSISTENT_flag = false;
			sleep(1);
			withdraw(Account,Amount,Password,Atm_id,PERSISTENT_flag);
			return;
		}
	else if(user_res == WrongPassword)
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id " + to_string(Account) + " is incorrect";
		logger.WriteToLogger(str_to_log);
	}
	else if(user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		PrintNotExistingUesr(Account,Atm_id);
	}
}
void Bank::get_balance(int Account,int Password,int Atm_id,bool PERSISTENT_flag)
{
	string str_to_log;
	reader_writer_bank_list.reader_locker();
	UserExistanceInBank user_res = IsPasswordCorrect(Account,Password);
	if(user_res == Valid)
	{
		accounts* account_to_get_balance = bank_accounts[Account];
		account_to_get_balance->reader_writer_user_account.reader_locker();
		int balance = account_to_get_balance->amount;

		reader_writer_bank_list.reader_unlocker();
		str_to_log = to_string(Atm_id) + ": Account "+ to_string(Account) + " balance is "+ to_string(balance);
		logger.WriteToLogger(str_to_log);
		account_to_get_balance->reader_writer_user_account.reader_unlocker();
	}
	else if(PERSISTENT_flag){
		reader_writer_bank_list.reader_unlocker();
		PERSISTENT_flag = false;
		sleep(1);
		get_balance(Account,Password,Atm_id,PERSISTENT_flag);
		return;
	}
	else if(user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		PrintNotExistingUesr(Account,Atm_id);
	}
	else
	{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error " + to_string(Atm_id) + ": Your transaction failed – password for account id "+to_string(Account)+" is incorrect";
		logger.WriteToLogger(str_to_log);
	}
}
void Bank::transfer_money_between_accounts(int src_id_account,int src_password,int target_id_account,int amount,int Atm_id,bool PERSISTENT_flag)
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
		else if(target_id_account < src_id_account)
		{
			dst_account->reader_writer_user_account.writer_locker();
			src_account->reader_writer_user_account.writer_locker();
		}
		else
		{
			reader_writer_bank_list.reader_unlocker();
			return;
		}

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
			if(PERSISTENT_flag){
				src_account->reader_writer_user_account.writer_unlocker();
				dst_account->reader_writer_user_account.writer_unlocker();
				reader_writer_bank_list.reader_unlocker();
				PERSISTENT_flag = false;
				sleep(1);
				transfer_money_between_accounts(src_id_account,src_password,target_id_account,amount,Atm_id,PERSISTENT_flag);
				return;
			}
			else{
				str_to_log = "Error "+to_string(Atm_id) + ": Your transaction failed – account id "+to_string(src_id_account) + " is balance is lower than "+to_string(amount);
				logger.WriteToLogger(str_to_log);
			}
		}
		if(src_id_account < target_id_account)
		{
			src_account->reader_writer_user_account.writer_unlocker();
			dst_account->reader_writer_user_account.writer_unlocker();
		}
		else
		{
			dst_account->reader_writer_user_account.writer_unlocker();
			src_account->reader_writer_user_account.writer_unlocker();
		}
		reader_writer_bank_list.reader_unlocker();
	}
	if(PERSISTENT_flag){
		reader_writer_bank_list.reader_unlocker();
		PERSISTENT_flag = false;
		sleep(1);
		transfer_money_between_accounts(src_id_account,src_password,target_id_account,amount,Atm_id,PERSISTENT_flag);
		return;
	}
	else if(user_res == NotExist)
	{
		reader_writer_bank_list.reader_unlocker();
		PrintNotExistingUesr(src_id_account,Atm_id);
	}
	else if(!is_target_user_exist )
	{
		reader_writer_bank_list.reader_unlocker();
		PrintNotExistingUesr(target_id_account,Atm_id);
	}
	else{
		reader_writer_bank_list.reader_unlocker();
		str_to_log = "Error "+ to_string(Atm_id) + ": Your transaction failed – password for account id "+to_string(src_id_account) +" is incorrect";
		logger.WriteToLogger(str_to_log);
	}


}
void Bank::close_atm(int target_atm_id,int source_atm_id,bool PERSISTENT_flag,bool is_write_to_log)
{
	string str_to_log;
	if(target_atm_id < numInitAtms)
	{
		reader_writer_atm_active_list.writer_locker();
		if(isAtmActive[target_atm_id])
		{
			isAtmActive[target_atm_id] = false;
			str_to_log = "‫‪Bank:‬‬ ‫‪ATM‬‬ ‫‪" + to_string(source_atm_id) + " ‫‪closed‬‬ ‫‪"+ to_string(target_atm_id) +" ‫‪successfully‬‬";
			if(is_write_to_log)
			{
				logger.WriteToLogger(str_to_log);
			}
			reader_writer_atm_active_list.writer_unlocker();
		}
		else
		{
			if(PERSISTENT_flag)
			{
				reader_writer_atm_active_list.writer_unlocker();
				PERSISTENT_flag = false;
				sleep(1);
				close_atm(target_atm_id,source_atm_id,PERSISTENT_flag,is_write_to_log);
				return;
			}
			else{
					str_to_log = "‫‪Error‬‬ " + to_string(source_atm_id) + ":‬‬ ‫‪Your‬‬ ‫‪close‬‬ ‫‪operation‬‬ ‫‪failed‬‬ ‫–‬ ‫‪ATM‬‬ ‫‪ID‬‬ "+ to_string(target_atm_id) +" ‫‪is‬‬ ‫‪already‬‬ ‫‪in‬‬ ‫‪a‬‬ ‫‪closed‬‬ state";
					if(is_write_to_log)
					{
						logger.WriteToLogger(str_to_log);
					}
					reader_writer_atm_active_list.writer_unlocker();
			}
		}


	}
	else{
		if(PERSISTENT_flag)
		{
			//reader_writer_atm_active_list.writer_unlocker();
			PERSISTENT_flag = false;
			sleep(1);
			close_atm(target_atm_id,source_atm_id,PERSISTENT_flag,is_write_to_log);
			return;
		}
		else{
			str_to_log = "Error " + to_string(source_atm_id) + ": Your transaction failed – ATM ID "+ to_string(target_atm_id) +" does not exist";
			if(is_write_to_log)
			{
				logger.WriteToLogger(str_to_log);
			}
		}
	}
}
void Bank::print_bank_status()
{
    // Clear the screen and move cursor (optional, if you want this in your application)
    printf("\033[2J");     // Clear the screen
    printf("\033[1;1H");   // Move the cursor to row 1, column 1

    printf("‫‪Current‬‬ ‫‪Bank‬‬ ‫‪Status‬‬\n");
    //return;
    // Lock the bank list first
    reader_writer_bank_list.reader_locker();

    // Create a vector of pairs to sort the accounts by account ID (pair.first)
    vector<pair<int, accounts*>> sorted_accounts(bank_accounts.begin(), bank_accounts.end());
    sort(sorted_accounts.begin(), sorted_accounts.end(), [](const pair<int, accounts*>& a, const pair<int, accounts*>& b) {
        return a.first < b.first;  // Sort by account ID (pair.first)
    });

    // Lock each account in the sorted order
    for (const auto& pair : sorted_accounts)
    {
    	//cout << pair.first << endl;
        accounts* account = pair.second;
        account->reader_writer_user_account.reader_locker();
    }
    //return;

    // Print the status of each account in sorted order
    for (const auto& pair : sorted_accounts)
    {
        accounts* account = pair.second;
        string str_to_cout = "‫‪Account‬‬ ‫‪" + to_string(account->account_id) + ":‬‬ ‫‪Balance‬‬ ‫‪-‬‬ " + to_string(account->amount) + " ‫‪$,‬‬ ‫‪Account‬‬ ‫‪Password‬‬ ‫‪-‬‬ " + to_string(account->password);
        cout << str_to_cout << endl;
    }

    // Insert status to remember (assuming this function is implemented)
    insert_status_to_remember();

    // Unlock each account in reverse order of locking (to avoid deadlocks)
    for (const auto& pair : sorted_accounts)
    {
        accounts* account = pair.second;
        account->reader_writer_user_account.reader_unlocker();
    }

    // Unlock the bank list
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
void Bank::collect_texas_from_all()
{
	int commisionPercent =1+  (rand() % 5);
	reader_writer_bank_list.reader_locker();
	vector<pair<int, accounts*>> sorted_accounts(bank_accounts.begin(), bank_accounts.end());
		sort(sorted_accounts.begin(), sorted_accounts.end(), [](const pair<int, accounts*>& a, const pair<int, accounts*>& b) {
			return a.first < b.first;  // Sort by account ID (pair.first)
		});;
	for(auto& pair:sorted_accounts)
	{
		accounts* curr_account = pair.second;
		curr_account->reader_writer_user_account.writer_locker();
		int value_for_bank = (commisionPercent * curr_account->amount) / 100;

		// Cast result to int (truncating the decimal part)
		curr_account->amount -= value_for_bank;
		string str_to_logger = "‫‪Bank:‬‬ ‫‪commissions‬‬ ‫‪of‬‬ ‫‪" + to_string(commisionPercent) + " %‬‬ ‫‪were‬‬ ‫‪charged,‬‬ ‫‪bank‬‬ ‫‪gained‬‬ " +to_string(value_for_bank) + " ‫‪from‬‬ ‫‪account‬‬ ‫‪" +to_string(curr_account->account_id);
		logger.WriteToLogger(str_to_logger);
		curr_account->reader_writer_user_account.writer_unlocker();
	}
	reader_writer_bank_list.reader_unlocker();
}
void Bank::check_and_apply_restore()
{
	reader_writer_restore_req_list.writer_locker();
  if (!restore_indices.empty()) {
		// Find the maximum value in the list
	  	auto max_iter = std::max_element(restore_indices.begin(), restore_indices.end(), CompareRestoreRequest());
	  	int maxValueToRestore = max_iter->restore_ind;
		int correspondingAtmId = max_iter->atm_id;
		//int maxValueToRestore = *std::max_element(restore_indices.begin(), restore_indices.end());
		restore_indices.clear();
		reader_writer_restore_req_list.writer_unlocker();
		restore_status_from_remember(maxValueToRestore);
		// Clear the list after reading the max value
		string str_to_log = to_string(correspondingAtmId) + ":‬‬ ‫‪Rollback‬‬ ‫‪to " +to_string(maxValueToRestore)+ "‬‬ ‫‪bank‬‬ ‫‪iterations‬‬ ‫‪ago‬‬ ‫‪was‬‬ ‫‪completed‬‬ ‫‪successfully‬‬";
		logger.WriteToLogger(str_to_log);
	}
  else{
	  reader_writer_restore_req_list.writer_unlocker();
  }
}
void Bank::insert_restore_int_to_req_list(int restore_ind,int atm_id)
{
	reader_writer_restore_req_list.writer_locker();
	restore_indices.push_back(RestoreRequest(restore_ind, atm_id));
	reader_writer_restore_req_list.writer_unlocker();
}
void Bank::restore_status_from_remember(int ind)
{
    // Bounds check for valid index
    if (ind < 0 || ind >= static_cast<int>(status_to_remeber.size())) {
        //cerr << "Index out of bounds!" << endl;
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
    //"‫‪<ATM‬‬ ‫‪ID>:‬‬ ‫‪Rollback‬‬ ‫‪to‬‬ ‫>‪<iterations‬‬ ‫‪bank‬‬ ‫‪iterations‬‬ ‫‪ago‬‬ ‫‪was‬‬ ‫‪completed‬‬ ‫‪successfully‬‬"
    // Unlock the bank accounts after modification
    reader_writer_bank_list.writer_unlocker();
}

void Bank::PrintNotExistingUesr(int account,int atm_id)
{
	string str_to_log = "‫‪Error‬‬ " + to_string(atm_id) + ":‬‬ ‫‪Your‬‬ ‫‪transaction‬‬ ‫‪failed‬‬ ‫–‬ ‫‪account‬‬ ‫‪id‬‬ " + to_string(account) + " ‫‪does‬‬ ‫‪not‬‬ ‫‪exist‬‬";
	logger.WriteToLogger(str_to_log);

}
