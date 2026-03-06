# 🎮 Project Sanzo
> **내일배움캠프 언리얼 7기 6조 CH3 프로젝트**
> Unreal Engine 5.6 & C++ 기반의 스타일리시 액션 슈팅 게임

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-white?logo=unrealengine&logoColor=white&color=050505)
![C++](https://img.shields.io/badge/C++-99.1%25-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-PC-blue)

---

## 📖 프로젝트 소개
**[Project Sanzo - 조선신녀비록]** 은 객체 지향 프로그래밍(OOP) 설계와 언리얼 프레임워크를 활용하여 개발된 **3D 액션 슈팅/로그라이크 게임**입니다.

역사적 요소와 판타지가 결합된 세계관 속에서, 플레이어는 무기(총, 활)를 전투 중에 자유롭게 선택하고, 스태미나 관리와 패링/회피를 통해 전투를 진행합니다.

플레이어는 레벨 업을 통해서 스테이터스, 기능 추가, 외모 등을 선택하여 업그레이드 할 수 있으며, 선택에 따라 달라지는 멀티 엔딩을 경험할 수 있습니다.

깃허브(GitHub) PR 템플릿과 브랜치 전략을 도입하여 체계적인 팀 협업을 실천했습니다.


* **개발 기간**: 2026.02.05 ~ 2026.03.05 (4주)
* **개발 엔진**: Unreal Engine 5.6.1
* **사용 언어**: C++ (99.1%), Blueprints
* **협업 툴**: GitHub, Slack, Notion

---

## 🎥 프로젝트 시연 영상 및 스크린샷 (Gameplay & Screenshots)

### 🎥 프로젝트 시연 영상
[![내일배움캠프 언리얼 7기 - 슈터 게임 프로젝트 6조 [조선신녀비록] 시연 영상](https://img.youtube.com/vi/ZuqtL_jlpgc/maxresdefault.jpg)](https://www.youtube.com/watch?v=ZuqtL_jlpgc) 

> 이미지 클릭 시 유튜브 영상으로 이동합니다.

### 📸 스크린샷

| 섬멸전 화면 |
| :---: | 
| <img width="2559" height="1439" alt="스크린샷 2026-03-05 124007" src="https://github.com/user-attachments/assets/685c14e5-6f1b-40e5-800d-3a55f7170c7e" />|
|<img width="2559" height="1439" alt="스크린샷 2026-03-05 124600" src="https://github.com/user-attachments/assets/ebc756a8-ea98-4222-a1fe-e066fa9e0414" />|
| 방호전 화면 |
|<img width="2559" height="1439" alt="스크린샷 2026-03-05 124133" src="https://github.com/user-attachments/assets/66b3f7e0-1913-40e9-9af8-941d41279788" />|
|<img width="2559" height="1439" alt="스크린샷 2026-03-05 124636" src="https://github.com/user-attachments/assets/6ae7814b-5087-4fc6-8114-d1ebcdde8b91" />|
| 대장전 화면 |
|<img width="2559" height="1439" alt="스크린샷 2026-03-05 124314" src="https://github.com/user-attachments/assets/b87c6697-0b0f-4fcd-8dad-f415b1bcd6aa" /> |
|<img width="2560" height="1600" alt="2026-03-05_122736" src="https://github.com/user-attachments/assets/fb6ce413-1857-450d-90cd-106fece6e587" />|
|<img width="2560" height="1600" alt="2026-03-05_122619" src="https://github.com/user-attachments/assets/fa7beecf-d6da-44ed-943c-b520ab3d7932" />|

---

## 💻 설치 및 실행 방법 (How to Build and Run)

### A. 패키징 버전

[GoogleDrive](https://drive.google.com/file/d/1fR1nKe2ZK3ieqCI6SH9HNQYN57xOp04l/view?usp=drive_link)

1. 구글 드라이브에서 다운로드 받은 zip 파일을 압축 해제합니다.

2. ProjectSanzo.exe 파일을 관리자 권한으로 실행합니다.

### B. Git 버전

> **⚠️ 주의:** 본 프로젝트의 캐릭터 모델링을 정상적으로 로드하기 위해서는 **VRM4U 플러그인**이 필요합니다.

1. 이 저장소를 로컬 컴퓨터로 클론합니다.  
   git clone [https://github.com/NbcampUnreal/7th-Team6-CH3-Project.git](https://github.com/NbcampUnreal/7th-Team6-CH3-Project.git)
   
3. **플러그인 설치**: [VRM4U GitHub Releases](https://github.com/ruyo/VRM4U/releases) 페이지에서 사용 중인 언리얼 엔진 5 버전에 맞는 플러그인을 다운로드한 후, 프로젝트 최상단 경로에 Plugins 폴더를 생성하고 압축을 해제하여 넣습니다.

4. 프로젝트 폴더로 이동하여 ProjectSanzo.uproject 파일을 우클릭합니다.

5. Generate Visual Studio project files를 클릭하여 .sln 파일을 생성합니다.

6. 생성된 ProjectSanzo.sln 파일을 Visual Studio 2022로 엽니다.

7. 솔루션 구성이 Development Editor / Win64로 되어 있는지 확인하고 빌드(Ctrl + Shift + B) 합니다.

8. 빌드가 완료되면 .uproject 파일을 더블 클릭하여 언리얼 에디터를 실행합니다.

9. 에디터 툴바에서 Play(플레이) 버튼을 눌러 게임을 실행합니다.

---

## 👨‍💻 팀원 소개 및 역할

| 이름 | 역할 | 담당 업무 (세부 구현 내용) | GitHub |
| :---: | :--- | :--- | :--- |
| **&nbsp;김형백  &nbsp;&nbsp;** | **플레이어 & 전투** | - `GameplayTag` 기반 캐릭터 상태(조준, 회피 등) 관리<br>- `SanzoStatComponent`를 통한 스태미나 로직 및 I-Frame 회피 구현<br>- 패링 시스템(성공/실패 분기, 슬로우 효과, 데미지 반사) 고도화<br>- Interface 기반 모듈화 레벨업 시스템 설계 | [@kbrother102](https://github.com/kbrother102)|
| **&nbsp;김동주  &nbsp;&nbsp;** | **적 AI** | - EnemyBase 클래스 및 Ranged/Melee/Boss, BT/Blackboard AI 구현<br>- 보스전 2페이즈 전환 및 패턴 자동 선택 로직 구현 <br>- 피격/스턴 애니메이션 몽타주 및 콜리전 최적화 | [@DJKIM2002](https://github.com/DJKIM2002) |
| **&nbsp;이준로  &nbsp;&nbsp;** | **UI & 시스템** | - UMG 기반 HUD, 메인 메뉴, 팝업 UI 및 예외 처리<br>- `DataTable` 기반 플레이어 능력치 강화 시스템 설계<br>- GameState/PlayerController를 통한 게임 흐름 제어 및 엔딩 연출<br>- 길 안내 `NavigationArrow` 컴포넌트 구현 | [@JRoLee](https://github.com/JRoLee) |
| **&nbsp;최윤서  &nbsp;&nbsp;** | **코어 루프 & 레벨** | - `StageManager`, `RoomBase`를 활용한 웨이브 및 클리어 로직<br>- Nav Mesh 볼륨 최적화 및 동적 환경 레벨 디자인<br>- 상호작용 오브젝트(도자기, 드랍아이템 등) 구현<br>- 이펙트 & 사운드 연출 | [@yoonseo4343](https://github.com/yoonseo4343) |
| **&nbsp;이용호  &nbsp;&nbsp;** | **무기 & 기믹** | - `SanzoWeaponBase` 상속 구조의 총기, 활 로직 및 데미지 처리<br>- `SanzoProjectile` 클래스를 활용한 유도 미사일 업그레이드 발동 조건 및 동적 데미지 산정<br>- 나이아가라(Niagara) 기반 원거리 적 레이저 조준선 및 타겟팅 기믹 | [@YongHo9961](https://github.com/YongHo9961) |

---

## 🌟 주요 기능 (Key Features)

### 1. 플레이어 전투 및 컨트롤 (Player Mechanics)
* GameplayTag를 활용한 정교한 캐릭터 상태 제어.
* 스태미나 기반의 액션 시스템(탈진, 회복)과 무적 프레임(I-Frame)이 적용된 회피 로직.
* 리스크 & 리턴이 확실한 패링 시스템 (스팸 방지, 데미지 반사, 타임 슬로우 효과).
* 인터페이스 기반의 모듈화된 레벨업 및 외형/스탯 업그레이드 연동.

### 2. 적 AI 및 보스전 (Enemy AI & Boss Fight)
* AI Controller, Behavior Tree(BT), Blackboard를 활용한 상태 기반 적 AI 구현.
* 근접/원거리 적 2종류 및 스턴(Stun) 상태 이상 구현, 스턴 게이지 UI 연동.
* 나이아가라(Niagara) 이펙트를 활용한 원거리 적의 레이저 조준선(Aim Laser) 및 타겟팅 시스템.
* 보스 몬스터 특수 패턴: 5가지 공격 패턴 구현, 체력 비례에 따른 페이즈 전환(2페이즈) 및 공격 범위 변화 등 긴장감 있는 보스전 구현.

### 3. 유기적인 스테이지 흐름 및 게임 루프 (Stage & Core Loop)
* SanzoStageManager와 SanzoRoomBase를 활용한 모듈화된 스테이지 및 몹 웨이브 관리.
* 파괴 가능한 오브젝트(문, 도자기)를 통한 환경 상호작용 및 파밍 요소.
* 최적화된 Nav Mesh를 통해 AI의 매끄러운 추적 경로 보장.

### 4. 무기 및 업그레이드 시스템 (Weapon & Upgrade)
* C++ 상속을 활용한 무기 베이스(SanzoWeaponBase) 및 총기(SanzoGun), 활(SanzoBow) 클래스 설계.
* 콜리전 트레이스를 통한 정교한 타격 판정 및 유도 미사일(Missile) 발사 기믹 구현.
* DataTable을 연동한 무기별 공격력, 공격 속도 제어 및 확률 기반 플레이어 강화(Upgrade) 시스템 지원.

### 5. UI 및 UX 편의성 (UI & Experience)
* 팝업 및 중복 클릭 방지가 적용된 안정적인 UUserWidget 기반 인터페이스.
* NavigationArrow 컴포넌트를 활용한 직관적인 목표 길찾기 안내.
* 유저 편의성을 고려한 인게임 BGM 토글 시스템 및 엔딩 시네마틱 재생 연출.


## 📁 프로젝트 구조 (Directory Structure)

```text
7th-Team6-CH3-Project/  
├── Config/ # 프로젝트 설정 파일 (Input, Engine, Game 등)  
├── Content/ # 에셋 폴더 (블루프린트, 맵, 메쉬, 머티리얼, UI 등)  
├── Source/  
│ └── ProjectSanzo/ # 메인 C++ 소스 코드  
│   ├── Character/ # 플레이어 캐릭터 로직 (GameplayTags, Component, Controller 등)  
│   ├── AI/ # Enemy 및 Boss, AI 클래스  
│   ├── Weapon/ # WeaponBase 및 Gun, Bow 클래스 등   
│   ├── Stage/ # RoomBase, StageManager, EnemySpawn 등 스테이지 제어
│   ├── Core/ # GameMode, GameState, GameInstance, UpgradeData  
│   ├── Items/ # ItemBase 기반 Item 클래스 및 SpawnData 
│   ├── Common/ # Tag, Log, DamageType
│   ├── Notifies/ # 패링, 회피에 사용할 무적 프레임
│   └── UI/ # UMG 위젯 C++ 기반 메인 UI, HUD 클래스  
├── .gitattributes  
├── .gitignore  
├── pull_request_template.md # 팀 협업을 위한 PR 템플릿  
└── ProjectSanzo.uproject # 언리얼 엔진 프로젝트 실행 파일
```
---
