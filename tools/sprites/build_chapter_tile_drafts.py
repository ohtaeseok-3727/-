"""Generate original pixel-art tiles and reproducible placement previews using Pillow."""

import csv
import json
import math
from pathlib import Path
from random import Random

from PIL import Image, ImageDraw, ImageFont

from chapter_tile_draft_data import CHAPTERS, DECALS, MATERIALS, ROUTES


SIZE = 32
WIDTH, HEIGHT = 18, 10
DIRECTIONS = ((0, -1, 1), (1, 0, 2), (0, 1, 4), (-1, 0, 8))
ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / 'SimpleGame/Assets/Environment/Tiles/ChapterDraft-v1'
INK, PAPER, MUTED, GOLD = '#151d25', '#e9e4d6', '#aebcc0', '#e6bd77'


def rgb(value):
    return tuple(bytes.fromhex(value))


def shade(color, offset):
    return tuple(max(0, min(255, channel + offset)) for channel in color)


def surface(key, variation):
    kind, base, dark, light, accent = MATERIALS[key]
    base, dark, light, accent = map(rgb, (base, dark, light, accent))
    seed = sum((i + 1) * ord(c) for i, c in enumerate(key))
    tile = Image.new('RGBA', (SIZE, SIZE), base)
    draw = ImageDraw.Draw(tile)
    for y in range(SIZE):
        for x in range(SIZE):
            index = (x * 17 + y * 29 + x * y * 3 + seed) % 31
            tone = (index % 5 - 2) * 2
            if kind in {'stone', 'slab', 'metal'}:
                step = 8 if kind == 'stone' else 16
                block = (x + (step // 2 if y // step % 2 else 0)) % step
                edge = y % step
                if block == 0 or edge == 0:
                    color = dark
                elif block == 1 or edge == 1:
                    color = shade(light, -12)
                else:
                    tone += ((x // step + 3 * (y // step)) % 3 - 1) * 6
                    color = shade(base, tone)
            elif kind == 'wood':
                edge = y % 8
                color = dark if edge == 0 else light if edge == 1 else shade(base, tone)
            elif kind == 'furrow':
                color = dark if x % 8 in (0, 1) else shade(base, 11 if x % 8 == 2 else tone)
            elif kind == 'water':
                wave = (y + (0, 1, 1, 0, 0, -1, -1, 0)[x // 4]) % 8
                color = shade(base, (2, 10, 5, 0, -4, -6, -3, 0)[wave] + tone // 2)
            elif kind == 'rift':
                wall = min(x, SIZE - 1 - x)
                offset = (0, 1, 0, -1)[(y // 8) % 4]
                color = shade(light, -5 + tone) if wall < 2 + offset else shade(dark, tone + wall // 3)
            else:
                color = shade(base, tone)
            draw.point((x, y), fill=color)

    rng = Random(seed + variation * 911)
    # Interior details vary, keeping the outside four pixels invariant for this material.
    for _ in range(16 if kind in {'grass', 'rock', 'grit', 'water'} else 7):
        x, y = rng.randrange(4, 24), rng.randrange(5, 25)
        if kind == 'grass':
            draw.line((x - 1, y + 2, x + 2, y + 2), fill=dark)
            draw.line((x, y + 1, x - 1, y - 1), fill=shade(light, -9))
            draw.line((x + 1, y + 1, x + 2, y), fill=light)
            if rng.randrange(7) == 0:
                draw.point((x + 1, y - 1), fill=accent)
        elif kind == 'water':
            length = rng.randrange(2, 6)
            draw.line((x, y, x + length, y), fill=shade(light, -13))
            if rng.randrange(4) == 0:
                draw.point((x + 1, y - 1), fill=accent)
        elif kind == 'rock':
            length = rng.randrange(2, 5)
            draw.polygon(((x, y), (x + 2, y - 2), (x + length, y - 1), (x + length, y + 1), (x + 1, y + 2)), fill=dark)
            draw.line((x, y, x + 2, y - 2, x + length - 1, y - 1), fill=shade(light, -12))
        elif kind == 'metal':
            draw.point((x, y), fill=accent)
            draw.point((x + 1, y + 1), fill=dark)
        else:
            draw.line((x, y, x + 1, y, x + 2, y + 1), fill=shade(base, 16))
            draw.point((x + 1, y + 1), fill=shade(base, -16))
    return tile


def route_tile(texture, key, mask):
    tile = Image.new('RGBA', (SIZE, SIZE))
    texture_px, pixels = texture.load(), tile.load()
    dark, light = rgb(MATERIALS[key][2]), rgb(MATERIALS[key][3])
    for y in range(SIZE):
        for x in range(SIZE):
            dx, dy = x - 15.5, y - 15.5
            distances = [math.hypot(dx, dy)]
            for ox, oy, bit in DIRECTIONS:
                if mask & bit and dx * ox + dy * oy >= 0:
                    distances.append(abs(dx * oy - dy * ox))
            distance = min(distances)
            if distance > 10:
                continue
            if distance > 9:
                color = dark
            elif distance > 8 and key != 'earth':
                color = shade(light, -6)
            else:
                color = texture_px[x, y][:3]
            pixels[x, y] = (*color, 255)
    return tile


def shore_tile(mask):
    tile = Image.new('RGBA', (SIZE, SIZE))
    draw = ImageDraw.Draw(tile)
    for y in range(SIZE):
        for x in range(SIZE):
            distances = (y, 31 - x, 31 - y, x)
            distance = min(d for d, (_, _, bit) in zip(distances, DIRECTIONS) if mask & bit)
            if distance == 0:
                draw.point((x, y), fill='#3d4d44')
            elif distance == 1:
                draw.point((x, y), fill='#718270')
            elif distance == 3 and (x + y) % 8 < 4:
                draw.point((x, y), fill=(124, 170, 161, 170))
    return tile


def decal(key):
    tile = Image.new('RGBA', (SIZE, SIZE))
    d = ImageDraw.Draw(tile)
    kind, color = DECALS[key]
    light = rgb(color)
    shadow = shade(light, -39)
    if kind == 'plant':
        d.ellipse((4, 19, 29, 29), fill=(19, 29, 22, 132))
        if key == 'pine':
            d.rectangle((14, 21, 17, 27), fill='#685239')
            for y, span in ((17, 12), (12, 9), (7, 6)):
                d.polygon(((15, y - 6), (15 - span, y + 5), (15 + span, y + 5)), fill=shadow)
                d.polygon(((15, y - 6), (15 - span, y + 3), (15, y + 4)), fill=light)
        else:
            for x, y in ((9, 13), (16, 9), (24, 15), (24, 23), (8, 22)):
                d.line((15, 25, x, y), fill=shadow, width=3)
                d.line((15, 24, x, y), fill=light)
                d.rectangle((x - 2, y, x, y + 2), fill=shade(light, -16))
    elif kind == 'debris':
        for x, y in ((6, 22), (15, 12), (23, 21)):
            d.polygon(((x - 3, y), (x, y - 4), (x + 4, y - 3), (x + 5, y + 2), (x, y + 4)), fill=shadow)
            d.polygon(((x - 3, y), (x, y - 4), (x + 4, y - 3), (x + 2, y)), fill=light)
    elif kind == 'crop':
        for x in (7, 15, 23):
            for y in (8, 17, 26):
                d.line((x, y, x, y - 5), fill=shadow)
                d.line((x - 2, y - 4, x, y - 2, x + 2, y - 5), fill=light)
    elif kind == 'crack':
        path = ((3, 25), (10, 19), (9, 15), (17, 15), (20, 7), (28, 4))
        d.line(path, fill=(23, 19, 31, 230), width=4)
        d.line(path, fill=light)
        d.line((17, 15, 20, 19, 27, 20), fill=shadow)
    elif kind == 'wave':
        for y in (9, 15, 22):
            d.line((7, y, 12, y, 15, y - 2, 20, y - 2, 24, y), fill=light)
    elif kind == 'rail':
        for y in (4, 12, 20, 28):
            d.rectangle((6, y, 25, y + 2), fill='#574537')
        for x in (10, 21):
            d.line((x, 0, x, 31), fill=light, width=2)
    else:
        d.polygon(((16, 3), (28, 16), (16, 28), (4, 16)), outline=shadow, width=2)
        d.line((8, 16, 16, 8, 24, 16, 16, 24, 8, 16), fill=light)
        if kind == 'eye':
            d.ellipse((12, 12, 19, 19), fill=light)
            d.point((16, 15), fill='#302942')
        elif key == 'claim':
            d.polygon(((10, 9), (22, 9), (22, 20), (16, 24), (10, 20)), fill=shadow, outline=light)
            d.line((13, 12, 16, 18, 19, 12), fill=light, width=2)
        elif key == 'death':
            d.rectangle((13, 11, 19, 19), fill=light)
            d.point((14, 15), fill=shadow)
            d.point((18, 15), fill=shadow)
            d.line((15, 21, 17, 21), fill=light, width=2)
        elif key == 'sealed':
            d.rectangle((11, 12, 21, 20), outline=light)
            d.line((13, 15, 19, 15), fill=light, width=2)
            d.line((13, 18, 19, 18), fill=light)
        else:
            d.line((16, 10, 16, 22), fill=light, width=2)
            d.line((12, 15, 20, 15), fill=light)
    return tile


class Atlas:
    def __init__(self):
        self.tiles = [Image.new('RGBA', (SIZE, SIZE))]
        self.info = [{'id': 0, 'layer': 'empty', 'key': 'empty'}]
        self.ids = {'empty': 0}

    def add(self, key, layer, tile, **details):
        tile_id = len(self.tiles)
        self.ids[key] = tile_id
        self.tiles.append(tile)
        self.info.append(dict(id=tile_id, key=key, layer=layer, **details))

    def save(self, path):
        cols = 16
        atlas = Image.new('RGBA', (cols * SIZE, math.ceil(len(self.tiles) / cols) * SIZE))
        for tile_id, tile in enumerate(self.tiles):
            x, y = (tile_id % cols) * SIZE, (tile_id // cols) * SIZE
            atlas.paste(tile, (x, y))
            self.info[tile_id]['rect'] = [x, y, SIZE, SIZE]
        atlas.save(path)


def course(layout):
    masks = {}
    for path in ROUTES[layout]:
        for (x, y), (tx, ty) in zip(path, path[1:]):
            assert x == tx or y == ty
            dx, dy = (tx > x) - (tx < x), (ty > y) - (ty < y)
            bit = next(bit for ox, oy, bit in DIRECTIONS if (ox, oy) == (dx, dy))
            opposite = ((bit << 2) | (bit >> 2)) & 15
            while (x, y) != (tx, ty):
                masks[x, y] = masks.get((x, y), 0) | bit
                x, y = x + dx, y + dy
                masks[x, y] = masks.get((x, y), 0) | opposite
        for x, y in (path[0], path[-1]):
            masks[x, y] |= 8 if x == 0 else 2 if x == WIDTH - 1 else 0
    return masks


def landscape(layout, x, y, materials):
    a, b, c, d = materials
    river_x = 7 + round(math.sin(y * .5) * 1.4)
    previous_x = 7 + round(math.sin(max(0, y - 1) * .5) * 1.4)
    river_left, river_right = sorted((river_x, previous_x))
    if layout in {'village', 'forest'}:
        if river_left <= x <= river_right + (layout == 'forest'):
            return 'river'
        if layout == 'village':
            if x < 5:
                return c if y < 5 and x > 0 else a
            return d if x >= 14 and y <= 3 else b
        return d if x >= 12 and y <= 5 else c if x >= 10 else a if x < 4 and y >= 4 else b
    if layout in {'canyon', 'ridge', 'evac'}:
        if layout == 'evac' and river_left <= x <= river_right + 2:
            return 'river'
        if layout == 'canyon' and x >= 12 and y <= 4:
            return d
        if layout == 'ridge' and ((x <= 4 and y <= 3) or (x >= 13 and y >= 6)):
            return d if x < 8 else c
        return b if y < 2 + int(1.4 * math.sin(x * .46)) or y >= 8 else a
    if layout == 'city':
        return b if 4 <= x <= 10 and 3 <= y <= 7 else c if x >= 13 else a
    if layout == 'accord':
        return 'flood' if x >= 11 and y >= 5 else b if 4 <= x < 10 and y < 5 else a
    if layout == 'royal':
        return d if x >= 9 else b if (x - 3) ** 2 + (y - 3) ** 2 < 10 else a
    if layout == 'divide':
        return 'rift' if x in (8, 9) else a
    if layout == 'front':
        return d if 7 <= x <= 10 else a if x < 7 else c
    if layout in {'ring', 'gate', 'tomb'}:
        return a if 5 <= x <= 13 and 2 <= y <= 8 else b if 3 <= x <= 15 else c
    if layout == 'war':
        return c if 5 <= x <= 11 and 3 <= y <= 7 else a
    return b if (y in (0, 9) or x in (0, 17)) else a if x < 12 else d


def place_tiles(atlas, chapter_id, chapter):
    _, _, palette, road, decals, layout = chapter
    masks = course(layout)
    terrain = [[landscape(layout, x, y, palette.split()) for x in range(WIDTH)] for y in range(HEIGHT)]
    rng = Random(501 + chapter_id)
    ground, shore, paths, marks = [], [], [], []
    for y, row in enumerate(terrain):
        ground_row, shore_row, path_row, mark_row = [], [], [], []
        for x, key in enumerate(row):
            wet = key in {'river', 'flood'}
            mask = masks.get((x, y), 0)
            ground_row.append(atlas.ids[f'{key}-{rng.randrange(3)}'])
            bank = sum(bit for ox, oy, bit in DIRECTIONS
                       if wet and 0 <= x + ox < WIDTH and 0 <= y + oy < HEIGHT
                       and terrain[y + oy][x + ox] != key)
            shore_row.append(atlas.ids[f'shore-{bank}'] if bank else 0)
            material = road
            if wet and layout in {'village', 'forest', 'evac'}:
                material = 'timber'
            elif wet and layout == 'accord':
                material = 'channel'
            elif layout == 'canyon' and key == 'canyon':
                material = 'earth'
            path_row.append(atlas.ids[f'route-{material}-{mask}'] if mask else 0)
            mark = None
            if not mask:
                if key == 'field' and (x + y) % 2 == 0:
                    mark = 'crop'
                elif key == 'woodland' and rng.random() < .39:
                    mark = 'pine' if rng.random() < .65 else 'fern'
                elif key in {'moss', 'meadow'} and rng.random() < .18:
                    mark = 'fern'
                elif key == 'rift':
                    if (x + y) % 2 == 0:
                        mark = 'vein'
                elif wet:
                    if layout in {'accord', 'evac'} and (x + y) % 4 == 0:
                        mark = 'silt'
                elif layout == 'village' and key == 'mine' and x == 15:
                    mark = 'track'
                elif ((x in {4, 6, 9, 12, 15} and y in {2, 4, 7}) or rng.random() < .08):
                    options = [name for name in decals.split() if name not in {'crop', 'fern', 'pine', 'track', 'silt'}]
                    if options:
                        mark = options[(x + y) % len(options)]
            elif layout in {'gate', 'ring', 'channels', 'tomb', 'strongholds'} and mask.bit_count() >= 3:
                mark = decals.split()[0]
            mark_row.append(atlas.ids[f'decal-{mark}'] if mark else 0)
        ground.append(ground_row)
        shore.append(shore_row)
        paths.append(path_row)
        marks.append(mark_row)
    return dict(ground=ground, bank=shore, road=paths, mark=marks)


def map_image(atlas, layers):
    tile_height = round(SIZE * math.sin(math.pi / 3))
    world = Image.new('RGBA', (WIDTH * SIZE, HEIGHT * tile_height))
    projected = {}
    for grid in layers.values():
        for y, row in enumerate(grid):
            for x, tile_id in enumerate(row):
                if tile_id:
                    if tile_id not in projected:
                        projected[tile_id] = atlas.tiles[tile_id].resize((SIZE, tile_height), Image.Resampling.NEAREST)
                    world.alpha_composite(projected[tile_id], (x * SIZE, y * tile_height))
    assert world.getchannel('A').getextrema() == (255, 255)
    return world


def font(size, bold=False):
    filename = 'malgunbd.ttf' if bold else 'malgun.ttf'
    return ImageFont.truetype(str(Path('C:/Windows/Fonts') / filename), size)


def chapter_card(atlas, world, chapter, layers, index):
    title, caption, palette, road, decals, _ = chapter
    card = Image.new('RGB', (624, 404), '#222e36')
    d = ImageDraw.Draw(card)
    d.rectangle((0, 0, 623, 403), outline='#48575d')
    d.text((23, 14), f'{index:02d}', font=font(25, True), fill=GOLD)
    d.text((75, 15), title, font=font(23, True), fill=PAPER)
    d.text((25, 49), caption, font=font(14), fill=MUTED)
    card.paste(world.convert('RGB'), (24, 75))
    used_ground = {tile_id for row in layers['ground'] for tile_id in row}
    materials = sorted({atlas.info[i]['material'] for i in used_ground})
    ids = [atlas.ids[f'{m}-0'] for m in materials]
    ids.extend(atlas.ids[f'route-{road}-{mask}'] for mask in (10, 3, 15))
    used_roads = {atlas.info[tile_id]['material'] for row in layers['road'] for tile_id in row if tile_id}
    ids.extend(atlas.ids[f'route-{key}-10'] for key in sorted(used_roads - {road}))
    ids.extend(atlas.ids[f'decal-{name}'] for name in decals.split())
    for i, tile_id in enumerate(ids):
        x = 24 + i * 39
        d.rectangle((x - 1, 365, x + SIZE, 398), fill='#11191f', outline='#556064')
        card.paste(atlas.tiles[tile_id], (x, 366), atlas.tiles[tile_id])
    return card


def assert_tiles(atlas):
    for key in MATERIALS:
        base = atlas.tiles[atlas.ids[f'{key}-0']]
        for i in (1, 2):
            tile = atlas.tiles[atlas.ids[f'{key}-{i}']]
            for box in ((0, 0, 32, 3), (0, 29, 32, 32), (0, 0, 3, 32), (29, 0, 32, 32)):
                assert tile.crop(box).tobytes() == base.crop(box).tobytes(), key
    for record in atlas.info:
        if record['layer'] != 'road':
            continue
        tile = atlas.tiles[record['id']].getchannel('A')
        assert tile.getextrema() == (0, 255)
        for bit, edge in ((1, (0, 0, 32, 1)), (2, (31, 0, 32, 32)), (4, (0, 31, 32, 32)), (8, (0, 0, 1, 32))):
            expected = bytes(255 if record['mask'] & bit and 6 <= i <= 25 else 0 for i in range(32))
            assert tile.crop(edge).tobytes() == expected, (record, bit)


def write_preview_html(chapters, variants):
    rows = []
    for item in chapters:
        index, title = item['chapter'], item['title']
        prefix = f'c{index:02}'
        rows.append(f'<article><img src="preview/{prefix}.png" alt="{title} 배치 시안" loading="lazy">'
                    f'<nav><a href="maps/{prefix}-ground.csv">바닥</a> · <a href="maps/{prefix}-road.csv">도로</a> · '
                    f'<a href="maps/{prefix}-bank.csv">물가</a> · <a href="maps/{prefix}-mark.csv">장식·상태</a></nav></article>')
    for item in variants:
        rows.append(f'<article><img src="{item["preview"]}" alt="중심문 귀환 상태" loading="lazy">'
                    f'<nav><a href="{item["layers"]["mark"]}">챕터 18 봉인 상태</a> · '
                    f'<a href="{item["layers"]["ground"]}">챕터 14 바닥 공유</a></nav></article>')
    html = '''<!doctype html><html lang="ko"><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1"><title>챕터 1~18 타일 배치 시안</title>
<style>body{background:#151d25;color:#e9e4d6;font:16px Malgun Gothic,sans-serif;margin:0}main{max-width:1320px;margin:auto;padding:32px 24px}h1{margin:0 0 8px}p{color:#aebcc0}section{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,520px),1fr));gap:20px}article{min-width:0}img{display:block;width:100%;image-rendering:pixelated}nav{background:#222e36;padding:10px 18px;border:1px solid #48575d}a{color:#e6bd77}</style>
<main><h1>챕터 1~18 타일 배치 시안</h1><p>각 이미지는 아래 숫자 격자 네 장을 합친 대표 구역입니다. 32px 타일 원본을 기존 60도 지면 구도에 맞춰 표시했습니다.</p>
<p><a href="tiles.png">PNG 타일 아틀라스</a> · <a href="tiles.json">타일 ID 표</a></p><section>'''
    (OUTPUT / 'preview.html').write_text(html + '\n'.join(rows) + '</section></main></html>\n', encoding='utf-8')


def return_variant(atlas, base, paths):
    marks = [row.copy() for row in base['mark']]
    for x, y in ((6, 2), (12, 2), (9, 8), (6, 5), (9, 5), (12, 5)):
        marks[y][x] = atlas.ids['decal-sealed']
    layers = dict(base, mark=marks)
    mark_path = 'maps/c18-return-mark.csv'
    preview_path = 'preview/c18-return.png'
    with (OUTPUT / mark_path).open('w', newline='', encoding='utf-8') as stream:
        csv.writer(stream, lineterminator='\n').writerows(marks)
    theme = ('중심문 귀환', '챕터 14의 바닥과 도로에 최종 봉인 표식을 배치',
             CHAPTERS[13][2], CHAPTERS[13][3], 'sealed vein', 'gate')
    chapter_card(atlas, map_image(atlas, layers), theme, layers, 18).save(OUTPUT / preview_path)
    return dict(chapter=18, base_chapter=14, preview=preview_path, layers=dict(paths, mark=mark_path))


def main():
    for folder in ('maps', 'preview'):
        (OUTPUT / folder).mkdir(parents=True, exist_ok=True)
    atlas = Atlas()
    for key in MATERIALS:
        for i in range(3):
            atlas.add(f'{key}-{i}', 'ground', surface(key, i), material=key, variation=i)
    for key in sorted({chapter[3] for chapter in CHAPTERS} | {'timber'}):
        for mask in range(1, 16):
            atlas.add(f'route-{key}-{mask}', 'road', route_tile(surface(key, 0), key, mask), material=key, mask=mask)
    for mask in range(1, 16):
        atlas.add(f'shore-{mask}', 'bank', shore_tile(mask), mask=mask)
    for key in DECALS:
        atlas.add(f'decal-{key}', 'mark', decal(key))
    assert_tiles(atlas)
    atlas.save(OUTPUT / 'tiles.png')

    cards, chapters, variants = [], [], []
    for index, chapter in enumerate(CHAPTERS, 1):
        layers = place_tiles(atlas, index, chapter)
        for name, grid in layers.items():
            with (OUTPUT / 'maps' / f'c{index:02}-{name}.csv').open('w', newline='', encoding='utf-8') as stream:
                csv.writer(stream, lineterminator='\n').writerows(grid)
        card = chapter_card(atlas, map_image(atlas, layers), chapter, layers, index)
        card.save(OUTPUT / 'preview' / f'c{index:02}.png')
        cards.append(card)
        chapters.append(dict(chapter=index, title=chapter[0], caption=chapter[1], width=WIDTH, height=HEIGHT,
                             layers={name: f'maps/c{index:02}-{name}.csv' for name in layers}))
        if index == 14:
            variants.append(return_variant(atlas, layers, chapters[-1]['layers']))

    for offset in (0, 6, 12):
        overview = Image.new('RGB', (1312, 1384), INK)
        d = ImageDraw.Draw(overview)
        d.text((24, 22), '오픈월드 타일 배치 시안', font=font(37, True), fill=PAPER)
        d.text((26, 76), f'챕터 {offset + 1:02d}–{offset + 6:02d}   ·   32px 원본 타일   /   바닥 · 도로 · 물가 · 상태', font=font(17), fill=MUTED)
        for i, card in enumerate(cards[offset:offset + 6]):
            overview.paste(card, (24 + (i % 2) * 640, 123 + (i // 2) * 420))
        overview.save(OUTPUT / 'preview' / f'chapters-{offset + 1:02}-{offset + 6:02}.png')

    manifest = dict(tile_pixels=SIZE, direction_bits={'north': 1, 'east': 2, 'south': 4, 'west': 8},
                    layers=['ground', 'bank', 'road', 'mark'], atlas='tiles.png', tiles=atlas.info,
                    chapters=chapters, variants=variants)
    (OUTPUT / 'tiles.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    write_preview_html(chapters, variants)
    print(f'{len(chapters)} chapters; {len(atlas.tiles) - 1} tiles; {len(chapters) * 4 + len(variants)} CSV layers')


if __name__ == '__main__':
    main()
