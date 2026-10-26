class WindBladePreview
{
    constructor()
    {
        this.stage = document.getElementById('stage');
        this.player = new MagePlayerMotion();
        this.playerCanvas = document.createElement('canvas');
        this.playerCanvas.style.cssText = 'position:absolute;inset:0;width:100%;height:100%;pointer-events:none';
        this.stage.append(this.playerCanvas);
        this.level = document.getElementById('level');
        this.delay = document.getElementById('delay');
        this.time = document.getElementById('time');
        this.play = document.getElementById('play');
        this.pause = document.getElementById('pause');
        this.running = false;
        this.elapsed = 500;
        this.frame = 0;
        this.blades = Array.from({ length: 3 }, (_, index) =>
        {
            const node = document.createElement('div');
            node.className = 'projectile';
            node.hidden = true;
            const image = new Image();
            image.alt = ['수직베기', '사선베기', '가로베기'][index];
            image.src = '../../SimpleGame/Assets/Characters/Skills/wind-blade-01.png';
            // Keep the convex leading edge facing forward; project the cutting plane.
            image.style.transform = [
                'matrix(1.2, 0, 0, 1, 0, 0)',
                'matrix(1.1, 0, -0.35, 0.72, 0, 0) scale(1.25)',
                'matrix(1.2, 0, 0, 0.4, 0, 0) scale(1.25)'
            ][index];
            const label = document.createElement('span');
            label.textContent = `${index + 1}`;
            node.append(image, label);
            this.stage.append(node);
            return { node, image };
        });
        this.play.addEventListener('click', () => this.start());
        this.pause.addEventListener('click', () => this.stop());
        this.time.addEventListener('input', () =>
        {
            this.stop();
            this.elapsed = Number(this.time.value);
            this.render();
        });
        for (const input of [this.level, this.delay])
        {
            input.addEventListener('input', () =>
            {
                this.stop();
                this.elapsed = 500;
                this.render();
            });
        }
        new ResizeObserver(() => this.render()).observe(this.stage);
        Promise.all([this.player.ready, ...this.blades.map(({ image }) => image.decode())]).then(() =>
        {
            this.play.disabled = false;
            document.getElementById('status').textContent = '준비됨 · 발사를 누르세요';
            this.render();
        }).catch(() =>
        {
            document.getElementById('status').textContent = '이미지를 불러오지 못했습니다.';
        });
    }

    render()
    {
        const count = Number(this.level.value);
        const delay = Number(this.delay.value);
        const width = this.stage.clientWidth;
        const travelPadding = Math.min(140, width * 0.16);
        const playerScale = Math.min(0.34, width / 1800);
        // The visible crescent occupies about 60% of its transparent image canvas.
        // Scale from the player's body rather than the full sprite cell.
        const size = 660 * playerScale;
        const footX = width * 0.15;
        const footY = 150 + 140 * playerScale;
        const origin = footX + 70 * playerScale + 20;
        this.playerCanvas.width = width;
        this.playerCanvas.height = this.stage.clientHeight;
        this.player.draw(this.playerCanvas.getContext('2d'), this.elapsed, footX, footY, playerScale);
        // Previous preview speed was 1.5x; this request doubles that speed.
        const lifetime = 1600 / 3;
        this.blades.forEach(({ node }, index) =>
        {
            const age = this.elapsed - MagePlayerMotion.releaseTime - index * delay;
            node.hidden = index >= count || age < 0 || age > lifetime;
            node.style.width = `${size}px`;
            node.style.height = `${size}px`;
            node.querySelector('span').style.left = `${size / 2 - 4}px`;
            node.style.top = `${150 - size / 2}px`;
            node.style.transform = `translateX(${origin + age / lifetime * (width - origin + travelPadding) - size / 2}px)`;
        });
        this.time.value = String(this.elapsed);
        document.getElementById('timeValue').textContent = `${(this.elapsed / 1000).toFixed(2)}초`;
        document.getElementById('delayValue').textContent = `${delay}ms`;
    }

    start()
    {
        this.stop();
        this.elapsed = 0;
        this.running = true;
        this.pause.disabled = false;
        document.getElementById('status').textContent = `${this.level.value}개 순차 발사 중`;
        this.lastTime = performance.now();
        const tick = (now) =>
        {
            if (!this.running)
            {
                return;
            }
            this.elapsed = Math.min(2300, this.elapsed + now - this.lastTime);
            this.lastTime = now;
            this.render();
            if (this.elapsed >= 2300)
            {
                this.stop();
                document.getElementById('status').textContent = '발사 완료';
                return;
            }
            this.frame = requestAnimationFrame(tick);
        };
        this.frame = requestAnimationFrame(tick);
    }

    stop()
    {
        cancelAnimationFrame(this.frame);
        this.running = false;
        this.pause.disabled = true;
        document.getElementById('status').textContent = '정지';
    }
}

new WindBladePreview();
