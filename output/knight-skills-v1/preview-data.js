'use strict';
window.KnightPreviewData = {
    images: {
        thrust: 'thrust-draft-v1.png',
        thrustFx: '../knight-thrust-effect-v1/thrust-effect-v1.png',
        aura: 'sword-aura-draft-v1.png',
        wave: 'sword-wave-draft-v1.png',
        walk: '../../SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png'
    },
    thrust: [
        { rect: [0,0,543,724], feet: [225,588], tip: [215,535] },
        { rect: [543,0,543,724], feet: [240,588], tip: [300,453] },
        { rect: [1086,0,580,724], feet: [214,588], tip: [557,390] },
        { rect: [1629,0,543,724], feet: [242,588], tip: [231,535] }
    ],
    thrustFx: [
        { rect:[0,0,768,512], pivot:[175,310] },
        { rect:[768,0,768,512], pivot:[133,310] },
        { rect:[0,512,768,512], pivot:[171,225] },
        { rect:[768,512,768,512], pivot:[130,226] }
    ],
    aura: [0,1,2,3].map(i => ({rect:[i*543,0,543,724],pivot:[110,365]})),
    slash: [
        {rect:[0,0,384,512],feet:[188,453]},
        {rect:[384,0,384,512],feet:[190,453]},
        {rect:[768,0,444,512],feet:[180,453]},
        // Exclude the previous pose's crescent; offset the pivot by the same 80px.
        {rect:[1232,0,304,512],feet:[106,453]}
    ],
    waveFx: [
        {rect:[0,512,384,512],pivot:[230,232]},
        {rect:[384,512,384,512],pivot:[219,234]},
        {rect:[768,512,384,512],pivot:[241,228]},
        {rect:[1152,512,384,512],pivot:[226,225]}
    ],
    strikes: [
        {time:180,x:43,y:17},
        {time:210,x:-32,y:-57},
        {time:240,x:12,y:68},
        {time:270,x:-8,y:-6},
        {time:300,x:-23,y:44},
        {time:330,x:24,y:-34}
    ],
    skills: [
        {id:'thrust',title:'연속 찌르기',tag:'ACTIVE · 6 HITS',duration:1800,
         description:'한 번 찌르는 동작에 맞춰 6개의 이펙트가 30ms 간격으로 발생합니다. 각도는 같고, 앞뒤·위아래 생성 위치는 불규칙하게 다릅니다.'},
        {id:'aura',title:'검기 상인',tag:'PASSIVE · LOOP',duration:1600,
         description:'대기 자세의 검을 중심으로 은백색 검기가 반복됩니다. 효과를 꺼 원래 검과 비교할 수 있습니다.'},
        {id:'wave',title:'검기 발산',tag:'ACTIVE · PROJECTILE',duration:1800,
         description:'준비 → 공격 모션 → 검기 방출 → 독립 이동과 소멸을 확인합니다.'}
    ]
};
