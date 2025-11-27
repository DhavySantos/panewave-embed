#include <config.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/uart.h>
#include <esp_adc/adc_oneshot.h>

QueueHandle_t adc_queue;

typedef struct {
  int values[8];
  int count;
} adc_data_t;

void adc_task(void *pv) {
  adc_channel_t channels[] = {ADC_CHANNEL_6};
  int length = sizeof(channels) / sizeof(channels[0]);
  adc_oneshot_unit_handle_t handle;
  adc_data_t adc_data;

  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = ADC_UNIT_1,
  };

  esp_err_t ret = adc_oneshot_new_unit(&init_config, &handle);
  if (ret != ESP_OK) {
    printf("Failed to initialize ADC unit\n");
    vTaskDelete(NULL);
    return;
  }

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_12,
  };

  for (int idx = 0; idx < length; idx++) {
    ret = adc_oneshot_config_channel(handle, channels[idx], &config);
    if (ret != ESP_OK) {
      printf("Failed to configure channel %d\n", idx);
      vTaskDelete(NULL);
      return;
    }
  }

  while (true) {
    for (int idx = 0; idx < length; idx++) {
      adc_oneshot_read(handle, channels[idx], &adc_data.values[idx]);
      adc_data.values[idx] = (adc_data.values[idx] * 100) / 4095;
    }

    adc_data.count = length;

    xQueueSend(adc_queue, &adc_data, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void uart_task(void *pv) {
  uart_config_t config = {
      .baud_rate = UART_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  ESP_ERROR_CHECK(uart_param_config(UART_PORT, &config));

  ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,
                               UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

  ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_TX_BUF_SIZE,
                                      UART_TX_BUF_SIZE, 0, NULL, 0));

  adc_data_t adc_data;
  char buffer[128];

  while (true) {
    if (xQueueReceive(adc_queue, &adc_data, portMAX_DELAY)) {
      continue;
    }

    int len = snprintf(buffer, sizeof(buffer), "ADC:");

    for (int idx = 0; idx < adc_data.count; idx++) {
      len += snprintf(buffer + len, sizeof(buffer) - len, " %d",
                      adc_data.values[idx]);
    }

    len += snprintf(buffer + len, sizeof(buffer) - len, "\r\n");

    uart_write_bytes(UART_PORT, (const char *)buffer, len);
  }
}

void app_main(void) {
	adc_queue = xQueueCreate(10, sizeof(adc_data_t));
  xTaskCreate(adc_task, "adc", 2048, NULL, 5, NULL);
  xTaskCreate(uart_task, "uart", 2048, NULL, 5, NULL);
}
