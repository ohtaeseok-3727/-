'use strict';
const $=id=>document.getElementById(id);
const reference=$('reference').getContext('2d'),attack=$('attack').getContext('2d');
const motions={
 jab:{file:'left-jab-drawn-v2.png',title:'왼손 정권지르기',sequence:[0,1,2,3,5,6,7],durations:[140,130,45,40,45,75,150],effect:'jab',start:315,rate:1,fxTimes:[40,40,45,45,55,70]},
 blade:{file:'mana-blade-motion-v1.png',title:'마력칼날 수평 휘두르기',sequence:[0,1,2,3,4,5,6,7],durations:[140,110,90,50,60,65,90,150],effect:'blade',start:340,rate:0.8,fxTimes:[30,30,40,40,50,65]},
 push:{file:'double-palm-motion-v2.png',title:'양손 앞으로 뻗기',sequence:[0,1,2,3,4,5,6,7],durations:[140,130,70,50,90,80,100,150],effect:'burst',start:340,rate:1.2,fxTimes:[80,75,45,65,85,90,85,80]}
};
const effects={
 jab:{file:'../Attack/Mage/mage-melee-effects-v1.png',columns:6,rows:[0,341,682,1024]},
 blade:{file:'mana-blade-v1.png',columns:3,rows:[0,512,1024]},
 burst:{file:'../Attack/Mage/mage-burst-b-v2.png',columns:4,rows:[0,512,1024]}
};
const sum=items=>items.reduce((a,b)=>a+b,0);
let current=motions.push,time=0,playing=false,last=null,raf=null,ready=false;
function indexAt(t,durations){if(t<0)return -1;for(let i=0;i<durations.length;i++){if(t<durations[i])return i;t-=durations[i];}return -1;}
function poseIndex(){const n=indexAt(time,current.durations);return n<0?current.sequence.length-1:n;}
function cycleLength(){return Math.max(sum(current.durations),current.start+sum(current.fxTimes)/current.rate)+120;}
function load(asset){asset.image=new Image();return new Promise((resolve,reject)=>{asset.image.onload=()=>resolve(asset);asset.image.onerror=()=>reject(new Error(asset.file+' 로드 실패'));asset.image.src=asset.file;});}
// Coordinates refer only to the newly drawn sheets. Freeze the release position during recovery.
function handPosition(source){
 if(current===motions.jab)return [340,290];
 if(current===motions.push)return [348,290];
 return [[125,345],[105,295],[100,295],[235,315],[295,295],[320,295]][Math.min(source,5)];
}
function drawEffect(source){
 const e=indexAt((time-current.start)*current.rate,current.fxTimes);
 if(e<0||!$('effects').checked)return e;
 const asset=effects[current.effect],p=handPosition(source);
 // Character is drawn at source scale 1. Old mini preview used .42: preserve relative FX size.
 let x=64+p[0],y=80+p[1],scale,rootX,rootY,col,row;
 if(current===motions.jab){scale=.55/.42;rootX=128;rootY=200;col=e;row=0;x+=20/.42;y-=5/.42;}
 else if(current===motions.blade){scale=.55/.42;rootX=[196,150,105,104,95,112][e];rootY=[262,260,260,256,255,264][e];col=e%3;row=Math.floor(e/3);}
 else {scale=.65/.42;rootX=[98,94,96,70,88,76,86,74][e];row=Math.floor(e/4);rootY=row?232:262;col=e%4;}
 attack.save();
 // Legacy circle/burst sheets use black-background additive compositing.
 attack.globalCompositeOperation=current===motions.blade?'source-over':'lighter';
 asset.frames.draw(attack,col,row,col*(asset.image.width/asset.columns),asset.rows[row],x-rootX*scale,y-rootY*scale,scale);
 attack.restore();return e;
}
function draw(){
 if(!ready)return;
 const step=poseIndex(),source=current.sequence[step];
 attack.clearRect(0,0,attack.canvas.width,attack.canvas.height);
 attack.fillStyle='#2d353e';attack.fillRect(0,0,attack.canvas.width,attack.canvas.height);
 attack.imageSmoothingEnabled=false;
 current.frames.draw(attack,source%4,Math.floor(source/4),(source%4)*384,Math.floor(source/4)*480,64,80,1);
 const e=drawEffect(source);
 $('label').textContent=`동작 ${step+1}/${current.sequence.length} · 원본 ${source+1}번 · 효과 ${e<0?'없음':e+1} · ${Math.round(time)}ms`;
 $('timeline').value=time;
}
function stop(){playing=false;if(raf!==null)cancelAnimationFrame(raf);raf=null;last=null;$('play').textContent='재생';}
function tick(now){
 if(!playing)return;
 if(last!==null)time=(time+Math.min(now-last,100))%cycleLength();
 last=now;draw();raf=requestAnimationFrame(tick);
}
function selectMotion(){
 stop();current=motions[$('motion').value];time=0;
 $('attack-title').textContent=current.title;
 $('description').textContent=current===motions.jab?'1번 · 손 앞에서 작은 마법진이 폭발합니다.':current===motions.blade?'2번 · 손에서 이어지는 마력칼날 궤적 (효과 0.8배 속도)':'3번 · 양손 앞 마법진에서 커지는 B안 폭발 (효과 1.2배 속도)';
 $('sequence-note').textContent='동작 순서: '+current.sequence.map(i=>i+1).join(' → ')+' · 캐릭터와 효과를 별도 시간축으로 재생합니다.';
 $('timeline').max=cycleLength()-1;draw();
}
$('motion').onchange=selectMotion;
$('effects').onchange=draw;
$('timeline').oninput=()=>{stop();time=Number($('timeline').value);draw();};
$('prev').onclick=()=>{stop();const n=(poseIndex()+current.sequence.length-1)%current.sequence.length;time=sum(current.durations.slice(0,n));draw();};
$('next').onclick=()=>{stop();const n=(poseIndex()+1)%current.sequence.length;time=sum(current.durations.slice(0,n));draw();};
$('play').onclick=()=>{if(!ready)return;if(playing){stop();return;}playing=true;last=null;$('play').textContent='정지';raf=requestAnimationFrame(tick);};
const walk={file:'walk-transparent-v2.png'};
document.querySelectorAll('button,input,select').forEach(el=>el.disabled=true);
Promise.all([load(walk),...Object.values(motions).map(load),...Object.values(effects).map(load)]).then(()=>{
 for(const motion of Object.values(motions))motion.frames=new PaddedFrames(motion.image,4,[0,480,960]);
 for(const effect of Object.values(effects))effect.frames=new PaddedFrames(effect.image,effect.columns,effect.rows);
 reference.imageSmoothingEnabled=false;reference.drawImage(walk.image,0,0,384,512,64,80,384,512);
 ready=true;document.querySelectorAll('button,input,select').forEach(el=>el.disabled=false);selectMotion();
}).catch(error=>{$('label').textContent=error.message;});
