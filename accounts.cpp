/*
 * acounts.cpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#include "accounts.hpp"

accounts::accounts(int Account_id,int Amount,int Password):reader_writer_user_account(lock_reader,lock_writer) {
	account_id = Account_id;
	amount = Amount;
	password = Password;
 if (pthread_mutex_init(&lock_reader, NULL) != 0) {
		printf("\n mutex init has failed\n");
	}

 if (pthread_mutex_init(&lock_writer, NULL) != 0) {
		printf("\n mutex init has failed\n");
	}
}

accounts::~accounts() {
}

bool accounts::validate_password(int password_entered)
{
	return (password_entered == password)? true: false;
}
