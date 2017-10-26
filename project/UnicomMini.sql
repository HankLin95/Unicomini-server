
.open project.db
.head on
.mode column

create table tbl_user                  --用户登录表
(
   username char(10) not null,         --用户名 (为6位字符)
   userpwd char(10) not null          --密码 (为6—10个字符)
);


create table tbl_card                  --买电子卡表
(
   card_money int not null,            --卡的金额(30/50/100)
   card_id char(10) unique not null,   --卡的充值密码 (预设为10个字符，前两字符代表金额)
   card_state int not null  ,           --卡的充值状态 (0代表未买，1代表买了未充，2代表已充)
   trans_flowid char(25) not null      --操作流水(来自客户端流水)
);


create table tbl_balance               --余额表
(
   tel char(14)  not null,             --电话号码(14个字符)
   balance float not null              --余额
);


create table tbl_charge                --充值缴费
(
   tel char(14) not null,              --电话号码
   card_id char(10) not null,          --卡的充值密码
   charge_time time not null,          --充值时间(获取系统时间)
   user_id char(10) not null,          --操作员(登录的用户名)
   trans_flowid char(25) not null      --操作流水(来自客户端流水)
);


create table tbl_bill                 --账单表
(
   tel char(14) not null,             --电话号码
   bill_month char(7) not null,       --账单时间(YYYYMM)
   local_tel float not null,          --本地通话 
   message float not null,            --短信息
   gprs float not null,               --GPRS
   increment float not null,          --增值业务
   sp float not null                  --SP
);



create table tbl_detail                --详细账单
(
   bill_month char(7) not null,        --账单时间(yyyy-mm或者yyyymm)(数据类型待定（int或char）)
   act_tel char(14) not null,          --主叫电话
   net_tel char(14) not null,          --被叫电话
   tel_type int  not null,             --通话类型
   tel_time int not null,              --通话时长(分钟数)
   tel_money float not null,           --金额
   remark char(21) not null            --备注
);


create table tbl_config           --参数配置
(
   dev_id int  not null,          --设备ID
   soft_ver char(8) not null,     --软件版本
   hard_ver char(8) not null      --硬件版本
);











