# 7 个配置文件的格式（MINI-ATE）

所有文件都是纯文本，编码 UTF-8，换行 CRLF 或 LF 都能读。
`.job` / `.tmf` 是 JSON，其余是 CSV。

---

## 1. `.sig` — 信号与通道映射

```
SigName,AFEType,ChType,Site0_CH
"VDD",DPS,Supply,1:1:0
"A0",DIO,Inout,1:1:0
"A1",DIO,Inout,1:1:1
```

- 第 1 列信号名，**必须和 `.grp`、向量表里的名字完全一致**（大小写敏感）
- `AFEType`：`DPS`（电源）/ `DIO`（数字）/ `AWG` / `DGT` / `RVS` / `BPMU` / `CBIT`
- `ChType`：`Supply` / `Inout` / `Input` / `Output`
- `Site0_CH` 格式 `板卡:站点内资源:通道`，如 `1:1:0`
- **DPS 和 DIO 是两套独立编号**，都从 `1:1:0` 开始不冲突
- 顺序决定了向量表的列顺序，也决定了 `Measure()` 出来的结果顺序

---

## 2. `.grp` — 信号分组

```
GrpName,SigName
"INPUT_PINS",A0,A1,A2,E1,E2,E3
"OUTPUT_PINS",Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7
"INOUT_PINS",A0,A1,A2,E1,E2,E3,Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7
```

- 组名随便起，但 Test.cpp 里 `ppmu.Signal("组名")` 要用到
- 组内顺序 = 测量结果顺序 = `.tmf` 里 ParamNumber 顺序，三者必须对齐

---

## 3. `.tim` — 时序波形表

```
TimingName,Period,Signal,WFC,Evt0 Event,Evt0 Expr,Evt1 Event,Evt1 Expr,Evt2 Event,Evt2 Expr,Evt3 Event,Evt3 Expr
WFT1,10us,INPUT_PINS,0,D,0us,,,,,,
,,,1,U,0us,,,,,,
,,OUTPUT_PINS,L,L,9us,,,,,,
,,,H,H,9us,,,,,,
,,,X,X,9us,,,,,,
```

- `WFC` 是向量表里每格用的符号：输入 `0`/`1`，输出 `L`/`H`/`X`
- `Event`：`D` 驱动低、`U` 驱动高、`L`/`H` 比较低/高、`X` 不比较
- `Expr` 是事件发生时刻，例 `0us`、`9us`
- 常规做法：周期 10µs，输入 0µs 处驱动，输出 9µs 处采样（留足传输延迟余量）

---

## 4. `.pat` — Pattern 索引

```
1,,FUNC,1001,true,.\pattern\func.csv,
```

- 第 3 列 = Pattern 名，对应代码里 `dio.Pattern("FUNC")`
- 第 6 列 = 向量文件相对路径（反斜杠）
- `true` = 启用

---

## 5. `pattern/*.csv` — 向量表

```
Label,WFT,Sequence,A0,A1,A2,E1,E2,E3,Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,
,WFT1,nop,0,0,0,0,0,1,L,H,H,H,H,H,H,H,
,WFT1,stop,0,0,0,0,0,0,H,H,H,H,H,H,H,H,
```

- 前三列固定：`Label`（可空）、`WFT`（用哪张时序表）、`Sequence`（`nop` 继续 / `stop` 结束）
- 后面每列一个信号，**顺序必须和 `.sig` 完全一致**
- 输入列写 `0`/`1`，输出列写 `H`/`L`/`X`（X = 不比较）
- 行尾多一个逗号是正常的
- 最后一行必须 `stop`，否则跑到文件尾才停

---

## 6. `.tmf` — 测试流与限值

