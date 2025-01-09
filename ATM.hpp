/*
 * ATM.hpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#ifndef ATM_HPP_
#define ATM_HPP_
using namespace std;
#include <stdlib.h>
#include <cstdio>
#include <bits/stdc++.h>
#include "globals.hpp"
#include "Bank.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "Consumer_Producer.hpp"
#include <stdbool.h>


void* single_atm_applier(void* argv);

struct ThreadArgs {
    int* atm_id;  // Can be any pointer (e.g., int*, std::string*, etc.)
    string arg;
};


#endif /* ATM_HPP_ */
