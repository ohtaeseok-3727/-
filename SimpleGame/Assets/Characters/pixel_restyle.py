"""Describe generated sheets without changing their pixels; replace body clips only."""
import json
from pathlib import Path


class PixelRestyle:
    def __init__(self, root):
        self.root = Path(root)
        self.manifest = json.loads(
            (self.root / 'PixelRestyle-v2/frames.json').read_text(encoding='utf-8'))

    def apply(self, assets, clips):
        indices = {}
        for name, sheet in self.manifest['sheets'].items():
            indices[name] = len(assets)
            assets.append(dict(path=sheet['path'], width=sheet['size'][0],
                               height=sheet['size'][1]))

        for clip in clips:
            profession = clip['id'].split('-')[0]
            if profession not in ('knight', 'mage'):
                continue
            walking = clip['id'].endswith('-walk')
            sheet_name = profession
            sheet = self.manifest['sheets'][sheet_name]
            row = 0 if walking else int(clip['id'][-1])
            frames = sheet['frames'][row * 4:row * 4 + 4]
            # Preserve all original timing, hit logic and independent effect clips.
            # Windup/contact/recovery phases map to the new four authored poses.
            count = len(clip['body'])
            if walking:
                sequence = [i % 4 for i in range(count)]
            elif profession == 'knight':
                sequence = [0, 0, 1, 1, 2, 2, 2, 2, 3, 3]
            else:
                sequence = ([0, 0, 1, 2, 2, 3, 3] if count == 7
                            else [0, 0, 1, 1, 2, 2, 3, 3])
            assert len(sequence) == count
            for body, pose in zip(clip['body'], sequence):
                source = frames[pose]
                scale = source['scale']
                body['sprite'] = dict(asset=indices[sheet_name], rect=source['rect'],
                                      offset=[-source['pivot'][0] * scale,
                                              -source['pivot'][1] * scale],
                                      scale=scale, blend=0)
            # Cyan gemstones are mounted on the mage's gloves. Follow the forward
            # glove in source pixels, rather than reusing old hand coordinates.
            if not walking:
                clip['anchors'] = []
                for pose in sequence:
                    if profession == 'knight':
                        clip['anchors'].append([0, 0])
                        continue
                    source = frames[pose]
                    hand, pivot, scale = source['hand'], source['pivot'], source['scale']
                    dx = (hand[0] - pivot[0]) * scale
                    dy = (hand[1] - pivot[1]) * scale
                    if clip['id'] == 'mage-1':
                        dx += 20
                        dy -= 5
                    clip['anchors'].append([dx, dy])


