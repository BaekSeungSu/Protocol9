#  Protocol 9 
프로젝트 제작 기간 : 2025.07.28(월) ~ 2025.08.20(수)

UE5(5.6.0) 기반 1인칭 서바이벌 FPS. 웨이브를 버티며 성장하고, 조건 충족 시 등장하는 보스를 처치해 세션을 마무리합니다.

---
##  시연 영상

<div align="center">
  <a href="https://youtu.be/C8dzLuDIPkU">
    <img src="https://img.youtube.com/vi/C8dzLuDIPkU/0.jpg" alt="시연 영상" width="720"/>
  </a>
</div>

##  프로젝트 문서 모음

* [프로젝트 기획서](https://www.notion.so/teamsparta/23e2dc3ef51480c0a482f4b4d87aaba3)

* [시스템 설계](http://notion.so/teamsparta/23e2dc3ef514800398d8f40987c4a28d)

##  핵심 특징

* **서바이벌 루프**: 웨이브 → 레벨업/강화 → 보스 등장 → 클리어/실패
* **다양한 적 계층**: 근접/원거리/비행 + 보스(패턴: 돌진·휘두르기·투사체)
* **무기 & 탄약 루프**: 히트스캔/프로젝트타일 발사, 반동·스프레드, 재장전, 탄약 UI
* **아이템 & 버프**: 힐/무적/속도/공격력/경험치, 지속시간·HUD 아이콘 연동
* **UI/HUD**: 체력·스태미나·경험치/레벨·킬카운트·보스 HP·무기/탄약·타이머
* **풀링 & 유틸리티**: 오브젝트/프로젝트타일 풀링, 미니맵 캡처, 타일/포그 매니저

---

##  팀 & 역할

| 이름  | 역할            | 추가 담당           |
| --- | ------------- | --------------- |
| 김동권 | 캐릭터           | 코드리뷰            |
| 장준서 | 아이템/인벤토리      | 발표자료            |
| 임동건 | 적 AI(보스 포함)   | 영상 편집           |
| 백승수 | 무기            | 발표              |
| 정윤호 | 레벨 디자인(맵/안개)  | 코드리뷰            |
| 이서호 | UI/게임모드/종료 조건 | GitHub & README |

---

##  개발 환경

* **Engine**: Unreal Engine **5.6.0**
* **Language**: C++
* **IDE**: JetBrains Rider (VS 키맵)
* **협업**: GitHub / Notion (브랜치 기반 개발 + 코드리뷰)
* **레포 언어 비율(참고)**: C++ 위주(≈97%) 

---

##  게임 흐름

1. **Press Any Key → Main Menu**
2. **In-Game(HUD)**: 웨이브 진행, 경험치로 레벨업, 아이템/무기 운용
3. **GameOver**

   * 보스 처치 → **Victory**
   * 사망 → **Game Over**
4. **ReasultStat**: 플레이타임, 킬로그 표시 / Restart, Return, Quit 선택

## 조작법 (Controls)

- **이동**: W / A / S / D  
- **사격**: 마우스 왼쪽 클릭  
- **재장전**: R  
- **무기 줍기**: E
- **무기 교체**: 숫자 1번키, 2번키
- **점프**: SpaceBar  
- **대시**: SpaceBar + Shift  
- **근접 공격**: V

---

##  모듈 구성

* **GameMode / Flow**

  * `MainGameMode` : 화면 전환(MainMenu/HUD/GameOver/Result), 타이머 틱, HUD 포인터 제공
  * `MainGameInstance` : BGM 등 전역 상태
* **Character & Components**

  * `MainCharacter` : HUD 포인터 캐싱(다음 틱), 데미지/스테미나/레벨/상태머신
  * `HPComponent`, `StaminaComponent`, `ControlComponent`, `CharacterStateMachine`
  * `LevelUpComponent`, `SoundComponent`, `MainPlayerController`
* **Weapons & Inventory**

  * `WeaponBase` (+ `WeaponData` DT): 히트스캔/프로젝트타일, 반동·스프레드, 재장전, 아이콘/탄약 UI 갱신
  * `BaseProjectile`, `RifleProjectile`, `PulseProjectile`
  * `InventoryComponent`, `Pickup_Weapon`
* **Monsters & Boss**

  * `MonsterBase` 파생: `MeleeMonsterBase`, `RangedMonsterBase`, `FlyingMonsterBase`
  * `MonsterSpawner` (+ `MonsterSpawnRow` DT), `MonsterBaseAIController`, `MonsterBaseAnimInstance`, `MonsterProjectile`
  * 보스: `BossMonsterBase` + `BossMonsterAttackNotify`/`Pattern*Notify`/`Rush/Sweep State`
* **Items & Pooling**

  * `ItemBase` 파생: `ExperienceItem`, `HealingItem`, `InvincibilityItem`, `SpeedItem`, `IncreaseDamageItem`
  * `PoolingManager`, `ObjectPoolingComponent`, `SpawnItem`
* **UI**

  * `UWBP_HUD` : 타이머/HP/EXP/스태미나/킬/무기·탄약/보스HP/버프 아이콘
  * `UWBP_GameOver`, `UWBP_ResultStats`, `UWBP_HelpAccordion`, `PlayerUIComponent`, `CrosshairWidget`
* **World Utility**

  * `TileBase`, `TileManager`, `FogManager`, `MinimapCapture`
  오브젝트 생성, 캐릭터 위치와 타일 동기화, 미니맵 기능

---


##  폴더 구조(요약)

> 레포에 존재하는 최상위 폴더 스냅샷(UE 관례 + 팀 커스텀 폴더): `Config/`, `Content/`, `Data/`, `Source/`, `UI/` 등. 

```
/
├─ Config/
├─ Content/
├─ Data/
├─ Source/Protocol9/...
├─ UI/
├─ Protocol9.uproject
└─ README.md
```

---

##  브랜치 전략 & 협업

| 브랜치명        | 설명                  |
| ----------- | ------------------- |
| `main`      | 최종 배포용 (관리자만 직접 푸시) |
| `develop`   | 통합 개발 브랜치           |
| `feature/*` | 기능 단위 개발 브랜치        |

* 규칙: 작업 전 `git pull`, 작업 후 `git push` & PR, 대용량 에셋은 저장소 외부 관리
* 충돌 시: `git status / diff / log`로 원인 파악 후 해결
예시:

```bash
git checkout -b feature/weapon-system
git add .
git commit -m "무기 시스템: 히트스캔/프로젝트타일 루프 및 UI 연동"
git push -u origin feature/weapon-system
```
---

## 세부 구현 문서/트러블 슈팅

* [GameMode/GameInstance/UI](https://github.com/dltjgh17/Unreal_TIL/tree/main/August/08.18)
* [Weapon](https://southern-guanaco-bbd.notion.site/253da1ba7bba8080995bdd615e71438a)
* [Enemy](https://www.notion.so/Protocol9-Enemy-2534fe28994880e8a6ebec768b5d56e4?source=copy_link)
* [Charater](https://www.notion.so/Protocol-9-25387dde041b80939f6cf2e9cd9578bb?showMoveTo=true)
* [Item](https://www.notion.so/254c2bd40e148094b4eef97579a9cbd6)
* [World Utility](https://hakunamatata0930.tistory.com/46)

---

