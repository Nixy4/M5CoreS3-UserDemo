快速入门
=========

:link_to_translation:`en:[English]`

本文描述了如何搭建 ESP-DL 环境，支持使用 `乐鑫 <https://www.espressif.com/zh-hans/products/devkits>`__ 或其他供应商设计的任意 ESP 开发板。

获取 ESP-IDF
------------

ESP-DL 的运行需要依赖 ESP-IDF。有关 ESP-IDF 的详细安装步骤，请查看 `ESP-IDF 编程指南 <https://idf.espressif.com/zh-cn/index.html>`__。

获取 ESP-DL 并运行示例
----------------------

1. 使用如下命令下载 ESP-DL：

   .. code:: shell

      git clone https://github.com/espressif/esp-dl.git

2. 打开终端，进入 :project:`tutorial/convert_tool_example` 文件夹：

   .. code:: shell

      cd ~/esp-dl/tutorial/convert_tool_example

   或是进入 :project:`examples` 文件夹下的其他示例项目。

3. 使用以下命令设置目标芯片：

   .. code:: shell

      idf.py set-target [SoC]

   将 [SoC] 替换为目标芯片，如 esp32、esp32s2、esp32s3。

      注意 ESP32-C3 仅适用于无需 PSRAM 的应用。

4. 烧录固件，打印结果：

   .. code:: shell

      idf.py flash monitor

   如果在第二步进入的是 :project:`tutorial/convert_tool_example` 文件夹，

   -  目标芯片是 ESP32，则

      .. code:: shell

         MNIST::forward: 37294 μs
         Prediction Result: 9

   -  目标芯片是 ESP32-S3，则

      .. code:: shell

         MNIST::forward: 6103 μs
         Prediction Result: 9

ESP-DL 用作组件
---------------

ESP-DL 是包含多种深度学习 API 的仓库。我们推荐将 ESP-DL 用作其他项目的组件。

比如，ESP-DL 可以作为 `ESP-WHO <https://github.com/espressif/esp-who>`__ 仓库的子模块，只需将 ESP-DL 加入到 `esp-who/components/ <https://github.com/espressif/esp-who/tree/master/components>`__ 目录即可。
