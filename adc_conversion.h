#ifndef ADC_CONVERSION_H_ 
#define ADC_CONVERSION_H_

void ADC0_init(void);
uint16_t ADC0_read(void);
uint16_t ADC0_conversion(void);
uint8_t ADC0_get_channel(void);
uint8_t ADC0_set_channel(uint8_t ch);

#endif
