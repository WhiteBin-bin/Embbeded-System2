#include "mbed.h"
#include "main.h"
#include "sx1272-hal.h"
#include "debug.h"
#include "stdio.h"
#include <cstdint>

/* Set this flag to '1' to display debug messages on the console */
#define DEBUG_MESSAGE   1

/* Set this flag to '1' to use the LoRa modulation or to '0' to use FSK modulation */
#define USE_MODEM_LORA  1
#define USE_MODEM_FSK   !USE_MODEM_LORA

#define RF_FREQUENCY                                    915000000 // Hz
#define TX_OUTPUT_POWER                                 14        // 14 dBm
#define LORA_BANDWIDTH                                  0         // [0: 125 kHz,
                                                                //  1: 250 kHz,
                                                                //  2: 500 kHz,
                                                                //  3: Reserved]
#define LORA_SPREADING_FACTOR                           7         // [SF7..SF12]
#define LORA_CODINGRATE                                 1         // [1: 4/5,
                                                                //  2: 4/6,
                                                                //  3: 4/7,
                                                                //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                            8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                             5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                      false
#define LORA_FHSS_ENABLED                               false  
#define LORA_NB_SYMB_HOP                                4     
#define LORA_IQ_INVERSION_ON                            false
#define LORA_CRC_ENABLED                                true


#define RX_TIMEOUT_VALUE                                1000      // in ms
#define BUFFER_SIZE                                     32        // Define the payload size here

#define Rx_ID                                           23

/*
 *  Global variables declarations
 */
typedef enum
{
    LOWPOWER = 0,
    IDLE,

    RX,
    RX_TIMEOUT,
    RX_ERROR,

    TX,
    TX_TIMEOUT,

    CAD,
    CAD_DONE
}AppStates_t;

volatile AppStates_t State = LOWPOWER;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*
 *  Global variables declarations
 */
SX1272MB2xAS Radio(NULL);

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

uint8_t Msg[BUFFER_SIZE - 1] = "Hello";

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

int16_t RssiValue = 0.0;
int8_t SnrValue = 0.0;


// state 변수
uint8_t isDetect_IR = 0;
uint8_t isDetect_Light = 0;
uint8_t isDetect_TILT = 0;

uint8_t isLedLighting = 0;
uint8_t isBuzzerSounding = 0;

uint8_t isWorking = 0;


// 센서 핀 값
uint8_t irRead = 1;
uint8_t tiltRead = 1;
float lightValue = 0; // 빛 감지 아날로그값
uint8_t btnValue = 1;

float threshold = 0.3;      // 임계값 (빛의 밝기에 따라 조정 가능)

//시간 측정
int hour = 0, minute = 0, second = 0;



// Ticker 타이머 객체 생성
Ticker ticker;
// 콜백 함수
void on_ticker_interrupt() {
    sprintf((char*)Msg, "IR:%d, Light:%d, TILT:%d, But:%d", 
        isDetect_IR, isDetect_Light, isDetect_TILT, isWorking);
    //sprintf((char*)Msg, "%d %d %d %d", isDetect_IR, isDetect_Light, isDetect_TILT, isWorking);
    isDetect_IR = 0;
    isDetect_TILT = 0;
    isDetect_Light = 0;

    strcpy((char*)Buffer + 1, (char*)Msg);

    Radio.Send(Buffer, BufferSize);
    debug("...Ping\r\n");
    State = TX;
    printf("+%d:%d:%d \n", hour, minute, second);
}


// 내부 인터럽트 객체 생성
InterruptIn btn_sensor(PB_12);
//DigitalIn btn_sensor(PB_12, PullUp);


// 인터럽트 핸들러 함수 (하강 신호 감지)
void on_fall() {
    printf("btn_sensor PB_12핀이 LOW로 전환됨\n");
    isWorking=!isWorking; // 전원 상태 전환
    wait_ms(200);
}



//Initialize Pin
AnalogIn  light_sensor(PA_4);
DigitalIn IR_sensor(PB_1);
DigitalIn tilt_sensor(PB_15,PullUp);

DigitalOut Led_Red(PB_14);   // Red LED
DigitalOut Led_Green(PB_13); // Green LED
PwmOut buzzer(PB_2);




