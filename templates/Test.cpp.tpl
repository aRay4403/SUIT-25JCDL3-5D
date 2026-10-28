// ============================================================================
//  Test.cpp 模板 —— 复制后把 {{...}} 换成实际内容
//  说明：所有测试项都用这个骨架。改三处就能换一项测试：
//        1) 加什么（FIMV 抽/灌多少，或 FVMI 加多少伏）
//        2) 量什么（Measure 到哪个盒子）
//        3) 比几个（CheckResultAndBin 的第三个参数，必须等于 .tmf 里参数个数）
// ============================================================================

#include "interface.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

unsigned long SITE = 0, SITE_ID = 0, ACTIVE_SITE_NUM = 0;

// {{DUT 型号}} 的引脚名单：输入几条、输出几条，按 .sig 顺序写
static const char *IN_PIN[{{输入条数}}] = {{{输入脚列表}}};
static const char *OUT_PIN[{{输出条数}}] = {{{输出脚列表}}};

// 把某条腿钳到指定电压（0 = 低电平，5 = 高电平）
static void ForcePin(const char *name, double volt) {
  ppmu.Signal(name).SetMode("FVMI").VoltForce(volt).CurrRange(40e-3).Execute();
  sys.DelayUs(2000);
}

USER_CODE void ProjectReuse() { cout << "ProjectReuse" << endl; }
USER_CODE void ProjectInit() { cout << "ProjectInit" << endl; }
USER_CODE void ProjectReset() { cout << "ProjectReset" << endl; }
USER_CODE void site_init() { cout << "site_init" << endl; }
USER_CODE void site_reset() { cout << "site_reset" << endl; }

