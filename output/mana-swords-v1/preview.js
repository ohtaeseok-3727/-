class ManaSword
{
    constructor(index)
    {
        this.index = index;
        const formation = [
            { x: 150, y: 160 },
            { x: 112, y: 111 },
            { x: 180, y: 215 },
            { x: 135, y: 191 },
            { x: 195, y: 135 }
        ];
        this.origin = { ...formation[index] };
        this.speed = 1200;
        this.range = 500;
        this.position = { ...this.origin };
        this.distance = 0;
        this.visible = true;
        this.launched = false;
        this.opacity = 1;
        this.hitTargets = new Set();
    }

    sample(milliseconds, interval)
    {
        const age = milliseconds - MagePlayerMotion.releaseTime - this.index * interval;
        this.launched = age >= 0;
        this.distance = Math.max(0, Math.min(this.range, age * this.speed / 1000));
        this.visible = age <= this.range / this.speed * 1000;
        this.opacity = Math.min(1, milliseconds / 250);
        this.position.x = this.origin.x + this.distance;
        this.hitTargets.clear();
        [310, 480].forEach((x, index) =>
        {
            if (this.launched && this.position.x + 40 >= x)
            {
                this.hitTargets.add(index);
            }
        });
    }

    draw(context, trail, showEffect)
    {
        if (!this.visible)
        {
            return;
        }
        context.save();
        context.globalAlpha = this.opacity;
        context.translate(this.position.x, this.position.y);
        if (this.launched && showEffect && trail.complete && trail.naturalWidth)
        {
            context.drawImage(trail, -160, -90, 180, 180);
        }
        ManaSword.drawBody(context);
        context.restore();
    }

    static drawBody(context)
    {
        // Geometry is rendered for every object; no sword bitmap is used.
        context.fillStyle = '#173e4b';
        context.beginPath();
        context.moveTo(-19, -8);
        context.lineTo(25, -8);
        context.lineTo(43, 0);
        context.lineTo(25, 8);
        context.lineTo(-19, 8);
        context.closePath();
        context.fill();
        context.fillStyle = '#46d9db';
        context.beginPath();
        context.moveTo(-17, -5);
        context.lineTo(25, -5);
        context.lineTo(39, 0);
        context.lineTo(-17, 0);
        context.closePath();
        context.fill();
        context.fillStyle = '#208598';
        context.beginPath();
        context.moveTo(-17, 0);
        context.lineTo(39, 0);
        context.lineTo(25, 5);
        context.lineTo(-17, 5);
        context.closePath();
        context.fill();
        context.fillStyle = '#b9f3e8';
        context.fillRect(-17, -1, 47, 2);
        context.fillStyle = '#249dad';
        context.fillRect(-22, -12, 5, 24);
        context.fillRect(-36, -3, 14, 6);
        context.fillStyle = '#46d9db';
        context.fillRect(-39, -5, 5, 10);
    }
}

class ManaSwordPreview
{
    constructor()
    {
        this.stage = document.getElementById('stage');
        this.context = this.stage.getContext('2d');
        this.design = document.getElementById('design').getContext('2d');
        this.context.imageSmoothingEnabled = false;
        this.design.imageSmoothingEnabled = false;
        this.player = new MagePlayerMotion();
        this.swords = Array.from({ length: 5 }, (_, i) => new ManaSword(i));
        this.trail = new Image();
        this.trail.src = '../../SimpleGame/Assets/Characters/Skills/dash-trail.png';
        this.elapsed = 780;
        this.frame = 0;
        this.running = false;
        this.play = document.getElementById('play');
        this.pause = document.getElementById('pause');
        this.delay = document.getElementById('delay');
        this.time = document.getElementById('time');
        this.effect = document.getElementById('effect');
        this.play.onclick = () => this.start();
        this.pause.onclick = () => this.stop();
        this.time.oninput = () =>
        {
            this.stop();
            this.elapsed = Number(this.time.value);
            this.render();
        };
        this.delay.oninput = () =>
        {
            this.stop();
            this.render();
        };
        this.effect.onchange = () => this.render();
        Promise.all([this.trail.decode(), this.player.ready]).then(() =>
        {
            this.play.disabled = false;
            this.render();
            document.getElementById('status').textContent = '준비됨';
        }).catch(() =>
        {
            this.play.disabled = true;
            document.getElementById('status').textContent = '플레이어 또는 돌진 이미지 로딩 실패';
        });
        this.render();
    }

    render()
    {
        const context = this.context;
        context.clearRect(0, 0, 900, 360);
        context.lineWidth = 1;
        context.strokeStyle = '#263d49';
        for (let x = 40; x < 900; x += 40)
        {
            context.beginPath();
            context.moveTo(x, 35);
            context.lineTo(x, 290);
            context.stroke();
        }
        const interval = Number(this.delay.value);
        this.swords.forEach(sword => sword.sample(this.elapsed, interval));
        [310, 480].forEach((x, target) =>
        {
            this.swords.forEach(sword =>
            {
                context.strokeStyle = sword.hitTargets.has(target) ? '#47bbc4' : '#7a7980';
                context.beginPath();
                context.arc(x, sword.origin.y, 12, 0, Math.PI * 2);
                context.stroke();
            });
        });
        this.player.draw(context, this.elapsed, 120, 245, 0.36);
        this.swords.forEach(sword => sword.draw(context, this.trail, this.effect.checked));
        context.fillStyle = '#b7d3d8';
        context.font = '16px system-ui';
        context.fillText('생성 위치', 105, 325);
        context.fillText('500px 이동', 565, 325);
        context.fillText('관통 표적', 345, 50);
        context.strokeStyle = '#64848c';
        context.beginPath();
        context.moveTo(140, 300);
        context.lineTo(640, 300);
        context.stroke();
        this.drawDesign();
        this.time.value = String(this.elapsed);
        document.getElementById('timeValue').textContent = `${(this.elapsed / 1000).toFixed(2)}초`;
        document.getElementById('delayValue').textContent = `${interval}ms`;
    }

    drawDesign()
    {
        const context = this.design;
        context.clearRect(0, 0, 900, 190);
        context.save();
        context.translate(220, 86);
        context.scale(2.8, 2.8);
        ManaSword.drawBody(context);
        context.restore();
        if (this.trail.complete && this.trail.naturalWidth)
        {
            context.drawImage(this.trail, 490, -77, 330, 330);
        }
        context.fillStyle = '#b7d3d8';
        context.font = '18px system-ui';
        context.fillText('검 본체 · 독립 객체', 135, 166);
        context.fillText('돌진 효과 · 투명 스프라이트', 520, 166);
    }

    start()
    {
        this.stop();
        this.elapsed = 0;
        this.running = true;
        this.pause.disabled = false;
        document.getElementById('status').textContent = '생성 및 발사 중';
        let previous = performance.now();
        const tick = now =>
        {
            if (!this.running)
            {
                return;
            }
            this.elapsed = Math.min(2200, this.elapsed + now - previous);
            previous = now;
            this.render();
            if (this.elapsed >= 2200)
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
        this.running = false;
        cancelAnimationFrame(this.frame);
        this.pause.disabled = true;
        document.getElementById('status').textContent = '정지';
    }
}

new ManaSwordPreview();
