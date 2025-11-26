#include <esp_adc/adc_oneshot.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void init_adc(void *pv) {
  adc_channel_t channels[] = {ADC_CHANNEL_6};
  int length = sizeof(channels) / sizeof(channels[0]);
  adc_oneshot_unit_handle_t handle;
  int values[8] = {0};

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
      adc_oneshot_read(handle, channels[idx], &values[idx]);
      printf("%d\n", (values[idx] * 100) / 4095);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void app_main(void) { xTaskCreate(init_adc, "adc", 2048, NULL, 5, NULL); }
