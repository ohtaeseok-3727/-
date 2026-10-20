import { Shockwave } from './shockwave.js';
import { SwordAura } from './sword-aura.js';

class EffectsPreview
{
    constructor()
    {
        this.canvas = document.querySelector('#stage');
        this.ctx = this.canvas.getContext('2d');
        this.shockwave = new Shockwave();
        this.sword = new SwordAura();
        this.time = 0;
        this.last = null;
        this.paused = false;
        document.querySelector('#pause').onclick = (event) =>
        {
            this.paused = !this.paused;
            event.target.textContent = this.paused ? '계속 재생' : '일시정지';
        };
        document.querySelector('#replay').onclick = () => { this.time = 0; };
        document.addEventListener('visibilitychange', () => { this.last = null; });
        requestAnimationFrame((stamp) => this.frame(stamp));
    }

    frame(stamp)
    {
        const delta = this.last === null ? 0 : Math.min(0.05, (stamp - this.last) / 1000);
        this.last = stamp;
        if (!this.paused)
        {
            this.time = (this.time + delta * Number(document.querySelector('#speed').value)) % 3;
        }
        const ctx = this.ctx;
        ctx.clearRect(0, 0, 1100, 550);
        ctx.fillStyle = '#1b2733';
        ctx.fillRect(0, 370, 1100, 180);
        ctx.strokeStyle = '#344657';
        ctx.beginPath();
        ctx.moveTo(550, 45);
        ctx.lineTo(550, 505);
        ctx.stroke();
        ctx.fillStyle = '#c9d5e6';
        ctx.font = '20px system-ui';
        ctx.fillText('충격파 · 지면 파편', 35, 55);
        ctx.fillText('검 오라 · 휘두르기 잔상', 590, 55);
        this.shockwave.draw(ctx, this.time);
        this.sword.draw(ctx, this.time, document.querySelector('#color').value);
        requestAnimationFrame((next) => this.frame(next));
    }
}

new EffectsPreview();
