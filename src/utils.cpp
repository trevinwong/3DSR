#include "utils.h"
#include <iomanip>
#include <iostream>
#include <algorithm>

int min3(int a, int b, int c)
{
    return std::min(std::min(a, b), c);
}

int max3(int a, int b, int c)
{
    return std::max(std::max(a, b), c);
}

void print_hex(uint32_t num)
{
    std::cout << std::setfill('0') << std::setw(8) << std::hex << num << '\n';
}

float convert_num_to_new_range(float old_low, float old_high, float new_low, float new_high, float num)
{
    float old_range = (old_high - old_low);
    float new_range = (new_high - new_low);
    return (((num - old_low) * new_range) / old_range) + new_low;
}