int main(void)
{
    uint8_t i;

    debug("\n\n\r     SX1272 Ping Pong Demo Application \n\n\r");

    // Initialize Radio driver
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.RxError = OnRxError;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    Radio.Init(&RadioEvents);

    // verify the connection with the board
    while (Radio.Read(REG_VERSION) == 0x00)
    {
        debug("Radio could not be detected!\n\r", NULL);
        wait(1);
    }

    debug_if((DEBUG_MESSAGE & (Radio.DetectBoardType() == SX1272MB2XAS)), "\n\r > Board Type: SX1272MB2xAS < \n\r");

    Radio.SetChannel(RF_FREQUENCY);
    phymac_id = 1;
    debug("[PHYMAC] ID : %i\n", phymac_id);

    debug_if(LORA_FHSS_ENABLED, "\n\n\r             > LORA FHSS Mode < \n\n\r");
    debug_if(!LORA_FHSS_ENABLED, "\n\n\r             > LORA Mode < \n\n\r");

    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
        LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
        LORA_IQ_INVERSION_ON, 2000);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
        LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
        LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0,
        LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
        LORA_IQ_INVERSION_ON, true);

    debug_if(DEBUG_MESSAGE, "Starting Ping-Pong loop\r\n");




    // Timer start
    ticker.attach(&on_ticker_interrupt, 1.0f);  // 1초 주기(float 단위) on_ticker_interrupt 함수 호출

    // interrupt start
    btn_sensor.mode(PullUp);
    btn_sensor.fall(&on_fall);

    // buzzer Initialization
    buzzer.period(1.0 / 1000);  // 초기 주파수를 1kHz로 설정 (1000Hz)
    buzzer.write(0.0);  // 듀티 사이클(소리크기)을 0%로 설정


    State = IDLE;
    Buffer[0] = Rx_ID;
    while (1)
    {
        //btnValue = btn_sensor.read();
        //printf("btnValue : %d\n", btnValue);

        if(isWorking){

            // light_sensor read 
            lightValue = light_sensor.read();  // (0.0 ~ 1.0)
            //printf("lightValue : %f\n", lightValue);
            //wait_ms(100);
            if (lightValue < threshold){ // 빛을 감지했을 때 약 0.02, 0.035 (0.3보다 작을 때)
                isDetect_Light=1;
                isLedLighting=0;
            } else{ // 빛을 감지 못했을 때 약 0.4 0.7
                //isDetect_Light=0;
                isLedLighting=1;
            }
        
            

            // tilt_sensor read
            tiltRead = tilt_sensor.read();
            //printf("tiltRead : %d\n", tiltRead);
            if(!tiltRead){ 
                isDetect_TILT = 1;
                isBuzzerSounding=1;
            }else{
                isBuzzerSounding=0;
            }

            // IR_sensor read
            irRead = IR_sensor.read();
            //printf("irRead : %d\n", irRead);
            if (!irRead)  
            {
                isDetect_IR = 1;
                isBuzzerSounding=1;
            } else{
                //isBuzzerSounding=0;
            }
        } 
        else {
            isLedLighting=0;
            isBuzzerSounding=0;
        }

        // 출력장치 동작
        if(isLedLighting){
            Led_Red = 0;    // Red LED 끄기
            Led_Green = 1;  // Green LED 켜기
        } else{
            Led_Red = 0;    // Red LED 끄기
            Led_Green = 0;  // Green LED 끄기
        }
        if(isBuzzerSounding){
            buzzer.write(0.5); // 소리내기
        } else{
            buzzer.write(0.0); // 소리끄기
        }

        if (isLedLighting or isBuzzerSounding){
            wait_ms(500); // 신호가 있으면 0.5초 기다리기
        }
                

        switch (State)
        {
        case IDLE:
            // wait_ms( 1000 );
            // strcpy( ( char* )Buffer+1, ( char* )Msg );

            // Radio.Send( Buffer, BufferSize );
            // debug( "...Ping\r\n" );
            // State = TX;
            break;
        case TX:
            break;
        case RX:
            break;

        default:
            //     State = TX;
            break;
        }
    }
}

void OnTxDone(void)
{
    Radio.Sleep();
    State = IDLE;


    // 초 증가
    second++;
    // 초가 60이면 분 증가, 초 초기화
    if (second >= 60) {
        second = 0;
        minute++;
    }
    // 분이 60이면 시간 증가, 분 초기화
    if (minute >= 60) {
        minute = 0;
        hour++;
    }
    // 시간이 24를 넘으면 초기화 (24시간 기준)
    if (hour >= 24) {
        hour = 0;
    }
    //printf("+%d:%d:%d ", hour, minute, second);

    //printf("Msg: %s\n\r", (char*)Msg);
    debug_if(DEBUG_MESSAGE, "+%d:%d:%d  Msg: %s\n\r", hour, minute, second, (char*)Msg);
    //debug_if(DEBUG_MESSAGE, "RX_ID: %d\n\r", Buffer[0]);
    debug_if(DEBUG_MESSAGE, "> OnTxDone\n\r");
}

void OnRxDone(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr)
{
    uint8_t dataIndFlag = 0;
    Radio.Sleep();
    State = IDLE;

    if (Buffer[PHYMAC_PDUOFFSET_TYPE] == PHYMAC_PDUTYPE_DATA &&
        Buffer[PHYMAC_PDUOFFSET_DSTID] == phymac_id)
    {
        dataIndFlag = 1;
    }

}

void OnTxTimeout(void)
{
    Radio.Sleep();
    // State = TX_TIMEOUT;
    State = IDLE;
    debug_if(DEBUG_MESSAGE, "> OnTxTimeout\n\r");
}

void OnRxTimeout(void)
{
    Radio.Sleep();
    Buffer[BufferSize] = 0;
    // State = RX_TIMEOUT;
    State = RX;
    debug_if(DEBUG_MESSAGE, "> OnRxTimeout\n\r");
}

void OnRxError(void)
{
    Radio.Sleep();
    // State = RX_ERROR;
    State = RX;
    debug_if(DEBUG_MESSAGE, "> OnRxError\n\r");
}