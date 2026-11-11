#include "stdafx.h"
#include "Chapter3Props.h"
#include "Chapter3PropData.h"

void Chapter3Props::Draw(Renderer& renderer, Chapter3Prop prop, Point foot, float height)
{
    const auto& f = Chapter3PropData::frames[static_cast<std::size_t>(prop)];
    const float width = height * (f[2] - f[0]) / (f[3] - f[1]);
    renderer.ImageQuad(
        Chapter3PropData::path,
        {foot.x - width * .5f, foot.y - height},
        {foot.x + width * .5f, foot.y - height},
        {foot.x + width * .5f, foot.y},
        {foot.x - width * .5f, foot.y},
        1.f,
        {float(f[0]) / Chapter3PropData::width, float(f[1]) / Chapter3PropData::height},
        {float(f[2]) / Chapter3PropData::width, float(f[3]) / Chapter3PropData::height});
}
