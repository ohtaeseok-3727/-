'use strict';

// Port of CheondanView's geometry. Effects contain no image assets.
class CheondanGeometry {
    static clamp(v) { return Math.max(0, Math.min(1, v)); }
    static ease(v) { v = this.clamp(v); return v * v * (3 - 2 * v); }
    static mix(a, b, t) { return [a[0] + (b[0] - a[0]) * t, a[1] + (b[1] - a[1]) * t]; }
    static edge(u) { return .65 * Math.sin(u * 97) + .35 * Math.sin(u * 173 + 1.3); }
    static color(r, g, b, a) { return `rgba(${r*255},${g*255},${b*255},${Math.max(0,a)})`; }
    static quad(ctx, points, color) {
        ctx.fillStyle = color;
        ctx.beginPath();
        points.forEach((p, i) => i ? ctx.lineTo(...p) : ctx.moveTo(...p));
        ctx.closePath(); ctx.fill();
    }
    static stroke(ctx, a, b, w, color) {
        const d = Math.hypot(b[0]-a[0], b[1]-a[1]);
        if (d < .001) return;
        const n = [(b[1]-a[1])*w/d, -(b[0]-a[0])*w/d];
        this.quad(ctx, [[a[0]+n[0],a[1]+n[1]],[b[0]+n[0],b[1]+n[1]],
            [b[0]-n[0],b[1]-n[1]],[a[0]-n[0],a[1]-n[1]]], color);
    }
    static blade(ctx, root, tip, width, alpha) {
        const len = Math.hypot(tip[0]-root[0],tip[1]-root[1]);
        if (len < .01) return;
        const n = [-(tip[1]-root[1])/len,(tip[0]-root[0])/len];
        const colors = [[1,.54,.08,alpha*.1],[1,.73,.2,alpha*.28],[1,.87,.49,alpha*.9],[1,.99,.91,alpha]];
        [1.8,1.35,1,.6].forEach((k,l) => {
            const pair = (u,side) => {
                const p = this.mix(root,tip,u);
                const w = width*k*(1-u)*(.7+.3*Math.sin(u*Math.PI))*(1+.1*this.edge(u));
                return [p[0]+n[0]*w*side,p[1]+n[1]*w*side];
            };
            for(let i=0;i<28;i++) this.quad(ctx,[pair(i/28,-1),pair((i+1)/28,-1),pair((i+1)/28,1),pair(i/28,1)],this.color(...colors[l]));
        });
        this.stroke(ctx,root,tip,1,this.color(1,1,.97,alpha));
    }
    static line(sign) { return sign === 1 ? [[243.2,93.6],[1075.2,655.2]] : [[1036.8,93.6],[204.8,655.2]]; }
    static ribbon(ctx, line, width, progress, color) {
        const [a,b]=line, len=Math.hypot(b[0]-a[0],b[1]-a[1]);
        const n=[-(b[1]-a[1])/len,(b[0]-a[0])/len];
        const p=(u,side)=>{
            const q=this.mix(a,b,u), taper=Math.pow(Math.max(0,Math.sin(Math.PI*u)),.48);
            const w=width*taper*(.35+.85*u)*(1+.12*this.edge(u));
            const off=this.edge(u)*3+side*w;
            return [q[0]+n[0]*off,q[1]+n[1]*off];
        };
        for(let i=0;i<96;i++) {
            const u=i/96,v=Math.min((i+1)/96,progress);
            if(u>=progress) break;
            this.quad(ctx,[p(u,-1),p(v,-1),p(v,1),p(u,1)],color);
        }
    }
    static cast(ctx,t,feet,sign) {
        if(t>=1.25) return;
        const charge=this.ease(t/.65), swing=this.ease((t-.65)/.17);
        const raised=[feet[0]-6*sign,feet[1]-41], lowered=[feet[0]+15*sign,feet[1]-7];
        const hilt=this.mix(raised,lowered,swing), angle=-2.05+2.62*swing;
        const len=(30+245*charge)*(1-.30*swing);
        const tip=[hilt[0]+sign*Math.cos(angle)*len,hilt[1]+Math.sin(angle)*len];
        if(t<.65) for(let s=0;s<10;s++) {
            const cycle=(t*1.7+s*.173)%1,a=s*2.39996;
            const start=[raised[0]+Math.cos(a)*(100+85*charge),raised[1]+Math.sin(a)*(85+60*charge)];
            const finish=this.mix(hilt,tip,.2+.055*s);
            const path=u=>{const p=this.mix(start,finish,u);return [p[0]+sign*Math.sin(u*Math.PI)*25,p[1]-Math.sin(u*Math.PI)*23];};
            for(let j=0;j<12;j++) {
                const u=this.clamp(cycle-j*.025),v=this.clamp(u-.025);
                this.stroke(ctx,path(v),path(u),1.4*(1-j/13),this.color(1,.81,.36,(1-cycle)*(.25+.65*charge)));
            }
        }
        if(t<.82) this.blade(ctx,hilt,tip,4+13*charge+30*swing,1);
    }
    static screen(ctx,t,sign) {
        if(t<.65 || t>=1.89) return;
        const line=this.line(sign), [a,b]=line;
        const swing=this.ease((t-.65)/.17);
        if(t<.82) {
            this.blade(ctx,this.mix(a,b,swing),this.mix(a,b,Math.max(0,swing-.2)),8+43*swing,.95);
            return;
        }
        const fade=1-this.ease((t-1.5)/.39), trail=(1-this.ease((t-1.05)/.23))*.85;
        this.ribbon(ctx,line,31,1,this.color(1,.51,.1,.16*trail));
        this.ribbon(ctx,line,15,1,this.color(1,.80,.38,.58*trail));
        this.ribbon(ctx,line,5,1,this.color(1,.98,.86,trail));
        if(t<1.05) return;
        const progress=this.ease((t-1.05)/.23);
        this.ribbon(ctx,line,10,progress,this.color(1,.66,.24,.85*fade));
        this.ribbon(ctx,line,6,progress,this.color(.012,.018,.033,fade));
        const len=Math.hypot(b[0]-a[0],b[1]-a[1]),n=[-(b[1]-a[1])/len,(b[0]-a[0])/len];
        for(let branch=0;branch<15;branch++) {
            const u=.08+branch*.059,growth=this.ease((progress-u)/.16);
            if(growth<=0) continue;
            const root=this.mix(a,b,u);root[0]+=n[0]*this.edge(u)*3;root[1]+=n[1]*this.edge(u)*3;
            const side=branch%2?-1:1, reach=42+65*Math.abs(Math.sin(branch*2.17));
            let prev=root;
            for(let seg=1;seg<=5;seg++) {
                const v=seg/5,off=side*reach*v*growth;
                const next=[root[0]+n[0]*off+Math.sin(seg*2.1+branch)*12*v*growth,root[1]+n[1]*off+v*23*growth];
                this.stroke(ctx,prev,next,2.5*(1-v*.7),this.color(.02,.03,.05,fade));
                this.stroke(ctx,[prev[0]+1,prev[1]],[next[0]+1,next[1]],.8*(1-v*.6),this.color(1,.88,.61,fade*.8));
                prev=next;
            }
        }
    }
}

