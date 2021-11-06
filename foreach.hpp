#pragma once
#ifndef MTL_FOREACH_HPP
#define MTL_FOREACH_HPP

#define MTL_FOREACH(index_name, size) \
    for( std::size_t index_name(0); index_name < size; index_name++)

#endif