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
        bus_config.flags.en_pull_up = true;   // interner Pull-up als Backup zum externen 4.7k-Widerstand

        onewire_bus_rmt_config_t rmt_config = {};
        rmt_config.max_rx_bytes = 10;

        if (onewire_new_bus_rmt(&bus_config, &rmt_config, &bus_) != ESP_OK) {
            ESP_LOGE(TEMP_TAG, "Failed to init 1-Wire bus");
            return;
        }

        onewire_device_iter_handle_t iter = nullptr;
        onewire_device_t device;
        if (onewire_new_device_iter(bus_, &iter) == ESP_OK) {
            if (onewire_device_iter_get_next(iter, &device) == ESP_OK) {
                ds18b20_config_t ds_config = {};
                if (ds18b20_new_device(&device, &ds_config, &sensor_) == ESP_OK) {
                    found_ = true;
                    ESP_LOGI(TEMP_TAG, "DS18B20 found and initialized");
                } else {
                    ESP_LOGE(TEMP_TAG, "Found device is not a DS18B20");
                }
            } else {
                ESP_LOGE(TEMP_TAG, "No 1-Wire device found on GPIO");
            }
            onewire_del_device_iter(iter);
        }

        auto& mcp_server = McpServer::GetInstance();
        mcp_server.AddTool(
            "self.temperature.get_room_temperature",
            "Get the current room temperature in degrees Celsius",
            PropertyList(),
            [this](const PropertyList&) -> ReturnValue {
                if (!found_) {
                    return std::string("Temperatursensor nicht verfügbar.");
                }
                float temperature = 0.0f;
                if (ds18b20_trigger_temperature_conversion(sensor_) != ESP_OK ||
                    ds18b20_get_temperature(sensor_, &temperature) != ESP_OK) {
                    return std::string("Fehler beim Lesen der Temperatur.");
                }
                char buf[48];
                snprintf(buf, sizeof(buf), "%.1f Grad Celsius", temperature);
                return std::string(buf);
            });
    }
    float ReadCelsius() {
        if (!found_) return NAN;
        float temperature = 0.0f;
        if (ds18b20_trigger_temperature_conversion(sensor_) != ESP_OK ||
            ds18b20_get_temperature(sensor_, &temperature) != ESP_OK) {
            return NAN;
        }
        return temperature;
    }
private:
    onewire_bus_handle_t bus_ = nullptr;
    ds18b20_device_handle_t sensor_ = nullptr;
    bool found_ = false;
};

#endif
