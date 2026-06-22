# Полные наборы команд устройств

## АКИП-3417 — OEM: Suin Instruments (TFG Series)

Производитель: **Shijiazhuang Suin Instruments Co., Ltd.** (www.suindigital.com)  
OEM-серия: **TFG3000 / TFG3100 / TFG3600**  
Протокол: ASCII SCPI, терминатор `\n` (0x0A), USB bulk через CH375  
Синтаксис каналов: **проприетарный** (`FREQ:CHA`, `VOLT:CHB` — не стандартный `:CHANnel1`)

### Соответствие моделей
| АКИП вариант | Suin аналог | Канал A (PLL) | Канал B (DDS) |
|---|---|---|---|
| АКИП-3417/1 | TFG3000 | до 10 МГц | до 10 МГц |
| АКИП-3417/2 | TFG3100 | до 200 МГц | до 10 МГц |
| АКИП-3417/3 | TFG3600 | до 500–1000 МГц | — (одноканальный) |

### IEEE 488.2
| Команда | Описание |
|---|---|
| `*IDN?` | Идентификация |
| `*RST` | Сброс в заводские настройки |
| `*OPC?` | Запрос завершения операции |
| `*TST?` | Самотестирование |
| `*CLS` | Очистка статуса |

### Частота
| Команда | Описание | Диапазон |
|---|---|---|
| `FREQ:CHA <Hz>` | Установить частоту канала A | 1 мкГц – 500 МГц |
| `FREQ:CHB <Hz>` | Установить частоту канала B | 1 мкГц – 10 МГц |
| `FREQ:CHA?` | Запрос частоты канала A | — |
| `FREQ:CHB?` | Запрос частоты канала B | — |

### Выход
| Команда | Описание |
|---|---|
| `OUTP:CHA ON\|OFF` | Включить/выключить выход A |
| `OUTP:CHB ON\|OFF` | Включить/выключить выход B |
| `OUTP:CHA?` | Запрос состояния выхода A |
| `OUTP:CHB?` | Запрос состояния выхода B |

### Амплитуда
| Команда | Описание | Диапазон |
|---|---|---|
| `VOLT:CHA <val> VPP` | Амплитуда канала A (VPP) | 0.001–20 VPP |
| `VOLT:CHB <val> VPP` | Амплитуда канала B (VPP) | 0.001–20 VPP |
| `VOLT:CHA <val> VRMS` | Амплитуда в Vrms | — |
| `VOLT:CHA <val> DBM` | Амплитуда в dBm | — |
| `VOLT:CHA?` | Запрос амплитуды A | — |
| `VOLT:CHB?` | Запрос амплитуды B | — |
| `VOLT:CHB:OFFS <val>` | Смещение DC канала B | −10 … +10 В |
| `VOLT:CHB:OFFS?` | Запрос смещения | — |

### Форма сигнала
| Команда | Описание | Значения |
|---|---|---|
| `FUNC:CHA SIN\|SQUARE` | Форма сигнала A (PLL) | SIN, SQUARE |
| `FUNC:CHB SIN\|SQUARE\|RAMP\|PULSE\|NOISE\|DC` | Форма сигнала B | SIN, SQUARE, RAMP, PULSE, NOISE, DC |
| `FUNC:CHA?` | Запрос формы A | — |
| `FUNC:CHB?` | Запрос формы B | — |
| `FUNC:CHB:SQU:DCYC <percent>` | Скважность меандра B | 0.1–99.9% |
| `FUNC:CHB:SQU:DCYC?` | Запрос скважности | — |
| `FUNC:CHB:RAMP:SYMM <percent>` | Симметрия пилы B | 0–100% |
| `FUNC:CHB:RAMP:SYMM?` | Запрос симметрии | — |
| `FUNC:CHB:PULS:PER <s>` | Период импульса B | — |
| `FUNC:CHB:PULS:WIDT <s>` | Длительность импульса B | — |

