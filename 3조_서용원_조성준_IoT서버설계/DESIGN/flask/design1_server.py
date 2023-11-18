from flask import Flask, render_template, redirect, jsonify  # Flask 모듈을 임포트하여 웹 애플리케이션을 만들 수 있게 함
from flask_mqtt import Mqtt  # Flask 확장 기능인 flask_mqtt를 통해 MQTT 프로토콜을 사용 가능
import time  # 시간과 관련된 기능을 사용하기 위해 time 모듈을 임포트
import json  # JSON 데이터를 처리하기 위해 json 모듈을 임포트

app = Flask(__name__)  # Flask 인스턴스 생성

# MQTT 브로커 설정
app.config['MQTT_BROKER_URL'] = 'sweetdream.iptime.org'  # MQTT 브로커의 URL 설정
app.config['MQTT_BROKER_PORT'] = 1883  # MQTT 서비스 포트 설정 (1883은 MQTT 기본 포트)
app.config['MQTT_USERNAME'] = 'iot'  # MQTT 브로커 사용자 이름 설정
app.config['MQTT_PASSWORD'] = 'csee1414'  # MQTT 브로커 비밀번호 설정

mqtt = Mqtt(app)  # Flask 애플리케이션에 MQTT 클라이언트 설정

# 서용원 MCU용 변수들임
pub_topic_SEO = 'iot/21800366'  # 서용원의 MCU로 메시지를 발행하기 위한 MQTT 토픽
sub_topic_all_data_SEO = 'iot/21800366/autodht22'  # 서용원의 MCU에서 발행하는 데이터를 구독하기 위한 MQTT 토픽

# 서용원 MCU의 마지막 데이터를 저장할 변수들
last_temperature_SEO = "Not Updated Yet"  # 마지막 온도 값 저장 변수
last_humidity_SEO = "Not Updated Yet"  # 마지막 습도 값 저장 변수
last_light_SEO = "Not Updated Yet"  # 마지막 빛 센서 값 저장 변수
mqtt_message_SEO = ''  # 서용원 MCU에서 받은 MQTT 메시지를 저장할 변수
last_command_SEO= None  # 마지막으로 실행된 명령어(온도 or 습도)를 구분하기 위한 변수

# 조성준 MCU용 변수들임
pub_topic_CHO = 'iot/21900699'  # 조성준의 MCU로 메시지를 발행하기 위한 MQTT 토픽
sub_topic_all_data_CHO = 'iot/21900699/autodht22'  # 조성준의 MCU에서 발행하는 데이터를 구독하기 위한 MQTT 토픽

# 조성준 MCU의 마지막 데이터를 저장할 변수들
last_temperature_CHO = "Not Updated Yet"  # 마지막 온도 값 저장 변수
last_humidity_CHO = "Not Updated Yet"  # 마지막 습도 값 저장 변수
last_light_CHO = "Not Updated Yet"  # 마지막 빛 센서 값 저장 변수
mqtt_message_CHO = ''  # 조성준 MCU에서 받은 MQTT 메시지를 저장할 변수
last_command_CHO= None  # 마지막으로 실행된 명령어(온도 or 습도)를 구분하기 위한 변수

print('@@ Use URL:/iot/21800366/{led,dht22,cds}')  # 사용자가 애플리케이션에서 어떤 URL을 사용할 수 있는지 콘솔에 출력

# '/' 경로로 웹 요청이 들어왔을 때 실행될 뷰 함수
@app.route('/iot/21800366/')
def main_page():
    # design1_index.html 템플릿을 렌더링하여 반환하고, 
    # MQTT 메시지와 마지막 센서 데이터를 웹 페이지로 전달
    return render_template('design1_index.html', result_SEO=mqtt_message_SEO, threevalue_SEO={"temperature_SEO": last_temperature_SEO, "humidity_SEO": last_humidity_SEO, "light_SEO": last_light_SEO}, 
                                                result_CHO=mqtt_message_CHO, threevalue_CHO={"temperature_CHO": last_temperature_CHO, "humidity_CHO": last_humidity_CHO, "light_CHO": last_light_CHO})

