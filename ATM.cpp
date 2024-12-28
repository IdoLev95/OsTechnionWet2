#include "ATM.hpp"

void* single_atm_applier(void* argv)
{
	int atm_id = 0; // TODO - needs to change
    // Cast the argument to a string pointer
    string* path_to_read_actions = static_cast<string*>(argv);

    // Open the file
    ifstream input_file(*path_to_read_actions);

    // Check if the file opened successfully
    if (!input_file.is_open()) {
        cerr << "Error: Unable to open file " << *path_to_read_actions << endl;
        pthread_exit(NULL); // Exit the thread if the file cannot be opened
    }

    // Read each line until reaching the end
    string line;
    while (getline(input_file, line)) {
        // Process each line
        cout << "Processing: " << line << endl;

        // Split the line into tokens (assuming space-separated values)
        stringstream ss(line);
        string command;
        ss >> command; // Read the command name (first token)

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

    // Close the file
    input_file.close();

    // Exit the thread successfully
    pthread_exit(NULL);
}
