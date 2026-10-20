import { clamp } from './motion.js';

export const PALETTES = {
    gold: { rgb: '255, 191, 83', edge: '#efac42', core: '#fff8d9' },
    silver: { rgb: '195, 215, 244', edge: '#a3bfdb', core: '#ffffff' },
    ice: { rgb: '82, 204, 247', edge: '#59bde5', core: '#e1fcff' }
};

function stroke(ctx, points, color, width)
{
    ctx.beginPath();
    points.forEach((p, i) => i ? ctx.lineTo(p.x, p.y) : ctx.moveTo(p.x, p.y));
    ctx.strokeStyle = color;
    ctx.lineWidth = width;
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';
    ctx.stroke();
}

function cubic(a, b, c, d, t)
{
    const u = 1 - t;
    return { x: u ** 3 * a[0] + 3 * u * u * t * b[0] + 3 * u * t * t * c[0] + t ** 3 * d[0],
        y: u ** 3 * a[1] + 3 * u * u * t * b[1] + 3 * u * t * t * c[1] + t ** 3 * d[1] };
}

export class SwordEffects
{
    constructor(motion)
    {
        this.motion = motion;
    }

    point(kind, u)
    {
        // The horizontal bowl and descending crescent follow the original effect sheet.
        const p = kind === 'slash'
            ? cubic([-103, -149], [-147, -23], [192, 19], [124, -89], u)
            : cubic([-60, -228], [38, -216], [139, -68], [73, 4], u);
        return { x: p.x + this.motion.x, y: p.y + this.motion.y };
    }

    ribbon(ctx, kind, from, to, width, color)
    {
        if (to - from < 0.002)
        {
            return;
        }
        const outer = [];
        const inner = [];
        for (let i = 0; i <= 70; i++)
        {
            const fraction = i / 70;
            const u = from + (to - from) * fraction;
            const p = this.point(kind, u);
            const next = this.point(kind, Math.min(1, u + 0.001));
            const prev = this.point(kind, Math.max(0, u - 0.001));
            const dx = next.x - prev.x;
            const dy = next.y - prev.y;
            const length = Math.hypot(dx, dy) || 1;
            const taper = Math.pow(Math.sin(fraction * Math.PI), 0.65);
            const thickness = width * taper * (0.38 + 0.62 * fraction);
            outer.push({ x: p.x - dy / length * thickness, y: p.y + dx / length * thickness });
            inner.push({ x: p.x + dy / length * thickness * 0.12, y: p.y - dx / length * thickness * 0.12 });
        }
        ctx.beginPath();
        ctx.moveTo(outer[0].x, outer[0].y);
        for (const p of outer.slice(1)) ctx.lineTo(p.x, p.y);
        for (const p of inner.reverse()) ctx.lineTo(p.x, p.y);
        ctx.closePath();
        ctx.fillStyle = color;
        ctx.fill();
    }

    aura(ctx, kind, time, palette)
    {
        const blade = this.motion.blade(kind, time);
        const pulse = 0.65 + Math.sin(time * 14) * 0.12;
        ctx.save();
        ctx.globalCompositeOperation = 'lighter';
        ctx.shadowColor = palette.edge;
        ctx.shadowBlur = 17;
        stroke(ctx, blade, `rgba(${palette.rgb}, ${pulse * 0.17})`, 14);
        ctx.shadowBlur = 5;
        stroke(ctx, blade, `rgba(${palette.rgb}, ${pulse * 0.5})`, 5);
        stroke(ctx, blade, palette.core, 1.5);
        ctx.restore();
    }

    groundLight(ctx, kind, time, palette)
    {
        const power = clamp((time - 0.25) / 0.08) * (1 - clamp((time - 0.42) / 0.35));
        if (!power) return;
        ctx.save();
        ctx.translate(this.motion.x + 60, this.motion.y + 2);
        ctx.scale(1, 0.2);
        const g = ctx.createRadialGradient(0, 0, 4, 0, 0, 230);
        g.addColorStop(0, `rgba(${palette.rgb}, ${power * 0.2})`);
        g.addColorStop(1, `rgba(${palette.rgb}, 0)`);
        ctx.fillStyle = g;
        ctx.fillRect(-230, -230, 460, 460);
        ctx.restore();
    }

