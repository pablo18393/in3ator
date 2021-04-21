void watchdogInit() {
  iwdg_init (IWDG_PRE_256, 2000);
}

void watchdogReload() {
  IWDG->KR = 0xAAAA;
}
