#include <stdio.h>
#include <stdint.h>

#define NUM_SBOXES 8
#define NUM_INPUTS 64  // 6-bit input, 64 combinations
#define NUM_OUTPUTS 16 // 4-bit output, 16 combinations

// S-Boxes for DES
static const uint8_t S_BOX[NUM_SBOXES][4][16] = {
    { {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
      {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
      {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
      {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13} },
    { {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
      {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
      {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
      {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9} },
    { {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
      {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
      {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
      {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12} },
    { {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
      {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
      {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
      {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14} },
    { {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
      {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
      {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
      {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3} },
    { {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
      {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
      {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
      {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13} },
    { {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
      {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
      {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
      {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12} },
    { {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
      {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
      {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
      {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11} }
};

// Function to compute Linear Approximation Table (LAT)
void computeLAT()
{
    int i, s, alpha, beta;
    int lat[NUM_SBOXES][NUM_INPUTS][NUM_OUTPUTS] = {0}; // Linear Approximation Table

    // Iterate over each S-box
    for (s = 0; s < NUM_SBOXES; s++)
    {
        // Iterate over each input and output combination
        for (alpha = 0; alpha < NUM_INPUTS; alpha++) // 64 input combinations
        {
            for (beta = 0; beta < NUM_OUTPUTS; beta++) // 16 output combinations
            {
                int alpha_vector[6], beta_vector[4];
                int input_vector[6], output_vector[4];

                // Convert input 6 bits to vector
                for (i = 0; i < 6; i++)
                {
                    alpha_vector[i] = (alpha >> (5 - i)) & 1;
                }

                // Convert output 4 bits to vector
                for (i = 0; i < 4; i++)
                {
                    beta_vector[i] = (beta >> (3 - i)) & 1;
                }

                // Iterate over all possible inputs
                for (int input = 0; input < NUM_INPUTS; input++)
                {
                    // Get the output from the S-box
                    int output = S_BOX[s][(2*(input >> 5))+(input % 2)][(input & 0x1E)>>1]; // Map 6-bit input to S-box




                    // Convert input to 6-bit vector
                    for (i = 0; i < 6; i++)
                    {
                        input_vector[i] = (input >> (5 - i)) & 1;
                    }
                    // Convert output to 4-bit vector
                    for (i = 0; i < 4; i++)
                    {
                        output_vector[i] = (output >> (3 - i)) & 1;
                    }
                    // Compute the inner product for alpha and input, beta and output
                    int inner_product1 = 0;
                    for (i = 0; i < 6; i++)
                    {
                        inner_product1 ^= alpha_vector[i] & input_vector[i];
                    }
                    int inner_product2 = 0;
                    for (i = 0; i < 4; i++)
                    {
                        inner_product2 ^= beta_vector[i] & output_vector[i];
                    }
                    int inner_product = inner_product1 ^ inner_product2;
                    if (inner_product == 0)
                    {
                        lat[s][alpha][beta]++;
                    }
                }
                lat[s][alpha][beta]-=32;
            }
        }
    }

    // Output the Linear Approximation Table (LAT)
    for (s = 0; s < NUM_SBOXES; s++)
    {
        printf("S-box %d Linear Approximation Table (LAT):\n", s + 1);
        for (alpha = 0; alpha < NUM_INPUTS; alpha++)
        {
            for (beta = 0; beta < NUM_OUTPUTS; beta++)
            {
                printf("%3d ", lat[s][alpha][beta]); // Set width to 3 to align the output
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main()
{
    computeLAT(); // Compute the Linear Approximation Table
    return 0;
}
