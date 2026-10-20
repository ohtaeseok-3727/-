import { KnightMotion, ASSET_ROOT, CYCLE, poseAt } from './motion.js';
import { SwordEffects, PALETTES } from './sword-effects.js';
import { Arena } from './arena.js';

function loadImage(name)
{
    return new Promise((resolve, reject) =>
    {
        const image = new Image();
        image.onload = () => resolve(image);
        image.onerror = () => reject(new Error(`${name} 이미지를 불러오지 못했습니다.`));
        image.src = ASSET_ROOT + name;
    });
}

class SwordStudio
{
    constructor(attack, reference)
    {
        this.canvas = document.querySelector('#stage');
        this.ctx = this.canvas.getContext('2d');
        this.motion = new KnightMotion(attack, reference);
        this.effects = new SwordEffects(this.motion);
        this.arena = new Arena();
        this.time = 0;
        this.last = null;
        this.playing = true;
        this.kind = 'slash';
        this.elements = Object.fromEntries(['play', 'replay', 'peak', 'speed', 'timeline', 'render-mode', 'palette', 'aura', 'sparks', 'phase', 'attack-name', 'time-readout'].map(id => [id, document.getElementById(id)]));
        this.bindControls();
        document.querySelector('#loading').hidden = true;
        for (const button of document.querySelectorAll('button')) button.disabled = false;
        requestAnimationFrame(stamp => this.frame(stamp));
    }

    bindControls()
    {
        const ui = this.elements;
        ui.play.onclick = () =>
        {
            this.playing = !this.playing;
            this.last = null;
        };
        ui.replay.onclick = () =>
        {
            this.time = 0;
            this.playing = true;
            this.last = null;
        };
        ui.peak.onclick = () =>
        {
            this.time = this.kind === 'slash' ? 0.421 : 0.445;
            this.playing = false;
        };
        ui.timeline.oninput = () =>
        {
            this.time = Number(ui.timeline.value) / 1000;
            this.playing = false;
        };
        for (const button of document.querySelectorAll('[data-attack]'))
        {
            button.onclick = () =>
            {
                this.kind = button.dataset.attack;
                this.time = 0;
                this.playing = true;
                for (const other of document.querySelectorAll('[data-attack]'))
                {
                    const active = other === button;
                    other.classList.toggle('active', active);
                    other.setAttribute('aria-pressed', String(active));
                }
            };
        }
        document.addEventListener('visibilitychange', () => { this.last = null; });
    }

    draw()
    {
        const ctx = this.ctx;
        const ui = this.elements;
        const mode = ui['render-mode'].value;
        const options = { palette: ui.palette.value, aura: ui.aura.checked, sparks: ui.sparks.checked };
        this.arena.draw(ctx, this.time);
        if (mode === 'new') this.effects.groundLight(ctx, this.kind, this.time, PALETTES[options.palette]);
        this.motion.draw(ctx, this.kind, this.time);
        if (mode === 'new') this.effects.draw(ctx, this.kind, this.time, options);
        if (mode === 'original') this.motion.drawOriginal(ctx, this.kind, this.time);
        ui.play.textContent = this.playing ? '일시정지' : '계속 재생';
        ui.timeline.value = Math.round(this.time * 1000);
        ui['time-readout'].textContent = `${this.time.toFixed(2)} s`;
        ui['attack-name'].textContent = this.kind === 'slash' ? 'HORIZONTAL SLASH' : 'DESCENDING SLASH';
        const { step } = poseAt(this.time);
        ui.phase.textContent = step < 2 ? '준비 · 검날에 모이는 빛' : step < 5 ? '휘두름 · 금빛 궤적' : step < 8 ? '잔광 · 흩어지는 입자' : '회복 · 다음 공격';
    }

    frame(stamp)
    {
        if (this.last !== null && this.playing)
        {
            this.time = (this.time + Math.min(0.08, (stamp - this.last) / 1000) * Number(this.elements.speed.value)) % CYCLE;
        }
        this.last = stamp;
        this.draw();
        requestAnimationFrame(next => this.frame(next));
    }
}

Promise.all([loadImage('attacks-transparent-v2.png'), loadImage('effects-transparent-v2.png')])
    .then(([attack, reference]) => new SwordStudio(attack, reference))
    .catch(error => { document.querySelector('#loading').textContent = error.message; });
