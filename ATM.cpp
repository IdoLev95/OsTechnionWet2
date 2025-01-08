#include "ATM.hpp"
#include "Consumer_Producer.hpp"
int is_vip_atm(stringstream& SS);

void* single_atm_applier(void* argv){
	int atm_id = *((int*)argv);
	cout << atm_id << endl;
    // Cast the argument to a string pointer
	string* path_to_read_actions = new string("ATM_file" + to_string(atm_id) + ".txt");


	//string* path_to_read_actions = string("ATM_file"+to_string(atm_id)+".txt"); // Create an int with value i
    //string* path_to_read_actions = static_cast<string*>(argv);
	consumer_producer cp;
    // Open the file
    ifstream input_file(*path_to_read_actions);

    // Check if the file opened successfully
    if (!input_file.is_open()) {
        //cerr << "Error: Unable to open file " << *path_to_read_actions << endl;
    	bank_params.close_atm(atm_id,atm_id,false);
        pthread_exit(NULL); // Exit the thread if the file cannot be opened
    }

    // Read each line until reaching the end
    std::string line;
    sleep(0.1);
    while (getline(input_file, line)) {
		int priority = 0;
    	bank_params.reader_writer_atm_active_list.reader_locker();
    	if(bank_params.isAtmActive[atm_id]){
    		bank_params.reader_writer_atm_active_list.reader_unlocker();
			// Process each line
			//cout << "Processing: " << line << endl;

			// Split the line into tokens (assuming space-separated values)
			std::stringstream ss(line);
			std::string command;
			ss >> command; // Read the command name (first token)

			// Example: Process different commands based on the first token
			if (command == "O") {
				int account_id, password, initial_balance;
				ss >> account_id >> password >> initial_balance;
				//cout << "Creating account: " << account_id << " with balance " << initial_balance << endl;
				priority = is_vip_atm(ss);
				if(priority >0){
					string vipss=to_string(account_id)+" "+to_string(password)+" "+to_string(initial_balance);
					cp.producer(vipss,priority);
				}	
				else{
				bank_params.insert_new_account(account_id, initial_balance, password, atm_id);
				}
			
			}
			else if (command == "Q") {
				int account_id, password;
				ss >> account_id >> password;
				//cout << "Closing " << account_id<< " with password " << password  << endl;
				priority = is_vip_atm(ss);
				if(priority >0){
					string vipss=to_string(account_id)+" "+to_string(password);
					cp.producer(vipss,priority);
				}
				else{
				bank_params.close_existing_account(account_id,password,atm_id);
			}
			}
			else if (command == "Q") {
						int account_id, password;
						ss >> account_id >> password;
						priority = is_vip_atm(ss);
						if(priority >0){
							string vipss=to_string(account_id)+" "+to_string(password);
							cp.producer(vipss,priority);
						}
						else{
						//cout << "Closing " << account_id<< " with password " << password  << endl;
						bank_params.close_existing_account(account_id,password,atm_id);
					}
				}
			else if (command == "D") {
						int account_id, password,amount;
						ss >> account_id >> password >> amount;
						priority = is_vip_atm(ss);
						if(priority >0){
							string vipss=to_string(account_id)+" "+to_string(password)+" "+to_string(amount);
							cp.producer(vipss,priority);
						}
						else{
						//cout << "Deposit " << account_id<< " with password " << password  << endl;
						bank_params.deposit(account_id,amount,password,atm_id);
					}
				}	
			else if (command == "W") {
				int account_id, password,amount;
					ss >> account_id >> password >> amount;
					priority = is_vip_atm(ss);
					if(priority >0){
						string vipss=to_string(account_id)+" "+to_string(password)+" "+to_string(amount);
						cp.producer(vipss,priority);
					}
					else{
					//cout << "Withdraw " << account_id<< " with password " << password  << endl;
					bank_params.withdraw(account_id,amount,password,atm_id);
					}
			}
			else if (command == "B") {
						int account_id, password;
						ss >> account_id >> password;
						priority = is_vip_atm(ss);
						if(priority >0){
							string vipss=to_string(account_id)+" "+to_string(password);
							cp.producer(vipss,priority);
						}
						else{
						//cout << "Checking balance " << account_id<< " with password " << password  << endl;
						bank_params.get_balance(account_id,password,atm_id);
					}
			}
			else if (command == "T") {
						int src_account_id, src_password,dst_account,amount;
						ss >> src_account_id >> src_password >> dst_account >> amount;
						priority = is_vip_atm(ss);
						if(priority >0){
							string vipss=to_string(src_account_id)+" "+to_string(src_password)+" "+to_string(dst_account)+" "+to_string(amount);
							cp.producer(vipss,priority);
						}
						else{
						//cout << "Transfer from account " << src_account_id<< " with password " << src_password << " To account " << dst_account << " the following balance: " << amount << endl;
						bank_params.transfer_money_between_accounts(src_account_id,src_password,dst_account,amount,atm_id);
					}
				}
			else if (command == "C"){
				int dst_atm_id;
				ss >> dst_atm_id;
				priority = is_vip_atm(ss);
				if(priority >0){
					string vipss=to_string(dst_atm_id);
					cp.producer(vipss,priority);
				}
				else{
				//cout << "Atm: " << atm_id << " is closing " << dst_atm_id << endl;
				bank_params.close_atm(dst_atm_id,atm_id);
				}
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
					bank_params.insert_restore_int_to_req_list(restore_ind);
				}
			}
			else {
				cout << "Unknown command: " << command << endl;
			}
    	}
    	else{
    		bank_params.reader_writer_atm_active_list.reader_unlocker();
    	}
    	sleep(1);
    	sleep(0.1);
    }
	
    bank_params.close_atm(atm_id,atm_id,false);
    // Close the file
    input_file.close();
    delete path_to_read_actions;
    // Exit the thread successfully
    pthread_exit(NULL);
}








int is_vip_atm(stringstream& SS){
	string command;
	SS >> command;
	string vip_command;
	SS >> vip_command;
	if (vip_command.substr(0, 4) == "VIP=") {
		int number;
		try {
			number = stoi(vip_command.substr(4));
			//cout << "VIP=" << number << endl;
		} catch (const invalid_argument& e) {
		//cout << "Invalid VIP number" << endl;
		}
		return number;
	}
	return 0;
}
