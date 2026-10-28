/* ============================================================================
 *  Test.cpp  —  74HC138 三八译码器测试程序
 *  【全行注释版】下面每一行代码，右边都有一句人话解释。没有一行是"跳过去"的。
 * ============================================================================
 *
 *  先记住一句话，整个文件都靠它读：
 *
 *      谁 . 干什么 ( 用多少 )
 *
 *  例：dps.Signal("VDD").Connect();   读作"电源，把 VDD 接上。"
 * ==========================================================================*/

/* ---------------------------------------------------------------------------
 *  【开头这 4 行是搬字典】
 *  平台把 dps、ppmu、ST_MEAS_RESULT 这些词的定义写在了头文件里，
 *  不搬进来编译器就不认识它们，会直接报错。
 *  带引号的 = 先在本工程目录里找；带尖括号的 = 去编译器自带库里找。
 * ------------------------------------------------------------------------ */
#include "interface.h"  // 搬进平台字典：dps/ppmu/sys/binObj 的定义，还有 USER_CODE 宏
#include <iostream>     // 搬进输入输出库：这样程序才能往屏幕上打印文字（cout 要用）
#include <string>       // 搬进字符串库：这样才支持 "VDD" 这种文字类型
#include <vector>       // 搬进盒子库：这样才支持 vector 这种"能装很多个的盒子"

using namespace std;    // 让后面的名字按"标准库"的规矩理解，不加这行，cout 就得写成 std::cout

/* ---------------------------------------------------------------------------
 *  【三个站点变量】
 *  本工程是单站（一次测一颗芯片），其实用不到它们。
 *  但平台内部的库函数引用了这三个名字，删掉就链接失败，所以必须留着。
 * ------------------------------------------------------------------------ */
unsigned long SITE = 0, SITE_ID = 0, ACTIVE_SITE_NUM = 0;  // 站点号、站点ID、活动站点数，全部初始化为 0

/* ---------------------------------------------------------------------------
 *  【两个引脚名单数组】
 *  数组 = 一排带编号的格子。IN_PIN[0] 是第一个格子，里面装着 "A0"。
 *  存成数组是为了后面能用 for 循环自动一个个取，不用手写 6 遍、8 遍。
 * ------------------------------------------------------------------------ */
static const char *IN_PIN[6] = {"A0", "A1", "A2", "E1", "E2", "E3"};  // 6 条输入腿：3 条地址 + 3 条使能
static const char *OUT_PIN[8] = {"Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7"};  // 8 条输出腿

/* ---------------------------------------------------------------------------
 *  【自己写的小工具 ForcePin】
 *  作用：把某一条输入腿"钳"到指定电压。传 0 就是低电平，传 5 就是高电平。
 *  为什么写它：VOH/VOL 两项要反复给 6 条腿设电平，每次都写一长串太啰嗦，
 *              包成一个小工具，用的时候一句 ForcePin("E1", 5) 就够了。
 *  两个参数：name = 腿的名字；volt = 要钳到几伏。
 * ------------------------------------------------------------------------ */
static void ForcePin(const char *name, double volt) {  // 定义一个小工具，叫 ForcePin，要传两个参数进来
  ppmu.Signal(name)          // 谁：测量单元，选中名字叫 name 的那条腿
      .SetMode("FVMI")       // 干什么：设成"加压测流"模式（我们要的是电压，所以加电压）
      .VoltForce(volt)       // 加多少：加 volt 伏（调用时传进来的那个数）
      .CurrRange(40e-3)      // 量程：40mA（40e-3 = 0.04A，e-3 就是乘 10 的 -3 次方）
      .Execute();            // 执行！前面全是设参数，这一下才真正下发到硬件
  sys.DelayUs(2000);         // 等 2000 微秒 = 2 毫秒，让电平稳定下来再往下走
}                            // 小工具到此结束

