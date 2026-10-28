# MINI-ATE API 速查

头文件位置：`<安装根目录>/runner/inc/`
主文件：`usrInterface.h`（仪器对象）、`usrDataType.h`（结果类型）、`Bindef.h`（判限与分 Bin）

---

## 一、句型

```
谁 . 干什么 ( 用多少 ) . 再干什么 ( 用多少 ) ... .Execute();
```

链式调用的前几节都只是**设参数**，只有 `.Execute()` 那一下才真正下发到硬件。
忘记写 `Execute()` 是新手最常见的"代码跑了但芯片没反应"的原因。

---

## 二、六个工具对象

| 对象 | 类型 | 用途 |
|---|---|---|
| `dps` | 器件电源 | 给 VDD 供电，可加压测压 / 加压测流 |
| `ppmu` | 每脚参数测量单元 | 给任意一条腿加电压或电流，并测量 |
| `pinlevel` | 数字电平单元 | 按向量驱动/比较高低电平（FUNC 用） |
| `dio` | 向量执行器 | 执行 pattern，取失败结果（FUNC 用） |
| `sys` | 系统工具 | `DelayUs(n)` 延时 n 微秒 |
| `binObj` | 分箱器 | 判限、分 Bin、异常处理 |
| `proberObj` | 探针台/Handler | 批次/晶圆结束回调 |

---

## 三、三种模式缩写（最关键）

| 缩写 | 全称 | 中文 | 什么时候用 |
|---|---|---|---|
| `FVMV` | Force Voltage, Measure Voltage | 加压测压 | 建立电压（给 0V / 5V）、开路短路的准备阶段 |
| `FIMV` | Force Current, Measure Voltage | 加流测压 | OSN/OSP/VOH/VOL —— 灌或抽固定电流，量电压 |
| `FVMI` | Force Voltage, Measure Current | 加压测流 | IIL/IDD —— 加固定电压，量电流 |

记法：**前两个字母 = 加什么，后两个字母 = 量什么**。

---

## 四、常用方法

| 方法 | 说明 |
|---|---|
| `.Signal("名字")` | 选中一个信号或一个信号组（组名来自 `.grp`） |
| `.Connect()` / `.DisConnect()` | 接上 / 断开 |
| `.SetMode("FVMV"/"FIMV"/"FVMI")` | 设工作模式 |
| `.VoltForce(v)` | 强制输出 v 伏 |
| `.CurrForce(i)` | 强制输出 i 安（**负号 = 往外抽**） |
| `.CurrRange(r)` | 电流量程。量小电流必须切小量程，否则分辨率为 0 |
| `.CurrClamp(hi, lo)` | 电流保护上下限 |
| `.VoltClamp(hi, lo)` | 电压钳位上下限（保险丝） |
| `.Execute()` | **执行** |
| `.Measure(vec)` | 测量，结果填进 vector |
| `.SetVih/.SetVil/.SetVoh/.SetVol` | 数字电平门限（FUNC 用） |
| `dio.Pattern("名").SetTimeOut(ms).Test()` | 跑向量 |
| `dio.Pattern("名").GetFailResult(vec)` | 取失败结果 |

### 判限

```cpp
binObj.CheckResultAndBin(0, vec, N);        // 模拟量：起始下标 0，比 N 个
binObj.CheckResultAndBin(0, dioVec);        // 数字量：不用传个数
binObj.SetBin(1);                           // 打合格标记
binObj.HandlerException(0);                 // 异常处理，放在 TEST_ERROR 里
```

> **第一个参数永远写 0**。它是"本测试项参数表内的起始下标"，不是"第几项测试"。
> 写 1/2/3 会让结果和限值整体错位。

---

## 五、测试函数骨架

```cpp
USER_CODE void XXX_T() {
  TEST_BEGIN
  vector<ST_MEAS_RESULT> RESULT;

  dps.Signal("VDD").Connect();
  ppmu.Signal("组名").Connect();
  sys.DelayUs(5000);

  // 建立条件 → 施加激励 → 测量 → 判限
  ppmu.Signal("组名").SetMode("FIMV").CurrForce(-100e-6)
      .CurrRange(100e-6).VoltClamp(2, -2).Execute();
  sys.DelayUs(5000);
  ppmu.Signal("组名").Measure(RESULT);
  binObj.CheckResultAndBin(0, RESULT, N);

  // 复位
  dps.Signal("VDD").DisConnect();
  ppmu.Signal("组名").DisConnect();

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}
```

三个宏的真实展开（来自 `usrInterface.h`）：

```c
#define TEST_BEGIN  try{ dynamic_cast<INNER_SYS&>(sys).EnableJump();
#define TEST_ERROR  }catch(...){ dynamic_cast<INNER_SYS&>(sys).DisableJump();
#define TEST_END    } dynamic_cast<INNER_SYS&>(sys).DisableJump();
```

也就是说，这三行把函数体包成了 `try { ... } catch(...) { ... }`，
任何一步抛异常都会跳到 `TEST_ERROR` 后面，不会把整个测试流程带崩。

---

## 六、数值写法

| 写法 | 等于 |
|---|---|
| `40e-3` | 0.04（40 mA） |
| `100e-6` | 0.0001（100 µA） |
| `2e-6` | 0.000002（2 µA） |
| `1e-3` | 0.001（1 mA） |
| `-100e-6` | −100 µA（负号 = 往外抽） |

---

## 七、低有效输出的两个套路

以 74HC138（输出低有效，同一时刻只有一路为低）为例：

**测 VOH —— 让 8 路同时为高**
```cpp
ForcePin("E1", 5);   // 关掉使能，8 路输出被强制为高
ppmu.Signal("OUTPUT_PINS").SetMode("FIMV").CurrForce(-1e-3)...  // 一次并测 8 路
```

**测 VOL —— 只能逐路**
```cpp
for (i = 0; i < 8; i++) {
    ForcePin("A0", (i & 0x1) ? 5 : 0);   // 把 i 拆成 3 个地址位
    ForcePin("A1", (i & 0x2) ? 5 : 0);
    ForcePin("A2", (i & 0x4) ? 5 : 0);
    ppmu.Signal(OUT_PIN[i]).SetMode("FIMV").CurrForce(1e-3)...  // 只量被选中的那一路
}
```

高有效输出（如 CD4511）反过来：VOH 靠灯测试 LT 点亮全段，VOL 靠消隐 BI 拉低全段。
