"""Create a transparent, aligned review sheet from idle-draft-v1.png.

The source contains a baked neutral checkerboard.  Only neutral pixels that
are connected to a sheet edge are removed, so neutral gray hair and armor
inside the black sprite outline remain foreground pixels.
"""

from __future__ import annotations

import json
from collections import deque
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "idle-draft-v1.png"
OUTPUT = ROOT / "idle-transparent-aligned-v2.png"
METADATA = ROOT / "idle-transparent-aligned-v2.metadata.json"
NOTES = ROOT / "idle-transparent-aligned-v2.verification.md"
FRAME_COUNT = 4

# The generated sheet has weak outlines where the shoulder plates meet the
# checkerboard.  These tight, per-cell envelopes keep those known foreground
# regions exact while the exterior flood removes the surrounding background.
SHOULDER_PROTECTION = (
    (
        [(112, 345), (137, 328), (175, 329), (200, 350), (198, 388), (185, 402), (126, 400), (100, 384), (102, 356)],
        [(343, 350), (365, 329), (397, 329), (421, 345), (436, 377), (427, 403), (365, 401), (342, 387)],
    ),
) * FRAME_COUNT


def is_neutral_background(red: int, green: int, blue: int) -> bool:
    """Recognize the light, low-saturation baked checkerboard only.

    The lower brightness bound deliberately excludes the near-black outline;
    the flood fill therefore cannot cross the outline into the character.
    """
    return min(red, green, blue) >= 80 and max(red, green, blue) - min(red, green, blue) <= 22


def exterior_background_mask(image: Image.Image, protected: Image.Image) -> list[bool]:
    width, height = image.size
    pixels = image.load()
    # Colored/dark pixels form the readable sprite structure.  Closing small
    # anti-aliased breaks in that structure prevents an exterior flood from
    # leaking into pale gray hair through a one-to-four-pixel outline gap.
    hard_foreground = Image.new("L", image.size, 0)
    hard_pixels = hard_foreground.load()
    for y in range(height):
        for x in range(width):
            red, green, blue, _ = pixels[x, y]
            if not is_neutral_background(red, green, blue):
                hard_pixels[x, y] = 255
    hard_foreground = hard_foreground.filter(ImageFilter.MaxFilter(5)).filter(ImageFilter.MinFilter(5))
    hard_pixels = hard_foreground.load()
    mask = [False] * (width * height)
    queue: deque[tuple[int, int]] = deque()

    def try_add(x: int, y: int) -> None:
        index = y * width + x
        if mask[index]:
            return

        red, green, blue, _ = pixels[x, y]
        if protected.getpixel((x, y)) or hard_pixels[x, y] or not is_neutral_background(red, green, blue):
            return

        mask[index] = True
        queue.append((x, y))

    for x in range(width):
        try_add(x, 0)
        try_add(x, height - 1)

    for y in range(height):
        try_add(0, y)
        try_add(width - 1, y)

    while queue:
        x, y = queue.popleft()
        for next_x, next_y in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
            if 0 <= next_x < width and 0 <= next_y < height:
                try_add(next_x, next_y)

    return mask


def foreground_bbox(image: Image.Image) -> tuple[int, int, int, int]:
    alpha = image.getchannel("A")
    bbox = alpha.getbbox()
    if bbox is None:
        raise ValueError("Frame has no opaque pixels after background removal.")
    return bbox


