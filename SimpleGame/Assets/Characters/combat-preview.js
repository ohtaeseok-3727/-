'use strict';
const $=id=>document.getElementById(id),data=COMBAT_VISUALS,ctx=$('stage').getContext('2d');
const images=[],cache=new Map();let clip,time=0,ready=false,playing=false,last=null,raf=null;
const sum=a=>a.reduce((n,f)=>n+f.ms,0);
function at(frames,t){if(t<0)return -1;for(let i=0;i<frames.length;i++){if(t<frames[i].ms)return i;t-=frames[i].ms;}return -1;}
function bodyIndex(){let i=at(clip.body,time);return i<0?clip.body.length-1:i;}
function isolate(s){const key=[s.asset,...s.rect].join(':');if(!cache.has(key)){const [x,y,w,h]=s.rect,c=document.createElement('canvas');c.width=w+128;c.height=h+128;const b=c.getContext('2d');b.imageSmoothingEnabled=false;b.drawImage(images[s.asset],x,y,w,h,64,64,w,h);cache.set(key,c);}return cache.get(key);}
function drawSprite(s,anchor=[0,0]){ctx.save();ctx.globalCompositeOperation=s.blend===1?'lighter':'source-over';const tile=isolate(s);ctx.drawImage(tile,s.offset[0]+anchor[0]-64*s.scale,s.offset[1]+anchor[1]-64*s.scale,tile.width*s.scale,tile.height*s.scale);ctx.restore();}
function draw(){if(!ready||!clip)return;ctx.fillStyle=$('background').value;ctx.fillRect(0,0,1000,640);ctx.imageSmoothingEnabled=false;
 const b=bodyIndex(),e=at(clip.effect,time-clip.effectStart),z=Number($('zoom').value);
 ctx.save();ctx.translate($('flip').checked?660:340,460);ctx.scale($('flip').checked?-z:z,z);drawSprite(clip.body[b].sprite);if(e>=0&&$('effects').checked)drawSprite(clip.effect[e].sprite,clip.anchors[b]);ctx.restore();
 ctx.strokeStyle='#82929d';ctx.beginPath();ctx.moveTo(60,461);ctx.lineTo(940,461);ctx.stroke();
 $('status').textContent=`${clip.profession===1?'기사':'마법사'} · ${clip.name} · 동작 ${b+1}/${clip.body.length} · 효과 ${e>=0&&$('effects').checked?e+1:'없음'} · ${Math.round(time)}ms`;
 $('timeline').value=time;
}
function stop(){playing=false;last=null;if(raf!==null)cancelAnimationFrame(raf);raf=null;$('play').textContent='재생';}
function choose(){stop();clip=data.clips.find(c=>c.id===$('clip').value);time=0;$('timeline').max=Math.ceil(clip.duration+120)-1;$('details').textContent=`동작 ${Math.round(sum(clip.body))}ms · 효과 시작 ${clip.effectStart}ms · 이미지와 타이밍은 게임 적용용 데이터와 공유합니다.`;draw();}
function profession(){const wanted=Number($('profession').value);$('clip').replaceChildren();for(const c of data.clips.filter(c=>c.profession===wanted)){const o=document.createElement('option');o.value=c.id;o.textContent=c.name;$('clip').append(o);} $('clip').selectedIndex=1;choose();}
function tick(now){if(!playing)return;if(last!==null)time=(time+Math.min(now-last,100)*Number($('speed').value))%(clip.duration+120);last=now;draw();raf=requestAnimationFrame(tick);}
$('profession').onchange=profession;$('clip').onchange=choose;
$('play').onclick=()=>{if(!ready)return;if(playing){stop();return;}playing=true;last=null;$('play').textContent='정지';raf=requestAnimationFrame(tick);};
for(const [id,d] of [['prev',-1],['next',1]])$(id).onclick=()=>{stop();const i=(bodyIndex()+d+clip.body.length)%clip.body.length;time=sum(clip.body.slice(0,i));draw();};
$('restart').onclick=()=>{stop();time=0;draw();};$('timeline').oninput=()=>{stop();time=Number($('timeline').value);draw();};
for(const id of ['effects','flip','zoom','background'])$(id).onchange=draw;
document.querySelectorAll('button,select,input').forEach(e=>e.disabled=true);
Promise.all(data.assets.map((a,i)=>new Promise((resolve,reject)=>{const im=new Image();images[i]=im;im.onload=resolve;im.onerror=()=>reject(new Error(a.path+' 로드 실패'));im.src=a.path;}))).then(()=>{ready=true;document.querySelectorAll('button,select,input').forEach(e=>e.disabled=false);if(new URLSearchParams(location.search).get('profession')==='mage')$('profession').value='0';profession();}).catch(e=>$('status').textContent=e.message);
