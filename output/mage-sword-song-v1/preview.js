'use strict';

class SongGeometry
{
    static ease(x) { x=Math.max(0,Math.min(1,x));return x*x*(3-2*x); }
    static point(c,r,a,d) { return [c[0]+Math.cos(a)*r,c[1]+Math.sin(a)*r*d]; }
    static line(ctx,a,b,width,color)
    {
        ctx.strokeStyle=color;ctx.lineWidth=width;ctx.beginPath();ctx.moveTo(...a);ctx.lineTo(...b);ctx.stroke();
    }
    static ring(ctx,c,r,d,width,color)
    {
        if(r<=0)return;
        ctx.strokeStyle=color;ctx.lineWidth=width;ctx.beginPath();ctx.ellipse(...c,r,r*d,0,0,Math.PI*2);ctx.stroke();
    }
    static disc(ctx,c,r,d,color)
    {
        ctx.fillStyle=color;ctx.beginPath();ctx.ellipse(...c,r,r*d,0,0,Math.PI*2);ctx.fill();
    }
    static seal(ctx,c,r,d,angle,color,alpha)
    {
        if(r<1)return;
        ctx.save();ctx.globalAlpha=alpha;
        this.disc(ctx,c,r,d,`rgba(${color},.035)`);
        this.ring(ctx,c,r,d,12,`rgba(${color},.1)`);
        for(const [k,w] of [[1,3.2],[.92,1.6],[.76,2],[.39,2.4]])this.ring(ctx,c,r*k,d,w,`rgb(${color})`);
        for(let i=0;i<48;i++)
        {
            const a=angle+i*Math.PI*2/48;
            this.line(ctx,this.point(c,r*.96,a,d),this.point(c,r*(i%4?.99:1.04),a,d),1.4,`rgb(${color})`);
        }
        for(let i=0;i<18;i++)
        {
            const a=-angle+i*Math.PI*2/18,p=this.point(c,r*.83,a,d),ink=`rgb(${color})`;
            this.line(ctx,this.point(p,9,a,d),this.point(p,9,a+Math.PI,d),1.8,ink);
            this.line(ctx,this.point(p,9,a+Math.PI,d),this.point(p,8,a+Math.PI*.4,d),1.8,ink);
            this.line(ctx,this.point(p,8,a+Math.PI*.4,d),this.point(p,8,a-Math.PI*.4,d),1.8,ink);
        }
        for(let i=0;i<6;i++)this.line(ctx,this.point(c,r*.72,angle+i*Math.PI/3,d),this.point(c,r*.72,angle+(i+2)*Math.PI/3,d),2,`rgb(${color})`);
        this.ring(ctx,c,r*.12,d,4,'#ddf7ff');ctx.restore();
    }
    static sword(ctx,p,direction,alpha,upper)
    {
        const color=upper?'105,214,255':'196,143,255';
        ctx.save();ctx.translate(...p);ctx.scale(1,direction);ctx.globalAlpha=alpha;
        ctx.fillStyle=`rgba(${color},.08)`;ctx.beginPath();ctx.moveTo(0,0);ctx.lineTo(-8,-62);ctx.lineTo(8,-62);ctx.fill();
        ctx.fillStyle=`rgb(${color})`;ctx.beginPath();ctx.moveTo(0,0);ctx.lineTo(-4,-14);ctx.lineTo(-3,-39);ctx.lineTo(3,-39);ctx.closePath();ctx.fill();
        this.line(ctx,[0,0],[0,-40],1.6,'#e8faff');this.line(ctx,[-8,-39],[8,-39],2.8,`rgb(${color})`);this.line(ctx,[0,-39],[0,-49],2.6,`rgb(${color})`);ctx.restore();
    }
}

class SwordSongEffect
{
    drawFloor(ctx,t,c,decorationsOnly=false)
    {
        if(t>=5.8)return;
        const G=SongGeometry,g=G.ease(t/.7),m=G.ease((t-4.7)/.55),fade=1-G.ease((t-5.25)/.55);
        G.ring(ctx,c,300*g,.866,3,`rgba(92,166,255,${.24*fade})`);
        if(!decorationsOnly&&t<5.25)G.seal(ctx,[c[0],c[1]-115*m],300*g*(1-.78*m),.866+(.34-.866)*m,t*.27,'166,115,255',g*.8);
    }
    drawSky(ctx,t,c,decorationsOnly=false)
    {
        if(t>=5.8)return;
        const G=SongGeometry,g=G.ease(t/.7),m=G.ease((t-4.7)/.55),core=[c[0],c[1]-115];
        if(!decorationsOnly&&t<5.25)G.seal(ctx,[c[0],c[1]-230+115*m],300*g*(1-.78*m),.34,-t*.34,'82,204,255',g*.9);
        for(let pulse=0;pulse<40;pulse++)
        {
            const impact=.7+(pulse+1)*.1,start=Math.max(.7,impact-.24);
            if(t<start||t>=impact+.08)continue;
            const progress=Math.max(0,Math.min(1,(t-start)/(impact-start))),alpha=1-G.ease((t-impact)/.08);
            for(let s=0;s<4;s++)
            {
                const upper=s%2===0,a=(pulse*4+s)*2.399963,r=35+225*Math.sqrt(((pulse*7+s*13)%37)/37);
                const floor=G.point(c,r,a,.866),sky=G.point([c[0],c[1]-230],r,a,.34);
                const from=upper?sky:floor,to=upper?floor:sky;
                G.sword(ctx,[from[0]+(to[0]-from[0])*progress,from[1]+(to[1]-from[1])*progress],upper?1:-1,alpha,upper);
            }
        }
        if(decorationsOnly)return;
        if(m>0&&t<5.25)
        {
            G.disc(ctx,core,8+36*m,1,`rgba(171,196,255,${m*.16})`);G.ring(ctx,core,15+45*m,1,3.6,`rgba(204,232,255,${m*.75})`);
        }
        if(t>=5.25)
        {
            const age=t-5.25,e=G.ease(age/.4),f=1-G.ease(age/.55);
            G.disc(ctx,core,35+180*e,(35+130*e)/(35+180*e),`rgba(133,166,255,${.18*f})`);
            G.disc(ctx,core,12+55*e,1,`rgba(232,245,255,${.85*(1-G.ease(age/.22))})`);
            G.ring(ctx,core,30+340*e,.62,2*(5*(1-e)+1),`rgba(163,227,255,${.9*f})`);
            G.ring(ctx,c,300*e,.866,8,`rgba(176,122,255,${.7*f})`);
            for(let i=0;i<18;i++)G.line(ctx,G.point(core,20+70*e,i*Math.PI*2/18,.7),G.point(core,80+220*e,i*Math.PI*2/18,.7),3*f,`rgba(181,214,255,${f*.65})`);
        }
    }
}

