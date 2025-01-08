#include "ATM.hpp"
#include "Consumer_Producer.hpp"

#include <stdbool.h>

int is_vip_atm(stringstream& SS);
bool is_PERSISTENT(stringstream& SS);

void* single_atm_applier(void* argv){
	int atm_id = 0; // TODO - needs to change
    // Cast the argument to a string pointer
    string* path_to_read_actions = static_cast<string*>(argv);
    // Open the file
    ifstream input_file(*path_to_read_actions);

    // Check if the file opened successfully
    if (!input_file.is_open()) {
        //cerr << "Error: Unable to open file " << *path_to_read_actions << endl;
        pthread_exit(NULL); // Exit the thread if the file cannot be opened
    }

    // Read each line until reaching the end
    std::string line;
    sleep(0.1);
    while (getline(input_file, line)) {
		int priority = 0;
		bool PERSISTENT_flag = false;
    	bank_params.reader_writer_atm_active_list.reader_locker();
    	if(bank_params.isAtmActive[atm_id]){
    		bank_params.reader_writer_atm_active_list.reader_unlocker();
			// Process each line
			//cout << "Processing: " << line << endl;
			// Split the line into tokens (assuming space-separated values)
			std::stringstream ss(line);
			std::string command;
			ss >> command; // Read the command name (first token)
			priority = is_vip_atm(ss);
			PERSISTENT_flag = is_PERSISTENT(ss);
			//cout << "PERSISTENT_flag is: " << PERSISTENT_flag << endl;// סתם לרצות את הקומפיילר הבן שרמוטה 
			if(priority >0){
				string vipss = ss.str();
				cp.producer(vipss,priority);
			}
			// Example: Process different commands based on the first token
			else if  (command == "O") {
				int account_id, password, initial_balance;
				ss >> account_id >> password >> initial_balance;
				//cout << "Creating account: " << account_id << " with balance " << initial_balance << endl;
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
			else if (command == "C"){
				int dst_atm_id;
				ss >> dst_atm_id;
				//cout << "Atm: " << atm_id << " is closing " << dst_atm_id << endl;
				bank_params.close_atm(dst_atm_id,atm_id,PERSISTENT_flag);
			
			}
			else {
				cout << "Unknown command: " << command << endl;
			}
    	}
    	else{
    		bank_params.reader_writer_atm_active_list.reader_unlocker();
    	}
    	//sleep(1);
    	//sleep(0.1);
    }
	

    // Close the file
    input_file.close();

    // Exit the thread successfully
    pthread_exit(NULL);
}



int is_vip_atm(stringstream& SS){
	string vip_priority = SS.str();
    size_t pos = vip_priority.find("VIP=");
    if (pos != string::npos) {
		cout << "is vip atm command is :" << vip_priority << endl;
        int number;
        try {
            number = stoi(vip_priority.substr(pos + 4));
            cout << "VIP=" << number << endl;
        } catch (const invalid_argument& e) {
            // cout << "Invalid VIP number" << endl;
        }
        return number;
    }
    return 0;
}

bool is_PERSISTENT(stringstream& SS){
	string vip_priority = SS.str();
	size_t pos = vip_priority.find("PERSISTENT");
	if (pos != string::npos) {
		cout << "PERSISTENT command is :" << vip_priority << endl;
		return true;
	}
	return false;
}