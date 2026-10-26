"""Palette/alpha conversion and artifact preview of Blender mirror explosion."""
import json
from pathlib import Path
from PIL import Image, ImageDraw


class MirrorCheondanPacker:
    def build(self):
        root = Path(__file__).resolve().parents[3]
        target = root / 'SimpleGame/Assets/Effects/CheondanMirror-v2'
        review = root / 'output/cheondan-mirror-v2'
        reference = Image.open(root / 'SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png').convert('RGBA')
        colors = {p[:3] for p in reference.getdata() if p[3] >= 128}
        swatches = [(15, 18, 25), (29, 35, 44), (48, 57, 68), (67, 77, 88),
                    (91, 103, 115), (124, 138, 149), (168, 180, 188), (209, 217, 221),
                    (244, 245, 231), (110, 54, 16), (195, 116, 33), (248, 183, 78), (255, 233, 166)]
        chosen = list(dict.fromkeys(min(colors, key=lambda c: sum((c[k] - s[k]) ** 2 for k in range(3))) for s in swatches))
        palette = Image.new('P', (1, 1))
        flat = [v for c in chosen for v in c]
        palette.putpalette(flat + list(chosen[0]) * (256 - len(chosen)))
        atlas = Image.new('RGBA', (480 * 8, 270 * 7))
        background = Image.new('RGB', (480, 270), (23, 27, 36))
        draw = ImageDraw.Draw(background)
        for x in range(0, 480, 30):
            draw.line((x, 0, x, 270), fill=(37, 44, 56))
        for y in range(0, 270, 30):
            draw.line((0, y, 480, y), fill=(37, 44, 56))
        for x, y, w in [(45, 180, 65), (340, 150, 70)]:
            draw.rectangle((x, y, x + w, 270), fill=(51, 58, 72))
            for xx in range(x + 9, x + w, 20):
                for yy in range(y + 12, 260, 30):
                    draw.rectangle((xx, yy, xx + 7, yy + 12), fill=(120, 112, 88))
        frames, bounds = [], []
        for i in range(50):
            source = Image.open(review / f'raw/mirror-{i + 1:04}.png').convert('RGBA')
            source = source.resize((480, 270), Image.Resampling.BOX)
            alpha = source.getchannel('A').point(lambda a: 255 if a >= 100 else 0)
            frame = source.convert('RGB').quantize(palette=palette, dither=Image.Dither.NONE).convert('RGBA')
            frame.putalpha(alpha)
            bounds.append(frame.getbbox())
            atlas.paste(frame, (i % 8 * 480, i // 8 * 270))
            bg = background.copy()
            bg.paste(frame, (0, 0), frame)
            frames.append(bg.resize((960, 540), Image.Resampling.NEAREST))
        atlas.save(target / 'mirror-atlas.png')
        frames[0].save(review / 'preview-direct.gif', save_all=True, append_images=frames[1:],
                       duration=[42] * 49 + [500], loop=0, disposal=2)
        contact = Image.new('RGB', (1280, 670), (23, 27, 36))
        stages = [9, 12, 15, 18, 22, 23, 24, 25, 28, 32, 38, 44]
        for cell, index in enumerate(stages):
            image = frames[index].resize((320, 180), Image.Resampling.NEAREST)
            ImageDraw.Draw(image).text((8, 8), f'Frame {index + 1}', fill=(245, 228, 185))
            contact.paste(image, (cell % 4 * 320, cell // 4 * 180))
        reference.thumbnail((300, 115), Image.Resampling.NEAREST)
        contact.paste(reference, (12, 548), reference)
        ImageDraw.Draw(contact).text((340, 590), 'SmallKnight reference palette / all shard motion rendered in Blender', fill='white')
        contact.save(review / 'review.png')
        opaque = {p[:3] for p in atlas.getdata() if p[3]}
        assert opaque <= colors
        assert set(atlas.getchannel('A').getdata()) == {0, 255}
        assert all(bounds[i] for i in range(2, 35)), 'Missing active phase'
        # First shard frame must have substantial open background, not a full plate.
        first = atlas.crop((0, 3 * 270, 480, 4 * 270))
        coverage = sum(a > 0 for a in first.getchannel('A').getdata()) / (480 * 270)
        assert coverage < .8, f'First explosion frame still covers screen: {coverage}'
        settings = {'frames': 50, 'fps': 24, 'cell': [480, 270], 'columns': 8,
                    'render': [960, 540], 'alpha_threshold': 100, 'filter': 'nearest',
                    'palette_reference': '../../Characters/SmallKnight/walk-transparent-v1.png',
                    'palette': chosen, 'game_integrated': False,
                    'phases': {'charge': [3, 10], 'sweep_and_matching_scar': [11, 24],
                               'explosion': [25, 45]}, 'mirror_hold': False,
                    'first_shard_frame_coverage': coverage}
        (target / 'settings.json').write_text(json.dumps(settings, indent=2) + '\n', encoding='utf-8')
        print(f'50 Blender frames; {len(opaque)} reference colors; binary alpha; first shard coverage {coverage:.1%}; atlas {atlas.size}')


if __name__ == '__main__':
    MirrorCheondanPacker().build()
