/*
 * globals.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: os
 */
#include "globals.hpp"
Bank bank_params (*(new pthread_mutex_t()),  // Placeholder mutex initialization
	    *(new pthread_mutex_t()),  // Placeholder mutex initialization
	    "bank_log.txt",            // Example log file path
	    *(new pthread_mutex_t()),  // Logger write mutex
	    *(new pthread_mutex_t()),  // Logger read mutex
		*(new pthread_mutex_t()),  // num Active Atms write mutex
		*(new pthread_mutex_t()),  // num Active Atms read mutex
		0
		);