### Shortcut-команды APPL
| Команда | Описание |
|---|---|
| `APPL:CHA:SIN <freq>,<amp>,<offset>` | Синус канала A за одну команду |
| `APPL:CHB:SIN <freq>,<amp>,<offset>` | Синус канала B |
| `APPL:CHB:SQU <freq>,<amp>,<offset>` | Меандр канала B |
| `APPL:CHB:RAMP <freq>,<amp>,<offset>` | Пила канала B |
| `APPL:CHB:PULS <freq>,<amp>,<offset>` | Импульс канала B |
| `APPL:CHB:NOIS <amp>,<offset>` | Шум канала B |
| `APPL:CHB:DC <offset>` | Постоянный ток канала B |

### AM-модуляция
| Команда | Описание | Диапазон |
|---|---|---|
| `AM:SOUR INT\|EXT` | Источник AM | INT (внутренний), EXT (внешний) |
| `AM:INT:FREQ <Hz>` | Частота AM-модуляции | ~20 Гц – 20 кГц |
| `AM:INT:FREQ?` | Запрос частоты AM | — |
| `AM:DEPT <percent>` | Глубина AM | 0–100% |
| `AM:DEPT?` | Запрос глубины AM | — |
| `AM:STAT ON\|OFF` | Включить/выключить AM *(не подтверждено — проверить на приборе)* | — |
| `AM:STAT?` | Запрос состояния AM | — |

### FM-модуляция *(не реализовано; проверить на приборе)*
| Команда | Описание |
|---|---|
| `FM:SOUR INT\|EXT` | Источник FM |
| `FM:INT:FREQ <Hz>` | Частота FM-модуляции |
| `FM:DEV <Hz>` | Девиация FM |
| `FM:STAT ON\|OFF` | Включить/выключить FM |
| `FM:STAT?` | Запрос состояния FM |

### Свип *(не реализовано)*
| Команда | Описание |
|---|---|
| `SWE:CHB:STAR <Hz>` | Начальная частота свипа |
| `SWE:CHB:STOP <Hz>` | Конечная частота свипа |
| `SWE:CHB:TIME <s>` | Время свипа |
| `SWE:CHB:STAT ON\|OFF` | Включить/выключить свип |

### Пакетный режим *(не реализовано)*
| Команда | Описание |
|---|---|
| `BURS:CHB:NCYC <n>` | Количество циклов в пакете |
| `BURS:CHB:INT:PER <s>` | Период пакета |
| `BURS:CHB:STAT ON\|OFF` | Включить/выключить пакет |
| `BURS:CHB:TRIG EXT\|MAN\|INT` | Источник триггера |

---

## Gratten GA1483 — Atten Technology

Производитель: **Atten Technology Co., Ltd.**  
Протокол: SCPI over TCP, порт **5025**  
Эталонная документация: **RIGOL DSG800 Programming Guide** (практически идентичный набор команд)

### Ссылки на документацию
- RIGOL DSG800 Programming Guide (2018): https://www.batterfly.com/PDF/RIGOL/dsg800a/DSG800_ProgrammingGuide_EN.pdf
- Siglent SSG3000X Programming Guide: https://int.siglent.com/u_file/document/SSG3000X_Programming%20Guide-PG0803X-E01B.pdf

### Диапазоны
| Параметр | Диапазон |
|---|---|
| Несущая частота | 250 кГц – 3 ГГц (GA1483) |
| Мощность выхода | −136 … +13 дБм, шаг 0.01 дБм |
| AM глубина | 0–100% |
| AM частота (внутр.) | 20–20000 Гц |
| FM девиация | до ~1 МГц |
| PM девиация | до ~10π рад |

### Реализованные команды
| Команда | Описание | Статус |
|---|---|---|
| `:FREQuency:CW <Hz>Hz` | Несущая частота | ✅ |
| `:FREQuency:CW?` | Запрос частоты | ✅ |
| `:POWer:LEVEL <dBm>DBM` | Мощность | ✅ |
| `:POWer:LEVEL?` | Запрос мощности | ✅ |
| `:OUTPut:STATE ON\|OFF` | RF-выход | ✅ |
| `:OUTPut:STATE?` | Запрос выхода | ✅ |
| `:AM:INTernal:FREQuency <Hz>Hz` | Частота AM | ✅ |
| `:AM:INTernal:FREQuency?` | Запрос частоты AM | ✅ |
| `:AM:DEPTh <percent>%` | Глубина AM | ✅ |
| `:AM:DEPTh?` | Запрос глубины AM | ✅ |
| `:AM:STATE ON\|OFF` | AM вкл/выкл | ✅ |
| `:AM:STATE?` | Запрос AM | ✅ |
| `*IDN?` | Идентификация | ✅ |
| `*RST` | Сброс | ✅ |
| `:SYSTem:ERRor?` | Системная ошибка | ✅ |