```json
{
  "Version": "V1.0",
  "ProjectInitName": "ProjectInit",
  "ProjectResetName": "ProjectReset",
  "ProjectReuseName": "ProjectReuse",
  "SiteList": [{
    "SiteNumber": 0,
    "SiteInitName": "site_init",
    "SiteResetName": "site_reset",
    "LoopCount": 1,
    "ErrorSbin": -1, "ErrorHbin": -1, "BinLoopCount": 0,
    "FuncList": [
      {
        "FuncNumber": 0,
        "FuncName": "OSN_T",
        "FuncDesc": "",
        "LoopCount": 1,
        "Sbin": 2, "Hbin": 2,
        "Results": "Fail",
        "ResultAnalysis": [
          { "ParamName": "OSN__A0", "ParamNumber": 0, "Min": "-1.2", "Max": "-0.2", "Unit": "V" }
        ]
      }
    ]
  }]
}
```

关键约束：

- `FuncName` 必须和 Test.cpp 里导出的函数名一字不差
- `FuncNumber` 从 0 连续编号
- `ResultAnalysis` 的**个数**必须等于代码里 `CheckResultAndBin(0, vec, N)` 的 N
- `ParamNumber` 从 0 连续递增，顺序 = `.grp` 里组内信号顺序
- `Results`：`Fail` 表示该步失败即按 `Sbin` 分档；`Pass` 用于 SetPassBin
- 最后一项固定是 `SetPassBin`，`Sbin`/`Hbin` = 1
- `Min`/`Max` 是字符串，`Unit` 用 `V` / `uA` / `mA` / 空

---

## 7. `.job` — 工程总入口

```json
{
  "JOB": "2.0",
  "jobDir": "C:\\...\\HC138_v1",
  "jobFilePath": "C:\\...\\HC138_v1\\HC138_v1.job",
  "Name": "HC138_v1",
  "Date": "2026-10-28 19:40:00",
  "ResDef": "HC138_v1.res",
  "SigMap": "HC138_v1.sig",
  "Pattern": "HC138_v1.pat",
  "Timing": "HC138_v1.tim",
  "TestProg": { "Src": [], "IncDir": [] },
  "Library": "Test.dll",
  "Makefile": "Makefile",
  "TMF": "HC138_v1.tmf",
  "AteHomePath": "c:\\MINI-ATE",
  "SigGroup": "HC138_v1.grp",
  "lotId": "HC138_LXX1"
}
```

- `AteHomePath` 必须是本机真实安装根目录，否则 Runner 起不来
- `jobDir` / `jobFilePath` 在 IDE 里保存时会自动改写，初值写对即可
- JSON 里的 Windows 路径要写双反斜杠

---

## 8. `.res` — 机台板卡资源（一般不用改，照抄案例）

```
1,OnLine,AWG_1,AWG,1
,,DGT_1,DGT,1
,,RVS_1,RVS,1
,,DPS_1,DPS,4
,,BPMU_1,BPMU,4
,,CBIT_1,CBIT,8
,,DIO_1,DIO,32
5,OffLine,,,
```

每行 = 一个槽位下的一块板卡：`槽位, 状态, 资源名, 类型, 数量`。
只有 `OnLine` 的资源能被软件申请到。

---

## 9. `Makefile`

```makefile
CXX = D:/mingw64/bin/g++
RUNNER_HOME = c:/MINI-ATE/runner
CFGOPT = -shared -Wall -Wl,-subsystem,windows -mthreads -fPIC
LIBS = -lsetupapi -lkernel32 -luser32 -lgdi32 -lwinspool -llibCommon -lcomdlg32 \
       -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 \
       -L$(RUNNER_HOME) -lproject -lTester -lBinDef -lUsrApi -lWaveCalc -lShmoo
INCS = -I./ -I$(RUNNER_HOME)/inc/
SRC = Test.cpp
TESTLIB = Test.dll

all:
	$(CXX) -g -std=c++17 $(SRC) $(INCS) $(LIBS) $(CFGOPT) -DINTERFACE_EXPORTS -o $(TESTLIB)
clean:
	$(RM) $(TESTLIB)
```
