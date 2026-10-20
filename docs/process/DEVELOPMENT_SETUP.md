# 개발 및 디버그 환경

2026-10-08 설치 상태: Visual Studio Community 2026, MSVC v145(14.51), Windows SDK 10.0.26100.0을 확인했다. 게임은 C++17을 사용한다.

## Visual Studio 사용

1. [SimpleGame.sln](../../SimpleGame.sln)을 연다.
2. 개발 시 `Debug | x64`를 선택한다. SimpleGame을 시작 프로젝트로 지정한다.
3. 중단점을 설정하고 F5로 빌드 및 디버그한다. Ctrl+F5는 디버거 없이 실행한다.

[Development.props](../../SimpleGame/Development.props)가 실행 파일과 작업 폴더를 지정한다. 출력은 `x64/Debug`, `x64/Release`, `Win32/Debug`, `Win32/Release`로 분리되며 중간 파일은 `intermediate`에 저장된다. Debug는 최적화를 끄고 PDB 및 기본 런타임 검사를 사용한다. 기존 자산 복사 작업과 함께 셰이더·플랫폼별 DLL도 출력 폴더에 복사한다.

DLL 원본은 `SimpleGame/Dependencies`(32비트) 및 `SimpleGame/Dependencies/x64`(64비트)에 보관한다. 기존 출력 폴더의 DLL에서 PE 아키텍처를 확인해 복사했다. 다운로드하거나 버전을 교체하지 않았다.

## 터미널 사용

프로젝트 폴더의 PowerShell에서 다음을 실행한다.

```powershell
. .\tools\Enter-DevShell.ps1
```

현재 터미널에 Visual Studio C++ 도구를 연결한다. Git/Python이 없거나 Windows 실행 별칭만 있으면 현재 PC에 있는 Codex 번들 도구를 연결한다. 영구 PATH나 실행 정책은 변경하지 않는다. 다른 PC에 번들 도구가 없다면 Git/Python을 따로 설치해야 한다.

아래 명령은 사용자가 빌드를 원할 때 직접 실행한다.

```powershell
msbuild SimpleGame.sln /m /p:Configuration=Debug /p:Platform=x64
```

## 검증 범위

설치 도구와 DLL 아키텍처, 프로젝트 파일·헤더 경로, 네 가지 구성의 MSBuild 속성을 정적으로 검사한다. 설정 시 게임 빌드·실행·중단점 동작 검증은 수행하지 않는다. 실제 컴파일 오류나 그래픽 드라이버 관련 실행 오류가 없다는 보장은 아니다.
