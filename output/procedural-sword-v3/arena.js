export class Arena
{
    constructor()
    {
        this.buffer = document.createElement('canvas');
        this.buffer.width = 1200;
        this.buffer.height = 650;
        const ctx = this.buffer.getContext('2d');
        const sky = ctx.createLinearGradient(0, 0, 0, 650);
        sky.addColorStop(0, '#10191e');
        sky.addColorStop(0.6, '#202d2e');
        sky.addColorStop(1, '#151e20');
        ctx.fillStyle = sky;
        ctx.fillRect(0, 0, 1200, 650);
        const light = ctx.createRadialGradient(640, 280, 20, 600, 330, 440);
        light.addColorStop(0, '#9b976219');
        light.addColorStop(1, '#88997700');
        ctx.fillStyle = light;
        ctx.fillRect(0, 0, 1200, 650);
        for (let i = 0; i < 9; i++)
        {
            const x = i * 172 - 86;
            ctx.fillStyle = '#0e181b';
            ctx.fillRect(x, 40 + i % 3 * 23, 42, 240);
            ctx.fillStyle = '#243031';
            ctx.fillRect(x + 4, 45 + i % 3 * 23, 3, 220);
        }
        ctx.fillStyle = '#1e2b2c';
        ctx.beginPath();
        ctx.ellipse(605, 454, 480, 118, 0, 0, Math.PI * 2);
        ctx.fill();
        for (const [rx, ry, opacity] of [[460, 108, 0.25], [435, 100, 0.13], [315, 72, 0.12]])
        {
            ctx.strokeStyle = `rgba(168, 170, 130, ${opacity})`;
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.ellipse(605, 449, rx, ry, 0, 0, Math.PI * 2);
            ctx.stroke();
        }
        for (let i = 0; i < 32; i++)
        {
            const a = i / 32 * Math.PI * 2;
            ctx.strokeStyle = '#59635244';
            ctx.beginPath();
            ctx.moveTo(605 + Math.cos(a) * 435, 449 + Math.sin(a) * 100);
            ctx.lineTo(605 + Math.cos(a) * 460, 449 + Math.sin(a) * 108);
            ctx.stroke();
        }
        for (let i = 0; i < 85; i++)
        {
            const x = 200 + ((i * 137.19) % 820);
            const y = 375 + ((i * 41.61) % 170);
            ctx.strokeStyle = '#85907a0f';
            ctx.beginPath();
            ctx.moveTo(x, y);
            ctx.lineTo(x + 14 + i % 25, y - 2);
            ctx.stroke();
        }
        const vignette = ctx.createRadialGradient(600, 360, 180, 600, 320, 660);
        vignette.addColorStop(0, '#03090b00');
        vignette.addColorStop(1, '#03090bbb');
        ctx.fillStyle = vignette;
        ctx.fillRect(0, 0, 1200, 650);
    }

    draw(ctx, time)
    {
        ctx.drawImage(this.buffer, 0, 0);
        ctx.fillStyle = '#080f13aa';
        ctx.beginPath();
        ctx.ellipse(518, 471, 76, 15, 0, 0, Math.PI * 2);
        ctx.fill();
        for (let i = 0; i < 24; i++)
        {
            const x = 190 + i * 37 + Math.sin(time + i) * 6;
            const y = 190 + i * 43 % 290 + Math.sin(time * 0.4 + i * 3) * 5;
            ctx.fillStyle = `rgba(207, 198, 151, ${0.08 + (Math.sin(time * 2 + i) + 1) * 0.08})`;
            ctx.fillRect(x, y, 1.4, 1.4);
        }
    }
}
