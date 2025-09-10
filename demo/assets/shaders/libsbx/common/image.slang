#ifndef COMMON_IMAGE_SLANG_
#define COMMON_IMAGE_SLANG_

namespace sbx::common {

enum index_type : uint {
  uniform = 0,
  non_uniform = 1
}; // enum index_type

static const uint max_image_array_size = 64;

T sample_image_array<let I: index_type = index_type::non_uniform, T: ITexelElement = float4>(Texture2D<T> images[max_image_array_size], uint index, SamplerState sampler, float2 uv) {
  return (I == index_type::non_uniform) ? images[NonUniformResourceIndex(index)].Sample(sampler, uv) : images[index].Sample(sampler, uv);
}

T access_uniform_array<let I: index_type = index_type::non_uniform, T, A: IArray<T>>(A array, uint index) {
  return (I == index_type::non_uniform) ? array[NonUniformResourceIndex(index)] : array[index];
}

} // namespace sbx::common

#endif // COMMON_IMAGE_SLANG_
