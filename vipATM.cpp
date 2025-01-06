#include "vipATM.hpp"

void* vip_atm_applier(void* argv){
    cp.consumer();
    std::istringstream ss(static_cast<char*>(argv));
    std::string token;
    std::string command;
    ss >> command;
    // Example: Process different commands based on the first token
        if (command == "O") {
            int account_id, password, initial_balance;
            ss >> account_id >> password >> initial_balance;
            cout << "Creating account: " << account_id << " with balance " << initial_balance << endl;

            bank_params.insert_new_account(account_id, initial_balance, password, atm_id);
        }
        else if (command == "Q") {
            int account_id, password;
            ss >> account_id >> password;
            cout << "Closing " << account_id<< " with password " << password  << endl;
            bank_params.close_existing_account(account_id,password,atm_id);
        }

        else {
            cout << "Unknown command: " << command << endl;
        }
    }
        
   return NULL;
}
