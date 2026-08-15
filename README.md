# Opal
**一个按照[TheCherno](https://github.com/TheCherno)的[Hazel](https://github.com/TheCherno/Hazel)项目重构的游戏引擎.**
**#使用**
本项目使用premake构建，当前仅支持使用Windows上的Visual Studio,可以在下载源代码后在对应文件夹中打开控制台输入以下代码构建解决方案:
...
vendor\bin\premake\premake5.exe vs2022
...
**这里2022为示范版本,请根据拥有的Visual Studio版本修改**
**##当前进度**
-使用WGL以及手动加载ModernOpenGL函数，当前未加入GLX部分代码以及对Linux的支持.
-自己的日志系统.**
-自己的GUI系统(1/2).
 -UI逻辑(参考了[rxi](https://github.com/rxi)的[microui](https://github.com/rxi/microui)代码)
 -字体渲染系统(未完成)