class SheetInspector:
    """Find empty separators and foot pivots, leaving original RGBA files intact."""
    def __init__(self, root):
        self.root = Path(root)

    @staticmethod
    def separators(counts, divisions):
        length = len(counts)
        edges = [0]
        for index in range(1, divisions):
            expected = length * index / divisions
            radius = length / divisions * .36
            candidates = [p for p in range(max(edges[-1] + 1, int(expected - radius)),
                                           min(length, int(expected + radius)))
                          if counts[p] == 0]
            if not candidates:
                raise ValueError(f'No transparent separator near {expected}')
            # Choose midpoint of the empty run nearest the expected boundary.
            chosen = min(candidates, key=lambda p: abs(p - expected))
            left = right = chosen
            while left > edges[-1] and counts[left - 1] == 0:
                left -= 1
            while right + 1 < length and counts[right + 1] == 0:
                right += 1
            edges.append((left + right) // 2)
        return edges + [length]

    def inspect(self, path, rows):
        import numpy as np
        from PIL import Image
        image = Image.open(self.root / path)
        assert image.mode == 'RGBA', path
        rgba = np.array(image)
        alpha = rgba[:, :, 3]
        assert alpha.min() == 0 and alpha.max() == 255
        # Ignore subpixel alpha fringe when locating visual silhouettes, as the
        # existing chapter loaders do. Preserve the actual RGBA source unchanged.
        mask = alpha > 128
        ys = self.separators(mask.sum(axis=1), rows)
        frames = []
        for row in range(rows):
            xs = self.separators(mask[ys[row]:ys[row + 1]].sum(axis=0), 4)
            for col in range(4):
                x0, x1, y0, y1 = xs[col], xs[col + 1], ys[row], ys[row + 1]
                local = mask[y0:y1, x0:x1]
                yy, xx = np.where(local)
                assert len(xx) > 1000
                left, right = int(xx.min()), int(xx.max()) + 1
                top, bottom = int(yy.min()), int(yy.max()) + 1
                feet = xx[yy >= bottom - max(8, (bottom - top) // 9)]
                pivot_x = float((feet.min() + feet.max() + 1) / 2)
                frames.append(dict(rect=[x0, y0, x1 - x0, y1 - y0],
                                   bounds=[left, top, right, bottom],
                                   pivot=[pivot_x, bottom],
                                   opaque_pixels=int(len(xx))))
                if path.endswith('mage-v2.png'):
                    colors = rgba[y0:y1, x0:x1, :3].astype(float)
                    cyan = ((colors[:, :, 1] > 100) & (colors[:, :, 2] > 110)
                            & (colors[:, :, 0] < colors[:, :, 1] * .8)
                            & (colors[:, :, 2] > colors[:, :, 0] * 1.25) & local)
                    gem_y, gem_x = np.where(cyan)
                    assert len(gem_x) > 6, (path, row, col, 'glove gem missing')
                    forward = gem_x >= gem_x.max() - 16
                    frames[-1]['hand'] = [float(np.mean(gem_x[forward])),
                                          float(np.mean(gem_y[forward]))]
        # Normalize each row against its standing recovery pose. This removes
        # generation size drift without cancelling crouch/extension within a row.
        upright = frames[3::4]
        height = float(np.median([f['bounds'][3] - f['bounds'][1] for f in upright]))
        for row in range(rows):
            reference = upright[row]['bounds']
            scale = 150.0 / (reference[3] - reference[1])
            for frame in frames[row * 4:row * 4 + 4]:
                frame['scale'] = scale
        return dict(path=path, size=list(image.size), frames=frames,
                    scale=150.0 / height,
                    transparent_pixels=int((alpha == 0).sum()),
                    partial_alpha_pixels=int(((alpha > 0) & (alpha < 255)).sum()),
                    silhouette_alpha_threshold=128,
                    pixels_modified=False)

    def write(self):
        sheets = {name: self.inspect(path, rows) for name, path, rows in [
            ('knight', 'PixelRestyle-v2/knight-v2.png', 4),
            ('mage', 'PixelRestyle-v2/mage-v2.png', 4),
            ('hero', 'NPC/Hero/PixelRestyle-v2/hero-v2.png', 4)]}
        target = self.root / 'PixelRestyle-v2/frames.json'
        target.write_text(json.dumps(dict(version=1, sheets=sheets),
                                     ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        print('Inspected 48 frames; source PNG pixels unchanged.')
        hero = sheets['hero']
        print('Hero size:', hero['size'])
        regions = []
        for index in (8, 5, 6):
            f = hero['frames'][index]
            x, y, _, _ = f['rect']
            l, t, r, b = f['bounds']
            regions.append([x + l, y + t, x + r, y + b])
        header = [
            '#pragma once',
            '// Generated by Assets/Characters/pixel_restyle.py. Do not edit.',
            'namespace PixelRestyleHero {',
            'static constexpr wchar_t path[] = L"' + hero['path'] + '";',
            f'static constexpr int width = {hero["size"][0]};',
            f'static constexpr int height = {hero["size"][1]};',
            'static constexpr int regions[][4] = {',
            *['{' + ','.join(map(str, region)) + '},' for region in regions],
            '};', '}']
        destination = self.root.parents[1] / 'src/generated/PixelRestyleHeroData.h'
        header += ['namespace PixelRestyleKnight {',
                   'struct Hilt { const wchar_t* path; int x,y; float rootX,rootY,angle; };',
                   'static constexpr Hilt hilts[] = {']
        anchors = json.loads((self.root / 'PixelRestyle-v2/sword-anchors.json')
                             .read_text(encoding='utf-8'))
        assert anchors['size'] == sheets['knight']['size']
        assert len(anchors['frames']) == len(sheets['knight']['frames'])
        for index, frame in enumerate(sheets['knight']['frames']):
            anchor = anchors['frames'][index]
            x, y = frame['rect'][:2]
            px, py = anchor['root']
            angle = anchor['angle']
            assert 0 <= px < frame['rect'][2] and 0 <= py < frame['rect'][3]
            header.append('{L"%s",%d,%d,%.6ff,%.6ff,%.6ff},' %
                          (sheets['knight']['path'], x, y, px, py, angle))
        header += ['};', '}']
        destination.write_text('\n'.join(header) + '\n', encoding='utf-8')


if __name__ == '__main__':
    SheetInspector(Path(__file__).resolve().parent).write()
