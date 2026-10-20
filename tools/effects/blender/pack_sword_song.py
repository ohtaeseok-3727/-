"""Quantize Blender frames to the fixed mage palette and a 32x64 pixel grid."""
from pathlib import Path
import json
from PIL import Image


class SwordSpritePacker:
    def build(self):
        root = Path(__file__).resolve().parents[3]
        target = root / 'SimpleGame/Assets/Effects/MageSwordSong'
        reference = Image.open(root / 'SimpleGame/Assets/Characters/SmallMage/walk-transparent-v2.png').convert('RGBA')
        source_colors = {rgb[:3] for rgb in reference.getdata() if rgb[3] >= 128}
        # Select exact reference colors by semantic swatches; truncating a sorted
        # true-color reference would retain only its darkest/cyan entries.
        # The entire sword is condensed blue mana, including guard and grip.
        source_colors = {rgb for rgb in source_colors
                         if rgb[2] > rgb[0] * 1.25 and rgb[1] > rgb[0] * 1.1}
        swatches = [(16, 28, 65), (24, 44, 95), (35, 68, 130),
                    (50, 95, 170), (65, 130, 210), (60, 180, 235),
                    (35, 215, 240), (110, 215, 250)]
        colors = [min(source_colors, key=lambda rgb: sum((rgb[i] - target[i]) ** 2 for i in range(3)))
                  for target in swatches]
        palette = Image.new('P', (1, 1))
        flat = [v for rgb in colors[:256] for v in rgb]
        palette.putpalette(flat + flat[:3] * ((768 - len(flat)) // 3))
        atlas = Image.new('RGBA', (32 * 16, 64))
        for index in range(16):
            frame = Image.open(root / f'output/mage-sword-song-blender-v3/raw/sword-{index + 1:04}.png').convert('RGBA')
            frame = frame.resize((32, 64), Image.Resampling.BOX)
            alpha = frame.getchannel('A').point(lambda a: 255 if a >= 96 else 0)
            frame = frame.convert('RGB').quantize(palette=palette, dither=Image.Dither.NONE).convert('RGBA')
            frame.putalpha(alpha)
            atlas.paste(frame, (index * 32, 0))
        atlas.save(target / 'sword-turntable.png')
        # Review the rendered sword beside the fixed reference without modifying it.
        preview = Image.new('RGB', (768, 320), (24, 28, 42))
        for index in range(8):
            frame = atlas.crop((index * 64, 0, index * 64 + 32, 64))
            frame = frame.resize((96, 192), Image.Resampling.NEAREST)
            preview.paste(frame, (index * 96, 16), frame)
        reference.thumbnail((192, 112), Image.Resampling.NEAREST)
        preview.paste(reference.convert('RGB'), (288, 208))
        review = root / 'output/mage-sword-song-blender-v3'
        preview.save(review / 'sword-review.png')
        reference_colors = source_colors
        opaque = {p[:3] for p in atlas.getdata() if p[3]}
        assert opaque <= reference_colors
        assert set(atlas.getchannel('A').getdata()) == {0, 255}
        assert all(atlas.crop((i * 32, 0, (i + 1) * 32, 64)).getbbox()
                   for i in range(16))
        print(f'RGBA atlas: {atlas.size}; 16 nonempty frames; {len(opaque)} reference colors; binary alpha verified')
        (target / 'settings.json').write_text(json.dumps({
            'frames': 16, 'cell': [32, 64], 'render': [96, 192],
            'palette_reference': '../../Characters/SmallMage/walk-transparent-v2.png',
            'alpha_threshold': 96, 'dither': False, 'filter': 'nearest',
            'material': 'blue mana throughout blade, guard, grip and pommel',
            'tip_in_cell': [16, 5], 'display_scale': 1.5
        }, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    SwordSpritePacker().build()