@app.route('/iot/21800366/<cmd>')  # 플라스크에서 주어지는 라우트를 이용함. <cmd>는 경로의 변수 부분임.
def get_command_SEO(cmd):  # cmd 인자가 있다고 할 때 적절히 처리하기 위한 함수임.
    global mqtt_message_SEO, last_command_SEO   # message는 mqtt로 보낼 때 이용하고, 
                                                #last command는 온도와 습도가 함께 넘어오기 때문에 그것을 파싱하기 위해 저장함
    last_command_SEO = cmd 

    # 인자가 정해진 명령어 리스트 중 하나인지 확인함
    if cmd in ['led', 'ledon', 'ledoff', 'usbled', 'usbledon', 'usbledoff']:
        mqtt.publish(pub_topic_SEO, cmd)  # MQTT를 통해 cmd를 해당 토픽으로 발행함
        
        # 발행한 MQTT 메시지에 따라 상태 메시지를 설정함
        if cmd == 'led':
            mqtt_message_SEO = 'LED toggled!'
        elif cmd == 'ledon':
            mqtt_message_SEO = 'LED ON!'
        elif cmd == 'ledoff':
            mqtt_message_SEO = 'LED OFF!'
        elif cmd == 'usbled':
            mqtt_message_SEO = 'USB LED toggled!'
        elif cmd == 'usbledon':
            mqtt_message_SEO = 'USB LED ON!'
        elif cmd == 'usbledoff':
            mqtt_message_SEO = 'USB LED OFF!'
        else:
            pass  # 그 외의 경우는 아무것도 하지 않음.

        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO,
                                threevalue_SEO={"temperature_SEO": last_temperature_SEO, 
                                                "humidity_SEO": last_humidity_SEO, 
                                                "light_SEO": last_light_SEO}, 
                                result_CHO=mqtt_message_CHO, 
                                threevalue_CHO={"temperature_CHO": last_temperature_CHO, 
                                                "humidity_CHO": last_humidity_CHO, 
                                                "light_CHO": last_light_CHO})
    
    elif cmd in ['dht22_tem', 'dht22_hum']:
        mqtt.publish(pub_topic_SEO, 'dht22')  # 'dht22' 명령을 MQTT 토픽에 발행함.
        # json 형태로 온도 습도가 모두 올 것이기 때문에 이렇게 처리하고 실질적인 값을 받은 후 서버에서 파싱함
        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO, 
                                threevalue_SEO={"temperature_SEO": last_temperature_SEO, 
                                                "humidity_SEO": last_humidity_SEO, 
                                                "light_SEO": last_light_SEO}, 
                                result_CHO=mqtt_message_CHO, 
                                threevalue_CHO={"temperature_CHO": last_temperature_CHO, 
                                                "humidity_CHO": last_humidity_CHO, 
                                                "light_CHO": last_light_CHO})
    elif cmd in ['cds']:
        mqtt.publish(pub_topic_SEO, cmd)  # 'cds' 명령을 MQTT 토픽에 발행함.
        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO, 
                                threevalue_SEO={"temperature_SEO": last_temperature_SEO, 
                                                "humidity_SEO": last_humidity_SEO, 
                                                "light_SEO": last_light_SEO}, 
                                result_CHO=mqtt_message_CHO, 
                                threevalue_CHO={"temperature_CHO": last_temperature_CHO, 
                                                "humidity_CHO": last_humidity_CHO, 
                                                "light_CHO": last_light_CHO})
    else:
        # cmd가 정해진 명령어 리스트에 없으면 홈 화면으로 갈 수 있게 유도함.
        return redirect('/iot/21800366/')

