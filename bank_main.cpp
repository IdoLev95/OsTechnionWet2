//smash.c

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h>
#include <cstdio>
#include <bits/stdc++.h>
#include <pthread.h>

#include "ATM.hpp"
#include "globals.hpp"
using namespace std;
/*=============================================================================
* classes/structs declarations
=============================================================================*/

/*=============================================================================
* global variables & data structures
=============================================================================*/

/*=============================================================================
* main function
=============================================================================*/
int main(int argc, char* argv[])
{



	bank_params.EraseLoggerContent();
	const int N = 20;
	pthread_t atm_threads[N];
    void** pointers = new void*[N]; // Allocate array of void*
	for (int i = 0; i < N; ++i) {
		string* value = new string("ATM_file"+to_string(i)+".txt"); // Create an int with value i
		pointers[i] = static_cast<void*>(value); // Store pointer as void*
	}
	for(int ind =0; ind < N;ind++)
	{
		pthread_create(&atm_threads[ind], NULL, &single_atm_applier, pointers[ind]);
	}
	for(int ind =0;ind < N;ind++)
	{
		// Waiting for the created thread to terminate
		pthread_join(atm_threads[ind], NULL);
	}
	return 0;
}

