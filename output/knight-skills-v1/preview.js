'use strict';

class SpriteRenderer
{
    constructor(images)
    {
        this.images = images;
    }

    draw(ctx, key, frame, x, y, scale, pivot, angle = 0, opacity = 1)
    {
        ctx.save();
        ctx.globalAlpha = opacity;
        ctx.translate(x, y);
        ctx.rotate(angle * Math.PI / 180);
        const [sx, sy, sw, sh] = frame.rect;
        ctx.drawImage(this.images[key], sx, sy, sw, sh,
            -pivot[0] * scale, -pivot[1] * scale, sw * scale, sh * scale);
        ctx.restore();
    }
}

class SkillPreview
{
    constructor(spec, renderer, data)
    {
        this.spec = spec;
        this.renderer = renderer;
        this.data = data;
        this.time = 0;
        this.playing = true;
        this.loop = true;
        this.element = document.createElement('article');
        this.element.innerHTML = `<header><h2>${spec.title}</h2><span class="tag">${spec.tag}</span></header>
            <p class="subtitle">${spec.description}</p>
            <canvas width="1080" height="380" aria-label="${spec.title} 재생 화면"></canvas>
            <div class="panel"><div class="controls">
            <button data-action="play">일시정지</button><button data-action="restart">처음부터</button>
            <button data-action="previous">이전 프레임</button><button data-action="next">다음 프레임</button>
            <label><input data-setting="loop" type="checkbox" checked>반복</label>
            <label><input data-setting="effects" type="checkbox" checked>이펙트</label>
            </div><div class="timeline"><input type="range" data-setting="time" min="0" max="${spec.duration}" value="0" step="1" aria-label="${spec.title} 시간"><output data-output="time"></output></div>
            <p class="status" role="status"></p><details><summary>효과 위치·크기 조정</summary><div class="settings">
            <label>가로 <input data-setting="x" type="range" min="-100" max="100" value="0"><output>0</output></label>
            <label>세로 <input data-setting="y" type="range" min="-100" max="100" value="0"><output>0</output></label>
            <label>크기 <input data-setting="scale" type="range" min="0.3" max="2" step="0.05" value="1"><output>1</output></label>
            <button data-action="reset">조정 초기화</button></div></details></div>`;
        document.getElementById('cards').append(this.element);
        this.ctx = this.element.querySelector('canvas').getContext('2d');
        this.element.querySelector('[data-action="play"]').onclick = () => this.setPlaying(!this.playing);
        this.element.querySelector('[data-action="restart"]').onclick = () => { this.time = 0; this.setPlaying(true); };
        this.element.querySelector('[data-action="previous"]').onclick = () => this.step(-1);
        this.element.querySelector('[data-action="next"]').onclick = () => this.step(1);
        this.setting('time').oninput = e => { this.time = Number(e.target.value); this.setPlaying(false); };
        for (const key of ['x','y','scale'])
        {
            this.setting(key).oninput = e => e.target.nextElementSibling.value = e.target.value;
        }
        this.element.querySelector('[data-action="reset"]').onclick = () =>
        {
            for (const key of ['x','y','scale'])
            {
                this.setting(key).value = key === 'scale' ? 1 : 0;
                this.setting(key).nextElementSibling.value = this.setting(key).value;
            }
        };
    }

    setting(key) { return this.element.querySelector(`[data-setting="${key}"]`); }

    setPlaying(value)
    {
        if (value && this.time >= this.spec.duration) { this.time = 0; }
        this.playing = value;
        this.element.querySelector('[data-action="play"]').textContent = value ? '일시정지' : '재생';
    }

    events()
    {
        if (this.spec.id === 'thrust')
        {
            const hits = this.data.strikes;
            const recovery = hits[hits.length - 1].time + 145;
            return [...new Set([0, hits[0].time - 45, ...hits.flatMap(s =>
                [s.time, s.time + 30, s.time + 65, s.time + 110]),
                recovery, recovery + 135, this.spec.duration])].sort((a,b) => a-b);
        }
        if (this.spec.id === 'aura') { return Array.from({length:17}, (_,i) => i*100); }
        return [0,130,270,360,410,480,560,680,800,920,1080,1200,1400,1800];
    }

    step(direction)
    {
        this.setPlaying(false);
        const points = this.events();
        this.time = direction > 0 ? (points.find(t => t > this.time + 0.1) ?? 0) :
            ([...points].reverse().find(t => t < this.time - 0.1) ?? this.spec.duration);
    }

    advance(dt)
    {
        if (this.playing)
        {
            this.time += dt;
            if (this.time >= this.spec.duration)
            {
                if (this.setting('loop').checked) { this.time %= this.spec.duration; }
                else { this.time = this.spec.duration; this.setPlaying(false); }
            }
        }
        this.render();
    }

    render()
    {
        const ctx = this.ctx;
        ctx.imageSmoothingEnabled = false;
        ctx.fillStyle = document.getElementById('background').value;
        ctx.fillRect(0,0,1080,380);
        ctx.strokeStyle = 'rgba(145,168,172,.22)';
        ctx.beginPath();ctx.moveTo(40,310);ctx.lineTo(1040,310);ctx.stroke();
        ctx.save();
        if (document.getElementById('flip').checked) { ctx.translate(1080,0);ctx.scale(-1,1); }
        this.fx = {x:Number(this.setting('x').value),y:Number(this.setting('y').value),
            scale:Number(this.setting('scale').value),enabled:this.setting('effects').checked};
        let label;
        if (this.spec.id === 'thrust') { label = this.thrust(); }
        else if (this.spec.id === 'aura') { label = this.aura(); }
        else { label = this.wave(); }
        ctx.restore();
        this.setting('time').value = this.time;
        this.element.querySelector('[data-output="time"]').value = `${Math.round(this.time)} ms`;
        this.element.querySelector('.status').textContent = label;
    }

