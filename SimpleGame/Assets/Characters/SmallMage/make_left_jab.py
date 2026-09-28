"""Left jab from the current walk master only. No old attack references.
Fixed-size source-pixel pieces, nearest-neighbour rotations, no recolouring.
"""
from pathlib import Path
import math,json
from PIL import Image,ImageDraw,ImageChops
P=Path(__file__).resolve().parent
src=Image.open(P/'walk-transparent-v2.png').convert('RGBA').crop((0,0,384,512))
mask=Image.new('L',src.size)
ImageDraw.Draw(mask).polygon([(267,282),(299,290),(338,303),(338,386),(280,386),(280,368),(256,368),(256,315)],fill=255)
base=src.copy()
base.putalpha(ImageChops.multiply(src.getchannel('A'),ImageChops.invert(mask)))
# Reveal the tunic and belt formerly occluded by the resting left glove.
# Copy adjacent cloth/belt pixels without resizing or recolouring.
base.paste(src.crop((235,315,256,368)),(255,315))
glove=src.crop((255,303,332,380))
# Original pixels from the far upper sleeve; each segment has fixed length.
sleeve=src.crop((119,281,153,321))
shoulder=(269,298)
length=34.0
wrists=[(299,345),(285,330),(303,315),(332,299),(337,298),(328,302),(307,322),(299,345)]
durations=[140,130,45,40,65,45,75,150]
def copy_pixels(dst,layer):
    # Binary paste preserves source RGBA values instead of blending new colours.
    dst.paste(layer,(0,0),layer.getchannel('A').point(lambda a:255 if a else 0))
def segment(dst,start,end):
    angle=math.atan2(end[1]-start[1],end[0]-start[0])-math.pi/2
    c,s=math.cos(angle),math.sin(angle)
    # Local sleeve centre at x15; segment starts y2, ends y36.
    tx=17-c*start[0]-s*start[1];ty=2+s*start[0]-c*start[1]
    layer=sleeve.transform(dst.size,Image.Transform.AFFINE,(c,s,tx,-s,c,ty),resample=Image.Resampling.NEAREST)
    copy_pixels(dst,layer)
atlas=Image.new('RGBA',(512*4,512*2))
frames=[]
for i,wrist in enumerate(wrists):
    frame=base.copy()
    if i in (0,7):
        frame=src.copy()
    else:
        dx,dy=wrist[0]-shoulder[0],wrist[1]-shoulder[1]
        distance=math.hypot(dx,dy)
        assert distance<=2*length
        h=math.sqrt(max(0,length*length-distance*distance/4))
        elbow=((shoulder[0]+wrist[0])/2-dy/distance*h,(shoulder[1]+wrist[1])/2+dx/distance*h)
        segment(frame,shoulder,elbow)
        segment(frame,elbow,wrist)
        layer=Image.new('RGBA',src.size)
        layer.paste(glove,(round(wrist[0]-44),round(wrist[1]-42)))
        copy_pixels(frame,layer)
    # The far arm passes behind the existing head, never replacing its pixels.
    frame.paste(src.crop((0,0,280,278)),(0,0))
    cell=Image.new('RGBA',(512,512))
    cell.paste(frame,(64,0))
    atlas.paste(cell,((i%4)*512,(i//4)*512))
    frames.append(cell)
atlas.save(P/'left-jab-pixels-v1.png')
(P/'left-jab-pixels-v1.json').write_text(json.dumps({'cell':512,'frames':8,'durations':durations,'wrists':wrists,'source':'walk-transparent-v2.png','scale':0.42,'anchor':[274,456]},indent=2),encoding='utf-8')
# Asset checks only, not animation/game execution.
palette=set(src.getdata())
used={p for p in atlas.getdata() if p[3]}
assert used<=palette,'Unexpected new pixel colours'
for cell in frames:
    for box in [(0,0,280,278),(0,278,180,512),(0,398,384,512)]:
        a,b,c,d=box
        assert cell.crop((a+64,b,c+64,d)).tobytes()==src.crop(box).tobytes(),'Unchanged head/right arm/feet differ'
    b=cell.getbbox();assert b and b[0]>0 and b[2]<512
print('8 frames; original body region unchanged; palette preserved; transparent gutters verified.')
review=Image.new('RGBA',atlas.size,(45,53,62,255))
review.alpha_composite(atlas)
review.convert('RGB').save(P/'left-jab-review.jpg')
