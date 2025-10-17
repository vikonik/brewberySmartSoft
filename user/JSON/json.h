#ifndef _JSON_H
#define _JSON_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "allDefenition.h"
#define UART_BUFFER_SIZE 512
#define JSON_OUTPUT_SIZE 256

typedef enum {
    JSON_PROCESSED,
    JSON_NOT_READY,
    BUFFER_CLEANED
} JsonProcessResult;

typedef struct {
    char buffer[UART_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
		uint16_t last_json_length;
} UART_JSON_Parser;
extern UART_JSON_Parser uart_parser;

// Инициализация парсера
void uart_parser_init(UART_JSON_Parser *parser);

// Добавление байта в буфер
void uart_receive_byte(UART_JSON_Parser *parser, char byte);

// Поиск полного JSON
int has_complete_json(UART_JSON_Parser *parser, char *output, int max_len);

// Парсинг JSON данных
void parse_json_data(const char *json, uint16_t json_length, DeviceStatus_t *data, void (*funcTimer)(void));

// Вывод данных
void print_parsed_data(const DeviceStatus_t *data);

// Вспомогательные функции
int get_buffer_data_count(UART_JSON_Parser *parser);
void clear_buffer(UART_JSON_Parser *parser);

void process_incoming_json(UART_JSON_Parser *parser, DeviceStatus_t *device);


#endif