/* ===========================================================================
 *  【5 个生命周期函数】
 *  它们不是你手动调的，是测试机在特定时刻自动调的：
 *      ProjectInit   整个工程开始时调一次
 *      site_init     每个站点开始时调一次
 *      ProjectReuse  连续测下一颗芯片时调一次
 *      site_reset    每个站点结束时调一次
 *      ProjectReset  整个工程结束时调一次
 *  这里它们只干一件事：打印一句话，方便在日志里看清流程走到哪了。
 *  名字不能改，因为 .tmf 文件里把名字写死了。
 *  cout 那行的意思：往屏幕上打印这段文字，然后换行（endl = 换行）。
 * ========================================================================= */
USER_CODE void ProjectReuse() {          // 定义一个函数：USER_CODE 表示要导出给测试机调用，void 表示不返回任何值
  cout << "ProjectReuse" << endl;        // 在屏幕上打印 ProjectReuse 四个字，然后换行
}                                        // 函数结束

USER_CODE void ProjectInit() {           // 定义"工程初始化"函数
  cout << "ProjectInit" << endl;         // 打印 ProjectInit
}                                        // 函数结束

USER_CODE void ProjectReset() {          // 定义"工程复位"函数
  cout << "ProjectReset" << endl;        // 打印 ProjectReset
}                                        // 函数结束

USER_CODE void site_init() {             // 定义"站点初始化"函数
  cout << "site_init" << endl;           // 打印 site_init
}                                        // 函数结束

USER_CODE void site_reset() {            // 定义"站点复位"函数
  cout << "site_reset" << endl;          // 打印 site_reset
}                                        // 函数结束

/* 下面两个是"批次结束/晶圆结束"的回调。本工程没启用（要调 Enable_EOW_Func 才生效），
 * 留着只是为了让平台库能编译通过，平时不会执行。 */
void EndOfWafer() {                      // 定义"晶圆测完"时要做的事
  cout << "EndOfWafer" << endl;          // 打印 EndOfWafer
}                                        // 函数结束

void EndOfLot() {                        // 定义"批次测完"时要做的事
  cout << "EndOfLot" << endl;            // 打印 EndOfLot
}                                        // 函数结束

inline void Enable_EOW_Func() {                                    // 定义"启用晶圆结束回调"的小函数
  proberObj.EnableWaferEndCheck(EndOfWafer);                       // 把"晶圆结束"这个事件挂到 EndOfWafer 函数上
}                                                                  // 函数结束

inline void Enable_EOL_Func() {                                    // 定义"启用批次结束回调"的小函数
  proberObj.EnableLotEndCheck(EndOfLot);                           // 把"批次结束"这个事件挂到 EndOfLot 函数上
}                                                                  // 函数结束

/* ===========================================================================
 *  第 1 项  OSN_T  ——  负向开路短路（教学样板，后面 7 项都是这个骨架）
 * ---------------------------------------------------------------------------
 *  干什么：给 14 条腿同时"抽走" 100 微安电流，然后量每条腿的电压。
 *  为什么：每条腿内部有个 ESD 保护二极管（像单向阀），电流能通过它，
 *          量到的电压就会落在一个固定范围里。
 *  怎么判：-1.2 ~ -0.2 V  = 好的
 *          顶到 -2 V      = 开路（里面断了）
 *          接近 0 V       = 短路（和别的腿粘一起了）
 *  整体 7 步：接上 → VDD归零 → 腿归零 → 抽电流 → 量电压 → 判限 → 复位
 * ========================================================================= */
