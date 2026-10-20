import { clamp } from './motion.js';
import { PixelGeometry as Pixel } from './pixel-geometry.js';

export const PALETTES = {
    gold: { rgb: '255, 193, 68', dark: '#855528', edge: '#e5a43a', middle: '#ffda78', core: '#fffbe7' },
    silver: { rgb: '206, 221, 244', dark: '#465974', edge: '#899fbb', middle: '#cadbea', core: '#ffffff' },
    ice: { rgb: '98, 219, 251', dark: '#245974', edge: '#409abb', middle: '#89e7f7', core: '#eeffff' }
};

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
        this.raster = document.createElement('canvas');
        this.raster.width = 300;
        this.raster.height = 163;
        this.ctx = this.raster.getContext('2d');
    }

    point(kind, u)
    {
        const p = kind === 'slash'
            ? cubic([-110, -145], [-130, -12], [197, 23], [125, -90], u)
            : cubic([-65, -235], [40, -211], [143, -66], [74, 2], u);
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
        for (let i = 0; i <= 52; i++)
        {
            const f = i / 52;
            const u = from + (to - from) * f;
            const p = this.point(kind, u);
            const next = this.point(kind, Math.min(1, u + 0.002));
            const prev = this.point(kind, Math.max(0, u - 0.002));
            const dx = next.x - prev.x;
            const dy = next.y - prev.y;
            const length = Math.hypot(dx, dy) || 1;
            // Uneven attached edges; the body remains one continuous silhouette.
            const tooth = [0.95, 1.05, 0.91, 1.17, 1.04, 0.94, 1.1][i % 7];
            const taper = Math.pow(Math.sin(f * Math.PI), 0.48);
            const thickness = width * taper * tooth * (0.52 + 0.48 * f);
            outer.push({ x: p.x - dy / length * thickness, y: p.y + dx / length * thickness });
            inner.push({ x: p.x + dy / length * thickness * 0.12, y: p.y - dx / length * thickness * 0.12 });
        }
        Pixel.polygon(ctx, [...outer, ...inner.reverse()], color);
    }

    aura(ctx, kind, time, palette)
    {
        const blade = this.motion.blade(kind, time);
        ctx.save();
        ctx.globalAlpha = 0.45;
        Pixel.segment(ctx, blade[0], blade[1], 16, palette.edge);
        ctx.globalAlpha = 0.85;
        Pixel.segment(ctx, blade[0], blade[1], 8, palette.middle);
        ctx.globalAlpha = 1;
        Pixel.segment(ctx, blade[0], blade[1], 4, palette.core);
        ctx.restore();
    }

    groundLight(ctx, kind, time, palette)
    {
        const strength = clamp((time - 0.28) / 0.04) * (1 - clamp((time - 0.39) / 0.15));
        if (strength === 0)
        {
            return;
        }
        ctx.save();
        ctx.translate(this.motion.x + 60, this.motion.y + 2);
        ctx.scale(1, 0.17);
        const glow = ctx.createRadialGradient(0, 0, 5, 0, 0, 220);
        glow.addColorStop(0, `rgba(${palette.rgb}, ${strength * 0.28})`);
        glow.addColorStop(1, `rgba(${palette.rgb}, 0)`);
        ctx.fillStyle = glow;
        ctx.fillRect(-220, -220, 440, 440);
        ctx.restore();
    }

    draw(ctx, kind, time, options)
    {
        const palette = PALETTES[options.palette];
        const layer = this.ctx;
        layer.setTransform(1, 0, 0, 1, 0, 0);
        layer.clearRect(0, 0, this.raster.width, this.raster.height);
        layer.setTransform(0.25, 0, 0, 0.25, 0, 0);
        if (options.aura)
        {
            this.aura(layer, kind, time, palette);
        }
        const age = time - 0.28;
        if (age >= 0 && age <= 0.65)
        {
            const progress = clamp(age / 0.105);
            const head = 1 - (1 - progress) ** 2;
            const tail = clamp((age - 0.12) / 0.22);
            const opacity = 1 - clamp((age - 0.13) / 0.2);
            layer.save();
            layer.globalAlpha = opacity;
            this.ribbon(layer, kind, tail, head, 58, palette.dark);
            this.ribbon(layer, kind, tail, head, 51, palette.edge);
            this.ribbon(layer, kind, tail, head, 42, palette.middle);
            this.ribbon(layer, kind, tail, head, 31, palette.core);
            layer.restore();
            const flashAge = age - 0.095;
            if (flashAge >= 0 && flashAge < 0.085)
            {
                const p = this.point(kind, 0.97);
                const radius = (kind === 'chop' ? 42 : 27) * (1 - flashAge / 0.085);
                Pixel.burst(layer, p.x, p.y, radius + 7, palette.edge);
                Pixel.burst(layer, p.x, p.y, radius, palette.core);
            }
            if (kind === 'chop')
            {
                this.impact(layer, age - 0.105, palette);
            }
        }
        ctx.save();
        ctx.imageSmoothingEnabled = false;
        ctx.drawImage(this.raster, 0, 0, 1200, 652);
        ctx.restore();
    }

    impact(ctx, age, palette)
    {
        if (age < 0 || age > 0.32)
        {
            return;
        }
        const p = this.point('chop', 1);
        const fade = 1 - age / 0.32;
        ctx.save();
        ctx.globalAlpha = fade;
        Pixel.segment(ctx, { x: p.x - 45 - age * 210, y: p.y + 4 },
            { x: p.x + 45 + age * 210, y: p.y + 4 }, 8 * fade, palette.edge);
        ctx.restore();
    }
}
