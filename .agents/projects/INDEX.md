# 프로젝트 로컬 작업 인덱스

이 프로젝트의 작업 기록과 함수 검색 DB는 `.local_knowledge/work/project_hub.sqlite3`에 저장한다. 문서 내용 검색 DB는 `.local_knowledge/search/index.sqlite3`에 별도로 저장한다. 두 DB 모두 이 프로젝트 안에 있고 Git에서 제외된다. 상위 폴더의 DB나 함수 사전을 사용하지 않는다.

프로젝트 루트에서 다음 명령을 실행한다.

```text
python tools/knowledge/project_hub.py start game-software "작업 제목" --item "확인할 실행 단위"
python tools/knowledge/project_hub.py begin TASK_ID ITEM_ID
python tools/knowledge/project_hub.py finish-item TASK_ID ITEM_ID --note "확인 결과"
python tools/knowledge/project_hub.py finish TASK_ID --summary "완료 결과"
python tools/knowledge/project_hub.py show TASK_ID
python tools/knowledge/project_hub.py history --project game-software

python tools/knowledge/project_hub.py index-functions --project game-software
python tools/knowledge/project_hub.py find-function game-software "함수명"

python tools/knowledge/local_knowledge.py index
python tools/knowledge/local_knowledge.py search "검색어"
python tools/knowledge/local_knowledge.py stats
```

함수의 역할과 주요 흐름은 [함수 사전](FUNCTIONS.md)에 기록한다. 함수 인덱스는 선언을 정적으로 추출하므로 동작을 판단할 때 구현 파일을 확인한다.