USER_CODE void OSN_T() {                        // 定义第 1 个测试项函数，名字必须和 .tmf 里写的一致
  TEST_BEGIN                                    // 宏：展开成 try{ 打开跳转; 意思是从这里开始"试运行"，出错会被抓住

  vector<ST_MEAS_RESULT> OSN_RESULT;            // 准备一个空盒子，装等下量到的 14 个电压值（ST_MEAS_RESULT 是"测量结果"类型）

  /* ---------- 第 1 步：接上 ---------- */
  dps.Signal("VDD").Connect();                  // 电源，把 VDD 这条腿接上（不接上，后面加电也没用）
  ppmu.Signal("INOUT_PINS").Connect();          // 测量单元，把 INOUT_PINS 这一组 14 条腿全部接上
  sys.DelayUs(5000);                            // 等 5000 微秒 = 5 毫秒，让连接稳定

  /* ---------- 第 2 步：VDD 归零 ----------
   * 开路短路测试必须断电，否则芯片内部电路会工作，干扰我们要量的二极管压降。 */
  dps.Signal("VDD")                             // 谁：电源，选中 VDD
      .SetMode("FVMV")                          // 模式：加压测压（Force Voltage Measure Voltage）
      .VoltForce(0)                             // 加 0 伏，等于断电
      .CurrRange(0.5)                           // 电流量程 0.5A
      .CurrClamp(0.5, -0.5)                     // 电流保护：超过 ±0.5A 就钳住，防止短路时烧片
      .Execute();                               // 执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  /* ---------- 第 3 步：14 条腿全部归零 ----------
   * 让 14 条腿从同一个起点出发，否则量出来的数互相没法比。 */
  ppmu.Signal("INOUT_PINS")                     // 谁：测量单元，选中 14 条腿
      .SetMode("FVMV")                          // 模式：加压测压
      .VoltForce(0)                             // 每条腿都压到 0 伏
      .CurrRange(40e-3)                         // 量程 40mA（40e-3 = 0.04A）
      .Execute();                               // 执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  /* ---------- 第 4 步：抽电流（本项的核心动作） ----------
   * 负号 = 往外抽；100e-6 = 0.0001A = 100 微安。
   * 正号就是往里灌（那是下一项 OSP 干的事）。 */
  ppmu.Signal("INOUT_PINS")                     // 谁：测量单元，选中 14 条腿
      .SetMode("FIMV")                          // 模式：加流测压（Force Current Measure Voltage），和上面 FVMV 不同
      .CurrForce(-100e-6)                       // 加 -100 微安（负号 = 从芯片往外抽）
      .CurrRange(100e-6)                        // 电流量程 100 微安（量程越小分辨率越高）
      .VoltClamp(2, -2)                         // 电压保险丝：最多 ±2V，不会无限飙高
      .Execute();                               // 执行
  sys.DelayUs(5000);                            // 等 5 毫秒，让电压稳定再量

  /* ---------- 第 5 步：量电压 ---------- */
  ppmu.Signal("INOUT_PINS").Measure(OSN_RESULT);  // 量 14 条腿的电压，14 个数塞进 OSN_RESULT 盒子

  /* ---------- 第 6 步：判限、分 Bin ----------
   * CheckResultAndBin(0, 盒子, 14) 三个参数：
   *   0   = 从"本测试项自己的参数表"第 0 个开始数（不是"第几项测试"，8 个函数都写 0）
   *   盒子 = 刚量到的 14 个数
   *   14  = 一共比 14 个
   * 比的对象是 .tmf 里 OSN_T 这一项的限值（-1.2 ~ -0.2 V），超了就判 FAIL、打 Sbin 2。 */
  binObj.CheckResultAndBin(0, OSN_RESULT, 14);  // 拿 14 个数去和限值比，并分 Bin

  /* ---------- 第 7 步：复位，把现场收拾干净 ----------
   * 不收拾的话，残留的电压电流会影响下一个测试项。 */
  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 电源：切回加压测压模式，给 0 伏
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，然后执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)  // 14 条腿：切回加压测压，给 0 伏
      .CurrRange(40e-3).Execute();              // 量程 40mA，然后执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 电源：断开
  ppmu.Signal("INOUT_PINS").DisConnect();       // 14 条腿：断开
  sys.DelayUs(5000);                            // 等 5 毫秒，收尾

  TEST_ERROR                                    // 宏：展开成 }catch(...){ 关闭跳转; 上面任何一步出错就跳到这里
  binObj.HandlerException(0);                   // 记一笔异常到日志，然后继续下一项，整个流程不会崩

  TEST_END                                      // 宏：展开成 } 关闭跳转; 收尾，函数结束
}                                               // OSN_T 函数结束

