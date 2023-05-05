#ifndef __LORA_CONFIG_H
#define __LORA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_LORA_RFSW_CTRL_GPIOX         GPIOD
#define CONFIG_LORA_RFSW_CTRL_PIN           GPIO_PIN_11

#define CONFIG_LORA_RFSW_TR_GPIOX           GPIOA
#define CONFIG_LORA_RFSW_TR_PIN             GPIO_PIN_10

#if 0
#define CONFIG_LORA_RFSW_VDD_GPIOX          GPIOA
#define CONFIG_LORA_RFSW_VDD_PIN            GPIO_PIN_10
#endif

#define CONFIG_LWAN_KEYS_FLASH_ADDR        (0x0801F000)
#define CONFIG_LWAN_SETTINGS_FLASH_ADDR    (0x0801E000)

#define CONFIG_MANUFACTURER                "GLE-"
#define CONFIG_DEVICE_MODEL                "R16S-REV.B"
#define CONFIG_VERSION                     "v0.0.0"

#ifdef __cplusplus
}
#endif

#endif /* __LORA_CONFIG_H */