class CheondanPreview {
    constructor() {
        this.canvas=document.getElementById('stage');this.ctx=this.canvas.getContext('2d');
        this.world=document.createElement('canvas');this.world.width=1280;this.world.height=720;
        this.w=this.world.getContext('2d');this.time=0;this.running=true;this.last=0;
        this.image=new Image();this.image.src='../../SimpleGame/Assets/Characters/SmallKnight/attacks-transparent-v2.png';
        this.image.onload=()=>{document.getElementById('status').textContent='준비됨 · 아래 단계 버튼을 누르면 해당 장면에서 멈춥니다.';};
        this.image.onerror=()=>{document.getElementById('status').textContent='기사 이미지를 불러오지 못했습니다. 이펙트는 계속 재생됩니다.';};
        this.play=document.getElementById('play');this.slider=document.getElementById('timeline');
        this.play.onclick=()=>this.setRunning(!this.running);
        document.getElementById('restart').onclick=()=>{this.time=0;this.setRunning(true);};
        this.slider.oninput=()=>{this.time=Number(this.slider.value);this.setRunning(false);this.render();};
        document.querySelectorAll('[data-time]').forEach(b=>b.onclick=()=>{this.time=Number(b.dataset.time);this.setRunning(false);this.render();});
        document.getElementById('direction').onchange=()=>this.render();
        document.addEventListener('visibilitychange',()=>{this.last=0;});
        requestAnimationFrame(t=>this.frame(t));
    }
    setRunning(v) { if(v&&this.time>=2.6)this.time=0;this.running=v;this.play.textContent=v?'일시정지':'재생'; }
    background() {
        const c=this.w; c.clearRect(0,0,1280,720);
        const g=c.createLinearGradient(0,0,0,720);g.addColorStop(0,'#0b1421');g.addColorStop(.65,'#1e2b36');g.addColorStop(1,'#101822');c.fillStyle=g;c.fillRect(0,0,1280,720);
        c.fillStyle='#1b2734';c.fillRect(0,240,1280,230);
        c.strokeStyle='#36434e';c.lineWidth=1;
        for(let row=0;row<5;row++) for(let x=-80;x<1360;x+=160)c.strokeRect(x+(row%2)*80,250+row*42,160,42);
        for(const x of [95,345,595,845,1095]) {
            c.fillStyle='#111b26';c.fillRect(x,120,52,355);c.fillStyle='#283643';c.fillRect(x-8,125,68,18);c.fillRect(x-10,448,72,28);
            c.fillStyle='#344451';c.fillRect(x+6,150,5,290);
        }
        c.fillStyle='#17232d';c.fillRect(0,477,1280,243);c.strokeStyle='#34424c';
        for(let y=485;y<740;y+=45){c.beginPath();c.moveTo(0,y);c.lineTo(1280,y);c.stroke();}
        for(let x=-700;x<2000;x+=145){c.beginPath();c.moveTo(640+(x-640)*.38,477);c.lineTo(x,720);c.stroke();}
        c.fillStyle='#536374';c.font='11px sans-serif';c.fillText('CHEONDAN / FRACTURE CHAMBER',28,690);
    }
    body(t,feet,sign) {
        const c=this.w,G=CheondanGeometry;
        c.fillStyle='rgba(0,0,0,.5)';c.beginPath();c.ellipse(feet[0],feet[1],24,7,0,0,Math.PI*2);c.fill();
        if(!this.image.complete||!this.image.naturalWidth) return;
        const charge=G.ease(t/.65),swing=G.ease((t-.65)/.17);
        let pose=t<.65?150+110*charge:t<.82?260+95*swing:Math.min(809,355+(t-.82)*700);
        const durations=[150,130,35,35,45,45,40,40,130,160],cells=[0,1,2,3,3,4,4,4,5,0];
        let i=0;for(;i<durations.length-1&&pose>=durations[i];i++)pose-=durations[i];
        c.save();c.translate(...feet);c.scale(sign,1);c.imageSmoothingEnabled=false;
        c.drawImage(this.image,cells[i]*256,650,256,374,-81.92*.32,-186.24*.32,256*.64*.32,374*.64*.32);c.restore();
    }
    fractureBackground(t,sign) {
        const G=CheondanGeometry,c=this.ctx;
        const fade=1-G.ease((t-1.5)/.39), separation=t>=1.05&&t<1.89?13*G.ease((t-1.05)/.085)*fade:0;
        if(separation<=0){c.drawImage(this.world,0,0);return;}
        const [a,b]=G.line(sign),len=Math.hypot(b[0]-a[0],b[1]-a[1]);
        const tangent=[(b[0]-a[0])/len,(b[1]-a[1])/len],n=[-tangent[1],tangent[0]],progress=G.ease((t-1.05)/.23);
        c.drawImage(this.world,0,0);
        // Clip and displace both halves of the live scene, never an effect sprite.
        for(const side of [-1,1]) {
            c.save();c.beginPath();
            const path=[];
            for(let i=-1;i<=97;i++){
                const u=i<0?-3:i>96?4:i/96,p=G.mix(a,b,u);
                const off=G.edge(u)*3;p[0]+=n[0]*off;p[1]+=n[1]*off;path.push(p);
            }
            c.moveTo(...path[0]);path.slice(1).forEach(p=>c.lineTo(...p));
            c.lineTo(path.at(-1)[0]+n[0]*side*4000,path.at(-1)[1]+n[1]*side*4000);
            c.lineTo(path[0][0]+n[0]*side*4000,path[0][1]+n[1]*side*4000);c.closePath();c.clip();
            // Small bands follow the same propagating front as the game's shader.
            for(let i=-1;i<34;i++) {
                const u=i/32,v=(i+1)/32,p=G.mix(a,b,u),q=G.mix(a,b,v);
                const spread=1-G.ease((u-progress)/.09),d=side*separation*spread;
                c.save();c.beginPath();
                [[p[0]-n[0]*3000,p[1]-n[1]*3000],[q[0]-n[0]*3000,q[1]-n[1]*3000],[q[0]+n[0]*3000,q[1]+n[1]*3000],[p[0]+n[0]*3000,p[1]+n[1]*3000]].forEach((v,j)=>j?c.lineTo(...v):c.moveTo(...v));
                c.closePath();c.clip();c.drawImage(this.world,(n[0]+tangent[0]*.22)*d,(n[1]+tangent[1]*.22)*d);c.restore();
            }
            c.restore();
        }
    }
    render() {
        const t=this.time,sign=Number(document.getElementById('direction').value),feet=[sign===1?420:860,550];
        this.background();this.body(t,feet,sign);CheondanGeometry.cast(this.w,t,feet,sign);
        this.fractureBackground(t,sign);CheondanGeometry.screen(this.ctx,t,sign);
        this.slider.value=t;document.getElementById('clock').textContent=`${t.toFixed(2)} s`;
        const phase=t<.65?0:t<.82?1:t<1.05?2:t<1.89?3:4;
        document.getElementById('phase').textContent=['검기 응축 · 성장','화면을 향한 사선 베기','거대한 잔상','화면 균열 · 배경 어긋남','다음 시전 대기'][phase];
        document.querySelectorAll('[data-time]').forEach((b,i)=>b.classList.toggle('active',i===phase));
    }
    frame(now) {
        const dt=this.last?Math.min((now-this.last)/1000,.1):0;this.last=now;
        if(this.running) {
            this.time+=dt*Number(document.getElementById('speed').value);
            if(this.time>=2.6){if(document.getElementById('loop').checked)this.time%=2.6;else{this.time=2.6;this.setRunning(false);}}
        }
        this.render();requestAnimationFrame(t=>this.frame(t));
    }
}
new CheondanPreview();
