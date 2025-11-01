#include <stdio.h>
#include <stdint.h>

#define NUM_SBOXES 8
#define NUM_INPUTS 64  // 6-bit input, 64 combinations
#define NUM_OUTPUTS 16 // 4-bit output, 16 combinations

// S-Boxes for DES
static const uint8_t S_BOX[NUM_SBOXES][4][16] = {
    {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 14, 7, 12, 0, 5, 9, 6}},
    {{10, 0, 9, 14, 13, 3, 15, 6, 12, 5, 10, 9, 7, 4, 11, 1},
     {15, 10, 8, 1, 3, 5, 11, 0, 7, 14, 6, 12, 13, 4, 9, 2},
     {0, 15, 6, 11, 1, 4, 13, 9, 12, 8, 7, 14, 5, 3, 10, 2},
     {7, 4, 13, 12, 10, 9, 3, 5, 0, 15, 14, 6, 11, 1, 8, 2}},
    {{7, 13, 14, 3, 15, 5, 2, 11, 10, 9, 0, 4, 1, 12, 8, 6},
     {9, 14, 3, 8, 0, 15, 7, 5, 6, 2, 12, 13, 1, 4, 10, 11},
     {15, 1, 12, 8, 4, 10, 7, 14, 2, 0, 6, 9, 3, 13, 11, 5},
     {2, 7, 11, 4, 9, 14, 10, 3, 6, 12, 1, 5, 0, 15, 8, 13}},
    {{15, 5, 12, 10, 9, 0, 14, 3, 1, 11, 13, 7, 6, 2, 8, 4},
     {1, 12, 5, 14, 3, 7, 8, 13, 10, 6, 9, 15, 4, 0, 2, 11},
     {7, 13, 6, 9, 8, 3, 15, 14, 1, 10, 12, 5, 4, 0, 11, 2},
     {2, 11, 15, 13, 9, 4, 7, 0, 8, 6, 14, 3, 12, 1, 10, 5}},
    {{10, 15, 13, 8, 9, 1, 14, 7, 0, 3, 4, 6, 5, 2, 12, 11},
     {3, 15, 6, 9, 0, 7, 4, 14, 2, 8, 5, 13, 12, 1, 10, 11},
     {4, 2, 11, 7, 9, 12, 14, 6, 1, 0, 10, 15, 3, 8, 13, 5},
     {0, 1, 12, 15, 14, 9, 11, 7, 6, 13, 3, 5, 10, 8, 4, 2}},
    {{8, 7, 12, 13, 14, 0, 1, 11, 15, 2, 6, 5, 9, 4, 10, 3},
     {7, 1, 13, 10, 2, 14, 0, 6, 8, 9, 5, 12, 3, 15, 4, 11},
     {15, 9, 8, 6, 7, 13, 10, 1, 4, 2, 12, 3, 11, 5, 14, 0},
     {2, 10, 12, 9, 14, 5, 1, 7, 8, 4, 15, 3, 13, 11, 6, 0}},
    {{3, 10, 15, 1, 0, 6, 9, 14, 5, 7, 13, 4, 8, 12, 11, 2},
     {13, 6, 9, 15, 3, 12, 1, 5, 14, 10, 8, 11, 0, 2, 7, 4},
     {7, 0, 6, 13, 15, 9, 1, 11, 3, 10, 5, 14, 4, 12, 8, 2},
     {10, 2, 7, 5, 9, 8, 0, 14, 11, 1, 12, 4, 13, 6, 3, 15}}};

// 计算每个 S 盒的线性近似式分布表
void computeLAT()
{
    int i, j, s, alpha, beta;
    int lat[NUM_SBOXES][NUM_INPUTS][NUM_OUTPUTS] = {0}; // 线性近似式分布表

    // 遍历每个 S 盒
    for (s = 0; s < NUM_SBOXES; s++)
    {
        // 对每个 S 盒的输入和输出进行分析
        for (alpha = 0; alpha < NUM_INPUTS; alpha++)
        { // 输入位组合
            for (beta = 0; beta < NUM_OUTPUTS; beta++)
            { // 输出位组合

                int alpha_vector[6], beta_vector[4];
                int input_vector[6], output_vector[4];

                // 将输入的 6 位转换为向量
                for (i = 0; i < 6; i++)
                {
                    alpha_vector[i] = (alpha >> (5 - i)) & 1;
                }

                // 将输出的 4 位转换为向量
                for (i = 0; i < 4; i++)
                {
                    beta_vector[i] = (beta >> (3 - i)) & 1;
                }
                for (int input = 0; input < NUM_INPUTS; input++)
                {
                    int output = S_BOX[s][input / 16][input % 16];
                    for (i = 0; i < 6; i++)
                    {
                        input_vector[i] = (input >> (5 - i)) & 1;
                    }
                    for (i = 0; i < 4; i++)
                    {
                        output_vector[i] = (output >> (3 - i)) & 1;
                    }
                    // 计算内积
                    int inner_product1 = 0;
                    for (i = 0; i < 6; i++)
                    {
                        inner_product1 ^= alpha_vector[i] & input_vector[i]; // 使用内积的方式
                    }
                    int inner_product2 = 0;
                    for (i = 0; i < 4; i++)
                    {
                        inner_product2 ^= beta_vector[i] & output_vector[i]; // 使用内积的方式
                    }
                    int inner_product = inner_product1 ^ inner_product2;
                    // 根据内积结果更新 LAT
                    if (inner_product == 0)
                    {
                        lat[s][alpha][beta]++;
                    }
                }
                {
                    lat[s][alpha][beta] -= 32;
                }
            }
        }
    }

    // 输出计算结果
    // 输出计算结果
    for (s = 0; s < NUM_SBOXES; s++)
    {
        printf("S-box %d Linear Approximation Table (LAT):\n", s + 1);
        for (alpha = 0; alpha < NUM_INPUTS; alpha++)
        {
            for (beta = 0; beta < NUM_OUTPUTS; beta++)
            {
                printf("%3d ", lat[s][alpha][beta]); // 设置宽度为3，确保数字对齐
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main()
{
    // 计算线性近似式分布表
    computeLAT();
    return 0;
}
