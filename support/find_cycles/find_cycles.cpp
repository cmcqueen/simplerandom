
#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#define RECORD_STATE_INTERVAL 2048

typedef uint32_t (*p_rng_func_t)(uint32_t);

typedef std::map<uint32_t, uint64_t>            cycles_map_t;
typedef std::map<uint64_t, std::set<uint32_t> > lengths_map_t;
typedef std::set<uint32_t>                      values_set_t;    

class FindCycles
{
private:
    cycles_map_t                            _cycles;    // Key is cycle's min value; value is length
    lengths_map_t                           _lengths;   // Key is length; value is set of cycle's min values
    values_set_t                            _values;
    p_rng_func_t                            _p_rng_func;
    uint64_t                                _func_range;

    uint64_t _find_cycle_with_seed(uint32_t start_value);

public:
    FindCycles(p_rng_func_t p_rng_func, uint64_t func_range);
    void find_cycles();
};

FindCycles::FindCycles(p_rng_func_t p_rng_func, uint64_t func_range)
{
    _p_rng_func = p_rng_func;
    _func_range = func_range;
}

void FindCycles::find_cycles()
{
    cycles_map_t::iterator cycles_iter;
    lengths_map_t::iterator lengths_iter;
    uint64_t j;

    for (j = 0; j < _func_range; j++)
    {
        _find_cycle_with_seed(j);
    }

#if 1
    for (cycles_iter = _cycles.begin(); cycles_iter != _cycles.end(); cycles_iter++)
    {
        cout << "Cycle min " << (*cycles_iter).first << ", length " << (*cycles_iter).second;
        cout << "\n";
    }
    cout << "\n";
#endif

    for (lengths_iter = _lengths.begin(); lengths_iter != _lengths.end(); lengths_iter++)
    {
        cout << "Length " << (*lengths_iter).first << ", " << (*lengths_iter).second.size() << " cycles";
        cout << "\n";
    }
    cout << "\n";

}

uint64_t FindCycles::_find_cycle_with_seed(uint32_t start_value)
{
    values_set_t local_values;
    values_set_t::iterator iter;
    uint32_t value = start_value;
    uint32_t length = 0;
    uint64_t return_length = 0;
    uint32_t record_state_counter = 0;
    uint32_t min_value = start_value;

    while (1)
    {
        value = _p_rng_func(value);
        if (value == start_value)
        {
            return_length = ((uint64_t)length) + 1;

            cout << "Found cycle min " << min_value << ", length " << return_length;
            cout << "\n";
            _cycles[min_value] = return_length;

            for (iter = local_values.begin(); iter != local_values.end(); iter++)
            {
                _values.insert(*iter);
            }

            _lengths[return_length].insert(min_value);

            break;
        }
        if (value < min_value)
        {
            break;
        }
#if 0
        if (_values.find(value) != _values.end())
        {
            break;
        }
        record_state_counter++;
        if (record_state_counter >= RECORD_STATE_INTERVAL)
        {
            local_values.insert(value);
            record_state_counter = 0;
        }
#else
        if ((value % RECORD_STATE_INTERVAL) == 0)
        {
            if (_values.find(value) != _values.end())
            {
                break;
            }
            else
            {
                local_values.insert(value);
            }
        }
#endif

        length++;
    }

    return return_length;
}

uint32_t shr3(uint32_t val)
{
    val ^= val << 17u;
    val ^= val >> 13u;
    val ^= val << 5u;
    return val;
}

uint32_t shr3_2(uint32_t val)
{
    val ^= val << 13u;
    val ^= val >> 17u;
    val ^= val << 5u;
    return val;
}

int main()
{
    FindCycles fc(shr3_2, 1uLL << 32);
    fc.find_cycles();
}
