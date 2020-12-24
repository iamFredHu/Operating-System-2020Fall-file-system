# Operating-System-2020Fall-file-system

## Lab5：简单文件系统的设计与实现

### 必做部分

（1）实现青春版Ext2文件系统。（参考 实验原理-Ext2文件系统原理简介）

系统结构参考ext2系统即可，不需要严格按照ext2设计，可简化，可自由发挥，但必须是模拟了文件系统，并能完成如下功能：

创建文件/文件夹（数据块可预分配）；
读取文件夹内容；
复制文件；
关闭系统；
系统关闭后，再次进入该系统还能还原出上次关闭时系统内的文件部署。

（2）为实现的文件系统实现简单的 shell 以及 shell 命令以展示实现的功能。 
可参考实现以下shell命令：
· ls - 展示读取文件夹内容
· mkdir - 创建文件夹
· touch - 创建文件
· cp - 复制文件
· shutdown - 关闭系统
以上未提及的功能（如复制文件夹、删除文件、读写文件等）不要求实现。 具体要求参见实验内容和实验要求。

### 实验指导书地址
[实验指导书](https://hitsz-lab.gitee.io/os_lab/)