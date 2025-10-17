#include "json.h"
#include "allDefenition.h"
#include "delay.h"
#include <stdio.h>
#include "menuManualControl.h"
#include "menuSetting.h"
#include "menuManualControl.h"

void beep(uint8_t num);

UART_JSON_Parser uart_parser;

/*
"id"									: "00000000-0000-0000-0000-000000000000"			//Уникальный идентификатор устройства
"temperature_current"	: 0-105																				//Текущая температура воды, °С
"heat_temperature"		: 0-105																				//Заданная температура нагрева, °С
"mute"								: 0-1																					//Беззвучный режим
"time"								: 0-86400																			//Текущее время, в сек. с начала суток
*/
void convertToJSON(DeviceStatus_t * model, char * data_out)
{	
	
	//char str[50];
	data_out[0] = '{';
	size_t len = 1; // Уже есть '{'
	
	len += sprintf(data_out + len, "\"id\":%s,", model->id);

	
	len += sprintf(data_out + len, "\"temperature_current\":%.1f,", model->temperatureCurrent);
	//strcat(data_out, str);	
//	sprintf(str, "\"pump\":%i,", model->Pump1.PumpRelay.SwitchedOn);
//	strcat(data_out, str);
//	sprintf(str, "\"heat\":%i,", model->Heat);
//	strcat(data_out, str);
	len += sprintf(data_out + len, "\"heat_temperature\":%i,", model->temperatureTurget);

	
//	sprintf(str, "\"mode\":\"%s\",", mode_as_string[model->mode]);
//	strcat(data_out, str);
	
	len += sprintf(data_out + len, "\"mute\":%i,", model->isMuted);

	len += sprintf(data_out + len, "\"time\":%d", (uint32_t)(getGlobalTime()/1000));

	
	data_out[len] = '}';
	//data_out[strlen(data_out)] = 0;
}

/*************************** разбираем принятые данные *********************/
// Самописные замены string.h функций
static void my_memset(char *dest, char value, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = value;
    }
}

