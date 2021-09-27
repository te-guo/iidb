---
geometry: margin=2cm
---

# PA1

> ddl w3

## sqlite3 安装

1. 对于 Linux 和 macOS 用户，在终端中输入 `sqlite3` 即可使用系统预装软件包
2. 对于 Windows 用户，在[官网](https://www.sqlite.org/download.html)下载 `sqlite-tools-win` 软件包，然后添加到系统变量

同样地，你可以选择安装 PostgreSQL 或 MySQL 等数据库发行版，并在作业中注明使用的方言

## 数据集 2pts

我们使用 TPC-H 数据集作为作业的数据来源

具体来说，我们提供了一些 csv 文件，你们需要首先将其导入 sqlite 数据库中

## 单表查询 5*1pts

对于以下需求，设计 SQL 语句输出结果，你只需要提交对应 SQL 语句

注意以下语句顺序执行，前序操作可能影响后续结果

1. （`ORDER`）求 `order` 数大于 2 的 `customer` 的 `O_TOTALPRICE`
2. （`LINEITEM`）对 `discount` 大于 0.02 的 `tax` 加 10%
3. （`LINEITEM`）对所有 `tax` 小于 0.05 的物品（`L_ORDERKEY`,`L_LINENUMBER`）按照 `L_ORDERKEY` 计算平均 `discount`
   1. 对结果按平均 `discount` 从大到小排序
   2. 展示平均 `discount` 最大的 10 行
4. （`LINEITEM`）求 `discount` 最大的 `item`，用 `L_ORDERKEY` 和 `L_LINENUMBER` 表示
   1. 禁止使用 `agg` 操作（即需要用基本运算符表示MAX的逻辑）
5. （`PARTSUPP`）对于相同的 `PS_PARTKEY`，求所有供应商的 `PS_AVAILQTY` 之和

## 多表查询 3*1pts

1. 解释如下 SQL 的作用

   ```sql
   SELECT `C_NAME`, `O_ORDERSTATUS`, `N_NATIONKEY` FROM `customer`, `order`, `nation` WHERE `C_CUSTKEY`=`O_CUSTKEY` AND `C_NATIONKEY`=`N_NATIONKEY` AND `N_NAME`='CHINA'
   ```

2. （`CUSTOMER`, `ORDER`）求所有 `total_price` 小于 10000 的 `customer` 行

3. 将 1 中表范围增加 `LINEITEM` 表，然后合理选择顺序使之执行最快
