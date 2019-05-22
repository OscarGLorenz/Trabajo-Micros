// File to concentrate default coin limits

#ifndef MONEDERO_VARS_H_
#define MONEDERO_VARS_H_

// Global variables for monedero #########################################

uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
unsigned long t2, t3, w2, w3, d, s;

uint8_t coin_state;
uint8_t calibrate;
uint8_t coin_id;

struct coin_params { 
	float ds_min;
	float ds_max;
};

struct coin_params coins[8];

void loadDefaultLimits(){       // Adjust when limits have been calibrated for adequate statistical sample
	coins[0].ds_min = 10.0; coins[0].ds_max = 0.00;
	coins[1].ds_min = 4.38; coins[1].ds_max = 5.14;
	coins[2].ds_min = 2.57; coins[2].ds_max = 2.70;
	coins[3].ds_min = 3.28; coins[3].ds_max = 3.76;
	coins[4].ds_min = 2.25; coins[4].ds_max = 2.30;
	coins[5].ds_min = 1.75; coins[5].ds_max = 1.85;
	coins[6].ds_min = 1.92; coins[6].ds_max = 2.08;
	coins[7].ds_min = 1.55; coins[7].ds_max = 1.62;
}

#endif // MONEDERO_VARS_H_
