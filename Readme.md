# Интерпретатор языка C+-
## Сборка
На Linux можно воспользоваться
```
$ ./compile.sh
```
Можно собрать вручную
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

После сборки появится директория `bin`, в которой будет программа `interpreter` - сам интерпретатор

## Использование интерпретатора
### Запуск
У интерпретатора есть два режима работы.

- Интерпретировать уже готовые файлы с кодом. Для этого нужно передать первым аргументом название файла:
```
$ ./interpreter ../samples/sum.cpm
```

-  Работать в интерактивном режиме. Для этого нужно просто запустить интерпретатор:
```
$ ./interpreter
```
Для завершения работы в интерактивном режиме введите `exit();`

### Ввод и вывод

Вывод всегда осуществляется на стандартный поток вывода. Ввод в случае работы в интерактивном режиме осуществляется со стандартного потока ввода. Если Интерпретируется код из файла, то вторым параметром интерпретатору можно передать название входного файла:

```
$ ./interpreter ../samples/multisum.cpm ../samples/multisum_input.txt
```
При работе в интерактивном режиме данные считываются построчно, подробнее об этом будет описано в примерах работы.

## Синтаксис

Синтаксис языка C+- по сути сужение синтаксиса C++

### Арифметические операторы

Стандартные арифметические операции:
```c++
2 + 2;
56 - 100;
10 / 2;
7 * 83;
-5 * 10;
```
Приоритет совпадает с приоритетом в C++

*Замечание:* деление **целочисленное**
### Целочисленные переменные
Целочисленные переменные создаются следующим образом:
```c++
int number;
int value = 10;
int result = (100 -  58) * 374 - 33 / 4;
```
То есть либо просто создание (тогда по умолчанию значение переменной `0`), либо создание с присвоением значения выражения в правой части равенства.

### Логические операторы
Операторы, возвращающие значения `1`, истина, или `0`, ложь:
```
1 <= 2;
2 < 3;
4 > 100;
100 >= 2;
5 != 8;
!(6 < 7);
4 == 4;
((4 < 6) && (5 > 7));
((4 < 6) || (5 > 7));
```
Приоритет совпадает с приоритетом в C++

### Строки

Константные строки можно объявлять в двойных кавычках:

```c++
"Hello, world!";
"Hello,\n\tworld!\n"
"\"Quine\"\\n\n"
```
Экранировать можно `\` и `"` (`\\` и `\"`), а также есть управляющие символы `\n` и `\t` - перевод строки и табуляция.

### Строковые операции

Для строк доступны все следующие операции:

```c++
string a = "a";
string b = "b";
a + b ==  "ab";
a < b == 1;
a > b == 0;
a <= b == 1;
a >= b == 0;
a == b == 0;
a != b == 1;
a * 3 == "aaa";
4 * b == "bbbb";
```

### Ввод и вывод
Для ввода есть несколько функций:
- `read_int()` - возвращает прочитанное число
- `read_word()` - возвращает прочитанную строку, строка читается до пробела
- `read_line()` - возвращает прочитанную строку до символа перевода строки

В случае попытки чтения числа при введённой строке, не переводимой в число, будет ошибка, и буфер ввода останется неизменным.

Для вывода есть две функции:
- `write(<expression>)` - вывести значение выражения `<expression>`
-  `write_line(<expression>)` - вывести значение  выражения `<expression>` и перевод строки

Пример работы буферизованного ввода в интерактивном режиме:
```c++
>>>  int a = read_int();
10 20 30
>>>  int b = read_int();
>>>  int c = read_int();
>>>  write_line(c);
30
```

### Условный оператор
То же самое, что и в C++:
```c++
if (<expression>) {
    <commands>
} else {
    <commands>
}
```
Блок с `else` при желании можно опускать

### Циклы
Два типа циклов, совпадающие с аналогами из C++:
```c++
int i = 0;
while (i < 10) {
    write_line(i);
}

for (int j = 0; j < 10; j = j + 1) {
    write_line(j * "a");
}
```

### Области видимости

Как и в C++, `if`, `for`, `while` и просто `{ ... }` создают свои области видимости. Повторно объявлять переменные всё ещё нельзя, но зато объявленные в области видимости переменные вне этой области перестают существовать.