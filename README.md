# SUIT-25JCDL3-5D · 加速科技 ST2500 集成电路测试

存放基于加速科技 MINI-ATE（ST2500 / ST2516 教学平台）的集成电路测试工程与可复用技能包。

## 目录

| 路径 | 说明 |
|---|---|
| `SKILL.md` | 技能包主文件：从一颗芯片的数据手册出发，做出完整 ATE 测试工程的标准流程 |
| `references/file-formats.md` | 7 个配置文件的逐字段格式（.job/.tmf/.sig/.grp/.res/.tim/.pat） |
| `references/api-cheatsheet.md` | dps / ppmu / pinlevel / dio / binObj API 速查与模式缩写 |
| `templates/Test.cpp.tpl` | 测试函数骨架模板 |
| `examples/HC138_v1/` | 完整可编译样例：74HC138 三八译码器，7 项测试 + 11 条向量 + 全行中文注释 |

## 这个工程做什么

给一颗芯片做"体检"：给它的引脚加电压电流，量它的反应，和标准范围比较，判合格与否。

`examples/HC138_v1` 是 74HC138 三八译码器的完整测试工程，包含：

- 7 个测试项：开路短路（OSN/OSP）、输入漏电（IIL）、输出高/低电平（VOH/VOL）、静态耗电（IDD）、功能（FUNC）
- 11 条功能向量：8 条译码验证 + 3 条使能关断验证
- `Test.cpp` 每一行都有中文注释

## 怎么用

1. 用 ST-IDE（SCWorkbench）打开 `examples/HC138_v1/HC138_v1.job`
2. 按 `readme.md` 里的接线表把芯片接到接口板
3. 编译：`cd examples/HC138_v1 && make`
4. 仿真：Runner 里 `run` → `sim` → `sim:run 1`

> 注意：`AteHomePath`、`Makefile` 的 `RUNNER_HOME`、`.cproject` 的 include 路径
> 都指向本机安装目录（`C:\MINI-ATE` 或 `D:\MINI-ATE`），换机器要同步改。
