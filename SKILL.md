---
name: ate-test-project-builder
description: >
  在加速科技 MINI-ATE（ST2500/ST2516 教学平台，ST-IDE / SCWorkbench）上，从一颗芯片的
  数据手册出发，做出一个能打开、能编译、能仿真运行的完整测试工程：7 个配置文件
  （.job/.tmf/.sig/.grp/.res/.tim/.pat）+ pattern 向量 + Test.cpp + Makefile，
  并给 Test.cpp 写全行中文注释、编译验证通过。
  当用户说"做一颗 XX 芯片的测试项目 / 测试程序 / 实训案例"、"帮我做开路短路测试"、
  "解释 ATE 测试代码"、"给代码加注释"时使用。
  不适用：单纯问 ATE 原理（直接讲）、或使用华峰 STS8200 平台（那套 API 完全不同）。
---

# ATE 测试工程构建器

给一颗 DUT 芯片做完整的 ATE 测试工程。产出物必须满足三条：**能打开、能编译、能讲清楚**。

## 一、先确认环境（不确认就别动手）

| 要确认的 | 怎么查 | 影响 |
|---|---|---|
| 平台安装根目录 | 找 `Starter.exe` / `SCWorkbench.ini` 里的 `-Dscate_home` | 所有配置里的 `AteHomePath`、`RUNNER_HOME`、`.cproject` include 路径都指向它 |
| 头文件 | `<根目录>/runner/inc/usrInterface.h`、`usrDataType.h`、`Bindef.h` | API 签名以这里为准，不要凭记忆写 |
| 编译器 | `<根目录>/mingw64/bin/g++.exe` 能否编译一个 hello world | 见"坑 1"，自带工具链可能已损坏 |
| 已有案例 | 课程资源里的 `3.程序案例/*` | **永远从最接近的案例复制骨架，不要从零建工程** |

> 判断 DUT 用什么方法测之前，先读它的数据手册，确认：引脚定义、真值表、VOH/VOL/IIH/IIL/ICC 的测试条件与限值。

## 二、标准流程（8 步）

### 第 1 步：选骨架
挑一个 DUT 类型最接近的现成工程整目录复制，改名为 `<型号>_v1`。
译码器/门电路 → 抄 `examples/HC138_v1`；显示译码 → 抄 CD4511；运放 → 抄 LM358 类。
**不要新建空工程**，Eclipse/CDT 的 `.project`、`.cproject` 很难手写对。

### 第 2 步：定测试项
按"先粗后细"的顺序排，这是产线上真实顺序，也方便排错：

| 顺序 | 项 | 干什么 | 抓什么坏品 |
|---|---|---|---|
| 1-2 | OSN / OSP | 抽/灌 100µA，量电压 | 开路、短路、ESD 二极管坏 |
| 3 | IIL | 输入脚加 0V，量漏电流 | 输入级损伤 |
| 4 | VOH | 输出抽 1mA，量电压 | 驱动能力不足 |
| 5 | VOL | 输出灌 1mA，量电压 | 下拉能力不足 |
| 6 | IDD | VDD 加 5V，量静态电流 | 内部漏电/微短路 |
| 7 | FUNC | 跑向量，比高低电平 | 逻辑功能错 |
| 8 | SetPassBin | 打 Bin 1 | — |

### 第 3 步：写 7 个配置文件（严格按这个顺序，后面依赖前面）

1. **`.sig`** 信号 → 仪器类型 → 通道。`VDD` 用 `DPS`，其余 I/O 用 `DIO`，通道 `1:1:0` 起顺序排。
   DPS 和 DIO 是两套独立编号，可以都从 `1:1:0` 开始。
2. **`.grp`** 信号分组：`INPUT_PINS` / `OUTPUT_PINS` / `INOUT_PINS`。Test.cpp 里全靠组名操作。
3. **`.tim`** 时序表 `WFT1`：周期 10µs，输入 0µs 处驱动，输出 9µs 处比较。
4. **`.pat`** 向量索引：`1,,FUNC,1001,true,.\pattern\func.csv,`
5. **`pattern/func.csv`** 向量表，**列顺序必须和 `.sig` 里信号顺序完全一致**（这是最容易错的地方）。
6. **`.tmf`** 测试流：每个 FuncList 项的 `ResultAnalysis` 个数必须等于 Test.cpp 里
   `CheckResultAndBin(0, vec, N)` 的 `N`。ParamNumber 从 0 连续递增。