class SwordSongPreview
{
    constructor()
    {
        this.canvas=document.getElementById('stage');this.effect=new SwordSongEffect();this.gpu=null;
        try { this.gpu=new ArcaneGpuPreview(this.canvas); }
        catch(error)
        {
            console.warn('Arcane preview fallback:',error);
            const replacement=this.canvas.cloneNode();this.canvas.replaceWith(replacement);this.canvas=replacement;
            document.getElementById('gpu-status').textContent='이 브라우저에서 고급 효과를 사용할 수 없어 기본 도형으로 표시합니다.';
        }
        this.background=document.createElement('canvas');this.background.width=1280;this.background.height=760;
        this.actors=this.background.cloneNode();this.swords=this.background.cloneNode();
        this.ctx=this.gpu?this.background.getContext('2d'):this.canvas.getContext('2d');
        this.time=0;this.running=true;this.last=0;this.slider=document.getElementById('time');this.play=document.getElementById('play');
        this.image=new Image();this.image.src=window.ArcaneCharacterImage;
        this.play.onclick=()=>this.setPlaying(!this.running);
        document.getElementById('restart').onclick=()=>{this.time=0;this.setPlaying(true);};
        this.slider.oninput=()=>{this.time=Number(this.slider.value);this.setPlaying(false);this.draw();};
        document.querySelectorAll('[data-time]').forEach(b=>b.onclick=()=>{this.time=Number(b.dataset.time);this.setPlaying(false);this.draw();});
        document.addEventListener('visibilitychange',()=>{this.last=0;});
        requestAnimationFrame(t=>this.frame(t));
    }
    setPlaying(v) { if(v&&this.time>=6.5)this.time=0;this.running=v;this.play.textContent=v?'일시정지':'재생'; }
    draw()
    {
        let c=this.ctx;const t=this.time,center=[640,450];
        const gradient=c.createLinearGradient(0,0,0,760);gradient.addColorStop(0,'#080f20');gradient.addColorStop(1,'#1b2637');c.fillStyle=gradient;c.fillRect(0,0,1280,760);
        c.strokeStyle='#2a384b';c.lineWidth=1;
        for(let y=300;y<760;y+=60)SongGeometry.line(c,[0,y],[1280,y],1,'#2a384b');
        for(let x=-400;x<1700;x+=100)SongGeometry.line(c,[640+(x-640)*.5,300],[x,760],1,'#2a384b');
        this.effect.drawFloor(c,t,center,!!this.gpu);
        if(this.gpu){c=this.actors.getContext('2d');c.clearRect(0,0,1280,760);}
        SongGeometry.disc(c,center,22,.3,'#090d19');
        if(this.image.complete&&this.image.naturalWidth)
        {
            c.imageSmoothingEnabled=false;
            c.drawImage(this.image,768,0,384,480,center[0]-192*.18,center[1]-455*.18,384*.18,480*.18);
        }
        if(this.gpu){c=this.swords.getContext('2d');c.clearRect(0,0,1280,760);}
        this.effect.drawSky(c,t,center,!!this.gpu);
        if(this.gpu)this.gpu.compose(this.background,this.actors,this.swords,t,center);
        const stage=t<.7?'쌍마법진 소환':t<4.7?'검 세례':t<5.25?'두 마법진 합체':t<5.8?'합체 폭발':'다음 시전 대기';
        const hits=Math.max(0,Math.min(40,Math.floor((t-.7+1e-7)*10)));
        document.getElementById('phase').textContent=`${stage} · 범위 타격 ${hits} / 40 · 폭발 ${t>=5.25?'1 / 1':'0 / 1'}`;
        this.slider.value=t;document.getElementById('status').textContent=`${t.toFixed(2)} s`;
    }
    frame(now)
    {
        const dt=this.last?Math.min((now-this.last)/1000,.1):0;this.last=now;
        if(this.running)
        {
            this.time+=dt*Number(document.getElementById('speed').value);
            if(this.time>=6.5){if(document.getElementById('loop').checked)this.time%=6.5;else{this.time=6.5;this.setPlaying(false);}}
        }
        this.draw();requestAnimationFrame(t=>this.frame(t));
    }
}
new SwordSongPreview();
