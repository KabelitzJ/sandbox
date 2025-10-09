#ifndef TEXTURE_SAMPLER_SLANG_
#define TEXTURE_SAMPLER_SLANG_

struct texture_sampler<T: ITexelElement> {

  private Texture2D<T> _texture;
  private SamplerState _sampler;

  __init(Texture2D<T> texture, SamplerState sampler) {
    _texture = texture;
    _sampler = sampler;
  }

  T sample(float2 uv) {
    return _texture.Sample(_sampler, uv);
  }
}; // struct texture_sampler

#endif // TEXTURE_SAMPLER_SLANG_