// ---------------------------------------------------------------------------
// 第 1 项 OSN_T —— 负向开路短路
// 干什么：14 条腿同时抽 -100µA，量电压。正常 -1.2~-0.2V；顶到 -2V = 开路；接近 0V = 短路
// ---------------------------------------------------------------------------
USER_CODE void OSN_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> OSN_RESULT;

  dps.Signal("VDD").Connect();
  ppmu.Signal("INOUT_PINS").Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").SetMode("FIMV").CurrForce(-100e-6)
      .CurrRange(100e-6).VoltClamp(2, -2).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").Measure(OSN_RESULT);
  binObj.CheckResultAndBin(0, OSN_RESULT, {{管脚总数}});

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  ppmu.Signal("INOUT_PINS").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 2 项 OSP_T —— 正向开路短路（和 OSN 只差一个正负号）
// ---------------------------------------------------------------------------
USER_CODE void OSP_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> OSP_RESULT;

  dps.Signal("VDD").Connect();
  ppmu.Signal("INOUT_PINS").Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").SetMode("FIMV").CurrForce(100e-6)
      .CurrRange(100e-6).VoltClamp(2, -2).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INOUT_PINS").Measure(OSP_RESULT);
  binObj.CheckResultAndBin(0, OSP_RESULT, {{管脚总数}});

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  ppmu.Signal("INOUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  ppmu.Signal("INOUT_PINS").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 3 项 IIL_T —— 输入漏电流：加 0V，量电流。注意模式是 FVMI
// ---------------------------------------------------------------------------
USER_CODE void IIL_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> IIL_RESULT;

  dps.Signal("VDD").Connect();
  ppmu.Signal("INPUT_PINS").Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INPUT_PINS").SetMode("FVMI").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);
  ppmu.Signal("INPUT_PINS").SetMode("FVMI").VoltForce(0)
      .CurrRange(2e-6).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("INPUT_PINS").Measure(IIL_RESULT);
  binObj.CheckResultAndBin(0, IIL_RESULT, {{输入脚数}});

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  ppmu.Signal("INPUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  ppmu.Signal("INPUT_PINS").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 4 项 VOH_T —— 输出高电平
// 低有效输出：拉高使能脚让全部输出为高，一次并测；高有效输出：用灯测试点亮全段
// ---------------------------------------------------------------------------
USER_CODE void VOH_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> VOH_RESULT;
  int i;

  dps.Signal("VDD").Connect();
  for (i = 0; i < {{输入脚数}}; i++) ppmu.Signal(IN_PIN[i]).Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  // {{让全部输出为高的条件，例如 ForcePin("E1", 5);}}

  ppmu.Signal("OUTPUT_PINS").SetMode("FIMV").CurrForce(-1e-3)
      .CurrRange(1e-3).VoltClamp(6, 0).Execute();
  sys.DelayUs(5000);

  ppmu.Signal("OUTPUT_PINS").Measure(VOH_RESULT);
  binObj.CheckResultAndBin(0, VOH_RESULT, {{输出脚数}});

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  for (i = 0; i < {{输入脚数}}; i++) {
    ppmu.Signal(IN_PIN[i]).SetMode("FVMV").VoltForce(0).CurrRange(40e-3).Execute();
  }
  sys.DelayUs(5000);
  ppmu.Signal("OUTPUT_PINS").SetMode("FVMV").VoltForce(0)
      .CurrRange(40e-3).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  for (i = 0; i < {{输入脚数}}; i++) ppmu.Signal(IN_PIN[i]).DisConnect();
  ppmu.Signal("OUTPUT_PINS").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 5 项 VOL_T —— 输出低电平
// 低有效输出同一时刻只有一路为低 → 必须循环逐路选中
// ---------------------------------------------------------------------------
USER_CODE void VOL_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> VOL_RESULT;
  vector<ST_MEAS_RESULT> one;
  int i;

  dps.Signal("VDD").Connect();
  for (i = 0; i < {{输入脚数}}; i++) ppmu.Signal(IN_PIN[i]).Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  for (i = 0; i < {{输出脚数}}; i++) {
    // {{按位拆地址，例如：
    //   ForcePin("A0", (i & 0x1) ? 5 : 0);
    //   ForcePin("A1", (i & 0x2) ? 5 : 0);
    //   ForcePin("A2", (i & 0x4) ? 5 : 0); }}
    // {{打开使能，例如 ForcePin("E1", 0); ForcePin("E2", 0); ForcePin("E3", 5);}}

    ppmu.Signal(OUT_PIN[i]).SetMode("FIMV").CurrForce(1e-3)
        .CurrRange(1e-3).VoltClamp(6, -1).Execute();
    sys.DelayUs(3000);

    one.clear();
    ppmu.Signal(OUT_PIN[i]).Measure(one);
    if (!one.empty()) VOL_RESULT.push_back(one[0]);

    ppmu.Signal(OUT_PIN[i]).SetMode("FVMV").VoltForce(0)
        .CurrRange(40e-3).Execute();
    ppmu.Signal(OUT_PIN[i]).DisConnect();
    sys.DelayUs(2000);
  }

  binObj.CheckResultAndBin(0, VOL_RESULT, {{输出脚数}});

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  for (i = 0; i < {{输入脚数}}; i++) {
    ppmu.Signal(IN_PIN[i]).SetMode("FVMV").VoltForce(0).CurrRange(40e-3).Execute();
  }
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  for (i = 0; i < {{输入脚数}}; i++) ppmu.Signal(IN_PIN[i]).DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 6 项 IDD_T —— 静态电源电流
// ---------------------------------------------------------------------------
USER_CODE void IDD_T() {
  TEST_BEGIN

  vector<ST_MEAS_RESULT> IDD_RESULT;

  dps.Signal("VDD").Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMI").VoltForce(5)
      .CurrRange(200e-6).CurrClamp(200e-6, -200e-6).Execute();
  sys.DelayUs(5000);

  dps.Signal("VDD").Measure(IDD_RESULT);
  binObj.CheckResultAndBin(0, IDD_RESULT, 1);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 7 项 FUNC_T —— 功能测试（跑向量，比高低电平）
// ---------------------------------------------------------------------------
USER_CODE void FUNC_T() {
  TEST_BEGIN

  vector<ST_DIO_RESULT> FUNC_RESULT;

  dps.Signal("VDD").Connect();
  pinlevel.Signal("INOUT_PINS").Connect();
  sys.DelayUs(5000);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(5)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);

  pinlevel.Signal("INOUT_PINS").SetMode("DIO").SetVih(5)
      .SetVil(0).SetVoh(3.5).SetVol(1.5)
      .VoltClamp(6, -1).Execute();
  sys.DelayUs(5000);

  dio.Pattern("FUNC").SetTimeOut(20000).Test();
  dio.Pattern("FUNC").GetFailResult(FUNC_RESULT);

  binObj.CheckResultAndBin(0, FUNC_RESULT);

  dps.Signal("VDD").SetMode("FVMV").VoltForce(0)
      .CurrRange(0.5).CurrClamp(0.5, -0.5).Execute();
  sys.DelayUs(5000);
  pinlevel.Signal("INOUT_PINS").SetMode("DIO").SetVih(0)
      .SetVil(0).SetVoh(0).SetVol(0)
      .VoltClamp(6, -1).Execute();
  sys.DelayUs(5000);
  dps.Signal("VDD").DisConnect();
  pinlevel.Signal("INOUT_PINS").DisConnect();
  sys.DelayUs(5000);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}

// ---------------------------------------------------------------------------
// 第 8 项 SetPassBin —— 全部通过后打合格标记
// ---------------------------------------------------------------------------
USER_CODE void SetPassBin() {
  TEST_BEGIN

  binObj.SetBin(1);

  TEST_ERROR
  binObj.HandlerException(0);
  TEST_END
}
