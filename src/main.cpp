#include <Arduino.h>
#include <ps3i2clib.h>
#include <Servo.h>
#include "cout.h"
#include <Vector3.h>
#include <Kinematics.hpp>
#include <SpotServo.hpp>
#include <LegMotion.h>
#include <vector>
#include <JumpMotion.h>
using namespace std;


PS3I2C ps(0x74);

SpotServo FL_Shoulder, FL_Elbow, FL_Wrist;
SpotServo BL_Shoulder, BL_Elbow, BL_Wrist;
SpotServo FR_Shoulder, FR_Elbow, FR_Wrist;
SpotServo BR_Shoulder, BR_Elbow, BR_Wrist;
double FL_angles[3]={0,0,0};
double BL_angles[3]={0,0,0};
double FR_angles[3]={0,0,0};
double BR_angles[3]={0,0,0};
Vector3 FL_Motion;
Vector3 BL_Motion;
Vector3 FR_Motion;
Vector3 BR_Motion;
Kinematics ik;
LegMotion legmotion(0.5);
vector<Vector3> jump_pos(3);
vector<int> jump_time(3);
JumpMotion jumpmotion(jump_pos,jump_time);

void setup() {
  //初期化
  Wire.begin();
  Wire.setSDA(34);
  Wire.setSCL(33);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  //s1.attach(5);
  //s1.write(90);
  //s1.write(0);

  //逆運動学の初期化
  ik.Initialize(46.3, 130, 127.5);

  //サーボの初期化
  double shoulder_liedown = 0.0;
  FL_Shoulder.Initialize(9, 0+ shoulder_liedown , 90, -81.0, FL, Shoulder, 800, 2100,-90.0,90.0); // 0 | 0: STRAIGHT | 90: OUT | -90 IN
  BL_Shoulder.Initialize(29, 0+ shoulder_liedown , 90, -9.0, BL, Shoulder, 800, 2100,90.0,-90.0); // 0 | 0: STRAIGHT | 90: OUT | -90 IN
  FR_Shoulder.Initialize(26, 0+ shoulder_liedown , 0, -11, BL, Shoulder, 800, 2100,90.0,-90.0); // 0 | 0: STRAIGHT | 90: OUT | -90 IN
  BR_Shoulder.Initialize(12, 0+ shoulder_liedown , 0, -85, BL, Shoulder, 800, 2100,-90.0,90.0); // 0 | 0: STRAIGHT | 90: OUT | -90 IN
  double elbow_liedown = 0.0;
  FL_Elbow.Initialize(31, elbow_liedown, 0, -1.0, FL, Elbow, 800, 2100, -90.0, 90.0);        // 4 | 0: STRAIGHT | 90: BACK
  BL_Elbow.Initialize(28, elbow_liedown, 0, 0, FL, Elbow, 800, 2100, -90.0, 90.0);        // 4 | 0: STRAIGHT | 90: BACK
  FR_Elbow.Initialize(25, elbow_liedown, 0, 0, FL, Elbow, 800, 2100, -90.0, 90.0);        // 4 | 0: STRAIGHT | 90: BACK
  BR_Elbow.Initialize(11, elbow_liedown, 0, -1, FL, Elbow, 800, 2100, -90.0, 90.0);        // 4 | 0: STRAIGHT | 90: BACK
  double wrist_liedown = 0.0;
  FL_Wrist.Initialize(30, wrist_liedown, 0, -19.0, FL, Wrist, 1600, 2400, -120.0, -60.0);    // 8 | 0: STRAIGHT | -90: FORWARD
  BL_Wrist.Initialize(27, wrist_liedown, 0, -41.0, FL, Wrist, 1600, 2400, -120.0, -60.0);    // 8 | 0: STRAIGHT | -90: FORWARD
  FR_Wrist.Initialize(24, wrist_liedown, 0, 61, FL, Wrist, 1600, 2400, -60.0, -120.0);    // 8 | 0: STRAIGHT | -90: FORWARD
  BR_Wrist.Initialize(10, wrist_liedown, 0, 34, FL, Wrist, 1600, 2400, -60.0, -120.0);    // 8 | 0: STRAIGHT | -90: FORWARD
  
  //ジャンプモーションの座標
  jump_pos.at(0).x=0;
  jump_pos.at(0).y=0;
  jump_pos.at(0).z=-90;
  jump_pos.at(1).x=0;
  jump_pos.at(1).y=0;
  jump_pos.at(1).z=120;
  jump_pos.at(2).x=-30;
  jump_pos.at(2).y=0;
  jump_pos.at(2).z=-50;
  jump_time.at(0)=1000;
  jump_time.at(1)=200;
  jump_time.at(2)=1000;

}

//kata-hizi 46.3mm
//hizi-tekubi 130mm
//tekubi-saki 127.5mm

