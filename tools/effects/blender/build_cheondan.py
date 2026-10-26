"""Blender authoring/rendering for Cheondan; no game execution.

Effects only. SmallKnight/walk-transparent-v1.png fixes pixel/palette style;
no character proportions, equipment, animation or source pixels are changed.
"""
import math
from pathlib import Path
import bpy


class CheondanStudio:
    def __init__(self):
        self.root = Path(__file__).resolve().parents[3]
        self.target = self.root / 'SimpleGame/Assets/Effects/CheondanBlender-v1'
        self.raw = self.root / 'output/cheondan-blender-v1/raw'
        self.target.mkdir(parents=True, exist_ok=True)
        self.raw.mkdir(parents=True, exist_ok=True)

    def material(self, name, color):
        mat = bpy.data.materials.new(name)
        mat.use_nodes = True
        nodes = mat.node_tree.nodes
        nodes.clear()
        emission = nodes.new('ShaderNodeEmission')
        emission.inputs['Color'].default_value = (*color, 1)
        out = nodes.new('ShaderNodeOutputMaterial')
        mat.node_tree.links.new(emission.outputs[0], out.inputs[0])
        return mat

    def ribbon(self, name, points, width, material, depth):
        vertices = []
        for i, (x, y) in enumerate(points):
            a, b = points[max(0, i - 1)], points[min(len(points) - 1, i + 1)]
            dx, dy = b[0] - a[0], b[1] - a[1]
            length = max(.001, math.hypot(dx, dy))
            taper = max(.025, math.sin(math.pi * i / (len(points) - 1)) ** .55)
            w = width * taper
            vertices.extend([(x - dy / length * w, y + dx / length * w, depth),
                             (x + dy / length * w, y - dx / length * w, depth)])
        mesh = bpy.data.meshes.new(name)
        mesh.from_pydata(vertices, [], [(i * 2, i * 2 + 1, i * 2 + 3, i * 2 + 2)
                                      for i in range(len(points) - 1)])
        obj = bpy.data.objects.new(name, mesh)
        bpy.context.collection.objects.link(obj)
        mesh.materials.append(material)
        return obj

    def visibility(self, obj, start, end):
        for frame, hidden in [(1, True), (start, False), (end + 1, True)]:
            obj.hide_render = hidden
            obj.keyframe_insert(data_path='hide_render', frame=frame)

    def build(self):
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
        gold = self.material('Amber silhouette', (.52, .21, .035))
        light = self.material('Gold edge', (1, .65, .17))
        white = self.material('Ivory cutting core', (1, .96, .75))
        dark = self.material('Deep fracture interior', (.028, .02, .038))
        # A curved cutting sheet sweeps clockwise; delayed narrow sheets trail it.
        for layer, (width, mat) in enumerate([(.25, gold), (.13, light), (.045, white)]):
            points = []
            for i in range(65):
                u = i / 64
                angle = -.25 + u * 2.65
                points.append((-.7 + 3.7 * math.cos(angle), .1 + 2.15 * math.sin(angle)))
            obj = self.ribbon('Sweeping slash ' + str(layer), points, width, mat, .02 * layer)
            self.visibility(obj, 14, 28 - layer)
            for frame, rotation, scale in [(14, .9, .22), (17, .25, .85),
                                            (20, -.5, 1.1), (24, -.85, 1.15), (28, -1, 1.18)]:
                obj.rotation_euler.z = rotation
                obj.scale = (scale, scale, 1)
                obj.keyframe_insert(data_path='rotation_euler', frame=frame)
                obj.keyframe_insert(data_path='scale', frame=frame)
        for j in range(3):
            points = [(x * .96, y * .96 - .13 * j) for x, y in points]
            obj = self.ribbon('Lingering curved wake ' + str(j), points, .025, light, .08)
            self.visibility(obj, 19 + j, 28 + j)
            obj.rotation_euler.z = -.85
        # Continuous diagonal wound. Every branch starts on the wound; no debris.
        diagonal = [(-4.9 + 9.8 * i / 80,
                     2.65 - 5.3 * i / 80 + .035 * math.sin(i * 2.3)) for i in range(81)]
        for layer, (width, mat) in enumerate([(.115, light), (.075, dark), (.012, white)]):
            obj = self.ribbon('Screen split ' + str(layer), diagonal, width, mat, .12 + layer * .01)
            self.visibility(obj, 23, 44 - layer)
            for frame, scale in [(23, .01), (26, 1), (44, 1)]:
                obj.scale.y = scale
                obj.keyframe_insert(data_path='scale', frame=frame)
        for j in range(14):
            u = .12 + j * .058
            x, y = -4.9 + 9.8 * u, 2.65 - 5.3 * u
            side = -1 if j % 2 else 1
            length = .55 + .7 * (.5 + .5 * math.sin(j * 7.1))
            branch = [(x, y)]
            for k in range(1, 5):
                branch.append((x + side * length * k / 4 + .14 * math.sin(k * 3 + j),
                               y + side * length * k / 4 * .9))
            for layer, (w, mat) in enumerate([(.032, light), (.012, white)]):
                obj = self.ribbon('Attached crack %02d-%d' % (j, layer), branch, w, mat, .17 + .01 * layer)
                self.visibility(obj, 27 + j % 4, 39)
        # Charge blade is deliberately compact so the large sweep reads as release.
        for layer, (w, mat) in enumerate([(.12, gold), (.065, light), (.018, white)]):
            obj = self.ribbon('Gathering sword aura ' + str(layer),
                              [(-1.8 + i * .035, -.9 + i * .07) for i in range(35)], w, mat, .03 * layer)
            self.visibility(obj, 2, 15)
            for frame, scale in [(2, .08), (8, .6), (14, 1.45)]:
                obj.scale = (scale, scale, 1)
                obj.keyframe_insert(data_path='scale', frame=frame)
        scene = bpy.context.scene
        scene.render.engine = 'CYCLES'
        scene.cycles.samples = 1
        scene.render.resolution_x, scene.render.resolution_y = 640, 360
        scene.render.resolution_percentage = 100
        scene.render.film_transparent = True
        scene.render.image_settings.file_format = 'PNG'
        scene.render.image_settings.color_mode = 'RGBA'
        scene.view_settings.view_transform = 'Standard'
        bpy.ops.object.camera_add(location=(0, 0, 10))
        scene.camera = bpy.context.object
        scene.camera.data.type = 'ORTHO'
        scene.camera.data.ortho_scale = 12
        scene.frame_start, scene.frame_end = 1, 46
        scene.render.fps = 24
        scene.render.filepath = str(self.raw / 'effect-')
        scene.frame_set(20)
        bpy.context.preferences.filepaths.save_version = 0
        bpy.ops.wm.save_as_mainfile(filepath=str(self.target / 'cheondan.blend'))
        bpy.ops.render.render(animation=True)


if __name__ == '__main__':
    CheondanStudio().build()
