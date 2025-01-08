#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// 计算玩家受到伤害的函数
void calculateDamage(float skillDamage, float* gamerShield, float* gamerHP) {
    if (*gamerShield <= 0) {
        if (*gamerHP - skillDamage >= 0) {
            *gamerHP -= skillDamage;
        }
        else {
            *gamerHP = 0;
        }
    }
    else if (*gamerShield >= skillDamage) {
        *gamerShield -= skillDamage;
    }
    else {
        int temp = *gamerShield;
        *gamerShield = 0;
        if (*gamerHP - (skillDamage - temp) >= 0) {
            *gamerHP -= (skillDamage - temp);
        }
        else {
            *gamerHP = 0;
        }
    }
}

// 计算玩家伤害的函数
void Gamer_Demage(float* Gamer_Attack_Power, float* Skill_Rate, int* Critical_Hit_Rate, float* Hit_Demage, float* Demage, float* Boss_HP, int* Boss_Resistance, int* Skill_Deresistance, float* Break_Special_Attack, int* isBossResistanceBroken) {
    srand((unsigned int)time(NULL));  // 初始化随机数生成器

    int randomValue = rand() % 100;  // 生成 0 到 99 之间的随机数
    if (randomValue < *Critical_Hit_Rate) {
        *Demage = *Gamer_Attack_Power * *Skill_Rate;
    }//不暴击
    else {
        printf("暴击！\n");
        *Demage = *Gamer_Attack_Power * *Skill_Rate * *Hit_Demage;
    }//暴击
    if (*Boss_Resistance - *Skill_Deresistance > 0) {
        *Boss_Resistance -= *Skill_Deresistance;
    }
    else if (*Boss_Resistance == 0) {
        *isBossResistanceBroken = 1;//标记韧性已经被击破
    }
    else if (*Boss_Resistance - *Skill_Deresistance <= 0) {
        *Boss_Resistance = 0;
        *isBossResistanceBroken = 0;  // 标记韧性被击破
    }
    if (*Boss_Resistance == 0 && *isBossResistanceBroken == 0) {  // 只有在韧性被击破的回合造成击破伤害
        *Demage = *Demage * (1 + *Break_Special_Attack);
        printf("击破！\n");
    }
    if (*Boss_Resistance == 0) {
        *Demage = *Demage * 1.1;
    }
    if (*Boss_HP - *Demage >= 0) {
        *Boss_HP -= *Demage;
    }
    else {
        *Boss_HP = 0;
    }
    printf("伤害为%f\n", *Demage);
}

// Boss 行动的函数
void bossCounterAttack(float* Boss_HP, float* Gamer_Sheild, float* Gamer_HP, int* poisoned, int* bossResistanceRecoveryCount, int* Boss_Resistance, float* Gamer_Dot_Rate, float* Max_Boss_HP) {
    if (*Boss_HP > 0) {
        int randomSkill = rand() % 3;
        if (randomSkill == 0) {
            calculateDamage(1, Gamer_Sheild, Gamer_HP);
            printf("Boss 使用了普通攻击，对你造成了%d 点伤害！\n", 1);
        }
        else if (randomSkill == 1) {
            calculateDamage(3, Gamer_Sheild, Gamer_HP);
            printf("Boss 使用了终结技，对你造成了%d 点伤害！\n", 3);
        }
        else {
            calculateDamage(1, Gamer_Sheild, Gamer_HP);
            printf("Boss 使用了毒爪，你中毒了！\n");
            *poisoned = 1;
        }
    }
    // 处理 Boss 韧性的恢复
    if (*bossResistanceRecoveryCount >= 2 && *Boss_Resistance == 0) {
        *Boss_Resistance = 100;
        *bossResistanceRecoveryCount = 0;
        printf("Boss 的韧性已恢复！\n");
    }
    else if (*Boss_Resistance == 0) {
        (*bossResistanceRecoveryCount)++;
        if (*Boss_HP - (*Max_Boss_HP * *Gamer_Dot_Rate) >= 0) {
            *Boss_HP -= (*Max_Boss_HP * *Gamer_Dot_Rate);
            printf("Boss受到%f的裂伤\n", *Max_Boss_HP * *Gamer_Dot_Rate);
        }
        else {
            *Boss_HP = 0;
        }
    }
}

// 恢复能量值的函数
void recoverEnergy(int* Energy, int amount, int Max_Energy) {
    if (*Energy + amount < Max_Energy) {
        *Energy += amount;
    }
    else {
        *Energy = Max_Energy;
    }
}

// 模拟获取鼠标输入
int getMouseInput() {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    if (SendInput(1, &input, sizeof(INPUT)) == 0) {
        printf("鼠标左键按下操作失败\n");
        return 0;
    }

    return 1;
}

