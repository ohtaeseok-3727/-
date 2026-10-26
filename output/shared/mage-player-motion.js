class MagePlayerMotion
{
    constructor()
    {
        this.image = new Image();
        this.image.src = '../../SimpleGame/Assets/Characters/SmallMage/mana-blade-motion-v1.png';
        this.ready = this.image.decode();
        this.durations = [140, 110, 90, 50, 60, 65, 90, 150];
    }

    static get releaseTime()
    {
        return 340;
    }

    draw(context, milliseconds, footX, footY, scale)
    {
        if (!this.image.complete || !this.image.naturalWidth)
        {
            return;
        }
        let remaining = Math.max(0, milliseconds);
        let frame = 7;
        for (let index = 0; index < this.durations.length; ++index)
        {
            if (remaining < this.durations[index])
            {
                frame = index;
                break;
            }
            remaining -= this.durations[index];
        }
        context.save();
        context.imageSmoothingEnabled = false;
        context.drawImage(this.image,
            (frame % 4) * 384, Math.floor(frame / 4) * 480, 384, 480,
            footX - 192 * scale, footY - 455 * scale, 384 * scale, 480 * scale);
        context.restore();
    }
}
