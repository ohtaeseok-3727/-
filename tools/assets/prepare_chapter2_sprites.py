"""User-authorized deterministic cleanup; never redraws/resizes source artwork."""
from pathlib import Path
import hashlib
import json

import numpy as np
from PIL import Image, ImageDraw


class SpriteSheetPreparer:
    ROOT = Path(__file__).resolve().parents[2]
    SOURCE = ROOT / 'output/chapter2-orc-combat-sprites'
    DEST = ROOT / 'SimpleGame/Assets/Monsters/Orc'
    CELL = 640
    BASELINE = 512
    THRESHOLD = 240

    def components(self, rgba):
        mask = rgba[:, :, 3] >= self.THRESHOLD
        height, width = mask.shape
        result = []
        for y, x in zip(*np.where(mask)):
            if not mask[y, x]:
                continue
            stack = [(int(y), int(x))]
            mask[y, x] = False
            points = []
            while stack:
                yy, xx = stack.pop()
                points.append((xx, yy))
                for ny, nx in ((yy - 1, xx), (yy + 1, xx), (yy, xx - 1), (yy, xx + 1)):
                    if 0 <= ny < height and 0 <= nx < width and mask[ny, nx]:
                        mask[ny, nx] = False
                        stack.append((ny, nx))
            if len(points) >= 500:
                result.append(np.array(points))
        return result

    def prepare(self, name, character, actions):
        source = self.SOURCE / f'{name}-v1.png'
        rgba = np.array(Image.open(source).convert('RGBA'))
        components = self.components(rgba)
        assert len(components) == len(actions) * 4, (name, len(components))
        # Source rows overlap nominal grid boundaries. Group complete silhouettes
        # by top edge, then sort each group of four by horizontal position.
        components.sort(key=lambda p: int(p[:, 1].min()))
        ordered = []
        for row in range(len(actions)):
            ordered.extend(sorted(components[row * 4:row * 4 + 4],
                                  key=lambda p: int(p[:, 0].min())))
        atlas = np.zeros((len(actions) * self.CELL, 4 * self.CELL, 4), dtype=np.uint8)
        frames = []
        for index, points in enumerate(ordered):
            row, column = divmod(index, 4)
            low = points.min(axis=0)
            high = points.max(axis=0) + 1
            source_anchor_x = round((column + 0.5) * rgba.shape[1] / 4)
            lift = 34 if name == 'barkan-attacks' and index == 5 else 0
            offset_x = column * self.CELL + self.CELL // 2 - source_anchor_x
            offset_y = row * self.CELL + self.BASELINE - lift - int(high[1])
            x, y = points[:, 0], points[:, 1]
            dx, dy = x + offset_x, y + offset_y
            local_low = low + [offset_x - column * self.CELL, offset_y - row * self.CELL]
            local_high = high + [offset_x - column * self.CELL, offset_y - row * self.CELL]
            margins = [int(local_low[0]), int(local_low[1]),
                       self.CELL - int(local_high[0]), self.CELL - int(local_high[1])]
            assert min(margins) >= 96, (name, index, margins)
            atlas[dy, dx, :3] = rgba[y, x, :3]
            atlas[dy, dx, 3] = 255
            assert np.array_equal(atlas[dy, dx, :3], rgba[y, x, :3])
            frames.append({'index': index, 'action': actions[row],
                           'rect': [column * self.CELL, row * self.CELL, self.CELL, self.CELL],
                           'pivot': [self.CELL // 2, self.BASELINE],
                           'source_bounds': [*map(int, low), *map(int, high)],
                           'content_bounds': [*map(int, local_low), *map(int, local_high)],
                           'margins_left_top_right_bottom': margins,
                           'jump_lift_pixels': lift, 'opaque_pixels': len(points)})
        target = self.DEST / character / f'{name}-v2.png'
        target.parent.mkdir(parents=True, exist_ok=True)
        Image.fromarray(atlas).save(target)
        saved = np.array(Image.open(target).convert('RGBA'))
        assert np.array_equal(saved, atlas)
        assert set(np.unique(saved[:, :, 3])) == {0, 255}
        metadata = {'schema': 1, 'source': source.relative_to(self.ROOT).as_posix(),
                    'image': target.name, 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
                    'size': [atlas.shape[1], atlas.shape[0]], 'grid': [4, len(actions)],
                    'cell': [self.CELL, self.CELL], 'alpha_threshold': self.THRESHOLD,
                    'partial_alpha_pixels': 0, 'resized': False, 'rgb_preserved': True,
                    'minimum_margin_pixels': min(min(f['margins_left_top_right_bottom']) for f in frames),
                    'runtime_integrated': False, 'anatomy_and_animation_approved': False,
                    'frames': frames}
        target.with_suffix('.json').write_text(json.dumps(metadata, indent=2) + '\n', encoding='utf-8')
        return metadata, target

    def run(self):
        sheets = [('common-combat', 'Common', ['idle', 'swing', 'hit', 'death']),
                  ('elite-combat', 'Elite', ['idle', 'swing', 'hit', 'death']),
                  ('barkan-attacks', 'Barkan', ['swing', 'jump_slam', 'battle_cry']),
                  ('barkan-support', 'Barkan', ['idle', 'hit', 'collapse'])]
        reports = []
        for name, character, actions in sheets:
            metadata, target = self.prepare(name, character, actions)
            reports.append(metadata)
            # Inspection-only contrast background; not a runtime asset.
            sprite = Image.open(target).convert('RGBA')
            preview = Image.new('RGBA', sprite.size, '#eeeeee')
            draw = ImageDraw.Draw(preview)
            for y in range(0, sprite.height, 80):
                for x in range(0, sprite.width, 80):
                    if (x // 80 + y // 80) % 2:
                        draw.rectangle((x, y, x + 79, y + 79), fill='#8893a2')
            preview.alpha_composite(sprite)
            preview.convert('RGB').save(self.SOURCE / f'{name}-v2-inspection.png')
            print(target.relative_to(self.ROOT), 'frames:', len(metadata['frames']),
                  'minimum margin:', metadata['minimum_margin_pixels'], 'partial alpha: 0')
        (self.SOURCE / 'inspection-v2.json').write_text(json.dumps(reports, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    SpriteSheetPreparer().run()
