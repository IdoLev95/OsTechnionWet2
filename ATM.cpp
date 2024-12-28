/*
 * ATM.cpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */
using namespace std;
#include <stdlib.h>
#include <cstdio>
#include <bits/stdc++.h>
#include "ATM.hpp"
void* single_atm_applier(void* argv)
{
	cout << "Hello world" << endl;
	string *file_actions = static_cast<string*>(argv);
	// You could use 'sp' directly, or this, which does a copy.
	cout << "The casted valuse is: " << *file_actions << endl;
	pthread_exit(NULL);

}


