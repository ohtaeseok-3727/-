"""Read-only PNG/manifest inspection; never builds/runs the game or edits image pixels."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / '_game_resource_production/08_final/C02_RUNTIME_ASSET_READINESS.json'


def png_info(path, grid=None):
    blob = path.read_bytes()
    if blob[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError('Not a PNG')
    offset, compressed, header = 8, bytearray(), None
    while offset < len(blob):
        size = struct.unpack_from('>I', blob, offset)[0]
        kind = blob[offset + 4:offset + 8]
        data = blob[offset + 8:offset + 8 + size]
        crc = struct.unpack_from('>I', blob, offset + 8 + size)[0]
        if zlib.crc32(kind + data) & 0xffffffff != crc:
            raise ValueError('PNG CRC mismatch')
        if kind == b'IHDR':
            header = struct.unpack('>IIBBBBB', data)
        elif kind == b'IDAT':
            compressed.extend(data)
        offset += size + 12
        if kind == b'IEND':
            break
    if not header:
        raise ValueError('Missing IHDR')
    width, height, depth, color, compression, filtering, interlace = header
    result = {'width': width, 'height': height, 'bit_depth': depth, 'color_type': color,
              'sha256': hashlib.sha256(blob).hexdigest()}
    if not grid:
        return result
    columns, rows = grid
    if (depth, color, compression, filtering, interlace) != (8, 6, 0, 0, 0):
        raise ValueError('Frame inspection expects non-interlaced RGBA8 PNG')
    if width % columns or height % rows:
        raise ValueError('Image does not divide into declared cells')
    cell_width, cell_height = width // columns, height // rows
    frames = [{'transparent': 0, 'partial_alpha': 0, 'opaque': 0,
               'bounds_alpha128': [cell_width, cell_height, -1, -1]}
              for _ in range(columns * rows)]
    raw = zlib.decompress(compressed)
    stride = width * 4
    if len(raw) != (stride + 1) * height:
        raise ValueError('Unexpected decompressed image size')
    previous = bytearray(stride)
    for y in range(height):
        row_offset = y * (stride + 1)
        filter_type = raw[row_offset]
        row = bytearray(raw[row_offset + 1:row_offset + stride + 1])
        if filter_type not in range(5):
            raise ValueError('Unknown PNG filter')
        if filter_type:
            for index in range(stride):
                left = row[index - 4] if index >= 4 else 0
                up = previous[index]
                corner = previous[index - 4] if index >= 4 else 0
                if filter_type == 1:
                    predict = left
                elif filter_type == 2:
                    predict = up
                elif filter_type == 3:
                    predict = (left + up) // 2
                else:
                    p = left + up - corner
                    a, b, c = abs(p - left), abs(p - up), abs(p - corner)
                    predict = left if a <= b and a <= c else up if b <= c else corner
                row[index] = (row[index] + predict) & 255
        for x in range(width):
            alpha = row[x * 4 + 3]
            frame = frames[(y // cell_height) * columns + x // cell_width]
            key = 'transparent' if alpha == 0 else 'opaque' if alpha == 255 else 'partial_alpha'
            frame[key] += 1
            if alpha >= 128:
                bounds = frame['bounds_alpha128']
                px, py = x % cell_width, y % cell_height
                bounds[:] = [min(bounds[0], px), min(bounds[1], py), max(bounds[2], px + 1), max(bounds[3], py + 1)]
        previous = row
    result.update(cell_width=cell_width, cell_height=cell_height, frames=frames)
    result['all_frames_have_transparency'] = all(f['transparent'] > 0 for f in frames)
    result['all_frames_have_content'] = all(f['bounds_alpha128'][2] > f['bounds_alpha128'][0] for f in frames)
    result['style_palette_anatomy_approved'] = False
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, help='Optional JSON report; source assets are read-only.')
    args = parser.parse_args()
    manifest = json.loads(MANIFEST.read_text(encoding='utf-8'))
    report = {'schema': 1, 'checks': 'file existence, PNG CRC/header, declared frame alpha',
              'not_checked': ['visual style', 'exact palette', 'anatomy', 'animation', 'game execution'],
              'assets': [], 'errors': []}
    for item in manifest['assets']:
        path = ROOT / item['path']
        result = {'id': item['id'], 'path': item['path'], 'exists': path.is_file(),
                  'declared_readiness': item['readiness']}
        if path.is_file():
            try:
                result.update(png_info(path, item.get('grid')))
                if item.get('expected_size') and [result['width'], result['height']] != item['expected_size']:
                    raise ValueError('Unexpected image dimensions')
                if item.get('grid') and not (result.get('all_frames_have_transparency') and
                                              result.get('all_frames_have_content')):
                    raise ValueError('A declared frame lacks transparent pixels or visible content')
            except (ValueError, struct.error, zlib.error) as error:
                report['errors'].append({'id': item['id'], 'error': str(error)})
        elif item['readiness'] != 'MISSING':
            report['errors'].append({'id': item['id'], 'error': 'Expected existing file is absent'})
        report['assets'].append(result)
    combat_grids = {
        'Common/common-combat-v2.png': [4, 4],
        'Elite/elite-combat-v2.png': [4, 4],
        'Barkan/barkan-attacks-v2.png': [4, 3],
        'Barkan/barkan-support-v2.png': [4, 3],
    }
    for relative in manifest.get('runtime_combat_assets', []):
        path = ROOT / relative
        result = {'id': 'RUNTIME-COMBAT-' + path.stem, 'path': relative,
                  'exists': path.is_file(), 'declared_readiness': 'RUNTIME_CONNECTED',
                  'grid': combat_grids.get('/'.join(Path(relative).parts[-2:]))}
        if path.is_file():
            try:
                result.update(png_info(path, result['grid']))
                if not result['all_frames_have_transparency'] or not result['all_frames_have_content']:
                    raise ValueError('A runtime combat frame lacks transparent pixels or visible content')
            except (ValueError, struct.error, zlib.error) as error:
                report['errors'].append({'id': result['id'], 'error': str(error)})
        else:
            report['errors'].append({'id': result['id'], 'error': 'Runtime combat file is absent'})
        report['assets'].append(result)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    found = sum(item['exists'] for item in report['assets'])
    print(f"Asset audit: {found} present, {len(report['assets']) - found} declared missing, {len(report['errors'])} inspection errors.")
    print('Presence/alpha checks do not mean production-ready visual approval.')
    return 1 if report['errors'] else 0


if __name__ == '__main__':
    raise SystemExit(main())
