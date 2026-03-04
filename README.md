# 🎮 Project Sanzo
> **내일배움캠프 언리얼 7기 6조 CH3 프로젝트**
> Unreal Engine 5 & C++ 기반의 스타일리시 액션 슈팅 게임

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-white?logo=unrealengine&logoColor=white&color=050505)
![C++](https://img.shields.io/badge/C++-99.1%25-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-PC-blue)

---

## 📖 프로젝트 소개
**Project Sanzo**는 객체 지향 프로그래밍(OOP) 설계와 언리얼 프레임워크를 적극적으로 활용하여 개발된 **3D 액션 슈팅 RPG**입니다.  
역사적 요소와 판타지가 결합된 세계관 속에서, 플레이어는 정교한 스태미나 관리와 패링 시스템을 통해 몰입감 넘치는 전투를 경험할 수 있습니다. 깃허브(GitHub) PR 템플릿과 브랜치 전략을 도입하여 체계적인 팀 협업을 실천했습니다.

* **개발 기간**: 202X.XX.XX ~ 202X.XX.XX (약 X주)
* **개발 엔진**: Unreal Engine 5.x
* **사용 언어**: C++ (99.1%), Blueprints
* **협업 툴**: GitHub, Slack, Notion

---

## 👨‍💻 팀원 소개 및 역할

| 이름 | 역할 | 담당 업무 (세부 구현 내용) | GitHub |
| :--- | :--- | :--- | :--- |
| **[이름]** | **플레이어 & 전투** | - `GameplayTag` 기반 캐릭터 상태(조준, 회피 등) 관리<br>- `SanzoStatComponent`를 통한 스태미나 로직 및 I-Frame 회피 구현<br>- 패링 시스템(성공/실패 분기, 슬로우 효과, 데미지 반사) 고도화<br>- Interface 기반 모듈화 레벨업 시스템 설계 | [@kbrother102](https://github.com/kbrother102) |
| **[이름]** | **적 AI & 몬스터** | - Melee/Ranged/Boss 몬스터 베이스 클래스 및 BT/Blackboard AI 구현<br>- 보스전 2페이즈 전환 및 페이즈 전용 사운드/이펙트 연동<br>- 피격/스턴 애니메이션 몽타주 및 콜리전 최적화 | [@djkim12](https://github.com/djkim12) |
| **[이름]** | **UI & 시스템** | - UMG 기반 HUD, 메인 메뉴, 팝업 UI 및 예외 처리<br>- `DataTable` 기반 플레이어 능력치 강화 시스템 설계<br>- GameState/PlayerController를 통한 게임 흐름 제어 및 엔딩 연출<br>- 길 안내 `NavigationArrow` 컴포넌트 구현 | [@JRoLee](https://github.com/JRoLee) |
| **[이름]** | **코어 루프 & 레벨** | - `StageManager`, `RoomBase`를 활용한 웨이브 및 클리어 로직<br>- Nav Mesh 볼륨 최적화 및 동적 환경 레벨 디자인<br>- 파괴 가능한 상호작용 오브젝트(도자기 등) 구현<br>- 글로벌 BGM 및 스테이지별 사운드 환경 구축 | [@yoonseo](https://github.com/yoonseo) |
| **[이름]** | **무기 & 기믹** | - `SanzoWeaponBase` 상속 구조의 총기(Gun) 로직 및 데미지 처리<br>- 유도 미사일 업그레이드 발동 조건 및 동적 데미지 산정<br>- 나이아가라(Niagara) 기반 원거리 적 레이저 조준선 및 타겟팅 기믹 | [@GitHubID](https://github.com/GitHubID) |

---

## 🌟 주요 기능 (Key Features)

### 1. 정교한 플레이어 매커니즘
* **GameplayTag 시스템**: 캐릭터의 상태를 태그로 관리하여 액션 간의 상호 배제 및 연계 로직을 유연하게 구현.
* **패링 & 회피**: 리스크-리턴이 확실한 패링(타임 슬로우, 반사)과 무적 프레임이 적용된 회피 시스템 제공.
* **성장 시스템**: 인터페이스 기반의 모듈화된 레벨업 및 외형 변신 업그레이드 연동.

### 2. 고도화된 AI 및 보스전
* **행동 트리(BT)**: AI Controller 및 Blackboard를 활용한 정찰, 추격, 공격 상태 기반 AI 구현.
* **페이즈 시스템**: 보스의 체력에 따라 공격 패턴과 범위가 변화하는 다이나믹한 보스전 구현.
* **나이아가라 기믹**: 레이저 조준선 등 가시적인 이펙트를 통해 전략적인 회피를 유도.

### 3. 모듈화된 스테이지 및 시스템
* **Stage Manager**: 룸 단위의 전투와 웨이브를 관리하여 확장성 있는 레벨 구조 설계.
* **DataTable 강화**: 데이터 기반의 밸런싱을 통해 무기 공격력, 유도탄 확률 등을 제어하는 업그레이드 시스템.
* **UX 편의성**: 네비게이션 화살표와 직관적인 UI를 통해 플레이어의 편의성 강화.

---

## 📂 프로젝트 구조 (Directory Structure)

```text
7th-Team6-CH3-Project/
├── Config/               # 프로젝트 설정 파일 (Input, Engine 등)
├── Content/              # 에셋 (BP, 맵, 메쉬, 머티리얼, UI 등)
├── Source/
│   └── ProjectSanzo/     # 메인 C++ 소스 코드
│       ├── Character/    # 플레이어 캐릭터 및 스탯 컴포넌트
│       ├── AI/           # 몬스터 및 보스 AI (BT, Task)
│       ├── Weapon/       # 무기 베이스 및 투사체 로직
│       ├── Stage/        # RoomBase, StageManager 제어 로직
│       ├── GameMode/     # 게임 모드 및 데이터 테이블 관리
│       ├── Controller/   # Player 및 AI 컨트롤러
│       └── UI/           # C++ 기반 UMG 위젯 클래스
└── ProjectSanzo.uproject # 언리얼 엔진 실행 파일
