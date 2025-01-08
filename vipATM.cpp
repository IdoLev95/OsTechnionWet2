#include "vipATM.hpp"

void* vip_atm_applier(void* argv){
    std::string line = cp.consumer();
	cout << line << endl;
    int atm_id = 0; // TODO - needs to change
	bool PERSISTENT_flag = false;
    std::istringstream ss (line);
    std::string command;
    ss >> command;
    // Example: Process different commands based on the first token
     if (command == "O") {
				int account_id, password, initial_balance;
				ss >> account_id >> password >> initial_balance;
				cout << "VIP thread exicuted the Creating account: " << account_id << " with balance " << initial_balance << endl;

				bank_params.insert_new_account(account_id, initial_balance, password, atm_id,PERSISTENT_flag);
			}
			else if (command == "Q") {
				int account_id, password;
				ss >> account_id >> password;
				//cout << "Closing " << account_id<< " with password " << password  << endl;
				bank_params.close_existing_account(account_id,password,atm_id,PERSISTENT_flag);
			}
			else if (command == "Q") {
						int account_id, password;
						ss >> account_id >> password;
						//cout << "Closing " << account_id<< " with password " << password  << endl;
						bank_params.close_existing_account(account_id,password,atm_id,PERSISTENT_flag);
					}
			else if (command == "D") {
						int account_id, password,amount;
						ss >> account_id >> password >> amount;
						//cout << "Deposit " << account_id<< " with password " << password  << endl;
						bank_params.deposit(account_id,amount,password,atm_id,PERSISTENT_flag);
					}
			else if (command == "W") {
				int account_id, password,amount;
					ss >> account_id >> password >> amount;
					//cout << "Withdraw " << account_id<< " with password " << password  << endl;
					bank_params.withdraw(account_id,amount,password,atm_id,PERSISTENT_flag);
					}
			else if (command == "B") {
						int account_id, password;
						ss >> account_id >> password;
						//cout << "Checking balance " << account_id<< " with password " << password  << endl;
						bank_params.get_balance(account_id,password,atm_id,PERSISTENT_flag);
					}
			else if (command == "T") {
						int src_account_id, src_password,dst_account,amount;
						ss >> src_account_id >> src_password >> dst_account >> amount;
						//cout << "Transfer from account " << src_account_id<< " with password " << src_password << " To account " << dst_account << " the following balance: " << amount << endl;
						bank_params.transfer_money_between_accounts(src_account_id,src_password,dst_account,amount,atm_id,PERSISTENT_flag);
					}
			else if (command == "C")
			{
				int dst_atm_id;
				ss >> dst_atm_id;
				//cout << "Atm: " << atm_id << " is closing " << dst_atm_id << endl;
				bank_params.close_atm(dst_atm_id,atm_id,PERSISTENT_flag);
			}
			else {
				cout << "Unknown command from vip atm: " << command << endl;
			}    	    
    // Exit the thread successfully
    pthread_exit(NULL);
    }
