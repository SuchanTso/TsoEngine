//
//  Font.hpp
//  TsoEngine
//
//  Created by SuchanTso on 2024/1/20.
//

#ifndef Font_hpp
#define Font_hpp
#include "msdf-atlas-gen.h"
#include "msdfgen.h"
#include "msdfgen-ext.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"
#include <filesystem>


namespace Tso {
struct MSDFData
{
    std::vector<msdf_atlas::GlyphGeometry> Glyphs;
    msdf_atlas::FontGeometry FontGeometry;
};
class Font{
public:
    Font(const std::filesystem::path& fontPath);
    ~Font();
    Ref<Texture2D>GetAtlasTexture(){return m_AtlasTexture;}
    const MSDFData* GetMSDFData(){return m_Data;}
private:
    MSDFData* m_Data;
    Ref<Texture2D> m_AtlasTexture;
};
}
#endif /* Font_hpp */