void loop() {
  
  //update function
  ps.update();
  jumpmotion.update(jump_pos,jump_time);
  FL_Shoulder.update_clk();
  FL_Elbow.update_clk();
  FL_Wrist.update_clk();
  BL_Shoulder.update_clk();
  BL_Elbow.update_clk();
  BL_Wrist.update_clk();
  FR_Shoulder.update_clk();
  FR_Elbow.update_clk();
  FR_Wrist.update_clk();
  BR_Shoulder.update_clk();
  BR_Elbow.update_clk();
  BR_Wrist.update_clk();
  
  double a=140;//map(ps.A_Ry(),0,255,90,220);
  double b=0;//map(ps.A_Rx(),0,255,100,-100);
  double c=50;//map(ps.A_Lx(),0,255,0,100);
  /*
  double a=map(ps.A_Ry(),0,255,90,220);
  double b=map(ps.A_Rx(),0,255,100,-100);
  double c=map(ps.A_Lx(),0,255,0,100);
  */
  int d=map(ps.A_Ry(),0,255,-180,0);


  //コントローラから制御量を計算
  double px=map(ps.A_Lx(),0,255,100,-100);
  double py=map(ps.A_Ly(),0,255,-100,100);
  double h=map(ps.A_Ry(),0,255,-50,50);
  double psize=sqrt(px*px+py*py)*0.05;
  double pangle=-atan2(-px,-py)*RAD_TO_DEG;
  
  if(ps.C_Up()){
    jumpmotion.start();
  }
  
  //psize=0.05*py;

  //歩行モーションパラメータ
  //歩行半径[mm]
  const double radius=50;
  //歩行モーション一周期あたりの地面踏んでいる時間の割合
  const double ground_per_period=0.7;
  //サーボモーターの速度
  double s_speed=1000;

  if(jumpmotion.playing()){
    //ジャンプモーションシーケンス
    FL_Motion=jumpmotion.getPos();
    BL_Motion=jumpmotion.getPos()*1.2;
    FR_Motion=jumpmotion.getPos();
    BR_Motion=jumpmotion.getPos()*1.2;
    s_speed=4000;
  }
  else{
    //各脚の歩行モーションを計算
    FL_Motion=legmotion.getLegPositon2(psize,radius,ground_per_period,pangle,0);
    BL_Motion=legmotion.getLegPositon2(psize,radius,ground_per_period,pangle,180);
    FR_Motion=legmotion.getLegPositon2(psize,radius,ground_per_period,pangle,180);
    BR_Motion=legmotion.getLegPositon2(psize,radius,ground_per_period,pangle,0);
    s_speed=1000;
  }



  //z方向のホームポジション
  double home_pos_z=200+h;

  //各足の目標座標から逆運動学で各軸の角度を計算
  ik.GetJointAngles(FL_Motion.x,FL_Motion.y+46.3+10,FL_Motion.z+home_pos_z,Right,FL_angles);
  ik.GetJointAngles(BL_Motion.x,BL_Motion.y+46.3+10,BL_Motion.z+home_pos_z,Right,BL_angles);
  ik.GetJointAngles(FR_Motion.x,-(FR_Motion.y+46.3-10),FR_Motion.z+home_pos_z,Left,FR_angles);
  ik.GetJointAngles(BR_Motion.x,-(BR_Motion.y+46.3-10),BR_Motion.z+home_pos_z,Left,BR_angles);

  //ik.GetJointAngles(b,c,a,Right,FL_angles);
  //ik.GetJointAngles(b,-c,a,Left,FR_angles);
  //ik.GetJointAngles(x,c,z,Right,FL_angles);

  //各軸の原点補正
  FL_angles[0]+=180*DEG_TO_RAD;
  FL_angles[1]*=-1;//+=-0*DEG_TO_RAD;
  FL_angles[2]+=-180*DEG_TO_RAD;
  BL_angles[0]+=180*DEG_TO_RAD;
  BL_angles[1]*=-1;//+=-0*DEG_TO_RAD;
  BL_angles[2]+=-180*DEG_TO_RAD;
  FR_angles[0]+=180*DEG_TO_RAD;
  FR_angles[2]+=-180*DEG_TO_RAD;
  BR_angles[0]+=180*DEG_TO_RAD;
  BR_angles[2]+=-180*DEG_TO_RAD;

  
  //サーボモーターに各軸の指令値を送る
  FL_Shoulder.SetGoal(FL_angles[0]*RAD_TO_DEG,s_speed);
  FL_Elbow.SetGoal(FL_angles[1]*RAD_TO_DEG,s_speed);
  FL_Wrist.SetGoal(FL_angles[2]*RAD_TO_DEG,s_speed);
  BL_Shoulder.SetGoal(BL_angles[0]*RAD_TO_DEG,s_speed);
  BL_Elbow.SetGoal(BL_angles[1]*RAD_TO_DEG,s_speed);
  BL_Wrist.SetGoal(BL_angles[2]*RAD_TO_DEG,s_speed);
  FR_Shoulder.SetGoal(FR_angles[0]*RAD_TO_DEG,s_speed);
  FR_Elbow.SetGoal(FR_angles[1]*RAD_TO_DEG,s_speed);
  FR_Wrist.SetGoal(FR_angles[2]*RAD_TO_DEG,s_speed);
  BR_Shoulder.SetGoal(BR_angles[0]*RAD_TO_DEG,s_speed);
  BR_Elbow.SetGoal(BR_angles[1]*RAD_TO_DEG,s_speed);
  BR_Wrist.SetGoal(BR_angles[2]*RAD_TO_DEG,s_speed);

  //cout<<"qqq";
  //cout<<"kata="<<BL_angles[0]*RAD_TO_DEG<<" hizi="<<FL_angles[1]*RAD_TO_DEG<<" tekubu="<<FL_angles[2]*RAD_TO_DEG<<",";
  
  //cout<<legmotion.getRad(psize);
  //cout<<jumpmotion.playing();
  //cout<<ps.A_Ly();
  //Serial.print(BR_Wrist.GetPoseEstimate());
  cout<<endl;
}