/* ===========================================================================
 *  第 2 项  OSP_T  ——  正向开路短路
 * ---------------------------------------------------------------------------
 *  和 OSN_T 一模一样，只改了一个符号：-100e-6 变成 +100e-6。
 *  为什么两个方向都要测：一条腿里有两个方向相反的保护二极管，
 *  OSN 抽电流走其中一个，OSP 灌电流走另一个，只测一边只能证明一半是好的。
 * ========================================================================= */
USER_CODE void OSP_T() {                        // 定义第 2 个测试项函数
  TEST_BEGIN                                    // 开始试运行（出错会被抓住）

  vector<ST_MEAS_RESULT> OSP_RESULT;            // 空盒子，装 14 个电压值

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD
  ppmu.Signal("INOUT_PINS").Connect();          // 测量单元，接上 14 条腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 电源：加压测压，给 0 伏（断电）
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)  // 14 条腿：加压测压，给 0 伏
      .CurrRange(40e-3).Execute();              // 量程 40mA，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INOUT_PINS").SetMode("FIMV").CurrForce(100e-6)  // 14 条腿：加流测压，灌 +100 微安（正号 = 往里灌）
      .CurrRange(100e-6).VoltClamp(2, -2).Execute();           // 量程 100 微安、电压钳位 ±2V，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INOUT_PINS").Measure(OSP_RESULT);  // 量 14 条腿的电压，塞进盒子

  binObj.CheckResultAndBin(0, OSP_RESULT, 14);  // 判限：0 = 从本项参数表第 0 个开始，14 = 比 14 个

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)  // 复位：14 条腿归零
      .CurrRange(40e-3).Execute();              // 量程 40mA，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  ppmu.Signal("INOUT_PINS").DisConnect();       // 断开 14 条腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // OSP_T 函数结束

/* ===========================================================================
 *  第 3 项  IIL_T  ——  输入低电平漏电流
 * ---------------------------------------------------------------------------
 *  干什么：VDD 给 5V，6 条输入腿都压到 0V，量从芯片里漏出来多少电流。
 *  为什么：输入脚理论上是"高阻"，不吃电流；漏得多说明内部有损伤。
 *  怎么判：-0.1 ~ 0.1 uA 之间算合格。
 *  注意：模式是 FVMI（加压测流），和开路短路的 FIMV 正好反过来。
 * ========================================================================= */
USER_CODE void IIL_T() {                        // 定义第 3 个测试项函数
  TEST_BEGIN                                    // 开始试运行

  vector<ST_MEAS_RESULT> IIL_RESULT;            // 空盒子，装 6 个电流值

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD
  ppmu.Signal("INPUT_PINS").Connect();          // 只接 6 条输入腿（INPUT_PINS 只含 A0~A2、E1~E3）
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)   // 这次要给电：VDD = 5V，芯片才真正工作
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INPUT_PINS").SetMode("FVMI").VoltForce(0)  // 6 条腿：加压测流，先给 0 伏把电平建立起来
      .CurrRange(40e-3).Execute();              // 先用大量程 40mA，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INPUT_PINS").SetMode("FVMI").VoltForce(0)  // 6 条腿：还是 0 伏
      .CurrRange(2e-6).Execute();               // 但量程切到 2 微安——量程小才量得到 0.1uA 的微小漏电
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("INPUT_PINS").Measure(IIL_RESULT);  // 量电流（模式是加压测流，所以量到的是电流）

  binObj.CheckResultAndBin(0, IIL_RESULT, 6);   // 判限：6 = 一共比 6 个值

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  ppmu.Signal("INPUT_PINS").SetMode("FVMV").VoltForce(0)  // 复位：6 条腿归零
      .CurrRange(40e-3).Execute();              // 量程 40mA，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  ppmu.Signal("INPUT_PINS").DisConnect();       // 断开 6 条腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // IIL_T 函数结束

