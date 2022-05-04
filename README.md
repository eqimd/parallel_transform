## Использование:
```bash
./read-measurer <function> <chunk size> <...files...>
```
Если указать `<chunk size>` как -1, то файлы будут читаться целиком.

### Статистика
* OS: Linux Mint 20.3 x86_64
* Kernel: 5.4.0-104-generic
* CPU: Intel i3-8145U
* Disk: SSD
* FS: ext4
* Scheduler: \[none\] mq-deadline

В целом, разбиение на блоки на большего размера влияет только на файлах достаточно большого размера, на маленьких же файлах влияния мало (иногда оно даже есть в отрицательную сторону, но возможно это входит просто в погрешность). Так же на маленьких файлах скорости чтения через разные функции тоже довольно близки, разница малозаметна, кроме разве что функции `readv`, в ней это скорее всего происходит из-за того, что приходится несколько раз писать байты через разные буферы. При этом на больших файлах разница чтения через `readv` сильно заметна -- буферов меньше, читать нужно из меньшего количества буферов, меньше лишних накладных расходов. Довольно удивительно, что чтение через `ifstream` обычно не хуже, а бывает даже лучше, чем через `fread`, хотя на файле в 1GB скорость у `ifstream` была заметно хуже.

Так же можно отметить, что время в sys тоже везде примерно одинаковое (на файлах до 1GB разница вообще несущественна), кроме как у `mmap` -- маппинг происходит быстро, и после этого просто побайтово читается кусок в памяти, при чем на файле в 4GB читается сильно быстрее, чем с помощью других функций. Мне кажется, что эта тенденция сохранится и при больших размерах, но у меня нету столько памяти, чтоб это проверить.

В общем и целом, на моей машине файлы размером около 4GB можно маппить и работать с ними, если же нужно будет работать с маленькими файлами, то использоваться `ifstream` -- просто из удобства, на совсем больших файлах использовать `readv` с большим размером блока.

### Таблица
Размер файла | Функция | Размер чанка | Время чтения | Время user/sys
------------ | ------- | ------------ | ------------ | --------------
64mb | `ifstream` | 4kb | 140ms | 0,14s / 0,04s
 | | | 1mb | 141ms | 0,13s / 0,05s
 | | | 16mb | 148ms | 0,15s / 0,05s
 | | | 64mb | 140ms | 0,14s / 0,03s
 | | `fread` | 4kb | 135ms | 0,12s / 0,03s
 | | | 1mb | 137ms | 0,12s / 0,03s
 | | | 16mb | 135ms | 0,12s / 0,03s
 | | | 64mb | 137ms | 0,13s / 0,02s
 | | `read` | 4kb | 141ms | 0,13s / 0,03s
 | | | 1mb | 138ms | 0,13s / 0,03s
 | | | 16mb | 137ms | 0,13s / 0,02s
 | | | 64mb | 136ms | 0,12s / 0,03s
 | | `readv` | 1mb | 159ms | 0,12s / 0,03s
 | | | 16mb | 157ms | 0,12s / 0,03s
 | | | 64mb | 165ms | 0,13s / 0,03s
 | | `mmap` | 4kb | 128ms | 0,12s / 0,00s
 | | | 1mb | 128ms | 0,12s / 0,00s
 | | | 16mb | 127ms | 0,12s / 0,00s
 | | | 64mb | 127ms | 0,11s / 0,01s
256mb | `ifstream` | 1mb | 539ms | 0,52s / 0,11s
 | | | 64mb | 553ms | 0,50s / 0,13s
 | | | 128mb | 546ms | 0,51s / 0,11s
 | | | 256mb | 541ms | 0,50s / 0,12s
 | | `fread` | 1mb | 541ms | 0,50s / 0,11s
 | | | 64mb | 596ms | 0,55s / 0,12s
 | | | 128mb | 543ms | 0,51s / 0,11s
 | | | 256mb | 542ms | 0,50s / 0,12s
 | | `read` | 1mb | 548ms | 0,51s / 0,11s
 | | | 64mb | 551ms | 0,50s / 0,12s
 | | | 128mb | 543ms | 0,50s / 0,12s
 | | | 256mb | 551ms | 0,50s / 0,13s
 | | `readv` | 1mb | 618ms | 0,50s / 0,12s
 | | | 64mb | 606ms | 0,49s / 0,11s
 | | | 128mb | 600ms | 0,48s / 0,12s
 | | | 256mb | 598ms | 0,49s / 0,11s
 | | `mmap` | 1mb | 506ms | 0,49s / 0,02s
 | | | 64mb | 503ms | 0,48s / 0,03s
 | | | 128mb | 506ms | 0,49s / 0,02s
 | | | 256mb | 506ms | 0,48s / 0,02s
