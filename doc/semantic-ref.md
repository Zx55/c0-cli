# 语义分析参考

## 变量定义

`[<const-qualifier>]<type-specifier><init-declarator-list>';'`

1. （全局）变量符号表操作

    理想情况下栈上只可能有变量，因此当前变量在栈上的位置应该是 参数数量 + 已经声明的变量的slot
    
    考虑必须要传入一个offset_vm来确定这个值

2. 指令区分配空间

3. 语义限制：重定义（考虑`domain`和`level`）、`void`类型、初始化时类型转换

## 函数定义

`<type-specifier><identifier><parameter-clause><compound-statement>`

1. 函数符号表操作（特别是写入该函数在指令区的偏移
    
    函数`id`和参数`id`不会调用它们的代码生成函数，即在栈上不分配空间

2. 语义限制：重定义（和变量重名）、`void`参数、参数的重定义（考虑作用域）、**返回类型的一致性**

## 条件表达式

`<expression>[<relational-operator><expression>]`

1. 代码生成

    * `lhs op rhs`

        ```asm
        ...lhs            # lhs expr
        ...rhs            # rhs expr
        icmp              # lhs - rhs
        ```

    * `lhs` (op = NEQ, rhs = 0)

        ```asm
        ...lhs            # lhs expr
        ```

    提供`get_op()`方法让上层的控制语句生成对应`jmp`

2. 语义限制：是否可比？类型转换（需要提前判断，多加指令）

3. op与外层的jmp对应

    ```
    j$(op) : > -> jg ; >= -> jge; < -> jl ; <= -> jle; == -> je ; != -> jne
    j$(!op): > -> jle; >= -> jl ; < -> jge; <= -> jg ; == -> jne; != -> je
    ```

## if-else

`'if' '(' <condition> ')' <statement> ['else' <statement>]`

1. 代码生成

    * `if (cond) stmt1 else stmt2`

        ```asm
            ...cond
            j$(!op) .False
            ...stmt1
            jmp .End
        .False:
            ...stmt2
        .End
        ```

        `.False`在生成`stmt1`和`jmp`指令后回填
        `.End`在生成`stmt2`后回填

    * `if (cond) stmt`

        ```
            ...cond
            j$(!op) .End
            ...stmt
        .End
        ```

## switch

`switch '(' <expr> ')' '{' {<case>} [<default>] '}'`

1. 代码生成

    不带`default`
    
    ```
        ...expr
        ...cond1
        icmp
        je .Case1
        ...expr
        ...cond2
        icmp
        je .Case2
        // ...
        jmp .End
    .Case1
        ...stmt1
        jmp .End
    .Case2
        ...stmt2
        jmp .End
        //...
    .End
    ```
   
    带`default`
    
    ```
        ...expr
        ...cond1
        icmp
        je .Case1
        // ...
        jmp .Default
    .Case1
        ...stmt1
        jmp .End
        // ...
    .Default:
        ...stmt
    .End
    ```

## while

`'while' '(' <condition> ')' <statement>`

`go-to-middle`翻译

1. 代码生成

    ```
        jmp .Test
    .Loop
        ...stmt
    .Test
        ...cond
        j$(op) .Loop
    .End
    ```

    第一个`.Test`在生成`stmt`后回填

## do-while

`'do' <statement> 'while' '(' <condition> ')' ';'`

1. 代码生成

    ```
    .Loop
        ...stmt
        ...cond
        j$(op) Loop
    .End
    ```

## for

`'for' '('<for-init-statement> [<condition>]';' [<for-update-expression>]')' <statement>`

// 见csapp P158 for中使用continue注意的问题

1. 代码生成

    ```
        ...init
        jmp .Test
    .Loop
        ...stmt
    .Update
        ...update
    .Test
        ...cond
        j$(op) .Loop
    .End
    ```

    `.Test`在生成`update`后回填

    特别处理`continue`的情况，应该跳到`.Update`

## return

1. 生成代码

    若返回类型非`void`，在栈上压入返回值，根据返回类型确定指令

2. 语义限制

    根据`_GenParam._ret`保证返回类型的一致性

## break

1. 指令生成

    生成一个需要回填的`jmp`

    向外层传递，如果外层是循环或者`switch`，必须处理`break`的地址回填；如果传递到最外层报错

    i.e.

    ```
    for (...; ...; ...) {
        if (...) {
            if (...) { ...; break; }
            ...; break;
        }
    }
    ```

    第一层`if`必须返回两个回填信息，所以回填必须是个数组，直到能够处理的外层才清空

2. 返回值

    `return { offset, len, [...break, { offset, FillType::BREAK}]， [...continue] }`

## Continue

1. 指令生成

    生成一个需要回填的`jmp`

    向外层传递，如果外层是循环，必须处理`continue`的地址回填；如果传递到最外层报错

    特别注意`for`中`continue`的特例

2. 返回值

    `return { offset, len, [...break]， [...continue, {offset, FillType::Continue}] }`

## scan

`'scan' '(' <identifier> ')' ';'`

1. 生成指令

    根据`id`的类型确定指令的类型

    ```
    ...id   # address of id
    iscan   # read value
    istore  # *address = value
    ```

2. 语义限制

    `id`必须非`const`

## print

`'print' '(' [<printable-list>] ')' ';'`

1. 生成指令

    根据`<expr>`类型确定指令

    ```$xslt
    ...expr1
    iprint
    bipush 32
    cprint      # ' '
    ...expr2
    dprint
    printl      # new line
    ```

2. 语义限制

    `void`类型无法打印、最后有个换行、多个打印之间有空格

## 赋值语句

`<identifier><assignment-operator><expression>`

1. 代码生成

    根据`expr`类型/`id`类型确定指令，修改符号表

    ```
    ...id
    ...expr
    iload
    ```

2. 语义限制

    类型检查(`void`)、类型转换、常量赋值

## 函数调用

`<identifier> '(' [<expression-list>] ')'`

1. 代码生成

    调用前构造参数，并将参数压入符号表
    
    ```
    ...expr1
    ...expr2
    call 1
    ```
   
    调用后，部分文法不使用返回值（如果有返回值），如`<for-update>`和`<stmt>`中的`<func-call>`，必须要弹出返回值

2. 语义限制
    
    函数是否定义？参数类型、数量？参数类型转换
    
## 标识符

1. 代码生成

    查表，查找该标识符在栈帧上的地址
    
    根据类型读取
    
    ```
    loada 0, $(offset)  // 局部变量，全局loada 1, $(offset)
    iload
    ```
   
2. 语义限制

    未赋值的不能使用
    
## inter-literal

1. 代码生成

    ```
    ipush $(value)
    ```
   
    一个优化是根据范围，可以换成`bipush`
    
## char-literal

1. 代码生成

    ```
    bipush $(value)
    ```
   
## floating-literal

1. 代码生成

    浮点字面数都存为常量，读取常量
    
    添加常量表

    ```$xslt
    loadc $(offset)
    ```  
   
## 取负

1. 代码生成

    根据表达式类型判断

    ```
    ...expr
    ineg
    ```
   
    计算完表达式结果，一定要根据结果设置表达式的类型
    
## 显式类型转换

逆序进行，转换到同一类型的可以省略

## 二项运算

类型检查、类型转换，设置表达式类型