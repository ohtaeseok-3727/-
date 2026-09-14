"""Reassemble source pixels, with rigid transforms only; no image generation.
Run explicitly to produce assets. Does not build or launch the game/preview.
"""
from pathlib import Path
import math, json
from PIL import Image, ImageDraw, ImageChops

ROOT = Path(__file__).resolve().parent
source = Image.open(ROOT / 'walk-transparent-v2.png').convert('RGBA')
SIZE = 512

def cut(points):
    mask = Image.new('L', source.size)
    ImageDraw.Draw(mask).polygon(points, fill=255)
    part = source.copy()
    part.putalpha(ImageChops.multiply(source.getchannel('A'), mask))
    box = mask.getbbox()
    return part.crop(box), box[:2]

# Coordinates refer to the first walking frame, never to regenerated artwork.
polygons = {
 'cape': [(104,264),(152,272),(111,309),(84,328),(81,382),(47,398),(47,347),(73,307)],
 'head': [(91,107),(318,107),(318,246),(278,246),(276,275),(170,279),(145,250),(115,235),(92,229)],
 'torso': [(150,271),(261,271),(273,290),(270,315),(270,396),(145,396),(148,325),(136,296)],
 'right_arm': [(111,275),(151,281),(147,319),(132,337),(99,320)],
 'left_arm': [(252,279),(278,280),(294,312),(279,335),(252,316)],
 'right_glove': [(99,321),(135,318),(158,334),(157,369),(140,383),(104,380),(85,360),(87,339)],
 'left_glove': [(280,310),(309,314),(330,334),(330,363),(311,382),(278,378),(264,359),(266,331)],
 'right_leg': [(130,377),(171,377),(167,425),(161,437),(174,442),(173,456),(109,456),(106,435)],
 'left_leg': [(217,377),(263,377),(265,429),(294,433),(297,456),(226,456),(217,434)],
}
parts = {key: cut(poly) for key,poly in polygons.items()}
part_dir = ROOT / 'rig-parts'
part_dir.mkdir(exist_ok=True)
for name,(im,offset) in parts.items(): im.save(part_dir / (name+'.png'))

def rot(v, degrees):
    a=math.radians(degrees); c=math.cos(a); s=math.sin(a)
    return (v[0]*c-v[1]*s,v[0]*s+v[1]*c)

def place(canvas,name,src_pivot,dst_pivot,angle=0):
    im,(ox,oy)=parts[name]
    a=math.radians(angle); c=math.cos(a); s=math.sin(a)
    # Inverse rigid mapping. No scaling; nearest-neighbour selects original pixels.
    tx=src_pivot[0]-ox-c*dst_pivot[0]-s*dst_pivot[1]
    ty=src_pivot[1]-oy+s*dst_pivot[0]-c*dst_pivot[1]
    layer=im.transform(canvas.size,Image.Transform.AFFINE,(c,s,tx,-s,c,ty),resample=Image.Resampling.NEAREST)
    canvas.alpha_composite(layer)

# Angles traced from the selected sheet: anticipation, extension, hold, recovery.
# Positive angles are clockwise in image coordinates. Limbs keep constant length.
# body, right leg, left leg, left upper-arm, left glove, right upper-arm, right glove, cape
poses=[
 [(0,0,0,0,0,0,0,0),(-4,18,-10,-32,-20,8,0,10),(-7,24,-12,-58,-35,10,0,20),(-9,28,-12,-90,-70,12,0,28),(-9,28,-12,-90,-70,12,0,24),(0,0,0,0,0,0,0,0)],
 [(0,0,0,0,0,0,0,0),(4,12,-10,70,70,6,0,-8),(7,18,-20,80,80,8,0,-18),(-8,24,-12,-80,-65,10,0,25),(-6,24,-12,-65,-45,10,0,20),(0,0,0,0,0,0,0,0)],
 [(0,0,0,0,0,0,0,0),(-3,16,-8,-35,-35,-35,-25,12),(-6,22,-10,-55,-50,-65,-55,18),(-8,26,-12,-72,-62,-80,-70,25),(-8,26,-12,-72,-62,-80,-70,20),(0,0,0,0,0,0,0,0)]
]
atlas=Image.new('RGBA',(SIZE*6,SIZE*3))
palms=[]
for row,sequence in enumerate(poses):
    row_palms=[]
    for col,(body,rl,ll,la,lg,ra,rg,cap) in enumerate(sequence):
        frame=Image.new('RGBA',(SIZE,SIZE))
        body=-body
        if la < -55: la=-55
        if row == 2: ra=-65 if col in (2,3,4) else (-40 if col == 1 else 0)
        # Stable pelvis and feet; modest stance spread without stretching legs.
        shift=(46,-8)
        def bodypoint(pt):
            v=rot((pt[0]-210,pt[1]-347),body)
            return (256+v[0],339+v[1])
        place(frame,'cape',(140,282),bodypoint((140,282)),body+cap)
        place(frame,'right_leg',(146,401),(192-(5 if col in (1,2,3,4) else 0),393),rl)
        place(frame,'left_leg',(242,401),(288+(5 if col in (1,2,3,4) else 0),393),ll)
        # Right/nonattacking arm is behind torso and the crossing left arm.
        rs=bodypoint((130,287))
        place(frame,'right_arm',(130,287),rs,body+ra)
        rv=rot((-8,57),body+ra)
        rh=(rs[0]+rv[0],rs[1]+rv[1])
        place(frame,'right_glove',(124,345),rh,body+rg)
        place(frame,'torso',(210,347),(256,339),body)
        place(frame,'head',(210,274),bodypoint((210,274)),body*0.35)
        if row == 2:
            place(frame,'right_arm',(130,287),rs,body+ra)
            place(frame,'right_glove',(124,345),rh,body+rg)
        ls=bodypoint((264,289))
        place(frame,'left_arm',(264,289),ls,body+la)
        lv=rot((32,53),body+la)
        lh=(ls[0]+lv[0],ls[1]+lv[1])
        place(frame,'left_glove',(296,345),lh,body+lg)
        # Foreground glove position is shared with the effect renderer.
        row_palms.append([round(lh[0]),round(lh[1])])
        atlas.alpha_composite(frame,(col*SIZE,row*SIZE))
    palms.append(row_palms)
atlas.save(ROOT/'attacks-rigid-parts-v1.png')
meta={'cell':SIZE,'scale':0.42,'anchor':[256,448],'palms':palms,'parts':{n:{'sourcePolygon':p} for n,p in polygons.items()},'poses':poses,
      'limitations':'Pose angles are manually traced approximations, not verified identical to the selected attack sheet.'}
(ROOT/'rigid-attack.json').write_text(json.dumps(meta,ensure_ascii=False,indent=2),encoding='utf-8')
print('Saved rigid atlas and part metadata. No game or preview executed.')
