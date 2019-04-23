// File to concentrate default coin limits

#ifndef _DEF_COIN_LIMITS_H
#define _DEF_COIN_LIMITS_H

// Coin characterization ------------
struct coin_ratios {
  float w3_d;
  float w3_t;
};

struct coin_params { 
  coin_ratios min;
  coin_ratios max;
};

coin_params coins[8];

void loadDefaultLimits(){       // Adjust when limits have been calibrated for adequate statistical sample
  coins[0].min.w3_d = 2;
  coins[0].max.w3_d = 2;
  coins[0].min.w3_t = 2;
  coins[0].max.w3_t = 2;
  
  coins[1].min.w3_d = 2;
  coins[1].max.w3_d = 2;
  coins[1].min.w3_t = 2;
  coins[1].max.w3_t = 2;
  
  coins[2].min.w3_d = 2;
  coins[2].max.w3_d = 2;
  coins[2].min.w3_t = 2;
  coins[2].max.w3_t = 2;

  coins[3].min.w3_d = 2;
  coins[3].max.w3_d = 2;
  coins[3].min.w3_t = 2;
  coins[3].max.w3_t = 2;

  coins[4].min.w3_d = 2;
  coins[4].max.w3_d = 2;
  coins[4].min.w3_t = 2;
  coins[4].max.w3_t = 2;

  coins[5].min.w3_d = 2;
  coins[5].max.w3_d = 2;
  coins[5].min.w3_t = 2;
  coins[5].max.w3_t = 2;

  coins[6].min.w3_d = 2;
  coins[6].max.w3_d = 2;
  coins[6].min.w3_t = 2;
  coins[6].max.w3_t = 2;

  coins[7].min.w3_d = 2;
  coins[7].max.w3_d = 2;
  coins[7].min.w3_t = 2;
  coins[7].max.w3_t = 2;
}

#endif // _DEF_COIN_LIMITS_H
