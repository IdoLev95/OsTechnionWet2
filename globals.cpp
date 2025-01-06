/*
 * globals.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: os
 */
#include "globals.hpp"
#include "Consumer_Producer.hpp"

Bank bank_params (*(new pthread_mutex_t()),  // Placeholder mutex initialization
	    *(new pthread_mutex_t()),  // Placeholder mutex initialization
	    "bank_log.txt",            // Example log file path
	    *(new pthread_mutex_t()),  // Logger write mutex
	    *(new pthread_mutex_t())   // Logger read mutex
		);

std::list<std::string> VIP_jobs[100]; // List of VIP jobs

consumer_producer cp; // Consumer producer object

