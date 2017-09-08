/*
 * mapInit.h
 *
 *  Created on: 25. mar. 2016
 *      Author: sisaev
 */

#ifndef MAPINIT_H_
#define MAPINIT_H_


template<typename T>
struct map_init_helper
{
    T& data;

    map_init_helper(T& d) : data(d) {}

    map_init_helper<T>& operator() (typename T::key_type const& key, typename T::mapped_type const& value)
    {
        data[key] = value;
        return *this;
    }
};

template<typename T>
map_init_helper<T> map_init(T& item)
{
    return map_init_helper<T>(item);
}



#endif /* MAPINIT_H_ */
