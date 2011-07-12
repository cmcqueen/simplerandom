
#include <stdint.h>
#include <stdio.h>

#define MIN_VALUE_SHIFT             3

#define ADJUST_SHIFT                4

#define MIN_VALUE_SHIFT_COUNT       (1 << MIN_VALUE_SHIFT)
#define MIN_VALUE_LOW_BITS_MASK     (MIN_VALUE_SHIFT_COUNT - 1)
#define MIN_VALUE_HIGH_BITS_MASK    (0xFFFFFFFF ^ MIN_VALUE_LOW_BITS_MASK)

#define PASTER(x,y)                 x ## y
#define EVALUATOR(x,y)              PASTER(x,y)

#define ADJUST_SEED                 EVALUATOR(adjust_seed_, ADJUST_SHIFT)
#define INVERSE_ADJUST_SEED         EVALUATOR(inverse_adjust_seed_, ADJUST_SHIFT)

uint32_t distance_uint32(uint32_t a, uint32_t b)
{
    uint32_t    diff;
    
    diff = a - b;
    if (diff > 0x80000000)
        diff = -diff;
    return diff;
}

uint32_t adjust_seed_1(uint32_t seed)
{
    return seed ^ (seed << 1);
}

uint32_t inverse_adjust_seed_1(uint32_t seed)
{
    return seed ^
            (seed << 1) ^
            (seed << 2) ^
            (seed << 3) ^
            (seed << 4) ^
            (seed << 5) ^
            (seed << 6) ^
            (seed << 7) ^
            (seed << 8) ^
            (seed << 9) ^
            (seed << 10) ^
            (seed << 11) ^
            (seed << 12) ^
            (seed << 13) ^
            (seed << 14) ^
            (seed << 15) ^
            (seed << 16) ^
            (seed << 17) ^
            (seed << 18) ^
            (seed << 19) ^
            (seed << 20) ^
            (seed << 21) ^
            (seed << 22) ^
            (seed << 23) ^
            (seed << 24) ^
            (seed << 25) ^
            (seed << 26) ^
            (seed << 27) ^
            (seed << 28) ^
            (seed << 29) ^
            (seed << 30) ^
            (seed << 31);
}

uint32_t adjust_seed_3(uint32_t seed)
{
    return seed ^ (seed << 3);
}

uint32_t inverse_adjust_seed_3(uint32_t seed)
{
    return seed ^
            (seed << 3) ^
            (seed << 6) ^
            (seed << 9) ^
            (seed << 12) ^
            (seed << 15) ^
            (seed << 18) ^
            (seed << 21) ^
            (seed << 24) ^
            (seed << 27) ^
            (seed << 30);
}

uint32_t adjust_seed_4(uint32_t seed)
{
    return seed ^ (seed << 4);
}

uint32_t inverse_adjust_seed_4(uint32_t seed)
{
    return seed ^
            (seed << 4) ^
            (seed << 8) ^
            (seed << 12) ^
            (seed << 16) ^
            (seed << 20) ^
            (seed << 24) ^
            (seed << 28);
}

uint32_t adjust_seed_7(uint32_t seed)
{
    return seed ^ (seed << 7);
}

uint32_t inverse_adjust_seed_7(uint32_t seed)
{
    return seed ^
            (seed << 7) ^
            (seed << 14) ^
            (seed << 21) ^
            (seed << 28);
}

uint32_t adjust_seed_8(uint32_t seed)
{
    return seed ^ (seed << 8);
}

uint32_t inverse_adjust_seed_8(uint32_t seed)
{
    return seed ^
            (seed << 8) ^
            (seed << 16) ^
            (seed << 24);
}

uint32_t adjust_seed_16(uint32_t seed)
{
    return seed ^ (seed << 16);
}

uint32_t inverse_adjust_seed_16(uint32_t seed)
{
    return seed ^ (seed << 16);
}

uint32_t adjust_seed_24(uint32_t seed)
{
    return seed ^ (seed << 24);
}

uint32_t inverse_adjust_seed_24(uint32_t seed)
{
    return seed ^ (seed << 24);
}

int main()
{
    uint32_t    seeds[MIN_VALUE_SHIFT_COUNT * 2];
    uint32_t    num_seeds;
    uint32_t    local_min_distance;
    uint32_t    min_distance;
    uint32_t    distance;
    uint32_t    val;
    uint32_t    i, j;
    int         seeds_idx;
    int         seeds_idx2;
    
    min_distance = 0xFFFFFFFF;
    i = MIN_VALUE_SHIFT_COUNT;
    do
    {
        //for (seeds_idx = 0; seeds_idx < MIN_VALUE_SHIFT_COUNT; seeds_idx++)
        //{
        //    seeds[seeds_idx] = 0xFFFFFFFF;
        //}
        num_seeds = 0;

        j = i;
        do
        {
            val = INVERSE_ADJUST_SEED(j);

            // Insert val into array, sorted
            for (seeds_idx = 0;
                (seeds_idx < num_seeds) && val > seeds[seeds_idx];
                seeds_idx++)
            {
                // Looking for appropriate spot
            }
            for (seeds_idx2 = num_seeds - 1; seeds_idx2 >= seeds_idx; seeds_idx2--)
                seeds[seeds_idx2 + 1] = seeds[seeds_idx2];
            seeds[seeds_idx] = val;
            num_seeds++;

            j++;
        } while ((j & MIN_VALUE_LOW_BITS_MASK) != 0);

        local_min_distance = 0xFFFFFFFF;
        for (seeds_idx = 0; seeds_idx < num_seeds; seeds_idx++)
        {
            distance = distance_uint32(seeds[seeds_idx], seeds[(seeds_idx + 1) % num_seeds]);
            if (distance < local_min_distance)
                local_min_distance = distance;
        }

        if ((local_min_distance < 0x100) ||
            (local_min_distance < min_distance))
        {
            printf("min dist %08X; seed out %08X; seeds", local_min_distance, i);
            for (seeds_idx = 0; seeds_idx < num_seeds; seeds_idx++)
                printf(" %08X", seeds[seeds_idx]);
            printf("\n");
        }
        
        if (local_min_distance < min_distance)
        {
            min_distance = local_min_distance;
        }
        i += MIN_VALUE_SHIFT_COUNT;
    } while (i != 0);
}
