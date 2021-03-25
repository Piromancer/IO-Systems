# Лабораторная работа 1

**Название:** "Разработка драйверов символьных устройств"

**Цель работы:** Получить знания и навыки разработки драйверов символьных устройств для операционной системы Linux.

## Описание функциональности драйвера

При записи текста в файл символьного устройства должно запоминаться количество введенных букв. Последовательность полученных результатов с момента загрузки модуля ядра должна выводиться при чтении созданного файла `/proc/var5` в консоль пользователя.

При чтении из файла символьного устройства в кольцевой буфер ядра должен осуществляться вывод тех же данных, которые выводятся при чтении файла `/proc/var5`.


## Инструкция по сборке

1. Склонировать репозиторий (git clone https://github.com/Piromancer/IO-Systems.git)

2. Перейти в директорию IO-Systems/lab1

3. Запустить комманду `make`:

## Инструкция пользователя

1. Загрузите драйвер командой

```
sudo insmod Lab1.ko
```

2. Получить права на запись в символьное устройство /dev/var5, например с помощью команды `sudo chmod 0666 /dev/var5`

3. Запишите данные в символьное устройство

```
echo "your-data1" > /dev/var5
echo "your-data2" > /dev/var5
echo "your-data3" > /dev/var5
```

4. Прочитайте содержание файла /proc/var5 и убедитесь, что в нем содержаться корректные данные

```
cat /proc/var5
```

5. Произведите чтение символьного устройства и убедитесь, что вывод идентичен выводу из пункта 4

```
cat /dev/mychdev
dmesg
```

6. Деинициализируйте драйвер:

```bash
sudo rmmod Lab1.ko
```

## Примеры использования

```bash
mint@mint:~/Desktop/Lab1$ sudo insmod Lab1.ko
mint@mint:~/Desktop/Lab1$ sudo chmod 666 /dev/var5
mint@mint:~/Desktop/Lab1$ echo "<;:test[{]" > /dev/var5
mint@mint:~/Desktop/Lab1$ echo " Hello" > /dev/var5
mint@mint:~/Desktop/Lab1$ echo "nice to meet you" > /dev/var5
mint@mint:~/Desktop/Lab1$ cat /proc/var5
1 word had 4 letters
2 word had 5 letters
3 word had 13 letters
mint@mint:~/Desktop/Lab1$ cat /dev/var5
mint@mint:~/Desktop/Lab1$ dmesg
...
[13986.077515] Initialization
[14132.978438] 1 word had 4 letters
[14132.978441] 2 word had 5 letters
[14132.978442] 3 word had 13 letters
mint@mint:~/Desktop/Lab1$ sudo rmmod Lab1.ko
```
