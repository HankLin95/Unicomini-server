
.open project.db
.head on
.mode column

create table tbl_user                  --�û���¼��
(
   username char(10) not null,         --�û��� (Ϊ6λ�ַ�)
   userpwd char(10) not null          --���� (Ϊ6��10���ַ�)
);


create table tbl_card                  --����ӿ���
(
   card_money int not null,            --���Ľ��(30/50/100)
   card_id char(10) unique not null,   --���ĳ�ֵ���� (Ԥ��Ϊ10���ַ���ǰ���ַ�������)
   card_state int not null  ,           --���ĳ�ֵ״̬ (0����δ��1��������δ�䣬2�����ѳ�)
   trans_flowid char(25) not null      --������ˮ(���Կͻ�����ˮ)
);


create table tbl_balance               --����
(
   tel char(14)  not null,             --�绰����(14���ַ�)
   balance float not null              --���
);


create table tbl_charge                --��ֵ�ɷ�
(
   tel char(14) not null,              --�绰����
   card_id char(10) not null,          --���ĳ�ֵ����
   charge_time time not null,          --��ֵʱ��(��ȡϵͳʱ��)
   user_id char(10) not null,          --����Ա(��¼���û���)
   trans_flowid char(25) not null      --������ˮ(���Կͻ�����ˮ)
);


create table tbl_bill                 --�˵���
(
   tel char(14) not null,             --�绰����
   bill_month char(7) not null,       --�˵�ʱ��(YYYYMM)
   local_tel float not null,          --����ͨ�� 
   message float not null,            --����Ϣ
   gprs float not null,               --GPRS
   increment float not null,          --��ֵҵ��
   sp float not null                  --SP
);



create table tbl_detail                --��ϸ�˵�
(
   bill_month char(7) not null,        --�˵�ʱ��(yyyy-mm����yyyymm)(�������ʹ�����int��char��)
   act_tel char(14) not null,          --���е绰
   net_tel char(14) not null,          --���е绰
   tel_type int  not null,             --ͨ������
   tel_time int not null,              --ͨ��ʱ��(������)
   tel_money float not null,           --���
   remark char(21) not null            --��ע
);


create table tbl_config           --��������
(
   dev_id int  not null,          --�豸ID
   soft_ver char(8) not null,     --����汾
   hard_ver char(8) not null      --Ӳ���汾
);