    marker(x,y)
    {
        if (!document.getElementById('guides').checked) { return; }
        this.ctx.strokeStyle = '#f4bb66';
        this.ctx.beginPath();this.ctx.moveTo(x-7,y);this.ctx.lineTo(x+7,y);
        this.ctx.moveTo(x,y-7);this.ctx.lineTo(x,y+7);this.ctx.stroke();
    }

    thrust()
    {
        const hits = this.data.strikes;
        const firstHit = hits[0].time;
        const recovery = hits[hits.length - 1].time + 145;
        // One thrust motion triggers six slightly staggered, parallel effects.
        const frameIndex = this.time < firstHit - 45 ? 0 :
            this.time < firstHit ? 1 : this.time < recovery ? 2 :
            this.time < recovery + 135 ? 3 : 0;
        const frame = this.data.thrust[frameIndex];
        const scale = 0.44;
        this.renderer.draw(this.ctx,'thrust',frame,310,310,scale,frame.feet);
        const strikePose = this.data.thrust[2];
        const tipX = 310 + (strikePose.tip[0]-strikePose.feet[0])*scale;
        const tipY = 310 + (strikePose.tip[1]-strikePose.feet[1])*scale;
        for (const hit of hits)
        {
            const t = this.time-hit.time;
            if (t<0 || t>=145) { continue; }
            const phase = t<30?0:t<65?1:t<110?2:3;
            const effect = this.data.thrustFx[phase];
            const x=tipX+hit.x+this.fx.x, y=tipY+hit.y+this.fx.y;
            if (this.fx.enabled)
            {
                this.renderer.draw(this.ctx,'thrustFx',effect,x,y,0.43*this.fx.scale,effect.pivot,0);
            }
            this.marker(x,y);
        }
        const count = hits.filter(s => this.time >= s.time).length;
        return `동작 1회 · 이펙트 ${count}/6개 · 30ms 시간차 · 동일 각도 · 모션 프레임 ${frameIndex+1}/4`;
    }

    aura()
    {
        const frame={rect:[0,0,384,512]};
        const scale=0.61;
        this.renderer.draw(this.ctx,'walk',frame,370,310,scale,[192,453]);
        // Hilt/tip measured on the existing downward-right resting sword.
        const x=370+(149-192)*scale+this.fx.x;
        const y=310+(374-453)*scale+this.fx.y;
        const index=Math.floor(this.time/100)%4;
        const effect=this.data.aura[index];
        if (this.fx.enabled)
        {
            this.renderer.draw(this.ctx,'aura',effect,x,y,0.18*this.fx.scale,effect.pivot,42);
        }
        this.marker(x,y);
        return `검기 ${index+1}/4 · 검 손잡이 기준 중첩 · 패시브 반복`;
    }

    wave()
    {
        const t=this.time;
        const index=t<130?0:t<270?1:t<480?2:3;
        const frame=this.data.slash[index];
        this.renderer.draw(this.ctx,'wave',frame,280,310,0.61,frame.feet);
        const flight=(t-360)*2;
        if (flight>=0 && flight<840 && this.fx.enabled)
        {
            const phase=flight<90?0:flight<180?1:flight<620?2:3;
            const effect=this.data.waveFx[phase];
            const x=450+flight*0.51+this.fx.x;
            const y=222+this.fx.y;
            const opacity=flight<620?1:Math.max(0,1-(flight-620)/220);
            this.renderer.draw(this.ctx,'wave',effect,x,y,0.65*this.fx.scale,effect.pivot,0,opacity);
            this.marker(x,y);
        }
        return t<360?`준비·방출 모션 ${index+1}/4`:flight<840?'검기 독립 이동 (속도 2배) · 캐릭터 회복 동작': '동작 종료';
    }
}

async function startKnightPreviews()
{
    const data=window.KnightPreviewData;
    const images={};
    await Promise.all(Object.entries(data.images).map(async ([name,path]) =>
    {
        const image=new Image();
        image.src=path;
        await image.decode();
        images[name]=image;
    }));
    const renderer=new SpriteRenderer(images);
    const previews=data.skills.map(spec => new SkillPreview(spec,renderer,data));
    document.getElementById('playAll').onclick=() => previews.forEach(p => p.setPlaying(true));
    document.getElementById('pauseAll').onclick=() => previews.forEach(p => p.setPlaying(false));
    let previous=null;
    function tick(now)
    {
        const dt=previous===null?0:Math.min(now-previous,100);
        previous=now;
        const speed=Number(document.getElementById('speed').value);
        previews.forEach(p => p.advance(document.hidden?0:dt*speed));
        requestAnimationFrame(tick);
    }
    requestAnimationFrame(tick);
    window.knightPreviews=previews;
}

startKnightPreviews().catch(error =>
{
    const message=document.createElement('p');
    message.setAttribute('role','alert');
    message.textContent=`이미지를 불러오지 못했습니다: ${error.message}. 미리보기 폴더와 원본 PNG의 위치를 확인해 주세요.`;
    document.getElementById('cards').append(message);
});
