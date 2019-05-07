// File to concentrate default coin limits

#ifndef MONEDERO_VARS_H_
#define MONEDERO_VARS_H_

// Global variables ################################################

// External variables to integrate with model
uint8_t queue;
uint8_t payment_money;

// Detection variables --------------
uint8_t t2, t3, w2, w3;

//Flags ------------------------------
uint8_t new_coin;
uint8_t calibrate;
uint8_t coin_id;

// Coin characterization ------------
struct coin_ratios {
  float w3_d;
  float w3_t;
};

struct coin_params { 
  struct coin_ratios min;
  struct coin_ratios max;
};

struct coin_params coins[8];

void loadDefaultLimits(){       // Adjust when limits have been calibrated for adequate statistical sample
  coins[0].min.w3_d = 0.83;
  coins[0].max.w3_d = 1.04;
  
  coins[1].min.w3_d = 1.00;
  coins[1].max.w3_d = 1.11;
  
  coins[2].min.w3_d = 1.18;
  coins[2].max.w3_d = 1.23;

  coins[3].min.w3_d = 1.06;
  coins[3].max.w3_d = 1.19;

  coins[4].min.w3_d = 1.28;
  coins[4].max.w3_d = 1.39;

  coins[5].min.w3_d = 1.28;
  coins[5].max.w3_d = 1.40;

  coins[6].min.w3_d = 1.30;
  coins[6].max.w3_d = 1.37;

  coins[7].min.w3_d = 1.34;
  coins[7].max.w3_d = 6.40;
}

#endif // MONEDERO_VARS_H_
