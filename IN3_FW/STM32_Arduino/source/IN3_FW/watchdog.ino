void watchdogInit() {
  iwdg_init (IWDG_PRE_256, 2000);
}

void watchdogReload() {
  IWDG->KR = 0xAAAA;
}

void iwdg_feed(void) {
  IWDG->KR = 0xAAAA;
}

void iwdg_init(iwdg_prescaler prescaler, uint16_t reload) {
  IWDG->KR = 0x5555;
  IWDG->PR = prescaler;
  IWDG->RLR = reload;
  IWDG->KR = 0xCCCC;
  IWDG->KR = 0xAAAA;
}