512mb | `ifstream` | 1mb | 1039ms | 1,06s / 0,21s
 | | | 32mb | 1102ms | 1,02s / 0,24s
 | | | 64mb | 1124ms | 1,06s / 0,22s
 | | | 256mb | 1179ms | 1,09s / 0,26s
 | | | 512mb | 1147ms | 1,05s / 0,24s
 | | `fread` | 1mb | 1206ms | 1,12s / 0,26s
 | | | 32mb | 1213ms | 1,14s / 0,24s
 | | | 64mb | 1179ms | 1,08s / 0,26s
 | | | 256mb | 1184ms | 1,09s / 0,27s
 | | | 512mb | 1152ms | 1,06s / 0,25s
 | | `read` | 1mb | 1148ms | 1,09s / 0,24s
 | | | 32mb | 1177ms | 1,08s / 0,27s
 | | | 64mb | 1163ms | 1,06s / 0,26s
 | | | 256mb | 1131ms | 1,04s / 0,27s
 | | | 512mb | 1161ms | 1,07s / 0,25s
 | | `readv` | 1mb | 1301ms | 1,07s / 0,24s
 | | | 32mb | 1324ms | 1,07s / 0,26s
 | | | 64mb | 1531ms | 1,30s / 0,25s
 | | | 256mb | 1415ms | 1,16s / 0,26s
 | | | 512mb | 1478ms | 1,25s / 0,23s
 | | `mmap` | 1mb | 1281ms | 1,24s / 0,04s
 | | | 32mb | 1255ms | 1,21s / 0,05s
 | | | 64mb | 1183ms | 1,16s / 0,02s
 | | | 256mb | 1210ms | 1,17s / 0,04s
 | | | 512mb | 1288ms | 1,26s / 0,03s
1gb | `ifstream` | 1mb | 2439ms | 2,10s / 0,59s
 | | | 32mb | 2202ms | 2,04s / 0,49s
 | | | 256mb | 2192ms | 2,04s / 0,46s
 | | | 1gb | 2176ms | 2,05s / 0,45s
 | | `fread` | 1mb | 2151ms | 2,00s / 0,46s
 | | | 32mb | 2142ms | 2,02s / 0,45s
 | | | 256mb | 2162ms | 2,03s / 0,45s
 | | | 1gb | 2150ms | 2,00s / 0,49s
 | | `read` | 1mb | 2162ms | 2,02s / 0,47s
 | | | 32mb | 2168ms | 2,04s / 0,46s
 | | | 256mb | 2150ms | 2,00s / 0,48s
 | | | 1gb | 2151ms | 2,01s / 0,47s
 | | `readv` | 1mb | 2495ms | 2,09s / 0,44s
 | | | 32mb | 2623ms | 2,19s / 0,47s
 | | | 256mb | 2611ms | 2,16s / 0,47s
 | | | 1gb | 2779ms | 2,36s / 0,45s
 | | `mmap` | 1mb | 2361ms | 2,23s / 0,04s
 | | | 32mb | 2322ms | 2,27s / 0,06s
 | | | 256mb | 2496ms | 2,46s / 0,06s
 | | | 1gb | 2313ms | 2,26s / 0,07s
4gb | `ifstream` | 1mb | 10816ms | 8,05s / 2,90s
 | | | 32mb | 10893ms | 8,06s / 2,85s
 | | | 256mb | 11251ms | 8,49s / 2,72s
 | | | 1gb | 11899ms | 9,09s / 2,90s
 | | | 4gb | 11749ms | 8,97s / 2,87s
 | | `fread` | 1mb | 11820ms | 8,89s / 3,05s
 | | | 32mb | 11766ms | 8,84s / 3,04s
 | | | 256mb | 11643ms | 8,86s / 2,97s
 | | | 1gb | 11752ms | 8,78s / 3,09s
 | | | 4gb | 11844ms | 8,89s / 3,05s
 | | `read` | 1mb | 11754ms | 8,95s / 3,02s
 | | | 32mb | 11845ms | 8,99s / 3,05s
 | | | 256mb | 11695ms | 8,96s / 2,93s
 | | | 1gb | 11821ms | 8,90s / 3,07s
 | | | 4gb | 11782ms | 8,81s / 3,04s
 | | `readv` | 32mb | 11155ms | 9,04s / 1,63s
 | | | 256mb | 10062ms | 9,01s / 1,13s
 | | | 1gb | 9872ms | 8,81s / 1,13s
 | | | 4gb | 9903ms | 8,87s / 1,11s
 | | `mmap` | 1mb | 9247ms | 9,02s / 0,37s
 | | | 32mb | 9064ms | 8,90s / 0,28s
 | | | 256mb | 9124ms | 8,94s / 0,29s
 | | | 1gb | 9097ms | 8,93s / 0,27s
 | | | 4gb | 9078ms | 8,90s / 0,29s
