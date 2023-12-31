# FormatExpect

我实现了一种文本格式匹配算法。

它看起来长这样。

```
{[65,90]}[32,32]{[33,126]}[32,32]{[33,126]}[13,13][10,10]{{[65,90;97,122]}[58,58]{[33,126]}[13,13][10,10]}[13,13][10,10]
```

仔细观察可以发现元素其实只有三种，即花括号、区间和整数。

这个算法的原理为通过匹配文本中指定`ascii`码区间内的文本判断一段字符串是否符合预定义的格式标准。先从简单的开始说起，请看如下格式文本。

```
[104,104][101,101][108,108][108,108][111,111]
```

这是一个单向链式匹配，匹配长度为5的字符串，在此例中，这串文本匹配的是“hello”。104,101,108,108,100分别对应字符串中的hello。

不难发现，每个中括号内的对象对应文本中的一个字符，并且在这种模式中，格式文本顺序和待匹配文本顺序相同。

我现在将其变换为如下形式。

```
[48,48][120,120][48,57;65,70][48,57;65,70]
```

前两个元素匹配固定文本`0x`，后两个元素匹配`ascii`值区间在`[48,57]∪[65,70]`中的文本。为了方便起见，我使用分号表示“或”的意思。

所以以上匹配的是合法的两位十六进制数值。

但是这样匹配效率太低，如果文本过长，或不定长，则无法工作。

所以我再次引入结构花括号，代表重复匹配。请看如下例。

```
{[65,90;97,122]}
```

不难理解，这匹配的是任意长度由大小写字母组成的字符串。但有一个问题，即如何跳出此循环匹配结构，我们可以这样写。

```
{[65,90;97,122]}[10,10]{[48,57]}
```

在循环结构之后添加任意结构（单个匹配元素或另一循环均可），若前一循环最后一位字符匹配成功后下一位字符与循环首位匹配结构不同，但与循环结构之后的一匹配结构相同，即跳出此循环，进入下一结构。

以上匹配文本照此规则，对应的文本为：首行任意数量字母、一换行符\n、第二行任意数量数字。

循环结构可以进行有限次嵌套，举例如下。

```
{[65,90;97,122][10,10]{[48,57]}}
```

将最外层循环去掉，可以匹配诸如以下格式的文本。

```
abcDef
421416
```

而加入外层循环，代表不限次匹配循环内的格式文本所能成功匹配的结构。

```
abcDef
421416
iLoveCProgrammingLangrage
123456
...
```

规则就这么多。现在回头看本文开篇所写的格式文本，你能猜出这匹配的是什么吗？

[![fexpexample1.jpg](https://i.postimg.cc/T2kq7THt/fexpexample1.jpg)](https://postimg.cc/bdttdfPn)