int main() {
    printf("欢迎来到我的游戏，你准备好与 Boss 对战了吗？\n");
    char operate;
    float Boss_HP = 30.0, Max_Boss_HP = 30.0, Gamer_Sheild = 0, Gamer_HP = 40.0, Max_Gamer_HP = 40.0;//Boss血量，玩家初始护盾值，玩家初始血量，玩家血量上限
    float Gamer_Attack_Power = 1.0;//基础攻击力
    int Cratical_Hit_Rate = 60;//暴击率
    float Hit_Demage = 1.5;//暴击伤害
    float Skill_Rate_A = 1.1, Skill_Rate_E = 1.4, Skill_Rate_Q = 3.5;//技能伤害倍率
    int Energy = 50;//初始能量
    int Max_Energy = 100;//能量上限
    float Sheild_up = 2.0;//防御技能护盾量
    float Health_up = 2.0;//治疗技能恢复量
    int count = 0;//记录回合数
    float e1 = 1, e2 = 3, e3 = 1;//Boss技能伤害
    int Gamer_poisoned = 0;//玩家中毒标记
    int Attack_point = 3;//初始战技点
    float Energy_Recover_A = 15, Energy_Recover_E = 35, Energy_Recover_D = 35, Energy_Recover_H = 40;//回能
    float Demage_A, Demage_E, Demage_Q;//伤害
    int Boss_Resistance = 100, Skill_Deresistance_A = 10, Skill_Deresistance_E = 25, Skill_Deresistance_Q = 40;//削韧
    int bossResistanceRecoveryCount = 0;  // 新增用于记录回合数的变量
    float Break_Special_Attack = 2.096f;//击破特攻
    int isBossResistanceBroken = 0;  // 新增标记，用于判断是否已经击破Boss韧性
    float Gamer_Dot_rate = 0.07f;//玩家破韧后持续伤害倍率
    printf("请进行操作:\n");

    while (count < 15) {
        if (getMouseInput()) {
            printf("你进行了攻击！\n");
            Gamer_Demage(&Gamer_Attack_Power, &Skill_Rate_A, &Cratical_Hit_Rate, &Hit_Demage, &Demage_A, &Boss_HP, &Boss_Resistance, &Skill_Deresistance_A, &Break_Special_Attack, &isBossResistanceBroken);
            if (Attack_point + 1 <= 5) {
                Attack_point += 1;
                printf("恢复战技点\n");
            }
            printf("能量值恢复！\n");
            recoverEnergy(&Energy, Energy_Recover_A, Max_Energy);
            isBossResistanceBroken = 0;  // 重置击破标记
        }

        // 其他操作逻辑保持不变

        // Boss 行动（如果 Boss 血量大于 0 才行动）
        bossCounterAttack(&Boss_HP, &Gamer_Sheild, &Gamer_HP, &Gamer_poisoned, &bossResistanceRecoveryCount, &Boss_Resistance, &Gamer_Dot_rate, &Max_Boss_HP);

        // 游戏结束判断
        if (Boss_HP <= 0) {
            printf("恭喜你，打败了对手！\n");
            break;
        }
        if (Gamer_HP <= 0) {
            printf("GameOver!\n");
            break;
        }

        // 处理中毒状态
        if (Gamer_poisoned) {
            printf("你中毒了，失去一点生命值！\n");
            if (Gamer_HP - 1 >= 0) {
                Gamer_HP -= 1;
            }
            else {
                Gamer_HP = 0;
            }
            if (Gamer_HP <= 0) {
                printf("GameOver!\n");
                break;
            }
        }

        // 优化后的状态显示
        printf("你目前的血量:%f 护盾:%f 战技点:%d 能量值:%d\nBoss 目前的血量:%f 韧性:%d\n", Gamer_HP, Gamer_Sheild, Attack_point, Energy, Boss_HP, Boss_Resistance);
        if (Gamer_poisoned > 0) {
            printf("你正处于中毒状态！请及时使用治疗！\n");
        }
        count++;
    }

    //游戏结算
    printf("游戏结束\nBoss 剩余血量为:%f\n 你的护盾值为:%f\n 你的血量为:%f\n", Boss_HP, Gamer_Sheild, Gamer_HP);
    if (Boss_HP > 0) {
        printf("很遗憾，你未能击败对手！");
    }
    else if (Gamer_HP > 0) {
        printf("恭喜你击败了对手！");
    }
    else if (Gamer_HP <= 0 && Boss_HP <= 0) {
        printf("你们同归于尽了！");
    }
    return 0;
}