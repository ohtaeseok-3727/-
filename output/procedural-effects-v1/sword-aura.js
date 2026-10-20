import { line } from './drawing.js';

export class SwordAura
{
    angle(time)
    {
        const progress = Math.max(0, Math.min(1, (time - 0.8) / 0.55));
        return -2.1 + progress * progress * (3 - 2 * progress) * 2.7;
    }

    blade(ctx, angle, color, alpha, ghost = false)
    {
        ctx.save();
        ctx.rotate(angle);
        ctx.globalAlpha = alpha;
        ctx.shadowColor = color;
        ctx.shadowBlur = ghost ? 12 : 22;
        line(ctx, [[22, 0], [145, 0]], color, ghost ? 10 : 15);
        if (!ghost)
        {
            ctx.shadowBlur = 0;
            ctx.fillStyle = '#e5f5ff';
            ctx.beginPath();
            ctx.moveTo(24, -5);
            ctx.lineTo(132, -4);
            ctx.lineTo(151, 0);
            ctx.lineTo(132, 4);
            ctx.lineTo(24, 5);
            ctx.closePath();
            ctx.fill();
            line(ctx, [[20, -16], [20, 16]], '#c3a169', 5);
            line(ctx, [[0, 0], [18, 0]], '#846440', 8);
        }
        ctx.restore();
    }

    draw(ctx, time, color)
    {
        ctx.save();
        ctx.translate(820, 330);
        ctx.globalCompositeOperation = 'lighter';
        for (let i = 14; i >= 1; i--)
        {
            const past = time - i * 0.022;
            if (past >= 0.8 && past <= 1.35 && time < 1.7)
            {
                this.blade(ctx, this.angle(past), color, 0.17 * (1 - i / 15), true);
            }
        }
        this.blade(ctx, this.angle(time), color, 0.8 + Math.sin(time * 9) * 0.15);
        const angle = this.angle(time);
        for (let i = 0; i < 18; i++)
        {
            const phase = (time * 0.7 + i / 18) % 1;
            const length = 25 + phase * 120;
            const offset = Math.sin(time * 6 + i * 2) * 13;
            const x = Math.cos(angle) * length - Math.sin(angle) * offset;
            const y = Math.sin(angle) * length + Math.cos(angle) * offset;
            ctx.globalAlpha = Math.sin(phase * Math.PI) * 0.7;
            ctx.fillStyle = color;
            ctx.beginPath();
            ctx.arc(x, y, 1.5 + i % 3 * 0.5, 0, Math.PI * 2);
            ctx.fill();
        }
        ctx.restore();
    }
}
