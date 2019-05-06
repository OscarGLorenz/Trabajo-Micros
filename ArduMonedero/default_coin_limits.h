// File to concentrate default coin limits

#ifndef _DEF_COIN_LIMITS_H
#define _DEF_COIN_LIMITS_H

// Coin characterization ------------
struct coin_ratios {
  float w2_d;
  float w3_d;
  float w2_t;
  float w3_t;
};

struct coin_params { 
  struct coin_ratios min;
  struct coin_ratios max;
};

struct coin_params coins[8];

void loadDefaultLimits(){       // Adjust when limits have been calibrated for adequate statistical sample
  coins[0].min.w2_d = 10.0; coins[0].max.w2_d = 0.00;
  coins[1].min.w2_d = 10.0; coins[1].max.w2_d = 0.00;
  coins[2].min.w2_d = 10.0; coins[2].max.w2_d = 0.00;
  coins[3].min.w2_d = 10.0; coins[3].max.w2_d = 0.00;
  coins[4].min.w2_d = 10.0; coins[4].max.w2_d = 0.00;
  coins[5].min.w2_d = 10.0; coins[5].max.w2_d = 0.00;
  coins[6].min.w2_d = 10.0; coins[6].max.w2_d = 0.00;
  coins[7].min.w2_d = 10.0; coins[7].max.w2_d = 0.00;
  
  coins[0].min.w3_d = 0.83; coins[0].max.w3_d = 1.04;
  coins[1].min.w3_d = 1.00; coins[1].max.w3_d = 1.11;
  coins[2].min.w3_d = 1.18; coins[2].max.w3_d = 1.23;
  coins[3].min.w3_d = 1.06; coins[3].max.w3_d = 1.19;
  coins[4].min.w3_d = 1.28; coins[4].max.w3_d = 1.39;
  coins[5].min.w3_d = 1.28; coins[5].max.w3_d = 1.40;
  coins[6].min.w3_d = 1.30; coins[6].max.w3_d = 1.37;
  coins[7].min.w3_d = 1.34; coins[7].max.w3_d = 6.40;

  coins[0].min.w2_t = 10.0; coins[0].max.w2_t = 0.00;
  coins[1].min.w2_t = 10.0; coins[1].max.w2_t = 0.00;
  coins[2].min.w2_t = 10.0; coins[2].max.w2_t = 0.00;
  coins[3].min.w2_t = 10.0; coins[3].max.w2_t = 0.00;
  coins[4].min.w2_t = 10.0; coins[4].max.w2_t = 0.00;
  coins[5].min.w2_t = 10.0; coins[5].max.w2_t = 0.00;
  coins[6].min.w2_t = 10.0; coins[6].max.w2_t = 0.00;
  coins[7].min.w2_t = 10.0; coins[7].max.w2_t = 0.00;

  coins[0].min.w3_t = 10.0; coins[0].max.w3_t = 0.00;
  coins[1].min.w3_t = 10.0; coins[1].max.w3_t = 0.00;
  coins[2].min.w3_t = 10.0; coins[2].max.w3_t = 0.00;
  coins[3].min.w3_t = 10.0; coins[3].max.w3_t = 0.00;
  coins[4].min.w3_t = 10.0; coins[4].max.w3_t = 0.00;
  coins[5].min.w3_t = 10.0; coins[5].max.w3_t = 0.00;
  coins[6].min.w3_t = 10.0; coins[6].max.w3_t = 0.00;
  coins[7].min.w3_t = 10.0; coins[7].max.w3_t = 0.00;
}

#endif // _DEF_COIN_LIMITS_H