    draw(ctx, kind, time, options)
    {
        const palette = PALETTES[options.palette];
        if (options.aura) this.aura(ctx, kind, time, palette);
        const age = time - 0.28;
        if (age < 0 || age > 0.75) return;
        const sweep = 0.15;
        const head = clamp(age / sweep);
        const tail = clamp((age - 0.17) / 0.38);
        const opacity = (1 - clamp((age - 0.15) / 0.48)) ** 1.4;
        ctx.save();
        ctx.globalCompositeOperation = 'lighter';
        ctx.globalAlpha = opacity;
        // Broad low-energy bloom, a colored body, and a narrow white-hot edge.
        ctx.shadowColor = palette.edge;
        ctx.shadowBlur = 25;
        this.ribbon(ctx, kind, tail, head, 36, `rgba(${palette.rgb}, 0.12)`);
        ctx.shadowBlur = 9;
        this.ribbon(ctx, kind, tail, head, 23, `rgba(${palette.rgb}, 0.48)`);
        ctx.shadowBlur = 0;
        this.ribbon(ctx, kind, tail, head, 12, palette.core);
        this.ribbon(ctx, kind, Math.min(head, tail + 0.03), head, 4, '#ffffff');
        for (let strand = 0; strand < 3; strand++)
        {
            const points = [];
            const begin = Math.min(head, tail + strand * 0.07);
            for (let j = 0; j <= 45; j++)
            {
                const u = begin + (head - begin) * j / 45;
                const p = this.point(kind, u);
                p.x += (strand - 1) * 4;
                p.y += 9 + strand * 7;
                points.push(p);
            }
            stroke(ctx, points, `rgba(${palette.rgb}, ${0.32 - strand * 0.08})`, 0.8);
        }
        ctx.restore();
        if (options.sparks) this.particles(ctx, kind, age, palette);
        if (kind === 'chop') this.impact(ctx, age - sweep, palette, options.sparks);
    }

    particles(ctx, kind, age, palette)
    {
        ctx.save();
        ctx.globalCompositeOperation = 'lighter';
        for (let i = 0; i < 60; i++)
        {
            const u = (i * 0.618034) % 1;
            const t = age - u * 0.15;
            const life = 0.24 + i % 7 * 0.045;
            if (t < 0 || t > life) continue;
            const start = this.point(kind, u);
            const a = i * 2.39996;
            const vx = Math.cos(a) * (28 + i % 9 * 12) + 35;
            const vy = Math.sin(a) * (24 + i % 5 * 18) - 22;
            const x = start.x + vx * t;
            const y = start.y + vy * t + t * t * 80;
            ctx.globalAlpha = (1 - t / life) ** 2;
            const size = i % 5 === 0 ? 2.4 : 1.2;
            stroke(ctx, [{ x: x - vx * 0.025, y: y - vy * 0.025 }, { x, y }], palette.edge, size);
            ctx.fillStyle = palette.core;
            ctx.fillRect(x - size / 2, y - size / 2, size, size);
            if (i % 13 === 0)
            {
                stroke(ctx, [{ x: x - 4, y }, { x: x + 4, y }], palette.core, 0.6);
                stroke(ctx, [{ x, y: y - 4 }, { x, y: y + 4 }], palette.core, 0.6);
            }
        }
        ctx.restore();
    }

    impact(ctx, age, palette, sparks)
    {
        if (age < 0 || age > 0.45) return;
        const p = this.point('chop', 1);
        const opacity = (1 - age / 0.45) ** 2;
        ctx.save();
        ctx.globalCompositeOperation = 'lighter';
        ctx.globalAlpha = opacity;
        ctx.strokeStyle = palette.edge;
        ctx.lineWidth = 1.4;
        ctx.beginPath();
        ctx.ellipse(p.x, p.y, 12 + age * 230, 3 + age * 42, 0, 0, Math.PI * 2);
        ctx.stroke();
        stroke(ctx, [{ x: p.x - 65 * opacity, y: p.y }, { x: p.x + 65 * opacity, y: p.y }], palette.core, 2);
        if (sparks)
        {
            for (let i = 0; i < 14; i++)
            {
                const vx = (i - 6.5) * 28;
                const vy = -100 - i % 4 * 35;
                const x = p.x + vx * age;
                const y = p.y + vy * age + 250 * age * age;
                stroke(ctx, [{ x: x - vx * 0.03, y: y - (vy + 500 * age) * 0.03 }, { x, y }], palette.core, 1.5);
            }
        }
        ctx.restore();
    }
}