/* ===========================================================================
 *  第 4 项  VOH_T  ——  输出高电平
 * ---------------------------------------------------------------------------
 *  难点：138 是"低有效"输出，正常译码时只有一路低，想量 8 路 VOH 就得让 8 路同时为高。
 *  办法：把 E1 拉高，关掉使能，8 路输出全部被强制为高，一次并测 8 路。
 *  怎么判：4.2 ~ 5.5 V 之间算合格（每路抽 1mA 的情况下）。
 *  新东西：for (i = 0; i < 6; i++) 是循环，把 6 条输入腿挨个接上，省得手写 6 遍。
 * ========================================================================= */
USER_CODE void VOH_T() {                        // 定义第 4 个测试项函数
  TEST_BEGIN                                    // 开始试运行

  vector<ST_MEAS_RESULT> VOH_RESULT;            // 空盒子，装 8 个电压值
  int i;                                        // 声明一个叫 i 的整数，专门用来当循环计数器

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD
  for (i = 0; i < 6; i++) ppmu.Signal(IN_PIN[i]).Connect();  // 循环 6 次：i=0接A0、i=1接A1……i=5接E3
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)   // 电源：给 5V
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ForcePin("A0", 0);                            // 调用小工具：把 A0 压到 0V
  ForcePin("A1", 0);                            // 把 A1 压到 0V
  ForcePin("A2", 0);                            // 把 A2 压到 0V
  ForcePin("E1", 5);                            // 关键一步：E1 拉高 = 关掉使能 = 8 路输出全变高
  ForcePin("E2", 0);                            // E2 是低有效使能，给 0 才不干扰
  ForcePin("E3", 5);                            // E3 是高有效使能，给 5 才不干扰

  ppmu.Signal("OUTPUT_PINS").SetMode("FIMV").CurrForce(-1e-3)  // 8 条输出腿：加流测压，抽 -1mA（-1e-3 = -0.001A）
      .CurrRange(1e-3).VoltClamp(6, 0).Execute();              // 量程 1mA、电压钳位 6~0V，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  ppmu.Signal("OUTPUT_PINS").Measure(VOH_RESULT);  // 量 8 条腿的电压，塞进盒子

  binObj.CheckResultAndBin(0, VOH_RESULT, 8);   // 判限：8 = 一共比 8 个值

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  for (i = 0; i < 6; i++) {                     // 循环 6 次，把 6 条输入腿挨个处理
    ppmu.Signal(IN_PIN[i]).SetMode("FVMV").VoltForce(0)  // 第 i 条腿：加压测压，给 0 伏
        .CurrRange(40e-3).Execute();            // 量程 40mA，执行
  }                                             // 循环结束
  sys.DelayUs(5000);                            // 等 5 毫秒
  ppmu.Signal("OUTPUT_PINS").SetMode("FVMV").VoltForce(0)  // 输出腿：加压测压，给 0 伏
      .CurrRange(40e-3).Execute();              // 量程 40mA，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  for (i = 0; i < 6; i++) ppmu.Signal(IN_PIN[i]).DisConnect();  // 循环 6 次，把 6 条输入腿挨个断开
  ppmu.Signal("OUTPUT_PINS").DisConnect();      // 断开 8 条输出腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // VOH_T 函数结束

/* ===========================================================================
 *  第 5 项  VOL_T  ——  输出低电平（唯一带循环的一项）
 * ---------------------------------------------------------------------------
 *  难点：138 同一时刻只有一路输出是低，没法像 VOH 那样一次量 8 路。
 *  办法：循环 8 次，每次给不同地址，让"轮到"的那一路变低，单独量它：
 *        i=0 → 地址 000 → Y0 变低，量 Y0
 *        i=1 → 地址 001 → Y1 变低，量 Y1   ……一直到 i=7 → Y7
 *  怎么判：0 ~ 0.3 V 之间算合格（每路灌 1mA 的情况下）。
 *  新东西：位运算 i & 0x1、三目运算 ? :、push_back、empty()
 * ========================================================================= */
