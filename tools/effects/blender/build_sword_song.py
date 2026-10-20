"""Blender 5.2: deterministic transparent sword turntable, no game execution.

Reference: SmallMage/walk-transparent-v2.png; retain the mini sprite pixel
scale and palette at conversion. No character geometry or pixels are reused.
"""
import bpy
import math
from pathlib import Path


class SwordSongStudio:
    def __init__(self):
        self.root = Path(__file__).resolve().parents[3]
        self.output = self.root / 'SimpleGame/Assets/Effects/MageSwordSong'
        self.frames = self.root / 'output/mage-sword-song-blender-v3/raw'
        self.frames.mkdir(parents=True, exist_ok=True)

    def material(self, name, color, emission=0):
        material = bpy.data.materials.new(name)
        material.diffuse_color = (*color, 1)
        material.use_nodes = True
        shader = material.node_tree.nodes.get('Principled BSDF')
        shader.inputs['Base Color'].default_value = (*color, 1)
        shader.inputs['Metallic'].default_value = 0
        shader.inputs['Roughness'].default_value = .3
        shader.inputs['Emission Color'].default_value = (*color, 1)
        shader.inputs['Emission Strength'].default_value = emission
        return material

    def mesh(self, name, vertices, faces, material):
        data = bpy.data.meshes.new(name)
        data.from_pydata(vertices, [], faces)
        obj = bpy.data.objects.new(name, data)
        bpy.context.collection.objects.link(obj)
        obj.data.materials.append(material)
        obj.parent = self.rig
        return obj

    def block(self, name, position, scale, material):
        bpy.ops.mesh.primitive_cube_add(size=1, location=position)
        obj = bpy.context.object
        obj.name = name
        obj.scale = scale
        obj.data.materials.append(material)
        obj.parent = self.rig
        return obj

    def build(self):
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
        self.rig = bpy.data.objects.new('ArcaneSword_Animated', None)
        bpy.context.collection.objects.link(self.rig)
        steel = self.material('Blue mana blade', (.015, .20, .68), .45)
        edge = self.material('Sky blue mana bevel', (.08, .58, 1), .65)
        gold = self.material('Blue mana guard and binding', (.025, .32, .83), .5)
        dark = self.material('Deep blue mana grip', (.018, .075, .25), .35)
        gem = self.material('Cyan mana core', (.025, .65, 1), .8)
        # Long tapered diamond section: split bevels remain legible at sprite scale.
        vertices = [(-.13, 0, -.55), (0, -.075, -.55), (.13, 0, -.55),
                    (0, .075, -.55), (-.105, 0, .68), (0, -.065, .68),
                    (.105, 0, .68), (0, .065, .68), (0, 0, 1.10)]
        blade = self.mesh('Diamond blade', vertices,
                          [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6),
                           (3, 0, 4, 7), (4, 5, 8), (5, 6, 8),
                           (6, 7, 8), (7, 4, 8)], steel)
        blade.data.materials.append(edge)
        for face in blade.data.polygons:
            face.material_index = face.index % 2
        self.block('Fuller light', (0, -.079, .08), (.025, .018, 1.12), gem)
        self.block('Grip', (0, 0, -.83), (.105, .12, .42), dark)
        for i in range(5):
            self.block('Grip binding', (0, 0, -.67 - i * .073), (.12, .14, .022), gold)
        for side in (-1, 1):
            wing = self.block('Swept guard', (side * .19, 0, -.59), (.40, .13, .09), gold)
            wing.rotation_euler.y = side * -.26
            self.block('Guard crystal', (side * .35, 0, -.54), (.065, .15, .10), gem)
        bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=1, radius=.12, location=(0, 0, -1.08))
        bpy.context.object.data.materials.append(gem)
        bpy.context.object.parent = self.rig
        for frame in range(1, 17):
            self.rig.rotation_euler.z = math.tau * (frame - 1) / 16
            self.rig.keyframe_insert(data_path='rotation_euler', frame=frame)
        scene = bpy.context.scene
        scene.render.engine = 'CYCLES'
        scene.cycles.samples = 16
        scene.render.resolution_x = 96
        scene.render.resolution_y = 192
        scene.render.resolution_percentage = 100
        scene.render.film_transparent = True
        scene.render.image_settings.file_format = 'PNG'
        scene.render.image_settings.color_mode = 'RGBA'
        scene.view_settings.view_transform = 'Standard'
        scene.world.color = (.18, .18, .18)
        bpy.ops.object.camera_add(location=(0, -6, .01))
        camera = bpy.context.object
        camera.rotation_euler = (math.pi / 2, 0, 0)
        camera.data.type = 'ORTHO'
        camera.data.ortho_scale = 2.55
        scene.camera = camera
        bpy.ops.object.light_add(type='AREA', location=(-2, -4, 4))
        bpy.context.object.data.energy = 450
        bpy.context.object.data.shape = 'DISK'
        bpy.context.object.data.size = 4
        scene.frame_start, scene.frame_end = 1, 16
        scene.render.filepath = str(self.frames / 'sword-')
        bpy.context.preferences.filepaths.save_version = 0
        bpy.ops.wm.save_as_mainfile(filepath=str(self.output / 'sword-song.blend'))
        bpy.ops.render.render(animation=True)


if __name__ == '__main__':
    SwordSongStudio().build()
