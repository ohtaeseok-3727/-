"""Convert Blender renders to fixed-palette pixel sprites and review artifacts."""
import json
from pathlib import Path
from PIL import Image, ImageDraw


class CheondanPacker:
    def build(self):
        root = Path(__file__).resolve().parents[3]
        target = root / 'SimpleGame/Assets/Effects/CheondanBlender-v1'
        review = root / 'output/cheondan-blender-v1'
        reference = Image.open(root / 'SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png').convert('RGBA')
        colors = {p[:3] for p in reference.getdata() if p[3] >= 128}
        swatches = [(20, 16, 28), (89, 49, 20), (164, 95, 31), (226, 157, 58),
                    (255, 214, 120), (255, 246, 211)]
        chosen = [min(colors, key=lambda c: sum((c[k] - s[k]) ** 2 for k in range(3))) for s in swatches]
        palette = Image.new('P', (1, 1))
        palette.putpalette([v for c in chosen for v in c] + list(chosen[0]) * 250)
        atlas = Image.new('RGBA', (320 * 8, 180 * 6))
        frames = []
        for i in range(46):
            source = Image.open(review / f'raw/effect-{i + 1:04}.png').convert('RGBA')
            source = source.resize((320, 180), Image.Resampling.BOX)
            alpha = source.getchannel('A').point(lambda a: 255 if a >= 80 else 0)
            frame = source.convert('RGB').quantize(palette=palette, dither=Image.Dither.NONE).convert('RGBA')
            frame.putalpha(alpha)
            atlas.paste(frame, (i % 8 * 320, i // 8 * 180))
            bg = Image.new('RGB', frame.size, (24, 29, 40))
            draw = ImageDraw.Draw(bg)
            for x in range(0, 320, 20):
                draw.line((x, 0, x, 180), fill=(36, 42, 52))
            for y in range(0, 180, 20):
                draw.line((0, y, 320, y), fill=(36, 42, 52))
            # Review-only background displacement, not a rendered game capture.
            if 25 <= i <= 42:
                shift = round(4 * min(1, (i - 24) / 3, (43 - i) / 5))
                shifted = Image.new('RGB', bg.size, (24, 29, 40))
                for y in range(180):
                    cut = round(160 + (y - 90) * 9.8 / 5.3)
                    for x in range(320):
                        side = -1 if x < cut else 1
                        shifted.putpixel((x, y), bg.getpixel((max(0, min(319, x + side * shift)), y)))
                bg = shifted
            bg.paste(frame, (0, 0), frame)
            frames.append(bg.resize((960, 540), Image.Resampling.NEAREST))
        atlas.save(target / 'cheondan-atlas.png')
        frames[0].save(review / 'preview.gif', save_all=True, append_images=frames[1:],
                       duration=[42] * 45 + [650], loop=0)
        contact = Image.new('RGB', (960, 540), (24, 29, 40))
        for cell, index in enumerate([8, 16, 20, 26, 32, 39]):
            thumbnail = frames[index].resize((320, 180), Image.Resampling.NEAREST)
            contact.paste(thumbnail, (cell % 3 * 320, cell // 3 * 180))
        reference.thumbnail((300, 150), Image.Resampling.NEAREST)
        contact.paste(reference, (20, 380), reference)
        ImageDraw.Draw(contact).text((340, 420), 'SmallKnight palette reference / Blender effect only', fill='white')
        contact.save(review / 'review.png')
        opaque = {p[:3] for p in atlas.getdata() if p[3]}
        assert opaque <= colors
        assert set(atlas.getchannel('A').getdata()) == {0, 255}
        assert all(atlas.crop((i % 8 * 320, i // 8 * 180, i % 8 * 320 + 320,
                               i // 8 * 180 + 180)).getbbox() for i in range(1, 44))
        settings = {'frames': 46, 'fps': 24, 'cell': [320, 180], 'columns': 8,
                    'render': [640, 360], 'filter': 'nearest', 'alpha_threshold': 80,
                    'palette_reference': '../../Characters/SmallKnight/walk-transparent-v1.png',
                    'palette': chosen, 'duration_ms': 1917, 'game_integrated': False,
                    'phases': {'charge': [2, 15], 'slash': [14, 28], 'wake': [19, 31], 'fracture': [23, 44]}}
        (target / 'settings.json').write_text(json.dumps(settings, indent=2) + '\n', encoding='utf-8')
        print(f'46 frames; RGBA {atlas.size}; binary alpha; {len(opaque)} exact reference colors; no game execution')


if __name__ == '__main__':
    CheondanPacker().build()