static void my_strncpy(char *dest, const char *src, int max_len) {
    int i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int string_length(const char *str, int max_len) {
    int len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}

// Инициализация парсера
void uart_parser_init(UART_JSON_Parser *parser) {
    my_memset(parser->buffer, 0, UART_BUFFER_SIZE);
    parser->head = 0;
    parser->tail = 0;
    parser->last_json_length = 0;
}

// Добавление байта в буфер
void uart_receive_byte(UART_JSON_Parser *parser, char byte) {
    parser->buffer[parser->head] = byte;
    parser->head = (parser->head + 1) % UART_BUFFER_SIZE;
    
    if (parser->head == parser->tail) {
        parser->tail = (parser->tail + 1) % UART_BUFFER_SIZE;
    }
}

// Поиск полного JSON сообщения
int has_complete_json(UART_JSON_Parser *parser, char *output, int max_len) {
    uint16_t start = parser->tail;
    uint16_t end = parser->head;
    
    if (start == end) {
        return 0;
    }
    
    int brace_count = 0;
    int in_string = 0;
    uint16_t json_start = start;
    uint16_t json_end = start;
    int found_start = 0;
    int output_index = 0;
    
    uint16_t i = start;
    while (i != end) {
        char c = parser->buffer[i];
        
        if (c == '"') {
            uint16_t prev_index = (i == 0) ? UART_BUFFER_SIZE - 1 : i - 1;
            if (parser->buffer[prev_index] != '\\') {
                in_string = !in_string;
            }
        }
        
        if (!in_string) {
            if (c == '{') {
                brace_count++;
                if (brace_count == 1) {
                    json_start = i;
                    found_start = 1;
                }
            } else if (c == '}') {
                if (brace_count > 0) {
                    brace_count--;
                }
                if (brace_count == 0 && found_start) {
                    json_end = (i + 1) % UART_BUFFER_SIZE;
                    
                    output_index = 0;
                    uint16_t j = json_start;
                    while (j != json_end && output_index < max_len - 1) {
                        output[output_index] = parser->buffer[j];
                        output_index++;
                        j = (j + 1) % UART_BUFFER_SIZE;
                    }
                    output[output_index] = '\0';
                    parser->last_json_length = output_index;
                    
                    parser->tail = json_end;
                    return 1;
                }
            }
        }
        
        i = (i + 1) % UART_BUFFER_SIZE;
    }
    
    return 0;
}

// Вспомогательные функции для парсинга
static const char* find_field_value(const char *json, uint16_t json_length, const char *field) {
    char search_pattern[64];
    int i = 0;
    int field_len = 0;
    
    search_pattern[i++] = '"';
    while (field[field_len] != '\0' && field_len < 50) {
        search_pattern[i++] = field[field_len++];
    }
    search_pattern[i++] = '"';
    search_pattern[i++] = ':';
    search_pattern[i] = '\0';
    
    // Поиск подстроки
    for (uint16_t pos = 0; pos <= json_length - i; pos++) {
        int match = 1;
        for (int j = 0; j < i; j++) {
            if (json[pos + j] != search_pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return &json[pos + i];
        }
    }
    return NULL;
}

static int string_to_int(const char *str) {
    int result = 0;
    int i = 0;
    
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return result;
}

static float string_to_float(const char *str) {
    float result = 0.0;
    float fraction = 0.0;
    float divisor = 1.0;
    int i = 0;
//    int has_decimal = 0;
    
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10.0 + (str[i] - '0');
        i++;
    }
    
    if (str[i] == '.') {
//        has_decimal = 1;
        i++;
        while (str[i] >= '0' && str[i] <= '9') {
            fraction = fraction * 10.0 + (str[i] - '0');
            divisor *= 10.0;
            i++;
        }
    }
    
    result += fraction / divisor;
    return result;
}

// Функции парсинга полей
static int parse_string_field(const char *json, uint16_t json_length, const char *field, char *output, int output_size) {
    char search_pattern[64];
    int i = 0;
    int field_len = 0;
    
    search_pattern[i++] = '"';
    while (field[field_len] != '\0' && field_len < 50) {
        search_pattern[i++] = field[field_len++];
    }
    search_pattern[i++] = '"';
    search_pattern[i++] = ':';
    search_pattern[i++] = '"';
    search_pattern[i] = '\0';
    
    for (uint16_t pos = 0; pos <= json_length - i; pos++) {
        int match = 1;
        for (int j = 0; j < i; j++) {
            if (json[pos + j] != search_pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            const char *value_start = &json[pos + i];
            const char *value_end = value_start;
            
            while (value_end < json + json_length && *value_end != '"') {
                value_end++;
            }
            
            int value_len = value_end - value_start;
            if (value_len >= output_size) value_len = output_size - 1;
            
            for (int j = 0; j < value_len; j++) {
                output[j] = value_start[j];
            }
            output[value_len] = '\0';
            return 1;
        }
    }
    return 0;
}

static int parse_int_field(const char *json, uint16_t json_length, const char *field, int *output) {
    const char *value_start = find_field_value(json, json_length, field);
    if (!value_start) return 0;
    *output = string_to_int(value_start);
    return 1;
}

static int parse_float_field(const char *json, uint16_t json_length, const char *field, float *output) {
    const char *value_start = find_field_value(json, json_length, field);
    if (!value_start) return 0;
    *output = string_to_float(value_start);
    return 1;
}

static int parse_uint8_field(const char *json, uint16_t json_length, const char *field, uint8_t *output) {
    int temp;
    if (parse_int_field(json, json_length, field, &temp)) {
        *output = (uint8_t)temp;
        return 1;
    }
    return 0;
}

static int parse_uint16_field(const char *json, uint16_t json_length, const char *field, uint16_t *output) {
    int temp;
    if (parse_int_field(json, json_length, field, &temp)) {
        *output = (uint16_t)temp;
        return 1;
    }
    return 0;
}

static int parse_uint32_field(const char *json, uint16_t json_length, const char *field, uint32_t *output) {
    int temp;
    if (parse_int_field(json, json_length, field, &temp)) {
        *output = (uint32_t)temp;
        return 1;
    }
    return 0;
}

/*
Основной парсинг JSON
"id" - устанавливает ID устройства
"heat_temperature" - Устанавливает температуру для текущего режима, uint8_t
"timer" - установка времени таймера м минутах 0..59 999(999 часов 59 минут),uint16_t
"mute" - 1-выключить звук 0-включить
"bt_status"-состояние блютуз, 1-вкл 0-выкл
"wifi_status"-состояние WiFi, 1-вкл 0-выкл
"isConnected" состояние подключения 1-есть 0-нет (bt_status = 1, и isConnected = 1 модуль блютуз включен и связь установлена)
"setTime" - установка времени в секундах 0..86400 это сутки в секундах
*/
void parse_json_data(const char *json, uint16_t json_length, DeviceStatus_t *data, void (*funcTimer)(void)) {
		uint32_t newTime = 0;
		uint16_t newTimer = 0;
    // Инициализация значений по умолчанию
    my_strncpy(data->id, "unknown", sizeof(data->id));
    data->woshingTime = 0;
    data->temperatureCurrent = 0.0;
    data->temperatureTurget = 0;
    data->flagRegimOn = 0;
    data->pidEnable = 0;
    data->btStatus = 0;
    data->wifiStatus = 0;
    data->isMuted = 0;
    
    // Парсим поля
    parse_string_field(json, json_length, "id", data->id, sizeof(data->id));
    parse_float_field(json, json_length, "temperature_current", &data->temperatureCurrent);
    if(parse_uint8_field(json, json_length, "heat_temperature", &data->temperatureTurget)){
			distanceSetTemperature(data->temperatureTurget, funcTimer);
		}
		if(parse_uint16_field(json, json_length, "timer", &newTimer)){
			 distanceSetTimer(newTimer, funcTimer);
		}
    parse_uint8_field(json, 	json_length, "mute", &data->isMuted);
    parse_uint16_field(json, 	json_length, "washing_time", &data->woshingTime);
    parse_uint8_field(json, 	json_length, "flag_regim_on", &data->flagRegimOn);
    parse_uint8_field(json, 	json_length, "pid_enable", &data->pidEnable);
    if(parse_uint8_field(json, 	json_length, "bt_status", &data->btStatus)){
			changeBtState();
		}
    if(parse_uint8_field(json, 	json_length, "wifi_status", &data->wifiStatus)){
			cahageWiFiState();
		}
		parse_uint8_field(json, 	json_length, "isConnected", (uint8_t*)&data->isConnected);
		parse_uint32_field(json, 	json_length, "setTime", &newTime);
		setGlobalTime(newTime);
		if(!deviceStatus.isMuted)
			beep(1);
}

// Вывод данных
void print_parsed_data(const DeviceStatus_t *data) {
    printf("Device Status:\n");
    printf("  id: %s\n", data->id);
    printf("  washing_time: %d\n", data->woshingTime);
    printf("  temperature_current: %.1f\n", data->temperatureCurrent);
    printf("  heat_temperature: %d\n", data->temperatureTurget);
    printf("  flag_regim_on: %d\n", data->flagRegimOn);
    printf("  pid_enable: %d\n", data->pidEnable);
    printf("  bt_status: %d\n", data->btStatus);
    printf("  wifi_status: %d\n", data->wifiStatus);
    printf("  is_muted: %d\n", data->isMuted);
}

// Вспомогательные функции
int get_buffer_data_count(UART_JSON_Parser *parser) {
    if (parser->head >= parser->tail) {
        return parser->head - parser->tail;
    } else {
        return UART_BUFFER_SIZE - parser->tail + parser->head;
    }
}

void clear_buffer(UART_JSON_Parser *parser) {
    parser->tail = parser->head;
}



// Обновленная функция process_json_message
void process_json_message(const char *json, uint16_t length, DeviceStatus_t *device) {
//    printf("Received JSON (%d bytes): ", length);
    
    // Безопасный вывод JSON (первые 100 символов для отладки)
//    uint16_t print_len = (length < 100) ? length : 100;
//    for (uint16_t i = 0; i < print_len; i++) {
//        putchar(json[i]);
//    }
//    if (length > 100) {
//        printf("...");
//    }
//    printf("\n");
    
    // Парсим данные
    parse_json_data(json, length, device, printShablonManalControl);
//    print_parsed_data(device);
}

void process_incoming_json(UART_JSON_Parser *parser, DeviceStatus_t *device) {
    char json_buffer[JSON_OUTPUT_SIZE];
    
    if (has_complete_json(parser, json_buffer, sizeof(json_buffer))) {
//        printf("Processing JSON message...\n");
        process_json_message(json_buffer, parser->last_json_length, device);
    }
    
    // Защита от переполнения буфера
    if (get_buffer_data_count(parser) > UART_BUFFER_SIZE * 0.8) {
//        printf("Buffer cleanup - too much data\n");
        clear_buffer(parser);
    }
}



