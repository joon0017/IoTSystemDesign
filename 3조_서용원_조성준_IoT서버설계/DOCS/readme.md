# 설계과제 1

IoT 시스템 설계

팀 구성원:

- 21800366 서용원
- 21900699 조성준

## 파일 구성

`/DOCS` : 이 프로젝트의 모든 문서가 포함되어 있는 directory

`/DOCS/readme.md` 현재 이 파일

`/DOCS/설계과제1 보고서.docx` 이 프로젝트의 보고서 및 소스코드 문서화 doc파일

`/DOCS/설계과제1 보고서.pdf` 이 프로젝트의 보고서 및 소스코드 문서화 pdf 파일

`/DESIGN` : 이 프로젝트의 모든 소스코드가 포함되어 있는 directory

`/DESIGN/design1_mqtt/design1_mqtt.ino` NodeMCU 제어를 위한 아두이노 소스코드

`/DESIGN/flask` Raspberry Pi에서 실행시키는 파일이 포함되어 있는 풀더

`/DESIGN/flask/design1_server.py` flask를 통해 웹페이지 서버를 구동시키기 위한 소스코드

`/DESIGN/flask/templates/design1_index.html` flask 서버에서 웹 페이지를 렌더링 할 때 사용되는 html 문서

## 사용법

1. `/DESIGN/flask ` 풀더를 서버(Raspberry Pi)로 옮긴다. 이후, 다음 명령어를 실행시킨다.
   `python3 ./flask/design1_server.py`
2. 웹 브라우저를 통해 `http://[raspberrypi의 ip 주소]:5000/iot/21800366` 으로 접속한다.
3. `/DESIGN/design1_mqtt/design1_mqtt.ino `파일을 아두이노로 실행 시킨다
4. NodeMCU를 USB 포트를 통해 연결 하고, 아두두이노 IDE를 통해, `/DESIGN/design1_mqtt/design1_mqtt.ino` 파일을 컴파일 하고, NodeMCU에 업로드 한다. 업로드가 끝나면, OLED에 온습도가 정상적으로 작동 되고 있는지 육안으로 확인 할 수 있다.
5. 다시 웹 브라우저로 돌아와, 원하는 버튼을 눌러 원격으로 기기를 제어 할 수 있다.
