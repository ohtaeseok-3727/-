const LABELS = new Map(Object.entries({
    selecting: '직업 선택', running: '진행 중', paused: '일시정지 / 비활성 창', menu: '메뉴 열림', defeated: '플레이어 패배',
    stable: '안정', watch: '주의 / 확인 전', alert: '위험', occupied: '적 점령', cleared: '주요 사건 해결', unknown: '미확인',
    idle: '대기', patrol: '순찰', chase: '추적', return: '복귀', windup: '공격 준비', recovery: '재사용 대기',
    heal: '치유', disabled: '비활성', dead: '사망', waiting: '대기', served: '상호작용 완료', safe: '안전',
    captive: '포로', rescued: '구출 후 퇴장', absent: '미등장 / 퇴장', scripted: '이벤트 동작', attacking: '공격', moving: '이동',
    melee: '근접병', ranged: '원거리병', healer: '치유병', boss: '보스', civilian: '주민', companion: '동행 용사'
}));

function label(value)
{
    return LABELS.get(value) ?? value;
}

function element(tag, text = '', className = '')
{
    const node = document.createElement(tag);
    node.textContent = text;
    node.className = className;
    return node;
}

function badge(state)
{
    const style = ['stable', 'cleared', 'safe', 'rescued', 'served'].includes(state) ? 'safe'
        : ['alert', 'chase', 'windup', 'attacking', 'captive', 'occupied'].includes(state) ? 'alert'
        : ['watch', 'waiting'].includes(state) ? 'watch' : '';
    return element('span', label(state), `badge ${style}`);
}

function position(value)
{
    return value ? `${Math.round(value.x)}, ${Math.round(value.y)}` : '—';
}

class WorldDashboard
{
    constructor()
    {
        this.snapshot = null;
        this.identity = '';
        this.ui = Object.fromEntries(Array.from(document.querySelectorAll('[id]'), node => [node.id, node]));
        for (const id of ['region-filter', 'present-only', 'dead-monsters'])
        {
            this.ui[id].addEventListener('change', () => this.renderEntities());
        }
        this.ui.search.addEventListener('input', () => this.renderEntities());
        this.ui['settlements-only'].addEventListener('change', () => this.renderRegions());
        this.ui['clear-filter'].addEventListener('click', () =>
        {
            this.ui['region-filter'].value = '';
            this.renderEntities();
        });
        this.ui['raw-panel'].addEventListener('toggle', () => this.renderRaw());
        if (location.protocol === 'file:')
        {
            this.ui.notice.textContent = 'tools/monitor/start-monitor.cmd를 실행한 뒤 http://127.0.0.1:8766/ 에 접속하세요.';
            return;
        }
        this.poll();
    }

    async poll()
    {
        const controller = new AbortController();
        const timeout = setTimeout(() => controller.abort(), 3000);
        try
        {
            const response = await fetch('/api/state', { cache: 'no-store', signal: controller.signal });
            if (!response.ok)
            {
                throw new Error(`HTTP ${response.status}`);
            }
            const received = await response.json();
            if (typeof received.connected !== 'boolean' || !Object.hasOwn(received, 'snapshot'))
            {
                throw new Error('수신 서버 응답 형식 불일치');
            }
            this.connection(received);
            if (received.snapshot)
            {
                const key = `${received.snapshot.sessionId}:${received.snapshot.sequence}`;
                if (key !== this.identity)
                {
                    this.snapshot = received.snapshot;
                    this.identity = key;
                    this.render();
                }
            }
            else if (this.snapshot)
            {
                this.snapshot = null;
                this.identity = '';
                this.render();
            }
        }
        catch (error)
        {
            this.ui.connection.textContent = '서버 연결 끊김';
            this.ui.connection.className = '';
            this.ui.notice.textContent = `수신 서버에 연결할 수 없습니다. start-monitor.cmd 실행 여부를 확인하세요. ${this.snapshot ? '화면은 마지막 수신 값입니다.' : ''}`;
            this.ui.notice.classList.add('stale');
            document.querySelector('.metrics').classList.add('stale-data');
        }
        finally
        {
            clearTimeout(timeout);
            setTimeout(() => this.poll(), 1000);
        }
    }

    connection(data)
    {
        const hasData = data.snapshot !== null;
        this.ui.connection.textContent = data.connected ? '● JSON 수신 중' : hasData ? '게임 연결 끊김' : '게임 수신 대기';
        this.ui.connection.className = data.connected ? 'online' : '';
        this.ui.received.textContent = data.receivedAtUnixMs
            ? `마지막 수신 ${new Date(data.receivedAtUnixMs).toLocaleTimeString('ko-KR', {timeZone:'Asia/Seoul'})} · ${Math.floor(data.ageSeconds)}초 전`
            : '아직 게임 데이터가 없습니다.';
        this.ui.notice.classList.toggle('stale', hasData && !data.connected);
        document.querySelector('.metrics').classList.toggle('stale-data', hasData && !data.connected);
        this.ui.notice.textContent = !hasData
            ? '수신 서버 준비 완료. 모니터링 코드가 포함된 게임을 실행하면 상태가 자동 표시됩니다.'
            : !data.connected ? '5초 이상 새 메시지가 없습니다. 아래 수치는 마지막 수신 값이며 현재 상태를 보장하지 않습니다.'
            : data.snapshot.game.status === 'selecting' ? '게임이 연결되었습니다. 직업을 선택하면 월드 상태를 수집합니다.'
            : '실제 게임 상태를 수신 중입니다. 지역 카드를 누르면 해당 지역의 NPC와 몬스터를 확인할 수 있습니다.';
    }

