"""Shared sword-tip scar + animated mirror plates. Blender authoring, no game run.

Effect palette/pixel reference: SmallKnight/walk-transparent-v1.png.
Character geometry, proportions, equipment and source pixels are not edited.
"""
import json
import math
import random
import sys
from pathlib import Path
import bpy

sys.path.insert(0, str(Path(__file__).resolve().parent))
from build_cheondan import CheondanStudio
from mirror_geometry import MirrorGeometry


class MirrorCheondanStudio(CheondanStudio):
    def __init__(self):
        super().__init__()
        self.target = self.root / 'SimpleGame/Assets/Effects/CheondanMirror-v2'
        self.raw = self.root / 'output/cheondan-mirror-v2/raw'
        self.target.mkdir(parents=True, exist_ok=True)
        self.raw.mkdir(parents=True, exist_ok=True)
        self.geometry = MirrorGeometry()

    def polygon(self, name, points, material, z=0, parent=None, center=(0, 0)):
        mesh = bpy.data.meshes.new(name)
        mesh.from_pydata([(x - center[0], y - center[1], z) for x, y in points], [],
                         [tuple(range(len(points)))])
        obj = bpy.data.objects.new(name, mesh)
        bpy.context.collection.objects.link(obj)
        mesh.materials.append(material)
        obj.parent = parent
        return obj

    def sweep(self):
        gold = self.material('Amber sweep edge', (.65, .30, .055))
        bright = self.material('Golden sweep', (1, .72, .28))
        core = self.material('Hot ivory blade', (1, .98, .88))
        wound = self.material('Wound interior', (.025, .022, .028))
        self.path = [self.geometry.tip(i / 96) for i in range(97)]
        # Every frame shares the exact same tip samples: no second rotated curve.
        for frame in range(11, 25):
            progress = min(1, max(.01, (frame - 10) / 9))
            end = max(2, round(progress * 96))
            start = max(0, end - 24)
            for layer, (width, mat) in enumerate([(.24, gold), (.125, bright), (.036, core)]):
                obj = self.ribbon(f'Swept tip frame {frame} layer {layer}',
                                  self.path[start:end + 1], width, mat, .62 + .01 * layer)
                self.visibility(obj, frame, frame)
            if frame <= 19:
                tip = self.path[end]
                # The visible blade points to the exact leading end of the scar.
                inner = (-10 + (tip[0] + 10) * .75, -8 + (tip[1] + 8) * .75)
                nx, ny = -(tip[1] + 8) / 14, (tip[0] + 10) / 14
                for layer, (w, mat) in enumerate([(.15, gold), (.075, core)]):
                    obj = self.polygon(f'Blade frame {frame} layer {layer}',
                                       [(inner[0] + nx * w, inner[1] + ny * w), tip,
                                        (inner[0] - nx * w, inner[1] - ny * w)], mat, .67 + .01 * layer)
                    self.visibility(obj, frame, frame)
        for frame in range(11, 25):
            end = max(2, round(min(1, (frame - 10) / 9) * 96))
            for layer, (width, mat) in enumerate([(.075, bright), (.038, wound), (.009, core)]):
                obj = self.ribbon(f'Same-path scar frame {frame} layer {layer}',
                                  self.path[:end + 1], width, mat, .53 + .01 * layer)
                self.visibility(obj, frame, frame)
        # Charge is a compact segment of the same initial blade orientation.
        initial = self.path[0]
        charge = self.ribbon('Gathered blade',
                             [(-10 + (initial[0] + 10) * t / 24,
                               -8 + (initial[1] + 8) * t / 24) for t in range(14, 25)],
                             .06, bright, .6)
        self.visibility(charge, 3, 10)

    def shards(self):
        rng = random.Random(322)
        rim = self.material('Ivory glass bevel', (.81, .84, .86))
        side_mat = self.material('Glass thickness', (.055, .065, .08))
        silver = [self.material(f'Silver mirror {i}', (.11 + i * .018, .14 + i * .019, .17 + i * .020))
                  for i in range(5)]
        highlight = self.material('Reflected light band', (.43, .49, .54))
        bright_band = self.material('Narrow reflected light', (.74, .79, .82))
        reflection = self.material('Reflected silhouette', (.055, .067, .082))
        impact = self.geometry.tip(.55)
        data = []
        for index, polygon in enumerate(self.geometry.cells()):
            cx = sum(p[0] for p in polygon) / len(polygon)
            cy = sum(p[1] for p in polygon) / len(polygon)
            center = (cx, cy)
            rig = bpy.data.objects.new(f'Mirror plate {index:02}', None)
            bpy.context.collection.objects.link(rig)
            rig.location = (cx, cy, .1)
            # Inset face leaves a crisp bevel, with dark extruded thickness.
            inset = [(cx + (x - cx) * .978, cy + (y - cy) * .978) for x, y in polygon]
            self.polygon(f'Plate {index} bevel', polygon, rim, .002, rig, center)
            self.polygon(f'Plate {index} silver', inset, silver[index % 5], .006, rig, center)
            for edge, a in enumerate(polygon):
                b = polygon[(edge + 1) % len(polygon)]
                mesh = bpy.data.meshes.new('Extruded edge')
                mesh.from_pydata([(a[0] - cx, a[1] - cy, -.085),
                                  (b[0] - cx, b[1] - cy, -.085),
                                  (b[0] - cx, b[1] - cy, 0), (a[0] - cx, a[1] - cy, 0)], [], [(0, 1, 2, 3)])
                obj = bpy.data.objects.new(f'Plate {index} side {edge}', mesh)
                bpy.context.collection.objects.link(obj)
                obj.parent = rig
                mesh.materials.append(side_mat)
            # A continuous stylized reflection is clipped into each moving plate.
            for band, (low, high, mat) in enumerate([(-2.4, -.7, highlight), (.7, .88, bright_band)]):
                piece = self.geometry.clip(inset, 1, -.65, high)
                piece = self.geometry.clip(piece, -1, .65, -low)
                if len(piece) >= 3:
                    self.polygon(f'Plate {index} reflection band {band}', piece, mat, .008, rig, center)
            for building, (left, right, top) in enumerate([(-6, -3.5, .7), (-1.8, -.5, -.4), (2.8, 5, 1.5)]):
                piece = inset
                for nx, ny, d in [(1, 0, right), (-1, 0, -left), (0, 1, top)]:
                    piece = self.geometry.clip(piece, nx, ny, d)
                if len(piece) >= 3:
                    self.polygon(f'Plate {index} reflected tower {building}', piece, reflection, .011, rig, center)
            for obj in rig.children:
                # Reveal only already separated, moving fragments; no mirror hold.
                self.visibility(obj, 25, 45)
            dx, dy = cx - impact[0], cy - impact[1]
            distance = max(.3, math.hypot(dx, dy))
            velocity = rng.uniform(2.2, 4.5)
            spin = (rng.uniform(-2.6, 2.6), rng.uniform(-2.6, 2.6), rng.uniform(-1.4, 1.4))
            for frame in [1, 24, 25, 29, 34, 40, 46]:
                t = max(0, (frame - 19) / 24)
                blast = t * (1.2 + t * 1.8)
                rig.location = (cx + dx / distance * velocity * blast,
                                cy + dy / distance * velocity * blast - .8 * t * t,
                                .1 + t * rng.uniform(.5, 1.1) if frame == 1 else .1 + t * 2)
                rig.rotation_euler = tuple(s * t for s in spin)
                scale = 1 + t * .8
                rig.scale = (scale, scale, scale)
                rig.keyframe_insert(data_path='location', frame=frame)
                rig.keyframe_insert(data_path='rotation_euler', frame=frame)
                rig.keyframe_insert(data_path='scale', frame=frame)
            data.append({'polygon': polygon, 'center': center, 'spin': spin})
        (self.target / 'geometry.json').write_text(json.dumps({'tip_path': self.path, 'plates': data}, indent=2), encoding='utf-8')

    def build(self):
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
        self.sweep()
        self.shards()
        scene = bpy.context.scene
        scene.render.engine = 'CYCLES'
        scene.cycles.samples = 1
        scene.render.resolution_x, scene.render.resolution_y = 960, 540
        scene.render.resolution_percentage = 100
        scene.render.film_transparent = True
        scene.render.image_settings.file_format = 'PNG'
        scene.render.image_settings.color_mode = 'RGBA'
        scene.view_settings.view_transform = 'Standard'
        bpy.ops.object.camera_add(location=(0, 0, 20))
        scene.camera = bpy.context.object
        scene.camera.data.type = 'ORTHO'
        scene.camera.data.ortho_scale = 16
        scene.frame_start, scene.frame_end = 1, 50
        scene.render.fps = 24
        scene.render.filepath = str(self.raw / 'mirror-')
        scene.frame_set(36)
        bpy.context.preferences.filepaths.save_version = 0
        bpy.ops.wm.save_as_mainfile(filepath=str(self.target / 'cheondan-mirror.blend'))
        bpy.ops.render.render(animation=True)


if __name__ == '__main__':
    MirrorCheondanStudio().build()