7. **`.job`** 总入口：串起上面全部文件，`AteHomePath` 写本机真实路径。

详细格式见 `references/file-formats.md`。

### 第 4 步：写 Test.cpp
所有测试项共用同一个骨架，照抄 `templates/Test.cpp.tpl`：

```
TEST_BEGIN
  ① 接上（Connect 电源 + 信号组）
  ② 建立条件（VDD 给多少、输入脚什么电平）
  ③ 施加激励（FIMV 加流 / FVMI 加压）
  ④ Measure 到 vector
  ⑤ CheckResultAndBin(0, vec, N)
  ⑥ 复位（归零 → DisConnect）
TEST_ERROR
  binObj.HandlerException(0);
TEST_END
```

两个高频设计难点，必须先想清楚再写代码：

- **输出是低有效（如 138）**：想并测全部 VOH，就拉高使能脚让全部输出变高；
  想测 VOL 只能循环逐路选中，用 `(i & 0x1) ? 5 : 0` 把循环变量拆成地址位。
- **输出是高有效（如 4511）**：反过来，VOH 靠 LT 点亮全段，VOL 靠 BI 消隐。

### 第 5 步：编译验证（必须做，不能只写不编）
```bash
cd <工程目录>
make            # Makefile 里 RUNNER_HOME 指向 <根目录>/runner
```
必须零 error 零 warning，并确认 `Test.dll` 生成。再抽查导出符号：
```bash
objdump -p Test.dll | grep -A 20 "Ordinal/Name Pointer"
```
应能看到 `OSN_T / OSP_T / IIL_T / VOH_T / VOL_T / IDD_T / FUNC_T / SetPassBin` 等。

### 第 6 步：写全行注释
用户是要"能讲给老师听"的，注释必须**每一行都有**，用 `//` 跟在代码右边：
- 讲"谁.干什么(多少)"的读法
- `-100e-6` 要说明"负号=抽、e-6=微"
- `Execute()` 要说明"前面都是设参数，这一下才生效"
- `TEST_BEGIN/TEST_ERROR/TEST_END` 要说明展开成 try/catch
- 循环里的位运算要举一个具体数字算给读者看
写完用脚本自查：非空行里没有 `//` 的行数必须为 0。

### 第 7 步：写 readme.md
器件引脚表、逐脚接线表、7 项限值表、IDE 操作步骤、排错速查。

### 第 8 步：交付说明
告诉用户：工程路径、怎么打开（Open Job）、怎么编译、怎么单跑某一项、仿真下数值是假的。

## 三、坑清单（都是真踩过的）

1. **自带编译器可能是坏的**：`mingw64/lib/gcc/.../cc1plus.exe` 一运行就段错误，
   表现是**编译静默失败**——不报错、不生成 dll。用 `cc1plus --version` 验证；
   坏了就在 Makefile 里写 `CXX = D:/mingw64/bin/g++`（换成机器上可用的那套）。
2. **案例工程路径写死 D 盘**：课程案例的 `.job`/`Makefile`/`.cproject` 全是 `D:\MINI-ATE`，
   装到 C 盘就会 `CreateProcess error=2` + 控制台无限刷 `connecting..`。全部改成实际路径。
3. **`CheckResultAndBin` 第一个参数永远写 0**：它是"本测试项参数表内的起始下标"，
   不是"第几项测试"。写成 1/2/3 会让结果和限值整体错位。
4. **向量列顺序 ≠ `.sig` 顺序** → DC 项全过、FUNC 全 fail。
5. **`.tmf` 参数个数 ≠ 代码里传的 N** → 判限错位或越界。
6. **写含 Windows 反斜杠的配置文件**：用脚本批量生成时，反斜杠容易被 shell 吃掉，
   生成出来变成 `c://MINI-ATE`。含反斜杠的 JSON 建议直接用编辑工具写。
7. **VOL 不能并测**：低有效输出同一时刻只有一路是低，必须循环逐路。

## 四、参考文件

- `references/file-formats.md` — 7 个配置文件的逐字段格式与真实样例
- `references/api-cheatsheet.md` — dps/ppmu/pinlevel/dio/binObj API 速查 + 模式缩写
- `templates/Test.cpp.tpl` — 测试函数骨架模板
- `examples/HC138_v1/` — 完整可编译样例（74HC138，7 项测试 + 11 条向量 + 全行注释）