USER_CODE void VOL_T() {                        // 定义第 5 个测试项函数
  TEST_BEGIN                                    // 开始试运行

  vector<ST_MEAS_RESULT> VOL_RESULT;            // 总盒子：最后装 8 个结果
  vector<ST_MEAS_RESULT> one;                   // 小盒子：每次只装 1 个结果
  int i;                                        // 循环计数器

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD
  for (i = 0; i < 6; i++) ppmu.Signal(IN_PIN[i]).Connect();  // 循环 6 次，把 6 条输入腿挨个接上
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)   // 电源：给 5V
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  for (i = 0; i < 8; i++) {                     // 循环开始：i 从 0 数到 7，一共转 8 圈

    ForcePin("A0", (i & 0x1) ? 5 : 0);          // 取 i 的二进制最低位：是 1 就给 A0 加 5V，是 0 就给 0V
    ForcePin("A1", (i & 0x2) ? 5 : 0);          // 取 i 的二进制中间位，决定 A1 是 5V 还是 0V
    ForcePin("A2", (i & 0x4) ? 5 : 0);          // 取 i 的二进制最高位，决定 A2 是 5V 还是 0V

    ForcePin("E1", 0);                          // 使能全开：E1 = 0
    ForcePin("E2", 0);                          // 使能全开：E2 = 0
    ForcePin("E3", 5);                          // 使能全开：E3 = 5（三个都对了芯片才译码）

    ppmu.Signal(OUT_PIN[i]).SetMode("FIMV").CurrForce(1e-3)  // 只给"轮到的那一路"：加流测压，灌 1mA
        .CurrRange(1e-3).VoltClamp(6, -1).Execute();         // 量程 1mA、电压钳位 6~-1V，执行
    sys.DelayUs(3000);                          // 等 3 毫秒，让电压稳定

    one.clear();                                // 把小盒子清空（不然上一圈的数会一直堆在里面）
    ppmu.Signal(OUT_PIN[i]).Measure(one);       // 量这一路的电压，1 个数放进小盒子

    if (!one.empty()) VOL_RESULT.push_back(one[0]);  // 如果小盒子不是空的，就把里面第 0 个数塞到总盒子末尾

    ppmu.Signal(OUT_PIN[i]).SetMode("FVMV").VoltForce(0)  // 这一路用完先归零
        .CurrRange(40e-3).Execute();            // 量程 40mA，执行
    ppmu.Signal(OUT_PIN[i]).DisConnect();       // 再断开这一路
    sys.DelayUs(2000);                          // 等 2 毫秒，进入下一圈
  }                                             // 循环结束：总盒子里现在有 8 个数了

  binObj.CheckResultAndBin(0, VOL_RESULT, 8);   // 判限：8 个数依次对应 Y0~Y7，比 8 个值

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  for (i = 0; i < 6; i++) {                     // 循环 6 次，处理 6 条输入腿
    ppmu.Signal(IN_PIN[i]).SetMode("FVMV").VoltForce(0)  // 第 i 条腿归零
        .CurrRange(40e-3).Execute();            // 量程 40mA，执行
  }                                             // 循环结束
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  for (i = 0; i < 6; i++) ppmu.Signal(IN_PIN[i]).DisConnect();  // 循环 6 次，断开 6 条输入腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // VOL_T 函数结束

/* ===========================================================================
 *  第 6 项  IDD_T  ——  静态电源电流（芯片待着费不费电）
 * ---------------------------------------------------------------------------
 *  干什么：VDD 加 5V，量芯片一共吃多少电流。
 *  怎么判：0 ~ 80 uA 之间算合格，超标说明内部漏电或微短路。
 *  小技巧：先用 FVMV 把 5V 稳稳定住，再切 FVMI 去量电流，读数更准。
 * ========================================================================= */
USER_CODE void IDD_T() {                        // 定义第 6 个测试项函数
  TEST_BEGIN                                    // 开始试运行

  vector<ST_MEAS_RESULT> IDD_RESULT;            // 空盒子，只有 1 个数

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD（这项不用碰任何信号腿）
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)   // 第一步：先稳压 5V（加压测压）
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMI").VoltForce(5)   // 第二步：换成加压测流，还是 5V
      .CurrRange(200e-6).CurrClamp(200e-6, -200e-6).Execute();  // 量程缩到 200 微安，提高分辨率
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").Measure(IDD_RESULT);        // 量电流，1 个数放进盒子

  binObj.CheckResultAndBin(0, IDD_RESULT, 1);   // 判限：只有 1 个数（IDD 就一个值）

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // IDD_T 函数结束

