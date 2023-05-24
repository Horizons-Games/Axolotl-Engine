#pragma once

#include <functional>
#include <memory>
#include <ranges>
#include <vector>

namespace axo
{
template<typename InnerView, typename FunctionArgument, typename FunctionReturn>
using transform_view = std::ranges::transform_view<InnerView, std::function<FunctionReturn(const FunctionArgument&)>>;

template<typename InnerView, typename FunctionArgument>
using filter_view = std::ranges::filter_view<InnerView, std::function<bool(const FunctionArgument&)>>;

template<typename Collection>
using ref_view = std::ranges::ref_view<Collection>;

template<typename ElementType>
using ref_vector_view = ref_view<const std::vector<ElementType>>;

template<typename PointerType>
using ref_unique_vector_view = ref_vector_view<std::unique_ptr<PointerType>>;
} // namespace axo
