// File to concentrate default coin limits

#ifndef _DEF_COIN_LIMITS_H
#define _DEF_COIN_LIMITS_H

// Coin characterization ------------


struct coin_params { 
  float min;
  float max;
};

struct coin_params coins[8];

void loadDefaultLimits(){       // Adjust when limits have been calibrated for adequate statistical sample

  //coins[0].min.d_s = 10.0; coins[0].max.d_s = 0.00;
  coins[1].min = 4.38; coins[1].max = 5.14;
  coins[2].min = 2.57; coins[2].max = 2.70;
  coins[3].min = 3.28; coins[3].max = 3.76;
  coins[4].min = 2.25; coins[4].max = 2.30;
  coins[5].min = 1.75; coins[5].max = 1.85;
  coins[6].min = 1.92; coins[6].max = 2.08;
  coins[7].min = 1.55; coins[7].max = 1.62;
}

#endif // _DEF_COIN_LIMITS_H