def main() -> None:
    source = Image.open(SOURCE).convert("RGBA")
    width, height = source.size
    if width % FRAME_COUNT != 0:
        raise ValueError(f"Source width {width} is not divisible by {FRAME_COUNT} frames.")

    protected = Image.new("L", source.size, 0)
    protected_draw = ImageDraw.Draw(protected)
    for frame_index, polygons in enumerate(SHOULDER_PROTECTION):
        for polygon in polygons:
            protected_draw.polygon([(frame_index * (width // FRAME_COUNT) + x, y) for x, y in polygon], fill=255)

    background = exterior_background_mask(source, protected)
    cleaned = source.copy()
    cleaned_pixels = cleaned.load()
    for index, remove_pixel in enumerate(background):
        if remove_pixel:
            x = index % width
            y = index // width
            red, green, blue, _ = cleaned_pixels[x, y]
            cleaned_pixels[x, y] = (red, green, blue, 0)

    cell_width = width // FRAME_COUNT
    frames = [cleaned.crop((frame * cell_width, 0, (frame + 1) * cell_width, height)) for frame in range(FRAME_COUNT)]
    bboxes = [foreground_bbox(frame) for frame in frames]

    # The median reference prevents one animation pose from defining all pivots.
    centers_x = sorted((left + right - 1) / 2 for left, _, right, _ in bboxes)
    foot_rows = sorted(bottom - 1 for _, _, _, bottom in bboxes)
    target_center_x = centers_x[len(centers_x) // 2]
    target_foot_row = foot_rows[len(foot_rows) // 2]

    aligned_frames: list[Image.Image] = []
    frame_records: list[dict[str, object]] = []
    for index, (frame, bbox) in enumerate(zip(frames, bboxes), start=1):
        left, top, right, bottom = bbox
        center_x = (left + right - 1) / 2
        foot_row = bottom - 1
        shift_x = round(target_center_x - center_x)
        shift_y = target_foot_row - foot_row
        if left + shift_x < 0 or right + shift_x > cell_width or top + shift_y < 0 or bottom + shift_y > height:
            raise ValueError(f"Frame {index} alignment would crop foreground pixels.")

        aligned = Image.new("RGBA", (cell_width, height), (0, 0, 0, 0))
        aligned.alpha_composite(frame, (shift_x, shift_y))
        aligned_frames.append(aligned)
        aligned.save(ROOT / f"idle-transparent-aligned-v2-frame-{index}.png")
        new_left, new_top, new_right, new_bottom = foreground_bbox(aligned)
        frame_records.append(
            {
                "frame": index,
                "cell": {"x": (index - 1) * cell_width, "y": 0, "width": cell_width, "height": height},
                "alignment_offset": {"x": shift_x, "y": shift_y},
                "foreground_bbox_in_cell": {"left": new_left, "top": new_top, "right_exclusive": new_right, "bottom_exclusive": new_bottom},
                "pivot_in_cell": {"x": target_center_x, "y": target_foot_row},
            }
        )

    output = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    for index, frame in enumerate(aligned_frames):
        output.alpha_composite(frame, (index * cell_width, 0))
    output.save(OUTPUT)

    retained_pixels = 0
    color_mismatches = 0
    for index, (source_frame, aligned_frame, record) in enumerate(zip(frames, aligned_frames, frame_records)):
        shift_x = record["alignment_offset"]["x"]
        shift_y = record["alignment_offset"]["y"]
        for y in range(height):
            for x in range(cell_width):
                output_x = x + shift_x
                output_y = y + shift_y
                if not (0 <= output_x < cell_width and 0 <= output_y < height):
                    continue
                original = source_frame.getpixel((x, y))
                retained = aligned_frame.getpixel((output_x, output_y))
                if retained[3]:
                    retained_pixels += 1
                    if retained[:3] != original[:3]:
                        color_mismatches += 1

    metadata = {
        "source": SOURCE.name,
        "output": OUTPUT.name,
        "format": "RGBA PNG",
        "sheet_dimensions": {"width": width, "height": height},
        "frame_count": FRAME_COUNT,
        "frame_dimensions": {"width": cell_width, "height": height},
        "alignment_reference": "median foreground horizontal center and bottom opaque row",
        "background_removal": "4-connected exterior flood fill of light neutral pixels, blocked by a 5px morphological closing of dark/colored sprite structure and tight per-frame shoulder protection; enclosed neutral pixels are retained",
        "static_validation": {
            "corner_alpha": [output.getpixel(point)[3] for point in ((0, 0), (width - 1, 0), (0, height - 1), (width - 1, height - 1))],
            "retained_pixel_rgb_checks": retained_pixels,
            "retained_pixel_rgb_mismatches": color_mismatches,
            "all_frame_bottom_opaque_rows": [record["foreground_bbox_in_cell"]["bottom_exclusive"] - 1 for record in frame_records],
        },
        "frames": frame_records,
    }
    METADATA.write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    NOTES.write_text(
        "# v2 정적 검증\n\n"
        "- 소스와 출력 시트는 모두 2172×724이며, 4개 셀은 각각 543×724이다.\n"
        "- 외곽에서 이어지는 밝은 무채색 체크무늬만 4방향 flood fill로 알파 0 처리했다.\n"
        "- 어두운 외곽선·유색 전경을 5px closing으로 연결하고, 외곽선이 약한 각 프레임의 양 어깨 방어구에는 좁은 보호 다각형을 적용한 뒤 flood fill을 수행했다.\n"
        "- 셀 안쪽에 완전히 막힌 무채색 영역은 보존한다. 배경인지 장식인지 판별할 수 없는 내부 픽셀을 삭제하지 않기 위해서다.\n"
        "- 프레임은 중앙 x와 발바닥 행의 중앙값에 맞췄고, 각 이동은 셀 경계 안에서 이루어져 잘린 전경 픽셀이 없다.\n"
        "- 이 정리는 체크무늬와 정렬만 다룬다. 미니 기사 이동 시트와의 체형·픽셀 크기·팔레트 정확 일치는 기존 시안의 미해결 제한으로 남는다.\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
