# Forest Village — 숲속 마을을 되살리는 힐링 어드벤처 RPG

▶︎ [데모 영상 보기 (작성 예정)] | ▶︎ [프로젝트 진행 보고서 보기 (링크 예정)] | ▶︎ [실행 파일 (링크 예정)]

![게임플레이 GIF](여기에_메인_게임플레이_GIF_경로_입력.gif)

## 한 줄 요약
장르: 3D 탑다운 어드벤처 RPG / 제작 기간: (작성 예정) / 사용 기술: Unreal Engine 5, C++ / 1인 개발

## 핵심 기능
1. **커스텀 물리 기반 이동 시스템** — `APawn` 기반의 커스텀 중력을 직접 구현하여, 걷기 애니메이션 부재의 한계를 통통 튀는 점프 이동 조작감으로 승화
2. **독립적 UMG UI 제어 시스템** — 인벤토리, 퀘스트, HUD 등 다중 창 토글 시 `IsInViewport()` 교차 검증을 통해 포커스 충돌 및 조작 먹통 현상 완벽 방지
3. **상태 기반 시각적 복원 연출** — 3단계 퀘스트 달성에 따라 `AVillageFacility`의 메쉬를 실시간으로 교체(Swap)하고 Niagara 파티클을 스폰하여 즉각적인 피드백 제공

## 내가 직접 만든 부분
- **클라이언트 프로그래밍 (100%)**: C++ 클래스 설계(GameMode, Pawn, UI), 입력 라우팅 최적화, 퀘스트 로직 구현
- **레벨 디자인 및 기획 (100%)**: 에셋 배치, 퀘스트 플로우 설계
- **사용 외부 에셋**: 
  - FANTASTIC - Village Pack (마을 모델링)
  - GUI Parts (인벤토리 UI 요소)
  - Stylized Low Poly Rocks Lite (자원 채집 외형)

## 기술적 도전
**문제**: NPC 대화 도중 상호작용 키(E)를 누르면 등 뒤의 자원이 동시에 채집되는 입력 이벤트 전파(Propagation) 충돌 문제 발생
**해결**: `DialogueWidget` 클래스 내부에서 `NativeOnKeyDown` 함수를 오버라이드하여 E키 입력을 낚아챈 후, `FReply::Handled()`를 반환하여 하위 폰(`PlayerPawn`)으로의 이벤트 전파를 원천 차단함
**배움**: UI와 월드 간의 복잡한 입력 라우팅 구조를 이해하고, 엔진 내장 방어 코드(`Handled()`)를 적재적소에 활용하는 캡슐화(Encapsulation)의 중요성을 체득함

→ [상세 트러블슈팅 및 추가 버그 해결 사례는 프로젝트 보고서 참조](보고서_링크_입력)

## 기술 스택
- 언어: C++ (Unreal C++)
- 게임 엔진: Unreal Engine 5 (Enhanced Input, Niagara FX 적용)
- 빌드 환경: JetBrains Rider
- 형상 관리: Git / GitHub

## 빌드 및 실행 방법
1. 본 리포지토리를 Clone 합니다: `git clone https://github.com/hwaryeongg/ForestVillage.git`
2. `.uproject` 파일을 우클릭하고 **Generate Visual Studio project files**를 선택합니다.
3. Rider 또는 Visual Studio로 `.sln` 파일을 열고, Development Editor - Win64 환경에서 솔루션을 빌드합니다.
4. 언리얼 에디터가 실행되면 `Play` 버튼을 눌러 게임을 시작합니다.

## 보고서·문서
- [프로젝트 진행 보고서](보고서_링크_입력) — 기획, 클래스 설계, 상세 문제 해결 과정 등 전체 개발 과정 상세 기록
