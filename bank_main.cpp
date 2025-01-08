//smash.c

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h>
#include <cstdio>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include "ATM.hpp"
#include "globals.hpp"
#include "vipATM.hpp"
#include "Consumer_Producer.hpp"
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
	bank_params.set_num_atms(N);
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
	for(int ind =0;ind <4; ind++){
		bank_params.print_bank_status();
		sleep(1);
	}
	bank_params.restore_status_from_remember(2);
	bank_params.print_bank_status();

	for(int ind =0;ind < N;ind++)
	{
		// Waiting for the created thread to terminate
		pthread_join(atm_threads[ind], NULL);
	}
	const int VIP_N = 10;
	pthread_t vip_threads[VIP_N];
	//void** vip_pointers = new void*[VIP_N]; // Allocate array of void*
	for (int i = 0; i < VIP_N; ++i) {
		string* value = new string(to_string(i)); // Create an int with value i
		cp.producer(*value, i+1);
	}
	for (int i = 0; i<VIP_N; ++i) {
		pthread_create(&vip_threads[i], NULL, &vip_atm_applier, NULL);

		}
	sleep(2);
	isNeededToFinishVipThreads = true;
	for(int ind =0;ind < VIP_N;ind++){
		// Waiting for the created thread to terminate
		pthread_join(vip_threads[ind], NULL);
	}
	return 0;
}