    render()
    {
        const snapshot = this.snapshot;
        const ui = this.ui;
        const counts = snapshot?.totals;
        ui['npc-count'].textContent = counts ? counts.npcs : '—';
        ui['npc-note'].textContent = counts ? `추적 대상 ${counts.npcTracked}개 · 현재 존재 개체만 집계` : '등록 상태 대기';
        ui['monster-count'].textContent = counts ? counts.monsters : '—';
        ui['monster-note'].textContent = counts ? `사망 보관 개체 ${counts.monsterDead}개 별도` : '지역 전체 합계';
        ui['region-count'].textContent = snapshot ? `${snapshot.regions.filter(r => r.kind === 'settlement').length} / ${snapshot.regions.length}` : '—';
        ui['game-status'].textContent = snapshot ? label(snapshot.game.status) : '대기';
        ui.chapter.textContent = snapshot ? `${snapshot.game.chapter || '월드 진입 전'} · 플레이어 ${position(snapshot.game.playerPosition)}` : '게임 연결을 기다립니다.';
        const selected = ui['region-filter'].value;
        const options = [new Option('전체 지역', '')];
        for (const region of snapshot?.regions ?? [])
        {
            options.push(new Option(region.name, region.id));
        }
        ui['region-filter'].replaceChildren(...options);
        if (options.some(option => option.value === selected))
        {
            ui['region-filter'].value = selected;
        }
        this.renderRegions();
        this.renderEntities();
        this.renderRaw();
    }

    renderRegions()
    {
        const rows = (this.snapshot?.regions ?? []).filter(region => !this.ui['settlements-only'].checked || region.kind === 'settlement');
        const cards = rows.map(region =>
        {
            const card = element('button', '', 'region-card');
            card.type = 'button';
            card.dataset.regionId = region.id;
            card.classList.toggle('selected', region.id === this.ui['region-filter'].value);
            card.append(element('h3', region.name), badge(region.status), element('p', region.detail));
            card.append(element('div', `NPC ${region.counts.npcs} · 몬스터 ${region.counts.monsters}`, 'counts'));
            card.append(element('small', `${region.kind === 'settlement' ? '마을권' : '지역'} · ${region.active ? '현재 지역' : '비활성 지역'} · ${region.discovered ? '발견됨' : '미방문'}`));
            card.addEventListener('click', () =>
            {
                this.ui['region-filter'].value = region.id;
                this.renderEntities();
            });
            return card;
        });
        this.ui.regions.replaceChildren(...(cards.length ? cards : [element('p', this.snapshot ? '해당하는 지역이 없습니다.' : '첫 JSON 메시지를 기다립니다.', 'empty')]));
    }

    renderEntities()
    {
        const regionId = this.ui['region-filter'].value;
        const query = this.ui.search.value.trim().toLocaleLowerCase();
        const npcRows = [];
        const monsterRows = [];
        for (const card of this.ui.regions.querySelectorAll('[data-region-id]'))
        {
            card.classList.toggle('selected', card.dataset.regionId === regionId);
        }
        for (const region of this.snapshot?.regions ?? [])
        {
            if (regionId && region.id !== regionId)
            {
                continue;
            }
            const matches = entity => `${entity.id} ${entity.name ?? ''} ${entity.state} ${label(entity.state)} ${label(entity.role)}`.toLocaleLowerCase().includes(query);
            for (const npc of region.npcs)
            {
                if (!matches(npc) || (this.ui['present-only'].checked && !npc.present))
                {
                    continue;
                }
                const row = element('tr', '', npc.present ? '' : 'absent');
                const name = element('td', npc.name);
                name.append(element('small', `${label(npc.role)} · ${npc.id}`));
                const state = element('td');
                state.append(badge(npc.state));
                row.append(name, element('td', region.name), state, element('td', npc.present ? '존재' : '부재'),
                    element('td', npc.health === null || npc.health === undefined ? '미정의' : `${npc.health} / ${npc.maxHealth}`),
                    element('td', position(npc.position)), element('td', npc.detail));
                npcRows.push(row);
            }
            for (const monster of region.monsters)
            {
                if (!matches(monster) || (!this.ui['dead-monsters'].checked && monster.health <= 0))
                {
                    continue;
                }
                const row = element('tr', '', monster.health > 0 ? '' : 'absent');
                const name = element('td', label(monster.role));
                name.append(element('small', monster.id));
                const state = element('td');
                state.append(badge(monster.state));
                row.append(name, element('td', region.name), state,
                    element('td', `${monster.health} / ${monster.maxHealth}`), element('td', position(monster.position)),
                    element('td', monster.scripted ? '챕터 스크립트' : '일반 AI'));
                monsterRows.push(row);
            }
        }
        this.table(this.ui.npcs, npcRows, 7);
        this.table(this.ui.monsters, monsterRows, 6);
        this.ui['npc-results'].textContent = this.snapshot ? `· ${npcRows.length}개 표시` : '';
        this.ui['monster-results'].textContent = this.snapshot ? `· ${monsterRows.length}개 표시` : '';
    }

    table(target, rows, columns)
    {
        if (rows.length === 0)
        {
            const row = element('tr');
            const cell = element('td', this.snapshot ? '조건에 맞는 개체가 없습니다.' : '수신 대기', 'empty');
            cell.colSpan = columns;
            row.append(cell);
            target.replaceChildren(row);
            return;
        }
        target.replaceChildren(...rows);
    }

    renderRaw()
    {
        if (this.ui['raw-panel'].open)
        {
            this.ui.raw.textContent = this.snapshot ? JSON.stringify(this.snapshot, null, 2) : '수신 대기';
        }
    }
}

new WorldDashboard();
