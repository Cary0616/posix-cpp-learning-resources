# POSIX C/C++ 学习资源

一个用于学习 C 语言面向对象编程风格和 POSIX 相关知识的示例项目。通过「汽车」与「巴士」的继承关系，演示在 C 中如何用结构体与函数指针实现类似 OOP 的设计。

## 项目结构

```
posix-cpp-learning-resources/
├── src/           # 源代码
│   ├── main.c     # 入口程序
│   ├── car.c      # 汽车基础实现
│   ├── car.h      # 汽车接口定义
│   ├── bus.c      # 巴士实现（基于 car 扩展）
│   ├── bus.h      # 巴士接口定义
│   └── common.h   # 公共类型与枚举
├── build/
│   └── compile.sh # 交互式编译脚本
├── define         # 编译配置文件
└── README.md
```

## 设计说明

- **car**：基础车辆类型，包含车牌、类型、颜色、生产/出厂日期，以及 `drive`、`destroy` 等操作（通过 `car_ops_t` 函数指针实现多态）。
- **bus**：在 `car_t` 基础上通过组合扩展，增加 `seatCount`，并重写 `drive`、`destroy` 行为。

## 环境要求

- `gcc` / `g++`
- Bash（用于运行编译脚本）

## 编译与运行

1. 确保项目根目录存在 `define` 配置文件（可参考仓库中的示例）。
2. 进入 `build` 目录执行编译脚本：

   ```bash
   cd build && ./compile.sh
   ```

3. 按提示选择：
   - `1`：直接编译（使用已有中间文件）
   - `2`：清理后重新编译
   - `3`：仅清理编译产物

4. 编译成功后，可执行文件位于 `out/bin/main`：

   ```bash
   ./out/bin/main
   ```

## 许可证

请根据项目实际情况添加或修改许可证信息。
