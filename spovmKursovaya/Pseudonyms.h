#pragma once
#include <type_traits>
template<class data_t>
using is_floating =
typename std::enable_if_t<std::is_floating_point_v<data_t>>;

template<class data_t>
using is_arithm =
std::enable_if_t<std::is_arithmetic_v<data_t>>;