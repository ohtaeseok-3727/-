export const ASSET_ROOT = '../../SimpleGame/Assets/Characters/SmallKnight/';
export const POSES = [0, 1, 2, 3, 3, 4, 4, 4, 5, 0];
export const EFFECTS = [-1, -1, 0, 1, 2, 3, 4, 5, -1, -1];
export const DURATIONS = [150, 130, 35, 35, 45, 45, 40, 40, 130, 160];
export const CYCLE = 1.8;

export function clamp(value, low = 0, high = 1)
{
    return Math.max(low, Math.min(high, value));
}

export function poseAt(time)
{
    let elapsed = time * 1000;
    for (let step = 0; step < DURATIONS.length; step++)
    {
        if (elapsed < DURATIONS[step])
        {
            return { pose: POSES[step], effect: EFFECTS[step], step };
        }
        elapsed -= DURATIONS[step];
    }
    return { pose: 0, effect: -1, step: 9 };
}

// Source-space sword endpoints, measured visually in each 256px column.
// These preview anchors approximate the artwork, not game hitboxes.
const BLADES = {
    slash: [
        [[80, 580], [113, 608]], [[64, 493], [24, 453]],
        [[139, 536], [221, 535]], [[177, 532], [258, 532]],
        [[149, 548], [217, 566]], [[80, 580], [113, 608]]
    ],
    chop: [
        [[83, 891], [119, 919]], [[96, 744], [123, 680]],
        [[81, 770], [79, 713]], [[122, 907], [180, 935]],
        [[119, 910], [177, 940]], [[83, 891], [119, 919]]
    ]
};

export class KnightMotion
{
    constructor(attack, effects)
    {
        this.attack = attack;
        this.effects = effects;
        this.scale = 0.94;
        this.x = 518;
        this.y = 467;
    }

    blade(kind, time)
    {
        const { pose } = poseAt(time);
        const baseline = kind === 'slash' ? 628 : 941;
        return BLADES[kind][pose].map(([x, y]) => ({
            x: this.x + (x - 128) * this.scale,
            y: this.y + (y - baseline) * this.scale
        }));
    }

    draw(ctx, kind, time)
    {
        const { pose } = poseAt(time);
        const top = kind === 'slash' ? 341 : 650;
        const bottom = kind === 'slash' ? 650 : 1024;
        const baseline = kind === 'slash' ? 628 : 941;
        const edges = kind === 'slash' ? [0, 256, 512, 768, 1034, 1280, 1536] : [0, 256, 512, 768, 1024, 1280, 1536];
        const sx = edges[pose];
        const width = edges[pose + 1] - sx;
        ctx.save();
        ctx.imageSmoothingEnabled = false;
        ctx.drawImage(this.attack, sx, top, width, bottom - top,
            this.x + (sx - pose * 256 - 128) * this.scale,
            this.y - (baseline - top) * this.scale,
            width * this.scale, (bottom - top) * this.scale);
        ctx.restore();
    }

    drawOriginal(ctx, kind, time)
    {
        const { effect, pose } = poseAt(time);
        if (effect < 0)
        {
            return;
        }
        const slash = kind === 'slash';
        const top = slash ? 341 : 650;
        const height = slash ? 309 : 374;
        const edges = slash ? [0, 256, 512, 779, 1024, 1280, 1536] : [0, 256, 512, 768, 1024, 1280, 1536];
        const sx = edges[effect];
        const width = edges[effect + 1] - sx;
        const ratio = this.scale / 0.64;
        const scale = (slash ? 0.92 : 0.8) * ratio;
        const effectBottom = [945, 945, 964, 959, 952, 944];
        const bootBottom = [939, 938, 938, 945, 949, 939];
        const y = slash ? this.y - 34 * ratio - 180 * scale
            : this.y + (bootBottom[pose] - 941) * this.scale - (effectBottom[effect] - top) * scale;
        ctx.save();
        ctx.imageSmoothingEnabled = false;
        ctx.drawImage(this.effects, sx, top, width, height,
            this.x + (slash ? 30 : 63) * ratio - (slash ? 128 : 170) * scale + (sx - effect * 256) * scale,
            y, width * scale, height * scale);
        ctx.restore();
    }
}
