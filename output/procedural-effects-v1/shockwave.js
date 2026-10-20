import { line, fade } from './drawing.js';

export class Shockwave
{
    draw(ctx, time)
    {
        if (time < 0.3 || time > 2.6)
        {
            return;
        }
        const age = time - 0.3;
        ctx.save();
        ctx.translate(275, 370);
        for (let i = 0; i < 3; i++)
        {
            const t = age - i * 0.1;
            if (t < 0 || t > 1.1) continue;
            ctx.globalAlpha = fade(t, 1.1);
            ctx.strokeStyle = i === 0 ? '#ffe3a0' : '#c88b42';
            ctx.lineWidth = 5 - i;
            ctx.beginPath();
            ctx.ellipse(0, 0, 15 + t * 230, 5 + t * 73, 0, 0, Math.PI * 2);
            ctx.stroke();
        }
        ctx.globalAlpha = Math.min(1, age * 8) * Math.min(1, fade(age - 1.2, 1.1));
        for (let i = 0; i < 11; i++)
        {
            const angle = i * Math.PI * 2 / 11;
            const reach = Math.min(1, age * 3);
            const points = [[0, 0]];
            for (let j = 1; j <= 5; j++)
            {
                const a = angle + Math.sin(i * 9 + j * 7) * 0.14;
                points.push([Math.cos(a) * j * 36 * reach, Math.sin(a) * j * 12 * reach]);
            }
            line(ctx, points, '#05090d', 7);
            line(ctx, points, '#d5a15a', 1.5);
        }
        for (let i = 0; i < 24; i++)
        {
            const delay = (i % 5) * 0.025;
            const t = age - delay;
            if (t < 0 || t > 1.6) continue;
            const angle = i * 2.39996;
            const x = Math.cos(angle) * (20 + t * (45 + i % 6 * 16));
            const floor = Math.sin(angle) * (12 + t * 35);
            const height = Math.max(0, (190 + i % 4 * 35) * t - 220 * t * t);
            ctx.globalAlpha = fade(t, 1.6);
            ctx.fillStyle = '#080d14';
            ctx.beginPath();
            ctx.ellipse(x, floor, 9, 3, 0, 0, Math.PI * 2);
            ctx.fill();
            ctx.save();
            ctx.translate(x, floor - height);
            ctx.rotate(t * (i % 2 ? 4 : -3));
            const size = 6 + i % 5 * 2;
            ctx.fillStyle = i % 2 ? '#9b8064' : '#c3a17a';
            ctx.beginPath();
            ctx.moveTo(-size, -size * 0.4);
            ctx.lineTo(size * 0.3, -size);
            ctx.lineTo(size, size * 0.3);
            ctx.lineTo(-size * 0.4, size);
            ctx.closePath();
            ctx.fill();
            ctx.restore();
        }
        ctx.restore();
    }
}
