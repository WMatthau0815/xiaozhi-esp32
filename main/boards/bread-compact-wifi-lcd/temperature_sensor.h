#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "mcp_server.h"
#include <esp_log.h>
#include <driver/gpio.h>
#include <cmath>
#include "onewire_bus.h"
#include "ds18b20.h"

#define TEMP_TAG "TemperatureSensor"

class TemperatureSensor {
public:
    TemperatureSensor(gpio_num_t gpio) {
        onewire_bus_config_t bus_config = {};
        bus_config.bus_gpio_num = gpio;
        bus_config.flags.en_pull_up = true;

        onewire_bus_rmt_config_t rmt_config = {};
        rmt_config.max_rx_bytes = 10;

        if (onewire_new_bus_rmt(&bus_config, &rmt_config, &bus_) != ESP_OK) {
            ESP_LOGE(TEMP_TAG, "Failed to init 1-Wire bus");
            return;
        }

        onewire_device_iter_handle_t iter = nullptr;
        onewire_device_t next_device;
        esp_err_t search_result = ESP_OK;

        if (onewire_new_device_iter(bus_, &iter) == ESP_OK) {
            do {
                search_result = onewire_device_iter_get_next(iter, &next_device);
                if (search_result == ESP_OK) {
                    ds18b20_config_t ds_cfg = {};
                    if (ds18b20_new_device_from_enumeration(&next_device, &ds_cfg, &sensor_) == ESP_OK) {
                        found_ = true;
                        ESP_LOGI(TEMP_TAG, "DS18B20 found and initialized");
                        break;
                    }
                }
            } while (search_result != ESP_ERR_NOT_FOUND);
            onewire_del_device_iter(iter);
        }

        if (!found_) {
            ESP_LOGE(TEMP_TAG, "No DS18B20 found on GPIO");
        }

        auto& mcp_server = McpServer::GetInstance();
        mcp_server.AddTool(
            "self.temperature.get_room_temperature",
            "Get the current room temperature in degrees Celsius",
            PropertyList(),
            [this](const PropertyList&) -> ReturnValue {
                float t = ReadCelsius();
                if (std::isnan(t)) {
                    return std::string("Temperatursensor nicht verfügbar.");
                }
                char buf[48];
                snprintf(buf, sizeof(buf), "%.1f Grad Celsius", t);
                return std::string(buf);
            });
    }

    float ReadCelsius() {
        if (!found_) return NAN;
        if (ds18b20_trigger_temperature_conversion_for_all(bus_) != ESP_OK) return NAN;
        float temperature = 0.0f;
        if (ds18b20_get_temperature(sensor_, &temperature) != ESP_OK) return NAN;
        return temperature;
    }

private:
    onewire_bus_handle_t bus_ = nullptr;
    ds18b20_device_handle_t sensor_ = nullptr;
    bool found_ = false;
};

#endif