### Нереализованные команды — AM (расширение)
| Команда | Описание |
|---|---|
| `:AM:SOURce INTernal\|EXTernal` | Источник AM |
| `:AM:SOURce?` | Запрос источника AM |
| `:AM:INTernal:WAVEform SINusoid\|SQUARE\|TRIangle` | Форма сигнала AM |
| `:AM:EXTernal:COUPling AC\|DC` | Связь внешнего AM |

### Нереализованные команды — FM
| Команда | Описание | Диапазон |
|---|---|---|
| `:FM:STATe ON\|OFF` | FM вкл/выкл | — |
| `:FM:STATe?` | Запрос FM | — |
| `:FM:SOURce INTernal\|EXTernal` | Источник FM | — |
| `:FM:DEViation <Hz>` | Девиация FM | 100 мГц – макс. |
| `:FM:DEViation?` | Запрос девиации | — |
| `:FM:INTernal:FREQuency <Hz>` | Частота FM | 20–20000 Гц |
| `:FM:INTernal:WAVEform SINusoid\|SQUARE\|TRIangle\|RAMP` | Форма FM | — |
| `:FM:EXTernal:COUPling AC\|DC` | Связь внешнего FM | — |

### Нереализованные команды — PM
| Команда | Описание |
|---|---|
| `:PM:STATe ON\|OFF` | PM вкл/выкл |
| `:PM:SOURce INTernal\|EXTernal` | Источник PM |
| `:PM:DEViation <rad>` | Девиация PM (0–10π рад) |
| `:PM:INTernal:FREQuency <Hz>` | Частота PM |
| `:PM:INTernal:WAVEform SINusoid\|SQUARE\|TRIangle` | Форма PM |

### Нереализованные команды — Свип
| Команда | Описание |
|---|---|
| `:SWEep:TYPE FREQuency\|LEVel` | Тип свипа |
| `:SWEep:STATe ON\|OFF` | Свип вкл/выкл |
| `:SWEep:FREQuency:STARt <Hz>` | Начальная частота |
| `:SWEep:FREQuency:STOP <Hz>` | Конечная частота |
| `:SWEep:FREQuency:STEP <Hz>` | Шаг свипа |
| `:SWEep:FREQuency:SPACing LINear\|LOGarithmic` | Вид шкалы |
| `:SWEep:DWELl <s>` | Время на шаг |
| `:SWEep:TIME <s>` | Общее время свипа |
| `:SWEep:MODE CONTinuous\|SINGle` | Режим свипа |

### Нереализованные команды — Импульсная модуляция
| Команда | Описание |
|---|---|
| `:PULM:STATe ON\|OFF` | PULM вкл/выкл |
| `:PULM:SOURce INTernal\|EXTernal` | Источник PULM |
| `:PULM:INTernal:PERiod <s>` | Период импульса |
| `:PULM:INTernal:WIDTh <s>` | Длительность импульса |

### Нереализованные команды — Утилиты
| Команда | Описание |
|---|---|
| `:DISPlay:UPDate ON\|OFF` | Обновление дисплея (OFF ускоряет команды) |
| `:ROSCillator:SOURce INTernal\|EXTernal` | Источник опорного генератора |
| `:SYSTem:PRESet` | Пресет |
| `:SYSTem:VERSion?` | Версия SCPI |
| `:LFOutput:FREQuency <Hz>` | Частота LF-выхода |
| `:LFOutput:AMPLitude <V>` | Амплитуда LF-выхода |
| `:LFOutput:STATe ON\|OFF` | LF-выход вкл/выкл |
