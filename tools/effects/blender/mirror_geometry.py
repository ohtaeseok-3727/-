"""Deterministic convex mirror cells; geometry only, usable outside Blender."""
import math
import random


class MirrorGeometry:
    @staticmethod
    def clip(polygon, nx, ny, distance):
        result = []
        if not polygon:
            return result
        previous = polygon[-1]
        old = previous[0] * nx + previous[1] * ny - distance
        for point in polygon:
            current = point[0] * nx + point[1] * ny - distance
            if (current <= 0) != (old <= 0):
                t = old / (old - current)
                result.append((previous[0] + (point[0] - previous[0]) * t,
                               previous[1] + (point[1] - previous[1]) * t))
            if current <= 0:
                result.append(point)
            previous, old = point, current
        return result

    @staticmethod
    def tip(u):
        angle = 1.26 - u
        return (-10 + 14 * math.cos(angle), -8 + 14 * math.sin(angle))

    def cells(self):
        rng = random.Random(629)
        seeds = []
        # Fine chips near the cut, larger plates at the perimeter.
        for i in range(24):
            x, y = self.tip(.15 + .72 * i / 23)
            seeds.append((x + rng.uniform(-.65, .65), y + rng.uniform(-.55, .55)))
        for row in range(5):
            for col in range(9):
                seeds.append((-8 + (col + rng.uniform(.2, .8)) * 16 / 9,
                              -4.5 + (row + rng.uniform(.2, .8)) * 9 / 5))
        cells = []
        for i, (x, y) in enumerate(seeds):
            polygon = [(-8, -4.5), (8, -4.5), (8, 4.5), (-8, 4.5)]
            for j, (ox, oy) in enumerate(seeds):
                if i != j:
                    polygon = self.clip(polygon, ox - x, oy - y,
                                        (ox * ox + oy * oy - x * x - y * y) / 2)
            if len(polygon) >= 3:
                cells.append(polygon)
        return cells