/* ===========================================================================
 *  第 7 项  FUNC_T  ——  功能测试（跑向量，看译码对不对）
 * ---------------------------------------------------------------------------
 *  干什么：把 11 条向量喂给芯片，每条 = 一组输入 + 一组期望输出，对不上就 FAIL。
 *  向量在哪：pattern/func.csv，用 Excel 或记事本就能打开看。
 *  和前面几项最大的区别：前面是"量数值"，这项是"比高低电平"，
 *  所以用 pinlevel（数字电平单元）和 dio（向量执行器），不用 ppmu。
 *  注意：装结果用的是 vector<ST_DIO_RESULT>，是"数字结果"，不是电压值。
 * ========================================================================= */
USER_CODE void FUNC_T() {                       // 定义第 7 个测试项函数
  TEST_BEGIN                                    // 开始试运行

  vector<ST_DIO_RESULT> FUNC_RESULT;            // 空盒子，装数字比较结果（注意类型和前面不一样）

  dps.Signal("VDD").Connect();                  // 电源，接上 VDD
  pinlevel.Signal("INOUT_PINS").Connect();      // 数字电平单元，接上 14 条腿（负责按向量驱动/比较）
  sys.DelayUs(5000);                            // 等 5 毫秒

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)   // 电源：给 5V
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  pinlevel.Signal("INOUT_PINS").SetMode("DIO").SetVih(5)  // 14 条腿：设成数字模式，驱动"高"时输出 5V
      .SetVil(0).SetVoh(3.5).SetVol(1.5)        // 驱动"低"输出 0V；读到 >3.5V 算高；读到 <1.5V 算低
      .VoltClamp(6, -1).Execute();              // 电压钳位 6~-1V，执行
  sys.DelayUs(5000);                            // 等 5 毫秒

  dio.Pattern("FUNC").SetTimeOut(20000).Test();  // 跑名叫 FUNC 的向量（在 .pat 里登记过），最多等 20 秒
  dio.Pattern("FUNC").GetFailResult(FUNC_RESULT);  // 把失败的比较结果取出来，全过的话盒子是空的

  binObj.CheckResultAndBin(0, FUNC_RESULT);     // 判限：数字版重载，不用传个数，内部自己判

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)   // 复位：电源归零
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();  // 量程 0.5A、保护 ±0.5A，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  pinlevel.Signal("INOUT_PINS").SetMode("DIO").SetVih(0)  // 14 条腿：电平全部归零
      .SetVil(0).SetVoh(0).SetVol(0)            // 四个门限都设成 0
      .VoltClamp(6, -1).Execute();              // 电压钳位 6~-1V，执行
  sys.DelayUs(5000);                            // 等 5 毫秒
  dps.Signal("VDD").DisConnect();               // 断开电源
  pinlevel.Signal("INOUT_PINS").DisConnect();   // 断开 14 条腿
  sys.DelayUs(5000);                            // 等 5 毫秒

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // FUNC_T 函数结束

/* ===========================================================================
 *  第 8 项  SetPassBin  ——  判合格
 * ---------------------------------------------------------------------------
 *  前面 7 项只要有任意一项 FAIL，流程就不会走到这里（.tmf 里配了分档规则）。
 *  能走到这一步，说明 7 项全过了，于是打上 Bin 1 = 合格。
 * ========================================================================= */
USER_CODE void SetPassBin() {                   // 定义第 8 个函数（它不是测试项，是"打标记"）
  TEST_BEGIN                                    // 开始试运行

  binObj.SetBin(1);                             // 打上"合格"标记，Bin 1

  TEST_ERROR                                    // 出错就跳到这里
  binObj.HandlerException(0);                   // 记录异常
  TEST_END                                      // 收尾
}                                               // SetPassBin 函数结束
