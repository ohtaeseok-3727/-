# GLFW 3.4 — Windows 배포 의존성

공식 GLFW 3.4 릴리스의 VC2022 DLL 패키지를 수정 없이 사용한다. 다운로드 URL과 원본 ZIP SHA-256은 [provenance.json](provenance.json)에 기록했다. 최신 버전 자동 추적이나 빌드 중 네트워크 다운로드는 없다.

- `include/GLFW`: 공개 헤더와 native 헤더.
- `Win32`: x86 `glfw3.dll`, `glfw3dll.lib`.
- `x64`: x64 `glfw3.dll`, `glfw3dll.lib`.
- [LICENSE.md](LICENSE.md): zlib/libpng 라이선스. 재배포에 함께 포함한다.

프로젝트의 네 구성(Debug/Release × Win32/x64)은 상대 경로로 해당 import library를 링크하며 `GLFW_DLL`을 정의한다. `Development.props`가 GLFW와 기존 GLEW DLL을 실행 파일 옆에 복사한다. GLFW의 import library는 DLL용이므로 게임의 Debug/Release CRT 선택과 별도로 사용할 수 있다.

대상은 기존 게임과 동일한 Windows 10/11 x86/x64, OpenGL 3.3 compatibility 드라이버 환경이다. macOS/Linux/ARM 네이티브 지원을 의미하지 않는다. 게임에는 WIC·Windows 글꼴 등 별도의 Win32 의존성이 남아 있다.

Visual Studio C++ 데스크톱 개발 워크로드와 Windows SDK가 필요하다. 기본 도구 집합은 기존 v145이며 VS2022에서는 설치된 v143을 `/p:PlatformToolset=v143`으로 지정하거나 프로젝트 속성에서 선택한다. 다른 도구 집합의 실제 빌드는 미검증이다.

배포는 Release 출력 폴더 전체(exe, DLL, Assets, Shaders)를 복사한다. Release 빌드 시 설치된 MSVC 도구 집합의 재배포 가능 CRT DLL도 `VCToolsRedistDir`에서 복사한다. 이 경로가 없는 환경은 해당 아키텍처의 Microsoft Visual C++ Redistributable 설치가 필요하다. Debug 출력은 개발 도구가 없는 PC의 배포용이 아니다. DLL import 정적 검사에서 GLFW는 VCRUNTIME140과 Windows Universal CRT를 요구한다.

공식 문서: [릴리스](https://github.com/glfw/glfw/releases/tag/3.4), [입력](https://www.glfw.org/docs/3.4/input_guide.html), [창](https://www.glfw.org/docs/3.4/window_guide.html).
