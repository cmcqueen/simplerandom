
#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

#define RECORD_STATE_INTERVAL 2000

typedef uint32_t (*p_rng_func_t)(uint32_t);

class FindCycles
{
private:
    std::map<uint32_t, uint32_t>            _cycles;
    std::map<uint32_t, std::set<uint32_t> > _lengths;
    std::set<uint32_t>                      _values;
    p_rng_func_t                            _p_rng_func;
    uint64_t                                _func_range;

public:
    FindCycles(p_rng_func_t p_rng_func, uint64_t func_range);
    void find_cycles();
    void find_cycle_with_seed(uint32_t start_value);
};

FindCycles::FindCycles(p_rng_func_t p_rng_func, uint64_t func_range)
{
    _p_rng_func = p_rng_func;
    _func_range = func_range;
}

void FindCycles::find_cycles()
{
    std::map<uint32_t, uint32_t>::iterator cycles_iter;
    std::map<uint32_t, std::set<uint32_t> >::iterator lengths_iter;
    uint64_t j;

    for (j = 0; j < _func_range; j++)
    {
        find_cycle_with_seed(j);
    }

    for (cycles_iter = _cycles.begin(); cycles_iter != _cycles.end(); cycles_iter++)
    {
        cout << "Cycle min " << (*cycles_iter).first << ", length " << (*cycles_iter).second;
    }
    cout << "\n";

    for (lengths_iter = _lengths.begin(); lengths_iter != _lengths.end(); lengths_iter++)
    {
        cout << "Length " << (*lengths_iter).first << ", " << (*lengths_iter).second.size() << " cycles";
    }
    cout << "\n";

}

void FindCycles::find_cycle_with_seed(uint32_t start_value)
{
    std::set<uint32_t> local_values;
    std::set<uint32_t>::iterator iter;
    uint32_t value = start_value;
    uint32_t length = 0;
    uint32_t record_state_counter = 0;
    uint32_t min_value = (uint32_t)-1;

    record_state_counter = 0;
    while (1)
    {
        length++;
        value = _p_rng_func(value);
        min_value = min(value, min_value);
        if (value == start_value)
        {
            cout << "Found cycle min " << min_value << ", length " << length;
            cout << "\n";
            _cycles[min_value] = length;
            local_values.insert(min_value);
            _lengths[length].insert(min_value);
            break;
        }
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
    }
    for (iter = local_values.begin(); iter != local_values.end(); iter++)
    {
        _values.insert(*iter);
    }
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
    FindCycles fc(shr3, 1uLL << 32);
    fc.find_cycles();
}
