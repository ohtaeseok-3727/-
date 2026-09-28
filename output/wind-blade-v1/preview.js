class WindBladePreview
{
    constructor()
    {
        this.stage = document.getElementById('stage');
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
            image.alt = `${index + 1}번 바람 칼날`;
            image.src = `wind-blade-0${index + 1}.png`;
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
        Promise.all(this.blades.map(({ image }) => image.decode())).then(() =>
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
        const size = Math.min(140, width * 0.16);
        const origin = width * 0.11;
        const lifetime = 1600;
        this.blades.forEach(({ node }, index) =>
        {
            const age = this.elapsed - index * delay;
            node.hidden = index >= count || age < 0 || age > lifetime;
            node.style.width = `${size}px`;
            node.style.height = `${size}px`;
            node.querySelector('span').style.left = `${size / 2 - 4}px`;
            node.style.top = `${150 - size / 2}px`;
            node.style.transform = `translateX(${origin + age / lifetime * (width - origin + size) - size / 2}px)`;
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