@app.route('/iot/21900699/<cmd>') # 플라스크에서 주어지는 라우트를 이용함. <cmd>는 경로의 변수 부분임.
def get_command_CHO(cmd): # cmd 인자가 있다고 할 때 적절히 처리하기 위한 함수임.
    global mqtt_message_CHO, last_command_CHO   # message는 mqtt로 보낼 때 이용하고,
                                                #last command는 온도와 습도가 함께 넘어오기 때문에 그것을 파싱하기 위해 저장함

    last_command_CHO=cmd
    
    # 인자가 정해진 명령어 리스트 중 하나인지 확인함
    if cmd in ['led', 'ledon', 'ledoff', 'usbled', 'usbledon', 'usbledoff']:
        mqtt.publish(pub_topic_CHO, cmd) # MQTT를 통해 cmd를 해당 토픽으로 발행함
        
        # 발행한 MQTT 메시지에 따라 상태 메시지를 설정함
        if cmd=='led':
                mqtt_message_CHO='LED toggled!'
        elif cmd=='ledon':
                mqtt_message_CHO='LED ON!'
        elif cmd=='ledoff':
                mqtt_message_CHO='LED OFF!'
        elif cmd=='usbled':
                mqtt_message_CHO='USB LED toggled!'
        elif cmd=='usbledon':
                mqtt_message_CHO='USB LED ON!'
        elif cmd=='usbledoff':
                mqtt_message_CHO='USB LED OFF!'
        else:
                pass  # 그 외의 경우는 아무것도 하지 않음.

        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO, threevalue_SEO={"temperature_SEO": last_temperature_SEO, "humidity_SEO": last_humidity_SEO, "light_SEO": last_light_SEO}, 
                                                result_CHO=mqtt_message_CHO, threevalue_CHO={"temperature_CHO": last_temperature_CHO, "humidity_CHO": last_humidity_CHO, "light_CHO": last_light_CHO})
    elif cmd in ['dht22_tem', 'dht22_hum']:
        mqtt.publish(pub_topic_CHO, 'dht22') # 'dht22' 명령을 MQTT 토픽에 발행함.
        # json 형태로 온도 습도가 모두 올 것이기 때문에 이렇게 처리하고 실질적인 값을 받은 후 서버에서 파싱함
        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO, threevalue_SEO={"temperature_SEO": last_temperature_SEO, "humidity_SEO": last_humidity_SEO, "light_SEO": last_light_SEO}, 
                                                result_CHO=mqtt_message_CHO, threevalue_CHO={"temperature_CHO": last_temperature_CHO, "humidity_CHO": last_humidity_CHO, "light_CHO": last_light_CHO})
    elif cmd in ['cds']:
        mqtt.publish(pub_topic_CHO, cmd) # 'cds' 명령을 MQTT 토픽에 발행함.
        # 웹 페이지에 필요한 데이터와 함께 HTML 템플릿을 렌더링하여 반환하여 웹 화면을 띄우게 함.
        return render_template('design1_index.html', result_SEO=mqtt_message_SEO, threevalue_SEO={"temperature_SEO": last_temperature_SEO, "humidity_SEO": last_humidity_SEO, "light_SEO": last_light_SEO}, 
                                                result_CHO=mqtt_message_CHO, threevalue_CHO={"temperature_CHO": last_temperature_CHO, "humidity_CHO": last_humidity_CHO, "light_CHO": last_light_CHO})
    else:
        # cmd가 정해진 명령어 리스트에 없으면 홈 화면으로 갈 수 있게 유도함.
        return redirect('/iot/21800366/')


@app.route('/<path:_>')  # 다른 모든 URL 패턴을 잡기 위한 라우트임.
def redirect_to_main(_):  # 다른 모든 경로를 무시함.
    return redirect('/iot/21800366/')  # 메인 페이지로 리다이렉트.

@app.route('/get_data')  # 자바스크립트에서 10초마다 서버에 요구할 수 있는 지점을 만들어줌.
def get_data():  # 센서 데이터를 반환하는 함수임.
    global last_temperature_SEO, last_humidity_SEO, last_light_SEO, last_temperature_CHO, last_humidity_CHO, last_light_CHO
    # 최신 센서 데이터를 저장하기 위해 변수를 선언함.

    data = {
        "temperature_SEO": last_temperature_SEO,
        "humidity_SEO": last_humidity_SEO,
        "light_SEO": last_light_SEO,
        "temperature_CHO": last_temperature_CHO,
        "humidity_CHO": last_humidity_CHO,
        "light_CHO": last_light_CHO
    }
    return jsonify(data)  # 웹에게 JSON 형태로 데이터를 반환함.

@app.errorhandler(404)  # 404 에러가 났을 때 처리하기 위한 라우트임.
def page_not_found(e):  # 404 에러를 처리하는 함수임. 'e'는 에러 객체임.
    # 메인 페이지처럼 마지막 알려진 값들로 메인 템플릿 웹에 보여줌.
    # 버튼을 누르면 정확한 주소로 넘어가기 때문에 괜찮음
    return render_template('design1_index.html', result_SEO=mqtt_message_SEO, 
                            threevalue_SEO={"temperature_SEO": last_temperature_SEO, 
                                            "humidity_SEO": last_humidity_SEO, 
                                            "light_SEO": last_light_SEO}, 
                            result_CHO=mqtt_message_CHO, 
                            threevalue_CHO={"temperature_CHO": last_temperature_CHO, 
                                            "humidity_CHO": last_humidity_CHO, 
                                            "light_CHO": last_light_CHO})

@mqtt.on_connect()  # MQTT가 연결될 때 실행될 콜백 함수를 정의하는 부분임.
def handle_connect(client, userdata, flags, rc):  # 클라이언트가 연결될 때의 콜백 함수임.
    # 데이터를 받기 위해 해당하는 토픽을 구독함.
    mqtt.subscribe(sub_topic_all_data_SEO)  # 서용원 mcu에서 10초마다 받는 온도 습도 조도를 구하기 위해 구독.
    mqtt.subscribe('iot/21800366/dht22')  # 서용원 mcu에서 온도 습도를 구하기 위해 구독함. 두 값이 한꺼번에 json으로 넘어올 것임.
    mqtt.subscribe('iot/21800366/cds')  # 서용원 mcu에서 조도를 구하기 위해 구독.
    mqtt.subscribe(sub_topic_all_data_CHO)  # 조성준 mcu에서 10초마다 받는 온도 습도 조도를 구하기 위해 구독.
    mqtt.subscribe('iot/21900699/dht22')  # 조성준 mcu에서 온도 습도를 구하기 위해 구독함. 두 값이 한꺼번에 json으로 넘어올 것임.
    mqtt.subscribe('iot/21900699/cds')  # 조성준 mcu에서 조도를 구하기 위해 구독.

