// File to concentrate default coin limits

#ifndef _DEF_COIN_LIMITS_H
#define _DEF_COIN_LIMITS_H

// Coin characterization ------------
struct coin_ratios {
  float w2_d;
  float w3_d;
  float w2_t;
  float w3_t;
  float d_s;
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
  
  coins[0].min.w3_d = 10.0; coins[0].max.w3_d = 0.00;
  coins[1].min.w3_d = 10.0; coins[1].max.w3_d = 0.00;
  coins[2].min.w3_d = 10.0; coins[2].max.w3_d = 0.00;
  coins[3].min.w3_d = 10.0; coins[3].max.w3_d = 0.00;
  coins[4].min.w3_d = 10.0; coins[4].max.w3_d = 0.00;
  coins[5].min.w3_d = 10.0; coins[5].max.w3_d = 0.00;
  coins[6].min.w3_d = 10.0; coins[6].max.w3_d = 0.00;
  coins[7].min.w3_d = 10.0; coins[7].max.w3_d = 0.00;

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

  //coins[0].min.d_s = 10.0; coins[0].max.d_s = 0.00;
  coins[1].min.d_s = 4.38; coins[1].max.d_s = 5.14;
  coins[2].min.d_s = 2.57; coins[2].max.d_s = 2.70;
  coins[3].min.d_s = 3.28; coins[3].max.d_s = 3.76;
  coins[4].min.d_s = 2.25; coins[4].max.d_s = 2.30;
  coins[5].min.d_s = 1.75; coins[5].max.d_s = 1.85;
  coins[6].min.d_s = 1.92; coins[6].max.d_s = 2.08;
  coins[7].min.d_s = 1.55; coins[7].max.d_s = 1.62;
}

#endif // _DEF_COIN_LIMITS_H
