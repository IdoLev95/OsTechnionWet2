#include "ATM.hpp"

bool is_PERSISTENT(stringstream& SS);
int is_vip_atm(stringstream& SS);

void* single_atm_applier(void* argv){
	ThreadArgs* threadArgs = static_cast<ThreadArgs*>(argv);

	// Access the atm_id and arg fields
	int atm_id = *(threadArgs->atm_id);  // Dereference the pointer to get atm_id value
	string path_to_read_actions = threadArgs->arg;

	    // Now you can use these values as needed
	    //string* path_to_read_actions = new string("ATM_file" + to_string(atm_id) + ".txt");

	//int atm_id = *((int*)argv);
    // Cast the argument to a string pointer
	//string* path_to_read_actions = new string("ATM_file" + to_string(atm_id) + ".txt");

	//string* path_to_read_actions = string("ATM_file"+to_string(atm_id)+".txt"); // Create an int with value i
    //string* path_to_read_actions = static_cast<string*>(argv);
    // Open the file
    ifstream input_file(path_to_read_actions);

    // Check if the file opened successfully
    if (!input_file.is_open()) {
        //cerr << "Error: Unable to open file " << *path_to_read_actions << endl;
    	bank_params.close_atm(atm_id,atm_id,false);
        pthread_exit(NULL); // Exit the thread if the file cannot be opened
    }

    // Read each line until reaching the end
    std::string line;
    usleep(100000);
    while (getline(input_file, line)) {

    	//bank_params.logger.WriteToLogger(line);
    	int priority = 0;
		bool PERSISTENT_flag = false;
    	bank_params.reader_writer_atm_active_list.reader_locker();
    	if(bank_params.isAtmActive[atm_id]){
    		bank_params.reader_writer_atm_active_list.reader_unlocker();

			// Split the line into tokens (assuming space-separated values)
			std::stringstream ss(line);
			std::string command;
			ss >> command; // Read the command name (first token)
    		priority = is_vip_atm(ss);
			PERSISTENT_flag = is_PERSISTENT(ss);
			//PERSISTENT_flag = false;
			//cout << "PERSISTENT_flag is: " << PERSISTENT_flag << endl;// סתם לרצות את הקומפיילר הבן שרמוטה
			if(priority >0){
				string vipss = ss.str();
				cp.producer(vipss,priority);
				sleep(1);
			}
			else{
				sleep(1);
				//usleep(100000);
				// Example: Process different commands based on the first token
				if  (command == "O") {
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
				else if(command == "R")
				{
					int restore_ind;
					ss>> restore_ind;
					priority = is_vip_atm(ss);
					if(priority >0){
						string vipss=to_string(restore_ind);
						cp.producer(vipss,priority); //TODO: This does not seems to work - missing for the cp the command. it seems to happen alot.
					}
					else
					{
						bank_params.insert_restore_int_to_req_list(restore_ind,atm_id);
					}
				}
				else {
					cout << "Unknown command: " << command << endl;
				}
			}
    	}
    	else{
    		bank_params.reader_writer_atm_active_list.reader_unlocker();
    	}

    	//sleep(1);
    	usleep(100000);
    }
	
    bank_params.close_atm(atm_id,atm_id,false,false);
    // Close the file
    input_file.close();
    //delete path_to_read_actions;
    // Exit the thread successfully
    pthread_exit(NULL);
}



int is_vip_atm(stringstream& SS){
	string vip_priority = SS.str();
    size_t pos = vip_priority.find("VIP=");
    if (pos != string::npos) {
		//cout << "is vip atm command is :" << vip_priority << endl;
        int number;
        try {
            number = stoi(vip_priority.substr(pos + 4));
           // cout << "VIP=" << number << endl;
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
		//cout << "PERSISTENT command is :" << vip_priority << endl;
		return true;
	}
	return false;
}
