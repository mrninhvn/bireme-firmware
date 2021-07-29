
// #ifdef LEFT_HAND

//     /* Left hand key definitions */
//     #define S00 {1,  0}
//     #define S01 {1,  1}
//     #define S02 {1, 15}
//     #define S03 {0, 12}
//     #define S04 {0, 16}
//     #define S05 {0, 20}
//     #define S06 {0, 22}
//     #define S07 {0, 10}
//     #define S08 {1,  2}
//     #define S09 {0, 20}
//     #define S10 {0,  5}
//     #define S11 {0, 15}
//     #define S12 {0, 19}
//     #define S13 {0, 23}
//     #define S14 {1, 11}
//     #define S15 {1,  3}
//     #define S16 {1, 14}
//     #define S17 {0,  9}
//     #define S18 {0, 14}
//     #define S19 {0, 21}
//     #define S20 {1, 10}
//     #define S21 {1,  4}
//     #define S22 {0,  3}
//     #define S23 {0,  7}
//     #define S24 {0, 13}
//     #define S25 {0, 17}
//     #define S26 {0, 24}
//     #define S27 {0,  4}
//     #define S28 {0, 31}
//     #define S29 {1, 13}
//     #define S30 {1, 12}
//     #define S31 {0, 28}
//     #define S32 {0, 26}
//     #define S33 {0, 27}
//     #define S34 {0, 25}

//     #define PIPE_NUMBER BIREME_LEFT_HAND_PIPE

// #elif defined (RIGHT_HAND)

//     /* Right hand key definitions */
//     #define S00 {1,  0}
//     #define S01 {1,  6}
//     #define S02 {0,  9}
//     #define S03 {1, 13}
//     #define S04 {0,  2}
//     #define S05 {0, 28}
//     #define S06 {0, 27}
//     #define S07 {1,  1}
//     #define S08 {1,  7}
//     #define S09 {0, 11}
//     #define S10 {0,  5}
//     #define S11 {1, 12}
//     #define S12 {0, 29}
//     #define S13 {0, 26}
//     #define S14 {1,  2}
//     #define S15 {1,  8}
//     #define S16 {0, 13}
//     #define S17 {1, 15}
//     #define S18 {0, 13}
//     #define S19 {0, 30}
//     #define S20 {1,  3}
//     #define S21 {1,  9}
//     #define S22 {0, 14}
//     #define S23 {0,  7}
//     #define S24 {0,  3}
//     #define S25 {0, 31}
//     #define S26 {0, 25}
//     #define S27 {0, 15}
//     #define S28 {0, 16}
//     #define S29 {0, 17}
//     #define S30 {0, 21}
//     #define S31 {0, 19}
//     #define S32 {0, 23}
//     #define S33 {0, 20}
//     #define S34 {0, 24}

//     #define PIPE_NUMBER BIREME_RIGHT_HAND_PIPE

// #else
//     #error Side not defined. Must be one of LEFT_HAND or RIGHT_HAND.
// #endif

#define HAND_SENSE 12
// #define RIGHT_HAND false
// #define COMPILE_LEFT

#define ALPHA_SENSE 20
#define ALPABETICAL false

// left hand pins

#define L_C01 6
#define L_C02 5
#define L_C03 4
#define L_C04 3
#define L_C05 2
#define L_C06 0
#define L_C07 30
#define L_R01 28
#define L_R02 23
#define L_R03 22
#define L_R04 21
#define L_R05 29

#define L_MASK (1<<L_C01 | \
 				1<<L_C02 | \
				1<<L_C03 | \
				1<<L_C04 | \
				1<<L_C05 | \
				1<<L_C06 | \
				1<<L_C07)

// right hand pins

#define R_C01 10
#define R_C02 9
#define R_C03 7
#define R_C04 6
#define R_C05 5
#define R_C06 4
#define R_C07 3
#define R_R01 14
#define R_R02 17
#define R_R03 18
#define R_R04 19
#define R_R05 13

#define R_MASK (1<<R_C01 | \
 				1<<R_C02 | \
				1<<R_C03 | \
				1<<R_C04 | \
				1<<R_C05 | \
				1<<R_C06 | \
				1<<R_C07)

// #ifdef COMPILE_LEFT

#define PIPE_NUMBER 0

#define C01 L_C01
#define C02 L_C02
#define C03 L_C03
#define C04 L_C04
#define C05 L_C05
#define C06 L_C06
#define C07 L_C07
#define R01 L_R01
#define R02 L_R02
#define R03 L_R03
#define R04 L_R04
#define R05 L_R05

#define INPUT_MASK L_MASK

// #endif

#ifdef COMPILE_RIGHT

#define PIPE_NUMBER 1

#define C01 R_C01
#define C02 R_C02
#define C03 R_C03
#define C04 R_C04
#define C05 R_C05
#define C06 R_C06
#define C07 R_C07
#define R01 R_R01
#define R02 R_R02
#define R03 R_R03
#define R04 R_R04
#define R05 R_R05

#define INPUT_MASK R_MASK

#endif

#define COLUMNS 7
#define ROWS 5



// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