@mqtt.on_message()  # MQTT 메시지가 도착했을 때 실행될 함수를 정의하는 부분임.
def handle_mqtt_message(client, userdata, message):
    # 10초마다 오는 값이나 버튼에 대한 센서 값을 저장할 변수들임
    global mqtt_message_SEO, last_temperature_SEO, last_humidity_SEO, last_light_SEO, mqtt_message_CHO, last_temperature_CHO, last_humidity_CHO, last_light_CHO
    
    topic = message.topic  # 토픽을 구분함. 10초마다 온건지, 아니면 dht인지, cds인지
    payload = message.payload.decode('utf-8')  # 메시지 페이로드(내용), utf-8로 디코딩
    
    try:
        data = json.loads(payload)  # JSON 형식의 메시지를 파이썬 딕셔너리로 변환

        # 서용원 mcu에서 10초마다 오는 값인 경우
        if topic == sub_topic_all_data_SEO:
            # 각 센서 값에 대한 문자열 형식 지정 및 저장
            last_temperature_SEO = f"{data['temperature']}°C"
            last_humidity_SEO = f"{data['humidity']}%"
            last_light_SEO = f"{data['cds']} lux"
            
        # 서용원 mcu에서 dht22 요청에 대해 돌아온 값일 경우
        elif topic == "iot/21800366/dht22":
            temperature = f"{data['temperature']}°C"
            humidity = f"{data['humidity']}%"
            # print(f"{temperature}, {humidity}")  # 콘솔에 온도와 습도 출력 (만들 때 확인용이었던 코드임)
            # 마지막으로 받은 명령어에 따라 온도 또는 습도 정보를 저장
            if last_command_SEO == "dht22_tem":
                mqtt_message_SEO = f"Temp:{temperature}"
            elif last_command_SEO == "dht22_hum":
                mqtt_message_SEO = f"Humidity: {humidity}"
                
        # 서용원 mcu에서 cds 요청에 대해 돌아온 값일 경우
        elif topic == "iot/21800366/cds":
            light = f"{data['cds']} lux"
            mqtt_message_SEO = f"Light: {light}" #조도 값 저장

        # 조성준 mcu에서 10초마다 오는 값인 경우
        elif topic == sub_topic_all_data_CHO:
            # 각 센서 값에 대한 문자열 형식 지정 및 저장
            last_temperature_CHO = f"{data['temperature']}°C"
            last_humidity_CHO = f"{data['humidity']}%"
            last_light_CHO = f"{data['cds']} lux"
            
        # 조성준 mcu에서 dht22 요청에 대해 돌아온 값일 경우
        elif topic == "iot/21900699/dht22":
            temperature = f"{data['temperature']}°C"
            humidity = f"{data['humidity']}%"
            # print(f"{temperature}, {humidity}")  # 콘솔에 온도와 습도 출력(위와 마찬가지로 테스트용도였음)
            # 마지막으로 받은 명령어에 따라 온도 또는 습도 정보를 저장
            if last_command_CHO == "dht22_tem":
                mqtt_message_CHO = f"Temp:{temperature}"
            elif last_command_CHO == "dht22_hum":
                mqtt_message_CHO = f"Humidity: {humidity}"
                
        # 조성준 mcu에서 cds 요청에 대해 돌아온 값일 경우
        elif topic == "iot/21900699/cds":
            light = f"{data['cds']} lux"
            mqtt_message_CHO = f"Light: {light}"  # 조도 값 저장
    except json.JSONDecodeError:
        print("Error decoding JSON")  # JSON 해석 중 에러가 발생했을 때 출력 (뜨지 않아야 정상임)
    # 콘솔에 각 토픽과 관련된 데이터 출력. 확인용이었는데 연결 여부가 잘 보여서 계속 띄우기로 결정함
    print(f"Topic: {topic}, Message(button): {mqtt_message_SEO}, Message(per 10 seconds): {last_temperature_SEO}, {last_humidity_SEO}, {last_light_SEO} \n Message(button): {mqtt_message_CHO}, Message(per 10 seconds): {last_temperature_CHO}, {last_humidity_CHO}, {last_light_CHO}  ")

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=False) 
