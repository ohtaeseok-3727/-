export class PixelGeometry
{
    static polygon(ctx, points, color)
    {
        ctx.beginPath();
        points.forEach((p, i) =>
        {
            const x = Math.round(p.x / 4) * 4;
            const y = Math.round(p.y / 4) * 4;
            if (i === 0)
            {
                ctx.moveTo(x, y);
            }
            else
            {
                ctx.lineTo(x, y);
            }
        });
        ctx.closePath();
        ctx.fillStyle = color;
        ctx.fill();
    }

    static segment(ctx, start, end, width, color)
    {
        const dx = end.x - start.x;
        const dy = end.y - start.y;
        const length = Math.hypot(dx, dy) || 1;
        const nx = -dy / length * width / 2;
        const ny = dx / length * width / 2;
        this.polygon(ctx, [
            { x: start.x + nx, y: start.y + ny },
            { x: end.x + nx, y: end.y + ny },
            { x: end.x - nx, y: end.y - ny },
            { x: start.x - nx, y: start.y - ny }
        ], color);
    }

    static burst(ctx, x, y, radius, color)
    {
        const points = [];
        for (let i = 0; i < 24; i++)
        {
            const angle = i / 24 * Math.PI * 2;
            const distance = i % 2 === 0 ? radius * (0.7 + i % 5 * 0.11) : radius * 0.23;
            points.push({ x: x + Math.cos(angle) * distance, y: y + Math.sin(angle) * distance });
        }
        this.polygon(ctx, points, color);
    }
}
